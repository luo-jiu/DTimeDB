//
// Created by illumwang on 12/6/23.
//
#include "iostream"
#include "engine/circularList/include/circular_list_header.h"
using namespace std;
int main(){
    Meta meta(1,"row1");
    Timestamp timestamp;
    Row row(meta,timestamp);
    row.add_field(DATA_TYPE::INTEGER,"1");
    row.add_field(DATA_TYPE::STRING,"Myclass");

    Page page(1,"example",PAGE_SIZE,PAGE_TYPE::DATA,42);
    if (page.add_row(row)){
        cout<<"successfully"<<endl;
        row.display_row();
    } else{
        cout<<"false"<<endl;
    }

    return 0;
}