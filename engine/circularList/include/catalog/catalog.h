//
// Created by illumwang on 23-12-26.
//

#ifndef DTIMEDB_CATALOG_H
#define DTIMEDB_CATALOG_H

#include "schema.h"

namespace ctl{
    /**
   * 关于表的元数据信息
   */
    struct TableInfo{

        TableInfo(Schema schema,std::string name){}
    };
    /**
     * Information about Index
     */
     struct IndexInfo{

     };
}
#endif //DTIMEDB_CATALOG_H
