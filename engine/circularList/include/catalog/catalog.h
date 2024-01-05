//
// Created by illumwang on 23-12-26.
//

#include <unordered_map>
#include "schema.h"

namespace ctl{
    using  table_oid_t=uint32_t ;
    using  column_oid_t =uint32_t;
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
        TableInfo(Schema schema,std::string name,table_oid_t id)
        : schema_(schema),name_(name),table_(id){}
        Schema                          schema_;
        const std::string               name_;
        const  table_oid_t            table_;
    };
    /**
     * Information about Index
     */
     struct IndexInfo{

     };
     class Catelog{
     public:
         static constexpr TableInfo *NULL_TABLE_INFO{nullptr};

         TableInfo *CreateTable(const std::string &table_name,const Schema &schema){
             if (table_names_.count(table_name)!=0){
                 return NULL_TABLE_INFO;
             }

         }
     private:
         std::unordered_map<std::string ,table_oid_t>               table_names_;
     };
}
