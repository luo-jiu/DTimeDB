//
// Created by illumwang on 23-12-24.
//

#ifndef DTIMEDB_PAGE_H
#define DTIMEDB_PAGE_H

#include <cstring>
#include "engine/circularList/include/common/rwlatch.h"

namespace ctl{
    class Page{
        static constexpr int PAGE_SIZE = 4096;
        static constexpr int INVALID_PAGE_ID=-1;
        using page_id_t=uint32_t;
        using lsn_t = uint32_t;
    public:
        Page(){ResetMemory();}
        ~Page()=default;
        /**
         *
         * @return 页内数据
         */
        inline char  *GetData(){return data_;}
        /** @return 页id*/
        inline page_id_t  GetPageId(){return page_id_;}
        inline int GetPageCount(){return pin_count_;}
    protected:
        static_assert(sizeof(page_id_t) == 4);
        static_assert(sizeof(lsn_t) == 4);

        static constexpr size_t SIZE_PAGE_HEADER = 8;
        static constexpr size_t OFFSET_PAGE_START = 0;
        static constexpr size_t OFFSET_LSN = 4;
    private:
        inline void ResetMemory(){memset(data_,OFFSET_PAGE_START,PAGE_SIZE);}
        char                                         data_[PAGE_SIZE];
        page_id_t                                 page_id_=INVALID_PAGE_ID;
       // 引用页的线程数量，并发读写的功能管理
        int                                           pin_count_=0;
        ReaderWriterLatch                   rwlatch_;
    };
}
#endif //DTIMEDB_PAGE_H
