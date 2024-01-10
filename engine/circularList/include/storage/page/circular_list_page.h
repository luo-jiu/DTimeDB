
#include "engine/circularList/include/storage/page/page.h"
/**
 * 节点页
 */
namespace ctl{
    class CircularListPage :public Page{
    public:
        static constexpr int PAGE_SIZE = 4096;
        static constexpr int INVALID_PAGE_ID = -1;
        using page_id_t = uint32_t;
        using lsn_t = uint32_t;

    private:
        CircularListPage                *next_page= nullptr;
        page_id_t                           page_id=INVALID_PAGE_ID;

    };
}

