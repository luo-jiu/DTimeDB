#include <mutex>
//
// Created by illumwang on 23-11-23.
//

/**
 *实现页的操作
 */

#include <engine/circularList/include/circular_list_header.h>
using namespace std;
using namespace circular_list;

 namespace circular_list {
     /**
     * 对于行的操作
     */
     //将数据加入到行里
     /**
      * 每添加一个field就调用一次这个函数
      * @param type
      * @param value
      */
     void Row::add_field(DATA_TYPE type, const char *value) {
         values.push_back(value);
     }
     //计算每行的数据大小
     /**
      * 有可能会有内存对齐的问题，测试以下看看行不行
      * @return 每行的数据大小
      */
     size_t Row::calculate_row_size()const {
         size_t size=sizeof(meta)+sizeof(timestamp)+sizeof(next_row);
         for (const auto& value:values) {
             size+= value.size();
         }
         return size;
     }
     //
 }