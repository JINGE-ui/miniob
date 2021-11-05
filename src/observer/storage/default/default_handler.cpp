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


#include "storage/default/default_handler.h"

#include <string>

#include "common/os/path.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/record_manager.h"
#include "storage/common/bplus_tree.h"
#include "storage/common/table.h"
#include "storage/common/condition_filter.h"

DefaultHandler &DefaultHandler::get_default() {
  static DefaultHandler handler;
  return handler;
}

DefaultHandler::DefaultHandler() {
}

DefaultHandler::~DefaultHandler() noexcept {
  destroy();
}

RC DefaultHandler::init(const char *base_dir) {
  // 检查目录是否存在，或者创建
  std::string tmp(base_dir);
  tmp += "/db";
  if (!common::check_directory(tmp)) {
    LOG_ERROR("Cannot access base dir: %s. msg=%d:%s", tmp.c_str(), errno, strerror(errno));
    return RC::GENERIC_ERROR;
  }

  base_dir_ = base_dir;
  db_dir_ = tmp + "/";

  LOG_INFO("Default handler init with %s success", base_dir);
  return RC::SUCCESS;
}

void DefaultHandler::destroy() {
  sync();

  for (const auto & iter : opened_dbs_) {
    delete iter.second;
  }
  opened_dbs_.clear();
}

RC DefaultHandler::create_db(const char *dbname) {
  if (nullptr == dbname || common::is_blank(dbname)) {
    LOG_WARN("Invalid db name");
    return RC::INVALID_ARGUMENT;
  }

  // 如果对应名录已经存在，返回错误
  std::string dbpath = db_dir_ + dbname;
  if (common::is_directory(dbpath.c_str())) {
    LOG_WARN("Db already exists: %s", dbname);
    return RC::SCHEMA_DB_EXIST;
  }

  if (!common::check_directory(dbpath)) {
    LOG_ERROR("Create db fail: %s", dbpath.c_str());
    return RC::GENERIC_ERROR; // io error
  }
  return RC::SUCCESS;
}

RC DefaultHandler::drop_db(const char *dbname) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::open_db(const char *dbname) {
  if (nullptr == dbname || common::is_blank(dbname)) {
    LOG_WARN("Invalid db name");
    return RC::INVALID_ARGUMENT;
  }

  if (opened_dbs_.find(dbname) != opened_dbs_.end()) {
    return RC::SUCCESS;
  }

  std::string dbpath = db_dir_ + dbname;
  if (!common::is_directory(dbpath.c_str())) {
    return RC::SCHEMA_DB_NOT_EXIST;
  }

  // open db
  Db *db = new Db();
  RC ret = RC::SUCCESS;
  if ((ret = db->init(dbname, dbpath.c_str())) != RC::SUCCESS) {
    LOG_ERROR("Failed to open db: %s. error=%d", dbname, ret);
  }
  opened_dbs_[dbname] = db;
  return RC::SUCCESS;
}

RC DefaultHandler::close_db(const char *dbname) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::execute(const char *sql) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::create_table(const char *dbname, const char *relation_name, int attribute_count, const AttrInfo *attributes) {
  Db *db = find_db(dbname);
  if (db == nullptr) {
    return RC::SCHEMA_DB_NOT_OPENED;
  }
  return db->create_table(relation_name, attribute_count, attributes);
}

RC DefaultHandler::drop_table(const char *dbname, const char *relation_name) {
  //by xiayuan:
  Db* db = find_db(dbname);
  if (db == nullptr) {
      return RC::SCHEMA_DB_NOT_OPENED;
  }
  Table* table = find_table(dbname, relation_name);
  if (nullptr == table) {
      return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  //浏览所有index，并删除
  TableMeta tablemeta = table->table_meta();
  for (int i = 0; i < tablemeta.index_num(); i++) {
      const IndexMeta* index_meta = tablemeta.index(i);
      std::string index_name = index_meta->name();
      if (drop_index(nullptr, dbname, relation_name, index_name.c_str()) != RC::SUCCESS) {
          LOG_ERROR("Failed to delete index: %s\n", index_name.c_str());
          return RC::GENERIC_ERROR;
      }
  }
  std::string relation_name_str = relation_name;
  //std::string data_file = base_dir_ + relation_name_str + ".data";   不对，干脆不改了
  //std::string table_file = base_dir_ + relation_name_str + ".table";
  std::string data_file = "./miniob/db/sys/" + relation_name_str + ".data";
  std::string table_file = "./miniob/db/sys/" + relation_name_str + ".table";
  if (remove(data_file.c_str()) != 0) {
      LOG_ERROR("Failed to delete .data file: %s\n", data_file.c_str());
      return RC::GENERIC_ERROR;
  }
  if (remove(table_file.c_str()) != 0) {
      LOG_ERROR("Failed to delete .table file: %s\n", table_file.c_str());
      return RC::GENERIC_ERROR;
  }
  delete table;
  /* a test
  std::fstream f1,f2;
  f1.open(data_file.c_str());
  f2.open(table_file.c_str());
  if (f1.is_open()) {
      f1.close();
      LOG_INFO("Failed to delete .data file: %s\n", data_file.c_str());
      return RC::GENERIC_ERROR;
  }
  if (f2.is_open()) {
      f2.close();
      LOG_INFO("Failed to delete .data file: %s\n", table_file.c_str());
      return RC::GENERIC_ERROR;
  }
  */
  if (db->open_all_tables() != RC::SUCCESS) {
      LOG_INFO("Failed to flush opened_tables_\n");
      return RC::GENERIC_ERROR;
  }

  ///
  return RC::SUCCESS;
}

RC DefaultHandler::create_index(Trx *trx, const char *dbname, const char *relation_name, const char *index_name, const char *attribute_name) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  return table->create_index(trx, index_name, attribute_name);
}

RC DefaultHandler::drop_index(Trx *trx, const char *dbname, const char *relation_name, const char *index_name) {
  //by xiayuan: drop index
    Table* table = find_table(dbname, relation_name);
    if (nullptr == table) {  //table不存在
        return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    TableMeta tablemeta = table->table_meta();
    if (tablemeta.index(index_name) == nullptr) {  //index不存在
        return RC::SCHEMA_INDEX_NOT_EXIST;
    }

    std::string relation_name_str = relation_name;
    std::string indexfile_name = relation_name_str + "-";
    indexfile_name += index_name;
    indexfile_name += ".index";
    //std::string index_path = base_dir_ + indexfile_name;
    std::string index_path = "./miniob/db/sys/" + indexfile_name;
    if (remove(index_path.c_str()) != 0) {  //删除失败
        LOG_ERROR("Failed to delete .index file: %s\n", index_path.c_str());
        return RC::GENERIC_ERROR;
    }

  return RC::SUCCESS;
}

RC DefaultHandler::insert_record(Trx *trx, const char *dbname, const char *relation_name, int insert_num, Tuplevalue *const tuplevalue []) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  return table->insert_record(trx, insert_num, tuplevalue);
}
RC DefaultHandler::delete_record(Trx *trx, const char *dbname, const char *relation_name,
                                 int condition_num, const Condition *conditions, int *deleted_count) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  CompositeConditionFilter condition_filter;
  RC rc = condition_filter.init(*table, conditions, condition_num);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  return table->delete_record(trx, &condition_filter, deleted_count);
}

RC DefaultHandler::update_record(Trx *trx, const char *dbname, const char *relation_name, const char *attribute_name, const Value *value,
                          int condition_num, const Condition *conditions, int *updated_count) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  return table->update_record(trx, attribute_name, value, condition_num, conditions, updated_count);
}

Db *DefaultHandler::find_db(const char *dbname) const {
  std::map<std::string, Db*>::const_iterator iter = opened_dbs_.find(dbname);
  if (iter == opened_dbs_.end()) {
    return nullptr;
  }
  return iter->second;
}

Table *DefaultHandler::find_table(const char *dbname, const char *table_name) const {
  if (dbname == nullptr || table_name == nullptr) {
    LOG_WARN("Invalid argument. dbname=%p, table_name=%p", dbname, table_name);
    return nullptr;
  }
  Db *db = find_db(dbname);
  if (nullptr == db) {
    return nullptr;
  }

  return db->find_table(table_name);
}

RC DefaultHandler::sync() {
  RC rc = RC::SUCCESS;
  for (const auto & db_pair: opened_dbs_) {
    Db *db = db_pair.second;
    rc = db->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to sync db. name=%s, rc=%d:%s", db->name(), rc, strrc(rc));
      return rc;
    }
  }
  return rc;
}