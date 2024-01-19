//
// Created by illumwang on 23-12-26.
//
#pragma once
#include <string>
#include "engine/circularList/include/type/type_id.h"
#include "exception/exception.h"
#include "exception"
/**
 * column构成整个表的结构，即schema
 */
namespace ctl{
    class Column{
        friend class Schema;
    public:
       Column(std::string column_name,TypeId type,uint32_t length)
       :column_name_(std::move(column_name)), column_type_(type),column_size_(TypeSize(type)){}
       /**@return  列的名称*/
        std::string GetName() const{return column_name_;}
        /** @return column's offset in the tuple */
        uint32_t GetOffset() const { return column_offset_; }
        /**@return column's name*/
        TypeId GetType() const{return  column_type_;}
        uint32_t GetSize()const{return column_size_;}
        /** @return a string representation of this column */
        std::string ToString() const;
    private:
        static uint8_t TypeSize(TypeId type){
            switch (type) {
                case TypeId::BOOLEAN:
                    return 1;
                case TypeId::INTEGER:
                    return 4;
                case TypeId::VARCHAR:
                    return 12;
                case TypeId::TIMESTAMP:
                    return 8;
                default:
                    return -1;
            }

        }
        /** Column name. */
        std::string                                      column_name_;
        /** Column value's type. */
        TypeId                                          column_type_;
        /** Column offset in the tuple. */
        uint32_t                                        column_offset_{0};
        uint32_t                                        column_size_;
    };
}
