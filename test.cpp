//#include <iostream>
//
//#include <engine/tsm/skip_list.h>
//using namespace dt::tsm;
//
//int main() {
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
