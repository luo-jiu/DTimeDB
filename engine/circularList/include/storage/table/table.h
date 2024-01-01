//
// Created by illumwang on 23-12-26.
//

#ifndef DTIMEDB_TABLE_H
#define DTIMEDB_TABLE_H

#include <string>
#include "engine/circularList/include/storage/page/page.h"
#include "engine/circularList/include/common/config.h"
#include "engine/circularList/recovery/log_manager.h"

namespace ctl{
    class Table : public Page{
        using page_id_t = uint32_t;
    public:
        void Init(page_id_t page_id,uint32_t page_size,page_id_t next_page_id);
        void CreateTable(const std::string &table_name);
    private:
        static constexpr size_t SIZE_TUPLE=8;
    };
}
#endif //DTIMEDB_TABLE_H
