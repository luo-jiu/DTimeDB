//
// Created by illumwang on 12/9/23.
//
#include "engine/circularList/include/circular_list_header.h"
/**
 * 对页的操作
 * @param new_row
 * @return
 */
 //计算当前页内的空间
 int Page::getSize()
 {
     int size=0;
     for (const auto& row:m_rows) {
         size+=sizeof (int)+m_rows.size();
     }
     return size;
 }
//向页中插入行数据
bool Page::add_row(const Row& new_row)
{
    if (getSize()+sizeof(Row)<=PAGE_SIZE)
    {
        m_rows.push_back(new_row);
        return true;
    }
    return false;
}
//从页中删除行数据
bool Page::drop_row(int *row_id) {

}

const Row& Page::getRow(size_t index){

}
bool Page::init_page(uint32_t pageId, const char *blockName, const char *pageName, Page_TYPE type){
    //delete m_page_head;
    delete m_page_tail;
}
bool Page::createPage() {
    return false;
}


