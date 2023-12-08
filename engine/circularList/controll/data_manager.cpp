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

 namespace circular_list{
 /**
 * 对于行的操作
 */
 //计算行的大小
     size_t Row::estimateRowSize() const
     {
         size_t row_size= sizeof(Meta)+sizeof(Timestamp);
         for(const Fields &field:fields){
             row_size+=sizeof(field);
         }
         return row_size;
     }
     //将数据加入到行里
     void Row::addField(DATA_TYPE type, const char *value)
     {
         Fields field;
         field.type=type;
         switch (type) {
             case DATA_TYPE::INTEGER:
                 field.intValue= stoi(value);
         }
         fields.push_back(field);
     }
/**
 * 对页的操作
 * @param new_row
 * @return
 */
    //向页中插入行数据
    //Page::Page():m_page_head(nullptr),m_page_tail(nullptr) {}
    bool Page::add_row(const Row& new_row){
        Row row=new_row;
        m_rows.push_back(row);
        return true;
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

    }
 }

