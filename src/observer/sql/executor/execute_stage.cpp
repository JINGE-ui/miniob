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
// Created by Longda on 2021/4/13.
//

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

#include<algorithm>

using namespace common;
using namespace std;

//by XY 用于比较的基本算子
typedef struct {
    int offset;
    int length;
    AttrType type;
}compDesc;

//by XY 支持聚合运算的投影
typedef struct {
    int offset;
    AttrType type;
    AggregationOp comp;
}aggr_tuplefield;

//by XY 检验select后的普通字段是否属于group by后的字段
typedef struct {
    string table_name;
    string attr_name;
}group_attr;


// by XY
std::vector<std::vector<string>> tables_value;
std::vector<std::string> Cartesian_result, cur_table;
std::unordered_map<std::string, int>relation_map_offset;
std::vector<std::string>  relation_map_order;
CompositeConditionFilter* condition_filter = new CompositeConditionFilter;
int curtable_record_size = 0;
std::vector<compDesc> group_by;     //用于排序


//
RC select_tables(Trx* trx, const char* db, const Selects& selects, TupleSet& tuple_set);
RC aggregation_func(Trx* trx, const Selects& selects, const char* db, const char* table_name, TupleSet& tupleset);
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);
RC get_condition(const Selects& selects, std::vector<DefaultConditionFilter*>& condition_filters, const char* table_name, Table* table);
RC select_single_table(Trx* trx, const char* db, const Selects& selects, TupleSet& tuple_set);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag) {
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event) {
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db = session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag) {
    case SCF_SELECT: { // select
      do_select(current_db, sql, exe_event->sql_event()->session_event());
      exe_event->done_immediate();
    }
    break;

    case SCF_INSERT:
    case SCF_UPDATE:
    case SCF_DELETE:
    case SCF_CREATE_TABLE:
    case SCF_SHOW_TABLES:
    case SCF_DESC_TABLE:
    case SCF_DROP_TABLE:
    case SCF_CREATE_INDEX:
    case SCF_DROP_INDEX: 
    case SCF_LOAD_DATA: {
      StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
      if (storage_event == nullptr) {
        LOG_ERROR("Failed to new StorageEvent");
        event->done_immediate();
        return;
      }

      default_storage_stage_->handle_event(storage_event);
    }
    break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_BEGIN: {
      session_event->get_client()->session->set_trx_multi_operation_mode(true);
      session_event->set_response(strrc(RC::SUCCESS));
      exe_event->done_immediate();
    }
    break;
    case SCF_COMMIT: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->commit();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_HELP: {
      const char *response = "show tables;\n"
          "desc `table name`;\n"
          "create table `table name` (`column name` `column type`, ...);\n"
          "create index `index name` on `table` (`column`);\n"
          "insert into `table` values(`value1`,`value2`);\n"
          "update `table` set column=value [where `column`=`value`];\n"
          "delete from `table` [where `column`=`value`];\n"
          "select [ * | `columns` ] from `table`;\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    default: {
      exe_event->done_immediate();
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right) {
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}
//为了select-tables新增的函数
void select_record_reader(const char* data, void* context) {
    std::vector<string>* ptr_string = (std::vector<string>*)context;
    string tmp(data, data + curtable_record_size);
    (*ptr_string).push_back(tmp);
}

//为了支持多表查询，对match_table重载，table需位于[0,layer]个
bool match_table(const Selects& selects, const char* table_name_in_condition, int layer) {
    if (table_name_in_condition == nullptr || selects.relation_num == 1) {   //不满足多表查询的匹配规则 
        return false;
    }
    for (int i = 0; i <= layer; i++) {
        if (strcmp(table_name_in_condition, relation_map_order[i].c_str()) == 0) {   //该表是第i个表，从0计数
            return true;
        }
    }
    return false;
}

//获取多表的0~layer层的 所有过滤条件， layer取值[0,table_num-1]
RC get_tables_condition(const char* db, const Selects& selects, std::vector<DefaultConditionFilter*> &condition_filters, int layer) {
    for (size_t i = 0; i < selects.condition_num; i++) {
        const Condition& condition = selects.conditions[i];
        if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // 两边都是值
            (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, layer)) ||  // 左边是属性右边是值
            (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, layer)) ||  // 左边是值，右边是属性名
            (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
                match_table(selects, condition.left_attr.relation_name, layer) && match_table(selects, condition.right_attr.relation_name, layer)) // 左右都是属性名，并且表名都符合
            ) {
            //获取左右表在笛卡尔积中的偏移
            int left_offset = 0, right_offset = 0;
            Table* left_table = nullptr, * right_table = nullptr;
            if (condition.left_is_attr == 1 && match_table(selects, condition.left_attr.relation_name, layer)) {   //左边是带表名.的属性
                left_offset = relation_map_offset[condition.left_attr.relation_name];   //应该不会出现key不存在的现象
                left_table = DefaultHandler::get_default().find_table(db, condition.left_attr.relation_name);
            }
            if (condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, layer)) {
                right_offset = relation_map_offset[condition.right_attr.relation_name];
                right_table = DefaultHandler::get_default().find_table(db, condition.right_attr.relation_name);
            }

            DefaultConditionFilter* condition_filter = new DefaultConditionFilter();
            RC rc = condition_filter->init(left_table, right_table, condition, left_offset, right_offset);
            if (rc != RC::SUCCESS) {   //非法列名
                delete condition_filter;
                for (DefaultConditionFilter*& filter : condition_filters) {
                    delete filter;
                }
                return rc;
            }
            condition_filters.push_back(condition_filter);
        }
        else {  //非法表名
            for (DefaultConditionFilter*& filter : condition_filters) {
                delete filter;
            }
            return RC::GENERIC_ERROR;
        }
    }
    return RC::SUCCESS;
}

//多表join 递归回溯 本质是nested loop join
RC run(const char* db, const Selects& selects, vector<vector<string>> &dimvalue, vector<string> &result, int layer, string curstring)
{
    if (layer < dimvalue.size() - 1)
    {
        if (dimvalue[layer].size() == 0) {    //应该不会发生
            return RC::GENERIC_ERROR;
        }
        std::vector<DefaultConditionFilter*> condition_filters;
        if (get_tables_condition(db, selects, condition_filters, layer) != RC::SUCCESS) {
            for (DefaultConditionFilter*& filter : condition_filters) {
                delete filter;
            }
            return RC::GENERIC_ERROR;
        }
        CompositeConditionFilter *record_filter = new CompositeConditionFilter;
        record_filter->init((const ConditionFilter**)condition_filters.data(), condition_filters.size());

        for (int i = 0; i < dimvalue[layer].size(); i++)
        {
            string tmp = curstring + dimvalue[layer][i];
            Record rec;
            rec.data = (char*)tmp.c_str();
            if (record_filter->filter(rec)) {   //满足过滤条件
                if (run(db,selects,dimvalue, result, layer + 1, tmp) != RC::SUCCESS) {
                    return RC::GENERIC_ERROR;
                }
            }  
        }
        for (DefaultConditionFilter*& filter : condition_filters) {
            delete filter;
        }
    }
    else if (layer == dimvalue.size() - 1)
    {
        if (dimvalue[layer].size() == 0) {
            return RC::GENERIC_ERROR;
        }

        std::vector<DefaultConditionFilter*> condition_filters;
        if (get_tables_condition(db, selects, condition_filters, layer) != RC::SUCCESS) {
            for (DefaultConditionFilter*& filter : condition_filters) {
                delete filter;
            }
            return RC::GENERIC_ERROR;
        }
        CompositeConditionFilter* record_filter = new CompositeConditionFilter;
        record_filter->init((const ConditionFilter**)condition_filters.data(), condition_filters.size());

        for (int i = 0; i < dimvalue[layer].size(); i++)
        {
            string tmp = curstring + dimvalue[layer][i];
            Record rec;
            rec.data = (char*)tmp.c_str();
            if (record_filter->filter(rec)) {   //满足过滤条件
                result.push_back(curstring + dimvalue[layer][i]);
            }
            
        }
        for (DefaultConditionFilter*& filter : condition_filters) {
            delete filter;
        }
    }
    return RC::SUCCESS;
}

// 这里没有对输入的某些信息做合法性校验，比如查询的列名、where条件中的列名等，没有做必要的合法性校验
// 需要补充上这一部分. 校验部分也可以放在resolve，不过跟execution放一起也没有关系
// 元数据校验，目前仅支持单表，一个bug：如果支持 select *,列名 from.. 会发生core dumped，所以干脆不支持了
RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event) {

  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  const Selects &selects = sql->sstr.selection;

  TupleSet tuple_set; //查询结果保存

  relation_map_offset.clear();
  relation_map_order.clear();
  Cartesian_result.clear();
  tables_value.clear();
  cur_table.clear();

  if (selects.relation_num > 1) {   //多表查询
      if (select_tables(trx, db, selects, tuple_set) != RC::SUCCESS) {
          const char* response = "FAILURE\n";
          session_event->set_response(response);
          return RC::GENERIC_ERROR;
      }
  } 
  else if(selects.relation_num == 1) {  //单表查询
        const char* table_name = selects.relations[0];
        if (select_single_table(trx, db, selects, tuple_set) != RC::SUCCESS) {
            const char* response = "FAILURE\n";
            session_event->set_response(response);
            return RC::SUCCESS;
        }        
  }
  else {  //relation_num<1
      LOG_ERROR("No table given");
      end_trx_if_need(session, trx, false);
      return RC::SQL_SYNTAX;
  }
  
  relation_map_offset.clear();
  relation_map_order.clear();
  Cartesian_result.clear();
  tables_value.clear();
  cur_table.clear();

  std::stringstream ss;
  tuple_set.print(ss, selects.relation_num);

  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
  return rc;
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.relation_num == 1;
}

//为了支持多表查询，对match_table重载
bool match_table(const Selects& selects, const char* table_name_in_condition) {
    if (table_name_in_condition == nullptr || selects.relation_num == 1) {   //不满足多表查询的匹配规则 
        return false;
    }
    if (relation_map_offset.find(table_name_in_condition) == relation_map_offset.end()) {   //该表不存在
        return false;
    }
    return true;
}

//为了支持聚合运算（单表）
static RC schema_add_field(Table* table, const char* field_name, TupleSchema& schema, AggregationOp comp) {
    if (strcmp(field_name, "*") == 0) {   //只能是count(*)
        schema.add_if_not_exists(INTS, table->name(), "*",comp);
        return RC::SUCCESS;
    }
    const FieldMeta* field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta) {
        LOG_WARN("No such field. %s.%s", table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
    }

    schema.add_if_not_exists(INTS, table->name(), field_meta->name(),comp);   //type暂时设置为int
    return RC::SUCCESS;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

//暂时不支持DATE类型   计算table_record的[start,end)之间的 max聚合字段，并写入tuple
RC get_max_aggregation(aggr_tuplefield myfield, Tuple &tuple, int start, int stop, const vector<string>& table_record) {
    int offset = myfield.offset;
    switch (myfield.type) {
    case INTS: {
        int max_int = *(int*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            int tmp = *(int*)(record + offset);
            if (tmp > max_int) {
                max_int = tmp;
            }
        }
        tuple.add(max_int);
    }break;
    case FLOATS: {
        float max_float = *(float*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            float tmp = *(float*)(record + offset);
            if (tmp > max_float) {
                max_float = tmp;
            }
        }
        tuple.add(max_float);
    }break;
    case CHARS: {
        const char* max_chars = (const char*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            const char* tmp = (const char*)(record + offset);
            if (strcmp(tmp, max_chars) > 0) {   //tmp>max_chars
                max_chars = tmp;
            }
        }
        tuple.add(max_chars, strlen(max_chars));
    }break;
    default: {
        LOG_PANIC("Unsupported field type. type=%d", myfield.type);
        return RC::GENERIC_ERROR;
    }
    }
    return RC::SUCCESS;
}
//暂时不支持DATE类型   计算table_record的[start,end)之间的 min聚合字段，并写入tuple
RC get_min_aggregation(aggr_tuplefield myfield, Tuple& tuple, int start, int stop, const vector<string>& table_record) {
    int offset = myfield.offset;
    switch (myfield.type) {
    case INTS: {
        int min_int = *(int*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            int tmp = *(int*)(record + offset);
            if (tmp < min_int) {
                min_int = tmp;
            }
        }
        tuple.add(min_int);
    }break;
    case FLOATS: {
        float min_float = *(float*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            float tmp = *(float*)(record + offset);
            if (tmp < min_float) {
                min_float = tmp;
            }
        }
        tuple.add(min_float);
    }break;
    case CHARS: {
        const char* min_chars = (const char*)(table_record[start].c_str() + offset);
        for (int i = start + 1; i < stop; i++) {
            const char* record = table_record[i].c_str();
            const char* tmp = (const char*)(record + offset);
            if (strcmp(tmp, min_chars) < 0) {   //tmp<max_chars
                min_chars = tmp;
            }
        }
        tuple.add(min_chars, strlen(min_chars));
    }break;
    default: {
        LOG_PANIC("Unsupported field type. type=%d", myfield.type);
        return RC::GENERIC_ERROR;
    }
    }
    return RC::SUCCESS;
}
//不支持CHARS和DATE    计算table_record的[start,end)之间的 avg聚合字段，并写入tuple
RC get_avg_aggregation(aggr_tuplefield myfield, Tuple& tuple, int start, int stop, const vector<string>& table_record) {
    int offset = myfield.offset;
    int group_num = stop - start;
    switch (myfield.type) {
    case INTS: {
        int total_int = 0;
        for (int i = start; i < stop; i++) {
            const char* record = table_record[i].c_str();
            int tmp = *(int*)(record + offset);
            total_int += tmp;
        }
        float avg_int = 1.0 * total_int / group_num;
        tuple.add(avg_int);
    }break;
    case FLOATS: {
        float total_float = 0;
        for (int i = start; i < stop; i++) {
            const char* record = table_record[i].c_str();
            float tmp = *(float*)(record + offset);
            total_float += tmp;
        }
        tuple.add(total_float/ group_num);
    }break;
    default: {
        LOG_PANIC("Unsupported field type. type=%d", myfield.type);
        return RC::GENERIC_ERROR;
    }
    }
    return RC::SUCCESS;
}

//获取单表的所有过滤条件
RC get_condition(const Selects& selects, std::vector<DefaultConditionFilter*> &condition_filters, const char* table_name, Table* table) {
    // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
    for (size_t i = 0; i < selects.condition_num; i++) {
        const Condition& condition = selects.conditions[i];
        if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // 两边都是值
            (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
            (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) ||  // 左边是值，右边是属性名
            (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
                match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
            ) {
            DefaultConditionFilter* condition_filter = new DefaultConditionFilter();
            RC rc = condition_filter->init(*table, condition);
            if (rc != RC::SUCCESS) {
                delete condition_filter;
                for (DefaultConditionFilter*& filter : condition_filters) {
                    delete filter;
                }
                return rc;
            }
            condition_filters.push_back(condition_filter);
        }
        else {      //非法表名
            for (DefaultConditionFilter*& filter : condition_filters) {
                delete filter;
            }
            return RC::GENERIC_ERROR;
        }
    }
    return RC::SUCCESS;
}

//升序 排序
bool cmp(string a,string b){
    const char* left_str = a.c_str();
    const char* right_str = b.c_str();

    for (compDesc dsc : group_by) {
        char* left_value = (char*)(left_str + dsc.offset);
        char* right_value = (char*)(right_str + dsc.offset);
        int cmp_result = 0;
        switch (dsc.type) {
            case CHARS: {  // 字符串都是定长的，直接比较
              // 按照C字符串风格来定
                cmp_result = strcmp(left_value, right_value);
            } break;
            case INTS: {
                // 没有考虑大小端问题
                // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
                int left = *(int*)left_value;
                int right = *(int*)right_value;
                cmp_result = left - right;
            } break;
            case FLOATS: {
                float left = *(float*)left_value;
                float right = *(float*)right_value;
                cmp_result = (int)(left - right);
            } break;
            case DATES: {
                int left = *(int*)left_value;
                int right = *(int*)right_value;
                cmp_result = left - right;
            }break;
            default: {
            }
        }
        if (cmp_result < 0) {
            return true;
        }
        if (cmp_result > 0) {
            return false;
        }
        //cmp_result=0，说明当前比较字段相等，接着比较下一个字段
    }
    //全部遍历完，仍然没有返回，说明a=b
    return true;
}

//比较两个记录在group by分组下，是否是同一组
bool group_together(string a, string b) {
    const char* left_str = a.c_str();
    const char* right_str = b.c_str();

    if (group_by.empty()) {
        return true;
    }

    for (compDesc dsc : group_by) {
        char* left_value = (char*)(left_str + dsc.offset);
        char* right_value = (char*)(right_str + dsc.offset);
        int cmp_result = 0;
        switch (dsc.type) {
        case CHARS: {  // 字符串都是定长的，直接比较
          // 按照C字符串风格来定
            cmp_result = strcmp(left_value, right_value);
        } break;
        case INTS: {
            // 没有考虑大小端问题
            // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
            int left = *(int*)left_value;
            int right = *(int*)right_value;
            cmp_result = left - right;
        } break;
        case FLOATS: {
            float left = *(float*)left_value;
            float right = *(float*)right_value;
            cmp_result = (int)(left - right);
        } break;
        case DATES: {
            int left = *(int*)left_value;
            int right = *(int*)right_value;
            cmp_result = left - right;
        }break;
        default: {
        }
        }
        if (cmp_result != 0) {  //在当前字段的比较下不相等
            return false;
        }
    }
    //全部遍历完，仍然没有返回，说明a=b
    return true;
}

//生成单表查询 myschema
void get_my_schema(const char* db, TupleSchema schema, vector<aggr_tuplefield>& myschema) {
    myschema.clear();
    for (const TupleField& field : schema.fields()) {
        const char* table_name = field.table_name();
        Table* table = DefaultHandler::get_default().find_table(db, table_name);
        assert(table != nullptr);
        const TableMeta& table_meta = table->table_meta();
        aggr_tuplefield myfield;
        if (strcmp(field.field_name(), "*") == 0) {
            myfield.type = field.type();
            myfield.offset = -1;
            myfield.comp = field.aggregationop();
        }
        else {
            const FieldMeta* field_meta = table_meta.field(field.field_name());
            assert(field_meta != nullptr);
            myfield.type = field_meta->type();
            myfield.offset = field_meta->offset();
            myfield.comp = field.aggregationop();
            
        }
        myschema.push_back(myfield);
    }
}

//生成多表查询的myschema
void get_my_schema(const char* db, TupleSchema schema, vector<aggr_tuplefield>& myschema, unordered_map<string,int>table_map_offset) {
    myschema.clear();
    for (const TupleField& field : schema.fields()) {
        aggr_tuplefield myfield;
        if (strcmp(field.field_name(), "*") == 0) {
            myfield.type = field.type();
            myfield.offset = -1;
            myfield.comp = field.aggregationop();
        }
        else {
            const char* table_name = field.table_name();
            int table_offset = table_map_offset[table_name];
            Table* table = DefaultHandler::get_default().find_table(db, table_name);
            assert(table != nullptr);
            const TableMeta& table_meta = table->table_meta();

            const FieldMeta* field_meta = table_meta.field(field.field_name());
            assert(field_meta != nullptr);
            myfield.type = field_meta->type();
            myfield.offset = field_meta->offset() + table_offset;
            myfield.comp = field.aggregationop();

        }
        myschema.push_back(myfield);
    }
}


//支持聚合的单表查询
RC select_single_table(Trx* trx, const char* db, const Selects& selects, TupleSet& tuple_set){
    const char* table_name = selects.relations[0];
    Table* table = DefaultHandler::get_default().find_table(db, table_name);
    if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", table_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    const TableMeta tablemeta = table->table_meta();
    curtable_record_size = tablemeta.record_size();
    cur_table.clear();   //当前表的所有记录存在cur_table
    table->scan_record(trx, nullptr, -1, (void*)(&cur_table), select_record_reader);
    //获取group by后的字段
    vector<string>group_list;
    group_list.clear();
    group_by.clear();
    for (int i = selects.groupby_num - 1; i >= 0; i--) {
        RelAttr attr = selects.groupby_attr[i];
        if (strcmp(attr.attribute_name, "*") == 0) {   // 不允许group by *
            return RC::GENERIC_ERROR;
        }
        if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
            const FieldMeta* field_meta = tablemeta.field(attr.attribute_name);
            if (nullptr == field_meta) {    //该字段不位于表中
                LOG_WARN("No such field. %s.%s", table->name(), attr.relation_name);
                return RC::SCHEMA_FIELD_MISSING;
            }
            //
            compDesc desc;
            desc.length = field_meta->len();
            desc.offset = field_meta->offset();
            desc.type = field_meta->type();
            group_by.push_back(desc);   //用于排序
            //
            group_list.push_back(attr.attribute_name);
        }
        else {  //非法表名
            return RC::GENERIC_ERROR;
        }
    }
    //获取select后的 字段
    TupleSchema schema;
    int ordinary_attr_exist = 0, aggr_attr_exist = 0;
    
    for (int i = 0; i < selects.attr_num; i++){
        const RelAttr& attr = selects.attributes[i];
        if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
            if (0 == strcmp("*", attr.attribute_name)) {  
                if (attr.comp == NONE_AGG) {    
                    ordinary_attr_exist = 1;
                    if (!group_list.empty()) {  //存在group by时不能 select *
                        return RC::GENERIC_ERROR;
                    }
                    // 否则可以 列出这张表所有字段
                    TupleSchema::from_table(table, schema);
                }
                else {   //聚合字段 *，只能是count(*)，其余非法
                    aggr_attr_exist = 1;
                    if (attr.comp != COUNT_AGG) {
                        return RC::GENERIC_ERROR;
                    }
                    schema.add_if_not_exists(INTS, table_name, "*", COUNT_AGG);
                }  
            } else {
                if (attr.comp == NONE_AGG) {    //普通字段  必须是group by后的字段
                    ordinary_attr_exist = 1;
                    if (group_list.empty() || count(group_list.begin(), group_list.end(), attr.attribute_name)) {
                        // 该字段位于group by中 或 没有group by  则：列出这张表相关字段
                        RC rc = schema_add_field(table, attr.attribute_name, schema, NONE_AGG);
                        if (rc != RC::SUCCESS) {
                            return rc;
                        }
                    }
                    else {  //不合法
                        return RC::GENERIC_ERROR;
                    }
                }
                else {  //聚合字段 可以不是group by后的字段
                    aggr_attr_exist = 1;
                    RC rc = schema_add_field(table, attr.attribute_name, schema, attr.comp);
                    if (rc != RC::SUCCESS) {    //字段不合法
                        return rc;
                    }
                }
            }
        }
        else {   //非法表名
            return RC::GENERIC_ERROR;
        }
    }
    //普通字段和聚合字段同时存在时 必须有group by
    if (ordinary_attr_exist && aggr_attr_exist && group_list.empty()) {
        return RC::GENERIC_ERROR;
    }
    //当前表为空，提前结束
    tuple_set.clear();
    tuple_set.set_schema(schema);
    if (cur_table.empty()) {
        LOG_INFO("empty single table\n");
        return RC::SUCCESS;
    }
    // 单表 不用考虑连接问题，直接过滤
    std::vector<DefaultConditionFilter*> condition_filters;
    if (get_condition(selects, condition_filters, table_name, table) != RC::SUCCESS) {
        return RC::GENERIC_ERROR;
    }
    condition_filter->init((const ConditionFilter**)condition_filters.data(), condition_filters.size());
    vector<string>::iterator it = cur_table.begin();
    while (it != cur_table.end()) {
        Record rec;
        rec.data = (char*)((*it).c_str());
        if (condition_filter->filter(rec)) {   //false
            it++;
        }
        else {
            it = cur_table.erase(it);
        }
    }
    //由于tupleschema没有记录offset，为了聚合运算，使用自定义的schema
    vector<aggr_tuplefield>myschema;
    get_my_schema(db, schema, myschema);

    //分类讨论
    if (group_by.empty()) {         //没有group by
        //普通字段和聚合字段同时存在时 必须有group by
        if (ordinary_attr_exist && aggr_attr_exist) {
            return RC::GENERIC_ERROR;
        }
        if (ordinary_attr_exist) {  //只有普通字段
            TupleRecordConverter converter(table, tuple_set);
            for (string record : cur_table) {
                converter.add_record(record.c_str());
            }
        }
        else if (aggr_attr_exist) {     //只有聚合字段
            int num = cur_table.size();
            Tuple tuple;
            for (aggr_tuplefield myfield : myschema) {
                switch (myfield.comp) {
                    case COUNT_AGG: {
                        tuple.add(num);
                    }break;
                    case MAX_AGG: {
                        if (get_max_aggregation(myfield, tuple, 0, num, cur_table) != RC::SUCCESS) {
                            return RC::GENERIC_ERROR;
                        }
                    }break;
                    case MIN_AGG: {
                        if (get_min_aggregation(myfield, tuple, 0, num, cur_table) != RC::SUCCESS) {
                            return RC::GENERIC_ERROR;
                        }
                    }break;
                    case AVG_AGG: {
                        if (get_avg_aggregation(myfield, tuple, 0, num, cur_table) != RC::SUCCESS) {
                            return RC::GENERIC_ERROR;
                        }
                    }break;
                    default: {
                        LOG_INFO("an unexpected error\n");
                        return RC::GENERIC_ERROR;
                    }
                }
            }
            tuple_set.add(std::move(tuple));
        }
    }
    else {
        sort(cur_table.begin(), cur_table.end(), cmp);          //先排序
        int count_num = 0, start = 0, stop = 0;
        for (size_t i = 1; i < cur_table.size() + 1; i++) {
            if (i == cur_table.size() || !group_together(cur_table[i - 1], cur_table[i])) {    //上一组记录为[start,stop)
                stop = i;
                Tuple tuple;
                for (aggr_tuplefield myfield : myschema) {
                    if (myfield.comp == NONE_AGG) {         //普通字段
                        const char* record = cur_table[start].c_str();
                        switch (myfield.type) {
                        case INTS: {
                            int value = *(int*)(record + myfield.offset);
                            tuple.add(value);
                        }break;
                        case FLOATS: {
                            float value = *(float*)(record + myfield.offset);
                            tuple.add(value);
                        }break;
                        case CHARS: {
                            const char* s = record + myfield.offset;  // 现在当做Cstring来处理
                            tuple.add(s, strlen(s));
                        }break;
                        case DATES: {
                            int value = *(int*)(record + myfield.offset);
                            tuple.addDate(value);
                        }break;
                        default: {
                            LOG_PANIC("Unsupported field type. type=%d", myfield.type);
                            return RC::GENERIC_ERROR;
                        }
                        }
                    }
                    else {  //聚合字段
                        switch (myfield.comp) {
                        case COUNT_AGG: {
                            tuple.add(stop - start);
                        }break;
                        case MAX_AGG: {
                            if (get_max_aggregation(myfield, tuple, start, stop, cur_table) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }break;
                        case MIN_AGG: {
                            if (get_min_aggregation(myfield, tuple, start, stop, cur_table) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }break;
                        default: {
                            if (get_avg_aggregation(myfield, tuple, start, stop, cur_table) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }
                        }
                    }
                }
                tuple_set.add(std::move(tuple));
                start = i;
            }
        }
    }
    return RC::SUCCESS;
   
}

// 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node) {
  // 列出跟这张表关联的Attr
  TupleSchema schema;
  Table * table = DefaultHandler::get_default().find_table(db, table_name);

  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  for (int i = selects.attr_num - 1; i >= 0; i--) {
    const RelAttr &attr = selects.attributes[i];
    if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
      if (0 == strcmp("*", attr.attribute_name)) {
        // 列出这张表所有字段
        TupleSchema::from_table(table, schema);
        break; // 没有校验，给出* 之后，再写字段的错误
      } else {
        // 列出这张表相关字段
        RC rc = schema_add_field(table, attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {

          return rc;
        }
      }
    }
    else {   //非法表名
        return RC::GENERIC_ERROR;
    }
  }
  // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
  std::vector<DefaultConditionFilter *> condition_filters;
  if (get_condition(selects, condition_filters, table_name, table) != RC::SUCCESS) {
      return RC::GENERIC_ERROR;
  }
  
  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}

RC select_tables(Trx* trx, const char* db, const Selects& selects, TupleSet& tuple_set) {
    //多表查询，获得每个表的全部内容
    int none_flag = 0;
    for (int i = selects.relation_num - 1, offset = 0; i >= 0; i--)
    {
        const char* table_name = selects.relations[i];
        relation_map_order.push_back(table_name);

        Table* table = DefaultHandler::get_default().find_table(db, table_name);
        if (nullptr == table) {
            LOG_WARN("No such table [%s] in db [%s]", table_name, db);
            return RC::SCHEMA_TABLE_NOT_EXIST;
        }
        const TableMeta tablemeta = table->table_meta();

        relation_map_offset[table_name] = offset;
        curtable_record_size = tablemeta.record_size();   //全局变量，当前表的一条记录的长度
        offset += tablemeta.record_size();  //累加offset

        cur_table.clear();   //当前表的所有记录存在cur_table
        table->scan_record(trx, nullptr, -1, (void*)(&cur_table), select_record_reader);
        if (cur_table.empty()) {  //如果存在一个表为空,多表查询的结果一定为空，所以只需获取tupleschema，即可结束
            none_flag = 1;
        }
        tables_value.push_back(cur_table);  //tables_value存放所有表的记录
        cur_table.clear();
    }

    //获取group by后的字段
    group_by.clear();
    vector<group_attr>group_list;
    group_list.clear();
    for (int i = selects.groupby_num - 1; i >= 0; i--) {
        RelAttr attr = selects.groupby_attr[i];
        if (strcmp(attr.attribute_name, "*") == 0) {   // 不允许group by *
            return RC::GENERIC_ERROR;
        }
        if (attr.relation_name == nullptr) {    //没有表名.修饰
            return RC::GENERIC_ERROR;
        }
        if (match_table(selects, attr.relation_name) == false) {  //非法表名
            return RC::GENERIC_ERROR;
        }
        Table* table = DefaultHandler::get_default().find_table(db, attr.relation_name);
        const FieldMeta* field_meta = table->table_meta().field(attr.attribute_name);
        if (nullptr == field_meta) {    //非法列名
            LOG_WARN("No such field. %s.%s", table->name(), attr.attribute_name);
            return RC::SCHEMA_FIELD_MISSING;
        }
        compDesc desc;
        desc.length = field_meta->len();
        desc.offset = field_meta->offset() + relation_map_offset[attr.relation_name];
        desc.type = field_meta->type();
        group_by.push_back(desc);   //用于排序

        group_attr tmp;
        tmp.table_name = attr.relation_name;
        tmp.attr_name = attr.attribute_name;
        group_list.push_back(tmp);
    }

    //获取select后的字段
    TupleSchema schema;
    int ordinary_attr_exist = 0, aggr_attr_exist = 0;
    for (int i = 0; i < selects.attr_num; i++) {
        const RelAttr& attr = selects.attributes[i];

        if (match_table(selects, attr.relation_name)) {     //有表名.修饰的合法表名
            if (0 == strcmp("*", attr.attribute_name)) {
                if (attr.comp == NONE_AGG) {
                    ordinary_attr_exist = 1;
                    if (!group_list.empty()) {  //存在group by时不能 select *
                        return RC::GENERIC_ERROR;
                    }
                    // 否则可以 列出这张表所有字段
                    Table* table = DefaultHandler::get_default().find_table(db, attr.relation_name);
                    assert(table != nullptr);
                    TupleSchema::from_table(table, schema);
                }
                else {   //聚合字段 *，只能是count(*)，其余非法
                    aggr_attr_exist = 1;
                    if (attr.comp != COUNT_AGG) {
                        return RC::GENERIC_ERROR;
                    }
                    schema.add_if_not_exists(INTS, attr.relation_name, "*", COUNT_AGG);
                }
            }
            else {
                if (attr.comp == NONE_AGG) {    //普通字段  必须是group by后的字段
                    ordinary_attr_exist = 1;
                    int flag = 0;
                    for (group_attr tmp : group_list) {
                        if (tmp.attr_name == attr.attribute_name && tmp.table_name == attr.relation_name) {
                            flag = 1;
                        }
                    }
                    if (group_list.empty() || flag) {
                        // 该字段位于group by中 或 没有group by  则：列出这张表相关字段
                        Table* table = DefaultHandler::get_default().find_table(db, attr.relation_name);
                        assert(table != nullptr);
                        RC rc = schema_add_field(table, attr.attribute_name, schema, NONE_AGG);
                        if (rc != RC::SUCCESS) {    //非法列名
                            return rc;
                        }
                    }
                    else {  //不合法
                        return RC::GENERIC_ERROR;
                    }
                }
                else {  //聚合字段 可以不是group by后的字段
                    aggr_attr_exist = 1;
                    Table* table = DefaultHandler::get_default().find_table(db, attr.relation_name);
                    assert(table != nullptr);
                    RC rc = schema_add_field(table, attr.attribute_name, schema, attr.comp);
                    if (rc != RC::SUCCESS) {    //字段不合法
                        return rc;
                    }
                }
            }
        }
        else if (attr.relation_name == nullptr) {   //没有表名.修饰
            if (attr.comp == NONE_AGG) {        //普通字段
                if (strcmp("*", attr.attribute_name) != 0) {   //多表查询 不是*，必须有表名.修饰
                    return RC::GENERIC_ERROR;
                }
                if (!group_list.empty()) {    //存在group by时不能 select *
                    return RC::GENERIC_ERROR;
                }
                //*：投影全部字段
                ordinary_attr_exist = 1;
                for (vector<string>::iterator it = relation_map_order.begin(); it < relation_map_order.end(); it++) {
                    Table* table = DefaultHandler::get_default().find_table(db, (*it).c_str());
                    TupleSchema::from_table(table, schema);
                }
            }
            else {      //只可能是count(*)
                aggr_attr_exist = 1;
                if (attr.comp != COUNT_AGG) {
                    return RC::GENERIC_ERROR;
                }
                schema.add_if_not_exists(INTS, "", "*", COUNT_AGG);
            }
        }
        else {   //非法表名
            return RC::GENERIC_ERROR;
        }

    }
    //普通字段和聚合字段同时存在时 必须有group by
    if (ordinary_attr_exist && aggr_attr_exist && group_list.empty()) {
        return RC::GENERIC_ERROR;
    }
    //当前表为空，提前结束
    tuple_set.clear();
    tuple_set.set_schema(schema);
    if (none_flag) {
        LOG_INFO("exit in advance\n");
        return RC::SUCCESS;
    }
    
    //边过滤边连接
    if (run(db, selects, tables_value, Cartesian_result, 0, "") != RC::SUCCESS) {   //获取的连接结果存在Cartesian_result，run函数按理不会执行失败
        LOG_INFO("Failed to excute run() function!\n");
        return RC::GENERIC_ERROR;
    }
    
    //由于tupleschema没有记录offset，为了聚合运算，使用自定义的schema
    vector<aggr_tuplefield>myschema;
    get_my_schema(db, schema, myschema, relation_map_offset);


    //分类讨论
    if (group_by.empty()) {         //没有group by
        //普通字段和聚合字段同时存在时 必须有group by
        if (ordinary_attr_exist && aggr_attr_exist) {
            return RC::GENERIC_ERROR;
        }
        if (ordinary_attr_exist) {  //只有普通字段
            TupleRecordConverter converter(nullptr, tuple_set);
            for (string record : Cartesian_result) {
                converter.add_record(record.c_str(), db, relation_map_offset);
            }
        }
        else if (aggr_attr_exist) {     //只有聚合字段
            int num = Cartesian_result.size();
            Tuple tuple;
            for (aggr_tuplefield myfield : myschema) {
                switch (myfield.comp) {
                case COUNT_AGG: {
                    tuple.add(num);
                }break;
                case MAX_AGG: {
                    if (get_max_aggregation(myfield, tuple, 0, num, Cartesian_result) != RC::SUCCESS) {
                        return RC::GENERIC_ERROR;
                    }
                }break;
                case MIN_AGG: {
                    if (get_min_aggregation(myfield, tuple, 0, num, Cartesian_result) != RC::SUCCESS) {
                        return RC::GENERIC_ERROR;
                    }
                }break;
                case AVG_AGG: {
                    if (get_avg_aggregation(myfield, tuple, 0, num, Cartesian_result) != RC::SUCCESS) {
                        return RC::GENERIC_ERROR;
                    }
                }break;
                default: {
                    LOG_INFO("an unexpected error\n");
                    return RC::GENERIC_ERROR;
                }
                }
            }
            tuple_set.add(std::move(tuple));
        }
    }
    else {
        sort(Cartesian_result.begin(), Cartesian_result.end(), cmp);          //先排序
        int start = 0, stop = 0;
        for (size_t i = 1; i < Cartesian_result.size() + 1; i++) {
            if (i == Cartesian_result.size() || !group_together(Cartesian_result[i - 1], Cartesian_result[i])) {    //上一组记录为[start,stop)
                stop = i;
                Tuple tuple;
                for (aggr_tuplefield myfield : myschema) {
                    if (myfield.comp == NONE_AGG) {         //普通字段
                        const char* record = Cartesian_result[start].c_str();
                        switch (myfield.type) {
                        case INTS: {
                            int value = *(int*)(record + myfield.offset);
                            tuple.add(value);
                        }break;
                        case FLOATS: {
                            float value = *(float*)(record + myfield.offset);
                            tuple.add(value);
                        }break;
                        case CHARS: {
                            const char* s = record + myfield.offset;  // 现在当做Cstring来处理
                            tuple.add(s, strlen(s));
                        }break;
                        case DATES: {
                            int value = *(int*)(record + myfield.offset);
                            tuple.addDate(value);
                        }break;
                        default: {
                            LOG_PANIC("Unsupported field type. type=%d", myfield.type);
                            return RC::GENERIC_ERROR;
                        }
                        }
                    }
                    else {  //聚合字段
                        switch (myfield.comp) {
                        case COUNT_AGG: {
                            tuple.add(stop - start);
                        }break;
                        case MAX_AGG: {
                            if (get_max_aggregation(myfield, tuple, start, stop, Cartesian_result) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }break;
                        case MIN_AGG: {
                            if (get_min_aggregation(myfield, tuple, start, stop, Cartesian_result) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }break;
                        default: {
                            if (get_avg_aggregation(myfield, tuple, start, stop, Cartesian_result) != RC::SUCCESS) {
                                return RC::GENERIC_ERROR;
                            }
                        }
                        }
                    }
                }
                tuple_set.add(std::move(tuple));
                start = i;
            }
        }
    }

    return RC::SUCCESS;
}
