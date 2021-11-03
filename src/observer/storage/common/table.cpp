/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/13.
//

#include <limits.h>
#include <string.h>
#include <algorithm>

#include "storage/common/table.h"
#include "storage/common/table_meta.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/default/disk_buffer_pool.h"
#include "storage/common/record_manager.h"
#include "storage/common/condition_filter.h"
#include "storage/common/meta_util.h"
#include "storage/common/index.h"
#include "storage/common/bplus_tree_index.h"
#include "storage/trx/trx.h"

Table::Table() :
    data_buffer_pool_(nullptr),
    file_id_(-1),
    record_handler_(nullptr) {
}

Table::~Table() {
    delete record_handler_;
    record_handler_ = nullptr;

    if (data_buffer_pool_ != nullptr && file_id_ >= 0) {
        data_buffer_pool_->close_file(file_id_);
        data_buffer_pool_ = nullptr;
    }

    LOG_INFO("Table has been closed: %s", name());
}

RC Table::create(const char* path, const char* name, const char* base_dir, int attribute_count, const AttrInfo attributes[]) {

    if (nullptr == name || common::is_blank(name)) {
        LOG_WARN("Name cannot be empty");
        return RC::INVALID_ARGUMENT;
    }
    LOG_INFO("Begin to create table %s:%s", base_dir, name);

    if (attribute_count <= 0 || nullptr == attributes) {
        LOG_WARN("Invalid arguments. table_name=%s, attribute_count=%d, attributes=%p",
            name, attribute_count, attributes);
        return RC::INVALID_ARGUMENT;
    }

    RC rc = RC::SUCCESS;

    // 使用 table_name.table记录一个表的元数据
    // 判断表文件是否已经存在

    int fd = ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
    if (-1 == fd) {
        if (EEXIST == errno) {
            LOG_ERROR("Failed to create table file, it has been created. %s, EEXIST, %s",
                path, strerror(errno));
            return RC::SCHEMA_TABLE_EXIST;
        }
        LOG_ERROR("Create table file failed. filename=%s, errmsg=%d:%s",
            path, errno, strerror(errno));
        return RC::IOERR;
    }

    close(fd);

    // 创建文件
    if ((rc = table_meta_.init(name, attribute_count, attributes)) != RC::SUCCESS) {
        LOG_ERROR("Failed to init table meta. name:%s, ret:%d", name, rc);
        return rc; // delete table file
    }

    std::fstream fs;
    fs.open(path, std::ios_base::out | std::ios_base::binary);
    if (!fs.is_open()) {
        LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", path, strerror(errno));
        return RC::IOERR;
    }

    // 记录元数据到文件中
    table_meta_.serialize(fs);
    fs.close();

    std::string data_file = std::string(base_dir) + "/" + name + TABLE_DATA_SUFFIX;
    data_buffer_pool_ = theGlobalDiskBufferPool();
    rc = data_buffer_pool_->create_file(data_file.c_str());
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to create disk buffer pool of data file. file name=%s", data_file.c_str());
        return rc;
    }

    rc = init_record_handler(base_dir);

    base_dir_ = base_dir;
    LOG_INFO("Successfully create table %s:%s", base_dir, name);
    return rc;
}

RC Table::open(const char* meta_file, const char* base_dir) {
    // 加载元数据文件
    std::fstream fs;
    std::string meta_file_path = std::string(base_dir) + "/" + meta_file;
    fs.open(meta_file_path, std::ios_base::in | std::ios_base::binary);
    if (!fs.is_open()) {
        LOG_ERROR("Failed to open meta file for read. file name=%s, errmsg=%s", meta_file, strerror(errno));
        return RC::IOERR;
    }
    if (table_meta_.deserialize(fs) < 0) {
        LOG_ERROR("Failed to deserialize table meta. file name=%s", meta_file);
        return RC::GENERIC_ERROR;
    }
    fs.close();

    // 加载数据文件
    RC rc = init_record_handler(base_dir);

    base_dir_ = base_dir;

    const int index_num = table_meta_.index_num();
    for (int i = 0; i < index_num; i++) {
        const IndexMeta* index_meta = table_meta_.index(i);
        const FieldMeta* field_meta = table_meta_.field(index_meta->field());
        if (field_meta == nullptr) {
            LOG_PANIC("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                name(), index_meta->name(), index_meta->field());
            return RC::GENERIC_ERROR;
        }

        BplusTreeIndex* index = new BplusTreeIndex();
        std::string index_file = index_data_file(base_dir, name(), index_meta->name());
        rc = index->open(index_file.c_str(), *index_meta, *field_meta);
        if (rc != RC::SUCCESS) {
            delete index;
            LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%d:%s",
                name(), index_meta->name(), index_file.c_str(), rc, strrc(rc));
            return rc;
        }
        indexes_.push_back(index);
    }
    return rc;
}

RC Table::commit_insert(Trx* trx, const RID& rid) {
    Record record;
    RC rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
        return rc;
    }

    return trx->commit_insert(this, record);
}

RC Table::rollback_insert(Trx* trx, const RID& rid) {

    Record record;
    RC rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
        return rc;
    }

    // remove all indexes
    rc = delete_entry_of_indexes(record.data, rid, false);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to delete indexes of record(rid=%d.%d) while rollback insert, rc=%d:%s",
            rid.page_num, rid.slot_num, rc, strrc(rc));
    }
    else {
        rc = record_handler_->delete_record(&rid);
    }
    return rc;
}

RC Table::insert_record(Trx* trx, Record* record) {
    RC rc = RC::SUCCESS;

    if (trx != nullptr) {
        trx->init_trx_info(this, *record);
    }
    rc = record_handler_->insert_record(record->data, table_meta_.record_size(), &record->rid);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Insert record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
        return rc;
    }

    if (trx != nullptr) {
        rc = trx->insert_record(this, record);
        if (rc != RC::SUCCESS) {
            LOG_ERROR("Failed to log operation(insertion) to trx");

            RC rc2 = record_handler_->delete_record(&record->rid);
            if (rc2 != RC::SUCCESS) {
                LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                    name(), rc2, strrc(rc2));
            }
            return rc;
        }
    }

    rc = insert_entry_of_indexes(record->data, record->rid);
    if (rc != RC::SUCCESS) {
        RC rc2 = delete_entry_of_indexes(record->data, record->rid, true);
        if (rc2 != RC::SUCCESS) {
            LOG_PANIC("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
        }
        rc2 = record_handler_->delete_record(&record->rid);
        if (rc2 != RC::SUCCESS) {
            LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
        }
        return rc;
    }
    return rc;
}
bool Table::check_date(const Value value){
  int date = *(int *)value.data;
  int year = date/10000;
  int month = date/100-year*100;
  int datetime = date-10000*year-100*month;
       //LOG_INFO("The date is:%d,%d,%d",year,month,datetime);
  int Month_buf[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };  //月份修正表
  if(month==2)                                                             //闰年2月+1天
    (((year%4==0)&&(year%100!=0))||(year%400==0))?Month_buf[1]+=1:Month_buf[1];
  if (month>12||month<1||datetime>Month_buf[month-1]||datetime<1)           //判断月份日期是否合法
    return false;
  return true;
}

RC Table::insert_record(Trx* trx, int insert_num, Tuplevalue *const tuplevalue []) {
    RC rc = RC::SUCCESS;
    if (insert_num <= 0 || nullptr == tuplevalue) {
        LOG_ERROR("Invalid argument. value num=%d, values=%p", insert_num, tuplevalue);
        return RC::INVALID_ARGUMENT;
    }
    int i,j;
    //如果遇到插入失败的，需要将之前的插入成功的记录全部删除
    std::vector<RID> ridlist;
    Record record;
    RC rc2;
    for(i=0;i<insert_num;i++){
        rc = insert_record(trx,tuplevalue[i]->value_num,tuplevalue[i]->values,&record);
        if(rc == RC::SUCCESS){
            LOG_INFO("the ridlist add rid : %d,%d",record.rid.page_num,record.rid.slot_num);
            rc = trx->commit();//这是个神奇的操作，希望能有用
            ridlist.push_back(record.rid);
        }
        else{
            for(int j=0;j<ridlist.size();j++){
                rc2 = record_handler_->get_record(&ridlist[j], &record);
                LOG_INFO("delete the record whose rid is: %d,%d",record.rid.page_num,record.rid.slot_num);
                rc2 = delete_record(trx,&record);
                if (rc2 != RC::SUCCESS) {
                    LOG_ERROR("Rollback: delete record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
                    return rc;
                }
            }
            return RC::INVALID_ARGUMENT;
        }         
    }
    return rc;
}


RC Table::insert_record(Trx* trx, int value_num, const Value* values,Record *record) {
    if (value_num <= 0 || nullptr == values) {
        LOG_ERROR("Invalid argument. value num=%d, values=%p", value_num, values);
        return RC::INVALID_ARGUMENT;
    }
    /*BY:CAQ 遍历values,查找type是DATES的value，进行value验证*/
    int i;
    for(i=0;i<value_num;i++){
      const Value &value = values[i];
      if(value.type == DATES){
        if(check_date(value)==false)
            return RC::INVALID_ARGUMENT;
     }
    }

    char* record_data;
    RC rc = make_record(value_num, values, record_data);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
        return rc;
    }

    record->data = record_data;
    // record.valid = true;
    rc = insert_record(trx, record);
    LOG_INFO("insert the record:the rid is %d,%d",record->rid.page_num,record->rid.slot_num);
    delete[] record_data;
    return rc;
}

const char* Table::name() const {
    return table_meta_.name();
}

const TableMeta& Table::table_meta() const {
    return table_meta_;
}

RC Table::make_record(int value_num, const Value* values, char*& record_out) {
    // 检查字段类型是否一致
    if (value_num + table_meta_.sys_field_num() != table_meta_.field_num()) {
        return RC::SCHEMA_FIELD_MISSING;
    }

    const int normal_field_start_index = table_meta_.sys_field_num();
    for (int i = 0; i < value_num; i++) {
        const FieldMeta* field = table_meta_.field(i + normal_field_start_index);
        const Value& value = values[i];
        if (field->type() != value.type) {
            LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
                field->name(), field->type(), value.type);
            return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
    }

    // 复制所有字段的值
    int record_size = table_meta_.record_size();
    char* record = new char[record_size];

    for (int i = 0; i < value_num; i++) {
        const FieldMeta* field = table_meta_.field(i + normal_field_start_index);
        const Value& value = values[i];
        memcpy(record + field->offset(), value.data, field->len());
    }

    record_out = record;
    return RC::SUCCESS;
}

RC Table::init_record_handler(const char* base_dir) {
    std::string data_file = std::string(base_dir) + "/" + table_meta_.name() + TABLE_DATA_SUFFIX;
    if (nullptr == data_buffer_pool_) {
        data_buffer_pool_ = theGlobalDiskBufferPool();
    }

    int data_buffer_pool_file_id;
    RC rc = data_buffer_pool_->open_file(data_file.c_str(), &data_buffer_pool_file_id);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to open disk buffer pool for file:%s. rc=%d:%s",
            data_file.c_str(), rc, strrc(rc));
        return rc;
    }

    record_handler_ = new RecordFileHandler();
    rc = record_handler_->init(*data_buffer_pool_, data_buffer_pool_file_id);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to init record handler. rc=%d:%s", rc, strrc(rc));
        return rc;
    }

    file_id_ = data_buffer_pool_file_id;
    return rc;
}

/**
 * 为了不把Record暴露出去，封装一下
 */
class RecordReaderScanAdapter {
public:
    explicit RecordReaderScanAdapter(void (*record_reader)(const char* data, void* context), void* context)
        : record_reader_(record_reader), context_(context) {
    }

    void consume(const Record* record) {
        record_reader_(record->data, context_);
    }
private:
    void (*record_reader_)(const char*, void*);
    void* context_;
};
static RC scan_record_reader_adapter(Record* record, void* context) {
    RecordReaderScanAdapter& adapter = *(RecordReaderScanAdapter*)context;
    adapter.consume(record);
    return RC::SUCCESS;
}

RC Table::scan_record(Trx* trx, ConditionFilter* filter, int limit, void* context, void (*record_reader)(const char* data, void* context)) {
    RecordReaderScanAdapter adapter(record_reader, context);
    return scan_record(trx, filter, limit, (void*)&adapter, scan_record_reader_adapter);
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, std::vector<RID >& ridlist){
  if (0 == limit) {
    return RC::SUCCESS;
  }

  if (limit < 0) {
    limit = INT_MAX;
  }
  
  IndexScanner *index_scanner = find_index_for_scan(filter);
  if (index_scanner != nullptr) {
      LOG_INFO("There is a index to use!");
    return scan_record_by_index(trx, index_scanner, filter, limit, ridlist);
  }
  
  //LOG_INFO("There is no index to use!");
  RC rc = RC::SUCCESS;
  RecordFileScanner scanner;
  //LOG_INFO("The open_scan file_id_= %d",file_id_);
  rc = scanner.open_scan(*data_buffer_pool_, file_id_, filter);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. file id=%d. rc=%d:%s", file_id_, rc, strrc(rc));
    return rc;
  }

  int record_count = 0;
  Record record;
  rc = scanner.get_first_record(&record);
  for ( ; RC::SUCCESS == rc && record_count < limit; rc = scanner.get_next_record(&record)) {
    if (trx == nullptr || trx->is_visible(this, &record)) {
      ridlist.push_back(record.rid);
      if (rc != RC::SUCCESS) {
        break;
      }
      record_count++;
    }
  }
  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_ERROR("failed to scan record. file id=%d, rc=%d:%s", file_id_, rc, strrc(rc));
  }
  scanner.close_scan();
  return rc;
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context)) {
  if (nullptr == record_reader) {
    return RC::INVALID_ARGUMENT;
  }

  if (0 == limit) {
    return RC::SUCCESS;
  }

  if (limit < 0) {
    limit = INT_MAX;
  }
  
  IndexScanner *index_scanner = find_index_for_scan(filter);
  if (index_scanner != nullptr) {
    return scan_record_by_index(trx, index_scanner, filter, limit, context, record_reader);
  }
  
  LOG_INFO("There is no index to use!");
  RC rc = RC::SUCCESS;
  RecordFileScanner scanner;
  LOG_INFO("The open_scan file_id_= %d",file_id_);
  rc = scanner.open_scan(*data_buffer_pool_, file_id_, filter);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. file id=%d. rc=%d:%s", file_id_, rc, strrc(rc));
    return rc;
  }

  int record_count = 0;
  Record record;
  rc = scanner.get_first_record(&record);
  LOG_INFO("The first_record is %d:%d",record.rid.page_num,record.rid.slot_num);
  for ( ; RC::SUCCESS == rc && record_count < limit; rc = scanner.get_next_record(&record)) {
    LOG_INFO("The record is visible:%d",trx->is_visible(this, &record));
    if (trx == nullptr || trx->is_visible(this, &record)) {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS) {
        break;
      }
      record_count++;
    }
  }

  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_ERROR("failed to scan record. file id=%d, rc=%d:%s", file_id_, rc, strrc(rc));
  }
  scanner.close_scan();
  return rc;
}

RC Table::scan_record_by_index(Trx *trx, IndexScanner *scanner, ConditionFilter *filter, int limit,std::vector<RID >& ridlist) {
  RC rc = RC::SUCCESS;
  RID rid;
  Record record;
  int record_count = 0;
  while (record_count < limit) {
    rc = scanner->next_entry(&rid);
    if (rc != RC::SUCCESS) {
      if (RC::RECORD_EOF == rc) {
        rc = RC::SUCCESS;
        break;
      }
      LOG_ERROR("Failed to scan table by index. rc=%d:%s", rc, strrc(rc));
      break;
    }

    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to fetch record of rid=%d:%d, rc=%d:%s", rid.page_num, rid.slot_num, rc, strrc(rc));
      break;
    }

    if ((trx == nullptr || trx->is_visible(this, &record)) && (filter == nullptr || filter->filter(record))) {
      ridlist.push_back(record.rid);
      if (rc != RC::SUCCESS) {
        LOG_TRACE("Record reader break the table scanning. rc=%d:%s", rc, strrc(rc));
        break;
      }
    }

    record_count++;
  }

  scanner->destroy();
  return rc;
}


RC Table::scan_record_by_index(Trx* trx, IndexScanner* scanner, ConditionFilter* filter, int limit, void* context,
    RC(*record_reader)(Record*, void*)) {
    RC rc = RC::SUCCESS;
    RID rid;
    Record record;
    int record_count = 0;
    while (record_count < limit) {
        rc = scanner->next_entry(&rid);
        if (rc != RC::SUCCESS) {
            if (RC::RECORD_EOF == rc) {
                rc = RC::SUCCESS;
                break;
            }
            LOG_ERROR("Failed to scan table by index. rc=%d:%s", rc, strrc(rc));
            break;
        }

        rc = record_handler_->get_record(&rid, &record);
        if (rc != RC::SUCCESS) {
            LOG_ERROR("Failed to fetch record of rid=%d:%d, rc=%d:%s", rid.page_num, rid.slot_num, rc, strrc(rc));
            break;
        }

        if ((trx == nullptr || trx->is_visible(this, &record)) && (filter == nullptr || filter->filter(record))) {
            rc = record_reader(&record, context);
            if (rc != RC::SUCCESS) {
                LOG_TRACE("Record reader break the table scanning. rc=%d:%s", rc, strrc(rc));
                break;
            }
        }

        record_count++;
    }

    scanner->destroy();
    return rc;
}

class IndexInserter {
public:
    explicit IndexInserter(Index* index) : index_(index) {
    }

    RC insert_index(const Record* record) {
        return index_->insert_entry(record->data, &record->rid);
    }
private:
    Index* index_;
};

static RC insert_index_record_reader_adapter(Record* record, void* context) {
    IndexInserter& inserter = *(IndexInserter*)context;
    return inserter.insert_index(record);
}

RC Table::update_index(Trx* trx, const char* attribute_name) {
    RC rc = RC::SUCCESS;
    const IndexMeta* index_meta = table_meta_.find_index_by_field(attribute_name);
    if (index_meta == nullptr) {    //不是索引
        return RC::SUCCESS;
    }
    const char* index_name = index_meta->name();
    Index* old_index = find_index(index_name);
    if (old_index == nullptr) {
        return RC::SUCCESS;
    }
    std::vector<Index*>::iterator it = std::find(indexes_.begin(), indexes_.end(), old_index);
    if (it == indexes_.end()) {
        LOG_INFO("an unexpected error!\n");
        return RC::SUCCESS;
    }
    indexes_.erase(it);
    delete old_index;

    //删除原来的.index文件
    std::string relation_name_str = table_meta_.name();
    std::string indexfile_name = relation_name_str + "-";
    indexfile_name += index_name;
    indexfile_name += ".index";
    std::string index_path = "./miniob/db/sys/" + indexfile_name;
    if (remove(index_path.c_str()) != 0) {  //删除失败
        LOG_ERROR("Failed to delete .index file: %s\n", index_path.c_str());
        return RC::GENERIC_ERROR;
    }

    // 重新创建索引相关数据，建立.index文件
    const FieldMeta* field_meta = table_meta_.field(attribute_name);
    BplusTreeIndex* index = new BplusTreeIndex();
    std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
    rc = index->create(index_file.c_str(), *index_meta, *field_meta);
    if (rc != RC::SUCCESS) {
        delete index;
        LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
        return rc;
    }

    // 重新遍历当前的所有数据，插入这个索引
    IndexInserter index_inserter(index);
    rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
    if (rc != RC::SUCCESS) {
        // rollback
        delete index;
        LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
        return rc;
    }
    indexes_.push_back(index);


    // 创建元数据临时文件
    std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
    std::fstream fs;
    fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    if (!fs.is_open()) {
        LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
        return RC::IOERR; // 创建索引中途出错，要做还原操作
    }
    if (table_meta_.serialize(fs) < 0) {
        LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
        return RC::IOERR;
    }
    fs.close();

    // 覆盖原始元数据文件
    std::string meta_file = table_meta_file(base_dir_.c_str(), name());
    int ret = rename(tmp_file.c_str(), meta_file.c_str());
    if (ret != 0) {
        LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). " \
            "system error=%d:%s", tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
        return RC::IOERR;
    }


    LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

    return rc;
}

RC Table::create_index(Trx *trx, const char *index_name, const char *attribute_name) {
  if (index_name == nullptr || common::is_blank(index_name) ||
      attribute_name == nullptr || common::is_blank(attribute_name)) {
    return RC::INVALID_ARGUMENT;
  }
  if (table_meta_.index(index_name) != nullptr ||
      table_meta_.find_index_by_field((attribute_name))) {
    return RC::SCHEMA_INDEX_EXIST;
  }

  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  if (!field_meta) {
    return RC::SCHEMA_FIELD_MISSING;
  }

  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, *field_meta);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, *field_meta);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS) {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR; // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). " \
              "system error=%d:%s", tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}


/*by CAQ*/
RC Table::update_record(Trx *trx, const char *attribute_name, const Value *value, int condition_num, const Condition conditions[], int *updated_count) {
  RC rc = RC::SUCCESS;
  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  //检查attribute是否存在
  if (nullptr == field_meta) {
    LOG_ERROR("No such field. %s.%s", this->name(), attribute_name);
    return RC::SCHEMA_FIELD_MISSING;
  }
  //检查attribute字段类型是否相同
  if(field_meta->type() != value->type){
    LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
    field_meta->name(), field_meta->type(), value->type);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  //如果value是DATES，需要检查合法性
  if(value->type==DATES){
    if(check_date(*value)==false){
      return RC::INVALID_ARGUMENT;
    }
  }
  //获得偏移量和value长度
  int attr_offset = field_meta->offset();
  int attr_len = field_meta->len();
  LOG_INFO("attr_offset= %d,attr_len=%d", attr_offset, attr_len);
  CompositeConditionFilter condition_filter;
  rc = condition_filter.init(*this, conditions, condition_num);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  std::vector<RID > ridlist;
  rc = scan_record(trx, &condition_filter, -1, ridlist);
  for (size_t i = 0; i < ridlist.size(); i++)
  {
    Record record;
    rc = record_handler_->get_record(&ridlist[i], &record);
    if (rc != RC::SUCCESS) {
    return rc;
    }
    //获得新的record数据
    //先把原来的record数据复制到record_data
    //再根据offset更新对应的value
    Record new_record;
    int record_size = table_meta_.record_size();
    char *record_data = new char [record_size];
    memcpy(record_data,record.data,record_size);
    memcpy(record_data + attr_offset, value->data, attr_len);
    new_record.data = record_data;
    //删除原来的record
    rc = delete_record(trx,&record);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Update record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
      return rc;
    }
    LOG_INFO("Delete the original record!");
    //插入新的record
    rc = insert_record(trx,&new_record);
    delete[] record_data;
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Update record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
      return rc;
    }
    LOG_INFO("Insert the new record!");
  }

  //by XY: update index
  if (update_index(trx, attribute_name) != RC::SUCCESS) {
      LOG_ERROR("failed to update index\n");
      return RC::GENERIC_ERROR;
  }
  return rc;
}

class RecordDeleter {
public:
    RecordDeleter(Table& table, Trx* trx) : table_(table), trx_(trx) {
    }

    RC delete_record(Record* record) {
        RC rc = RC::SUCCESS;
        rc = table_.delete_record(trx_, record);
        if (rc == RC::SUCCESS) {
            deleted_count_++;
        }
        return rc;
    }

    int deleted_count() const {
        return deleted_count_;
    }

private:
    Table& table_;
    Trx* trx_;
    int deleted_count_ = 0;
};

static RC record_reader_delete_adapter(Record* record, void* context) {
    RecordDeleter& record_deleter = *(RecordDeleter*)context;
    return record_deleter.delete_record(record);
}

RC Table::delete_record(Trx* trx, ConditionFilter* filter, int* deleted_count) {
    RecordDeleter deleter(*this, trx);
    RC rc = scan_record(trx, filter, -1, &deleter, record_reader_delete_adapter);
    if (deleted_count != nullptr) {
        *deleted_count = deleter.deleted_count();
    }
    return rc;
}

RC Table::delete_record(Trx* trx, Record* record) {
    RC rc = RC::SUCCESS;
    if (trx != nullptr) {
        rc = trx->delete_record(this, record);
    }
    else {           
        rc = delete_entry_of_indexes(record->data, record->rid, false);// 重复代码 refer to commit_delete
        if (rc != RC::SUCCESS) {
            LOG_ERROR("Failed to delete indexes of record (rid=%d.%d). rc=%d:%s",
                record->rid.page_num, record->rid.slot_num, rc, strrc(rc));
        }
        else {
            rc = record_handler_->delete_record(&record->rid);
        }
    }
    return rc;
}

RC Table::commit_delete(Trx* trx, const RID& rid) {
    RC rc = RC::SUCCESS;
    Record record;
    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
        return rc;
    }
    rc = delete_entry_of_indexes(record.data, record.rid, false);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
            rid.page_num, rid.slot_num, rc, strrc(rc));// panic?
    }

    rc = record_handler_->delete_record(&rid);
    if (rc != RC::SUCCESS) {
        return rc;
    }

    return rc;
}

RC Table::rollback_delete(Trx* trx, const RID& rid) {
    RC rc = RC::SUCCESS;
    Record record;
    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
        return rc;
    }

    return trx->rollback_delete(this, record); // update record in place
}

RC Table::insert_entry_of_indexes(const char* record, const RID& rid) {
    RC rc = RC::SUCCESS;
    for (Index* index : indexes_) {
        rc = index->insert_entry(record, &rid);
        if (rc != RC::SUCCESS) {
            break;
        }
    }
    return rc;
}

RC Table::delete_entry_of_indexes(const char* record, const RID& rid, bool error_on_not_exists) {
    RC rc = RC::SUCCESS;
    for (Index* index : indexes_) {
        rc = index->delete_entry(record, &rid);
        if (rc != RC::SUCCESS) {
            if (rc != RC::RECORD_INVALID_KEY || !error_on_not_exists) {
                break;
            }
        }
    }
    return rc;
}

Index* Table::find_index(const char* index_name) const {
    for (Index* index : indexes_) {
        if (0 == strcmp(index->index_meta().name(), index_name)) {
            return index;
        }
    }
    return nullptr;
}

IndexScanner* Table::find_index_for_scan(const DefaultConditionFilter& filter) {
    const ConDesc* field_cond_desc = nullptr;
    const ConDesc* value_cond_desc = nullptr;
    if (filter.left().is_attr && !filter.right().is_attr) {
        field_cond_desc = &filter.left();
        value_cond_desc = &filter.right();
    }
    else if (filter.right().is_attr && !filter.left().is_attr) {
        field_cond_desc = &filter.right();
        value_cond_desc = &filter.left();
    }
    if (field_cond_desc == nullptr || value_cond_desc == nullptr) {
        return nullptr;
    }

    const FieldMeta* field_meta = table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
    if (nullptr == field_meta) {
        LOG_PANIC("Cannot find field by offset %d. table=%s",
            field_cond_desc->attr_offset, name());
        return nullptr;
    }

    const IndexMeta* index_meta = table_meta_.find_index_by_field(field_meta->name());
    if (nullptr == index_meta) {
        return nullptr;
    }

    Index* index = find_index(index_meta->name());
    if (nullptr == index) {
        return nullptr;
    }

    return index->create_scanner(filter.comp_op(), (const char*)value_cond_desc->value);
}

IndexScanner* Table::find_index_for_scan(const ConditionFilter* filter) {
    if (nullptr == filter) {
        return nullptr;
    }

    // remove dynamic_cast
    const DefaultConditionFilter* default_condition_filter = dynamic_cast<const DefaultConditionFilter*>(filter);
    if (default_condition_filter != nullptr) {
        return find_index_for_scan(*default_condition_filter);
    }

    const CompositeConditionFilter* composite_condition_filter = dynamic_cast<const CompositeConditionFilter*>(filter);
    if (composite_condition_filter != nullptr) {
        int filter_num = composite_condition_filter->filter_num();
        for (int i = 0; i < filter_num; i++) {
            IndexScanner* scanner = find_index_for_scan(&composite_condition_filter->filter(i));
            if (scanner != nullptr) {
                return scanner; // 可以找到一个最优的，比如比较符号是=
            }
        }
    }
    return nullptr;
}

RC Table::sync() {
    RC rc = data_buffer_pool_->flush_all_pages(file_id_);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to flush table's data pages. table=%s, rc=%d:%s", name(), rc, strrc(rc));
        return rc;
    }

    for (Index* index : indexes_) {
        rc = index->sync();
        if (rc != RC::SUCCESS) {
            LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
                name(), index->index_meta().name(), rc, strrc(rc));
            return rc;
        }
    }
    LOG_INFO("Sync table over. table=%s", name());
    return rc;
}