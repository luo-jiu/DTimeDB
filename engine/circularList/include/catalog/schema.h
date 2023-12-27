//
// Created by illumwang on 23-12-26.
//

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
    private:
        uint32_t                                            length_;
        std::vector<Column>                         columns_;
    };
}
