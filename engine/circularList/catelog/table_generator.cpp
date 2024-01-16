
#include "engine/circularList/include/catalog/table_generator.h"
#include "engine/circularList/include/common/rid.h"

namespace ctl{
//    std::vector<Field> TableGenerator::MakeValues(ctl::TableGenerator::ColumnInsertMeta *col_meta, uint32_t count) {
//        std::vector<Field> fields;
//        switch (col_meta->type_) {
//            case INTEGER:
//
//        }
//    }
    void TableGenerator::FillTable(ctl::TableInfo *info, ctl::TableGenerator::TableInsertMeta *table_meta) {
        uint32_t  num_inserted=0;
        uint32_t batch_size=128;
        /**
         * 当表内数据小于容量时
         */
        while (num_inserted<table_meta->num_rows_){
            std::vector<std::vector<Field>> fields;
            uint32_t num_fields=std::min(batch_size,table_meta->num_rows_-num_inserted);
            for (auto &col_meta:table_meta->col_meta_) {
               // fields.emplace_back(MakeValues(&col_meta,num_fields));
            }
            for (uint32_t i=0;i<num_fields;i++){
                std::vector<Field> entry;
                entry.reserve(fields.size());
            }
            RID rid;
            bool inserted=info->table_;

        }
    }


}