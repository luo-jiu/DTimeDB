////
//// Created by illumwang on 12/2/23.
////
//
//#ifndef DTIMEDB_CIRCULARLIST_H
//#define DTIMEDB_CIRCULARLIST_H
//
//#include <cstring>
//#include <unordered_map>
//#include "data_def.h"
//#include "vector"
//
//namespace CircularList{
//    const int TAG=0;
//    using namespace std;
//    typedef struct pageOffset{
//        char path[16]={};
//        std::streampos offset;
//        pageOffset():offset(0) {
//            std::memset(path, 0, sizeof(path));
//        }
//    }OFFSET;
//    struct Meta{
//        char * pageName;
//        char * rowName;
//        Meta(const char* page, const char* row):pageName(strdup(page)),rowName(strdup(row)){}
//    };
//    struct Tuple{
//        Meta meta;
//        char * timestamp;
//        char * value;
//        enum DATA_TYPE datatype;
//        struct Tuple * nextTuple;
//    };
//
//    struct PageHead{
//        char * blockName;
//        char * pageName;
//        bool isStored;
//        OFFSET offset;     //waiting for computing
//        PageHead * nextPage;
//    };
//    class Page{
//    private:
//        static  const int PAGE_SIZE=4096;
//        vector<Tuple> tuples;
//    public:
//        bool isFull(){
//            return tuples.size()>=PAGE_SIZE;
//        }
//        void insertData(const Tuple& tuple){
//
//        }
//    };
//
////设计环形链表组织数据结构
//    struct CircularListNode{
//    private:
//        Page page;
//        CircularListNode* next;
//    public:
//
//    };
//    class CircularList{
//    private:
//        CircularListNode* sentienl;
//    };
//    struct BlockHead{
//        string filePath;
//        string fileName;
//    };
//    class Block{
//        BlockHead blockHead;
//        Page * page;
//    private:
//    };
//}
//
//#endif //DTIMEDB_CIRCULARLIST_H
