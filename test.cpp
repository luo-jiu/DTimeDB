//#include <iostream>
#include <vector>
#include "engine/circularList/include/catalog/column.h"
#include "engine/circularList/include/catalog/schema.h"
#include "engine/circularList/include/catalog/table_generator.h"
//
//#include <engine/tsm/skip_list.h>
//using namespace dt::tsm;
//
using namespace ctl;
int main() {
    ctl::Column col1("name",ctl::VARCHAR,10);
    ctl::Column col2("age",ctl::INTEGER,5);
    std::vector<Column> columns={col1,col2};
    Schema schema(columns);
//    TableGenerator tableGenerator("mytable",schema);
    std::cout<<schema.ToString()<<std::endl;
    const std::vector<Column> &schemacol=schema.GetColumns();
    for(const auto& col:schemacol){
        std::cout<<"col name: "<<col.GetName()<<", Type: "<<col.GetType()<<std::endl;
    }
    Field intfield(INTEGER,5);
    std::cout<<intfield<<std::endl;
    Field varcharField(VARCHAR,"hello");
    std::cout<<varcharField<<std::endl;
    return 0;
}
