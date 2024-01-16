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
    return 0;
}
//    SkipList<int> sl;
//
//    auto time1 = std::chrono::system_clock::now();
//    auto time2 = std::chrono::system_clock::now();
//    auto time3 = std::chrono::system_clock::now();
//
//    int a = 100;
//    int b = 300;
//    int c = 400;
//
//    sl.put(time1, a);
//    sl.put(time2, b);
//    sl.put(time3, c);
//
//    int value;
//    if (sl.get(time3, value)) {
//        std::cout << "Key 1 has value " << value << std::endl;
//    }
//
//    sl.del(time2);
//    if (!sl.get(time2, value)) {
//        std::cout << "Key 1 not found" << std::endl;
//    }
//
//    return 0;
//}
