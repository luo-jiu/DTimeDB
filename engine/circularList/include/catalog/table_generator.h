//
// Created by illumwang on 23-12-28.
//
//
#include <cstdint>
#include <vector>
#include "engine/circularList/include/type/type_id.h"
#include "engine/circularList/include/type/field.h"

namespace ctl{
    class TableGenerator{
    private:
        struct ColumnInsertMeta{
            const char      *name_;
            const TypeId    type_;
            uint64_t           min_;
            uint64_t           max_;
            /**每一列列值计数器**/
            uint64_t           serial_counter_{0};
            ColumnInsertMeta(const char *name,const TypeId type,uint64_t min,uint64_t max)
            :name_(name),type_(type),min_(min),max_(max){}
            uint32_t GeneSerialValue(){
                return serial_counter_++;
            }
        };
        /**
         * 表的元数据
         */
         struct TableInsertMeta{
             const char                                                 *name_;
             /**num of rows**/
             uint32_t                                                     num_rows_;
             /**
              * num of columns
              */
             std::vector<ColumnInsertMeta>                   col_meta_;
             TableInsertMeta(const char *name,uint32_t num_rows,std::vector<ColumnInsertMeta>  cols)
             :name_(name),num_rows_(num_rows),col_meta_(std::move(cols)){}
         };
         void FillTable();
         std::vector<Field> MakeValues(ColumnInsertMeta *col_meta, uint32_t count);

    };
}
