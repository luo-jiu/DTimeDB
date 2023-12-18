//
// Created by illumwang on 12/9/23.
//
#include "engine/circularList/include/circular_list_header.h"
/**
 * 对页的操作
 * @param new_row
 * @return
 */
//
//计算页尾校验和
//uint32_t PageTail::calculateOffest() const {
//
//}
//计算正页数据大小
size_t Page::calculate_page_size() const
{
    size_t size= sizeof(PageHead)+sizeof(PageTail);
    for(const auto &row:m_rows){
        size+= sizeof(row);
    }
    return size;
}
//向页中插入行数据
bool Page::insert_row(const Row& row)
{
    size_t totalSize=calculate_page_size();
    //如果页面积足够则添加新数据
    if (totalSize+row.calculate_row_size()<=PAGESIZE){
        m_rows.push_back(row);
        return true;
    }
    return false;
}
//从页中删除行数据
bool Page::drop_row(int *row_id)
{

}
//从页中获取行数据
//const Row& Page::getRow(size_t index){
//
//}
//输出页中的所有数据
//void Page::
//初始化页
bool Page::init_page(uint32_t pageId, const char *blockName, const char *pageName, PAGE_TYPE type)
{
    m_page_head=PageHead(pageId,pageName,PAGESIZE,type);
    m_page_tail=PageTail(0);
    m_rows.clear();
    return true;
}
//输出全页数据
vector<Row> Page::get_page_data() const
{
    return m_rows;
}



