//
// Created by illumwang on 12/2/23.
//

#ifndef DTIMEDB_CIRCULARLIST_H
#define DTIMEDB_CIRCULARLIST_H
#include "iostream"
#include "json/json.h"
#include <cstring>
#include <unordered_map>
#include "dataDef.h"
#include "vector"
using namespace luo::json;

namespace circularList{
        const int TAG=0;
        using namespace std;
        typedef struct pageOffset{
            char path[16]={};
            std::streampos offset;
            pageOffset():offset(0) {
                std::memset(path, 0, sizeof(path));
            }
        }OFFSET;
       class meta{
       public:
            uint32_t id;
            char * pageName;
            char * rowName;
            meta(const char* page, const char* row):pageName(strdup(page)),rowName(strdup(row)){}
       };
#define size_of_attribute(Struct,Attribute) sizeof (((Struct *)0)->Attribute)
        class Tuple{
            private:
                meta meta;
                char * timestamp;
                char * value;
                enum DATA_TYPE datatype;
                struct Tuple * nextTuple;
            public:

            Tuple(const char *timestamp, const char *value, DATA_TYPE datatype, class meta meta)
                    : timestamp(nullptr), value(nullptr), datatype(datatype), nextTuple(nullptr), meta(meta) {

            }
            ~Tuple(){

            }
        };
//
        typedef struct page_head{
            char * blockName;
            char * pageName;
            bool isStored;
            OFFSET offset;     //waiting for computing
            page_head * nextPage;
        }PageHead;
        class Page{
        private:
            int file_des;
            static  const uint32_t PAGE_SIZE=4096;
            PageHead pageHead;
            vector<Tuple> tuples;
        public:
            void *get_page(uint32_t page_num);
            void insert_tuple(const Tuple& newTuple);
            void allocate_page();

        };

    //设计环形链表组织数据结构
        struct CircularListNode{
        private:
            Page page;
            CircularListNode* next;
        public:

        };
        class CircularList{
        private:
            CircularListNode* sentienl;
        };
        struct BlockHead{
            string filePath;
            string fileName;
        };
        class Block{
        private:
            BlockHead blockHead;
            Page * page;
        public:
            Block(const char*filename){

            }
        };
}

#endif //DTIMEDB_CIRCULARLIST_H
