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
     //计算行的大小
     size_t Row::estimateRowSize() const {
         size_t row_size = sizeof(Meta) + sizeof(Timestamp);
         for (const Fields &field: fields) {
             row_size += sizeof(field);
         }
         return row_size;
     }

     //将数据加入到行里
     void Row::addField(DATA_TYPE type, const char *value) {
         Fields field;
         field.type = type;
         switch (type) {
             case DATA_TYPE::INTEGER:
                 field.intValue = stoi(value);
                 break;
             case DATA_TYPE::DOUBLE:
                 field.doubleValue = stod(value);
                 break;
             case DATA_TYPE::STRING:
//                 field.stringValue=new char[strlen(value)+1];
//                 strcpy(field.stringValue,value);
         }
         fields.push_back(field);
     }
     //计算每行的数据大小
     void Row::calculate_row_size(circular_list::Row &row) {

     }
     //
 }