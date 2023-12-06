#include <mutex>
//
// Created by illumwang on 23-11-23.
//

/**
 *实现页的操作
 */

#include <engine/circularList/include/circular_list_struct.h>
using namespace std;
using namespace circular_list;
/**
 * 对页的操作
 * @param new_row
 * @return
 */
 //向页中插入行数据
bool Page::insert_row(const Row& new_row){
    Row insert_row=new_row;
        m_rows.push_back(insert_row);
        return true;
}
//从页中删除行数据


//一页中存储行数据的大小

