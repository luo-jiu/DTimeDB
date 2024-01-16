//
// Created by illumwang on 23-12-26.
//
#pragma once
#include <vector>
#include "column.h"

namespace ctl{
    /**
     * 表的模式，定义表的结构，元数据
     */
    class Schema{
    public:
        explicit Schema(const std::vector<Column> &columns);
        std::string ToString() const;
        static Schema *CopySchema(const Schema *from,const std::vector<uint32_t> &attr){
                std::vector<Column> cols;
                cols.reserve(attr.size());
            for (const auto i:attr) {
                cols.emplace_back(from->columns_[i]);
            }
            return new Schema{cols};
        }
        const std::vector<Column> &GetColumns() const {
            return columns_;
        }
        const Column &GetColumn(const uint32_t col_id) const{
            return columns_[col_id];
        }
    private:
        uint32_t                                            length_;
        std::vector<Column>                         columns_;
    };
}
