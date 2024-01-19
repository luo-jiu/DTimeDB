//
// Created by illumwang on 23-12-26.
//
#pragma once
#include <unordered_map>
#include <memory>
#include <atomic>
#include "schema.h"
#include "engine/circularList/include/storage/table/table_heap.h"

namespace ctl{
    //表的唯一标识
    using  table_oid_t=uint32_t ;
    //列的唯一标识
    using  column_oid_t =uint32_t;
    //索引的唯一标识
    using  index_oid_t=uint32_t;
    /**
   * 关于表的元数据信息
   */
    struct TableInfo{
        /**
         *
         * @param schema    表的格式
         * @param name      表的名字
         * @param id            表的id
         */
        TableInfo(Schema schema,std::string name,table_oid_t oid)
        : schema_{std::move(schema)},name_{std::move(name)},oid_(oid){}
        Schema                          schema_;
        const std::string               name_;
        const  table_oid_t            oid_;
    };
    /**
     * Information about Index
     */
     struct IndexInfo{

     };
     class Catelog{
     public:
         static constexpr TableInfo *NULL_TABLE_INFO{nullptr};
    /**
     * 返回表的元数据meta
     * @param table_name
     * @param schema
     * @return
     */
         TableInfo *CreateTable(const std::string &table_name,const Schema &schema){
             if (table_names_.count(table_name)!=0){
                 return NULL_TABLE_INFO;
             }
             const auto  table_oid=GetNextOID();
             const std::string table_filename="table_"+std::to_string(table_oid)+".ctl";
             auto table=std::make_unique<TableHeap>();
            //建 TableInfo，并传递给 tables_ 进行管理
            auto meta=std::make_unique<TableInfo>(schema,table_name,table_oid);
         }
         TableInfo *GetTable(const std::string &table_name){

         }
     private:
         table_oid_t GetNextOID(){
             auto now=std::chrono::system_clock::now();
             auto timestamp=std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
             return static_cast<table_oid_t>(timestamp);
         }
         std::unordered_map<std::string ,table_oid_t>               table_names_;
         std::atomic<table_oid_t>                                            next_table_oid_{0};
     };
}
