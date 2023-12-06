//
// Created by illumwang on 12/2/23.
//
#pragma
#ifndef DTIMEDB_CIRCULARLIST_H
#define DTIMEDB_CIRCULARLIST_H
#include "circular_list_struct.h"
#include "iostream"
#include "json/json.h"
#include <cstring>
#include <unordered_map>
#include "data_def.h"
#include "vector"
using namespace luo::json;
using namespace std;
using namespace luo::json;
using namespace std;
using namespace circular_list;
namespace circularList
{
    class CircularList
    {
    public:

    };
    //所有页通用结构
    class BlockHeader{
    private:
        uint32_t m_block_size;


    };
    template<class E>
    class Block
    {
    private:
        Page* page;
    public:
        Block(const char *file_name)
        {

        }


    };
}
//namespace circularList{
//    const int TAG=0;
//        typedef struct pageOffset{
//            char path[16]={};
//            std::streampos offset;
//            pageOffset():offset(0) {
//                std::memset(path, 0, sizeof(path));
//            }
//        }OFFSET;
//       class meta{
//       public:
//            uint32_t id;
//            char * pageName;
//            char * rowName;
//            meta(const char* controll, const char* row):pageName(strdup(controll)),rowName(strdup(row)){}
//       };
//#define size_of_attribute(Struct,Attribute) sizeof (((Struct *)0)->Attribute)
//
////
//        typedef struct page_head{
//            char * blockName;
//            char * pageName;
//            bool isStored;
//            OFFSET offset;     //waiting for computing
//            page_head * nextPage;
//        }PageHead;
//        class Page{
//        private:
//            int file_des;
//            static  const uint32_t PAGE_SIZE=4096;
//            PageHead pageHead;
//            vector<Tuple> tuples;
//        public:
//            void *get_page(uint32_t page_num);
//            void insert_tuple(const Tuple& newTuple);
//            void allocate_page();
//
//        };
//
//    //设计环形链表组织数据结构
//        struct CircularListNode{
//        private:
//            Page controll;
//            CircularListNode* next;
//        public:
//
//        };
//        class CircularList{
//        private:
//            CircularListNode* sentienl;
//        };
//        struct BlockHead{
//            string filePath;
//            string fileName;
//        };
//        class Block{
//        private:
//            BlockHead blockHead;
//            Page * controll;
//        public:
//            Block(const char*filename){
//
//            }
//        };
//}

#endif //DTIMEDB_CIRCULARLIST_H
