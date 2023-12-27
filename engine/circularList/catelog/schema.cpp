//
// Created by illumwang on 23-12-28.
//
#include <sstream>
#include "engine/circularList/include/catalog/schema.h"
namespace ctl{
    Schema::Schema(const std::vector<Column> &columns) {
        uint32_t cur_offset=0;
        for (uint32_t index = 0; index < columns.size(); ++index) {
            Column column=columns[index];
            column.column_offset_=cur_offset;
            cur_offset+=column.GetSize();
            //add column
            this->columns_.push_back(column);
        }
        length_=cur_offset;
    }

    std::string Schema::ToString() const {
        std::ostringstream              os;
        os<<"Schema["
        <<"NumColumns:"<<","
        <<"Length:"<<length_<<"]";
        return os.str();
    }
}