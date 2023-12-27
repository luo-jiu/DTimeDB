//
// Created by illumwang on 23-12-26.
//

#ifndef DTIMEDB_CIRCULAR_LIST_PAGE_H
#define DTIMEDB_CIRCULAR_LIST_PAGE_H

#include "engine/circularList/include/storage/page/page.h"

namespace ctl{
    class CircularList{
    public:

    private:
        Page                page;
        Page*               next;
    };
}
#endif //DTIMEDB_CIRCULAR_LIST_PAGE_H
