//
// Created by illumwang on 24-1-12.
//

#include <cstdint>
#include <string>
#include <sstream>
#include "config.h"

namespace ctl{
    class RID{
    public:
        RID()=default;
        RID(page_id_t page_id):page_id_(page_id){}

        int64_t GetPageId()const{return page_id_;}
        void Set(page_id_t page_id){
            page_id_=page_id;
        }
        std::string ToString(){
            std::stringstream  os;
            os<<"page_id:"<<page_id_;
            return os.str();
        }
    private:
        page_id_t  page_id_{-1};
    };
}
