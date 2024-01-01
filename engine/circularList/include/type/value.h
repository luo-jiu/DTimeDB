//
// Created by illumwang on 23-12-24.
//
#pragma once
#include <cstdint>
#include <string>
#include "type_id.h"
#include "type.h"

namespace ctl{
    //行值的抽象类，存储不同类型的数据
    class Value{
        friend class Type;
    public:
        //构造函数，设置数值的生命周期管理
        explicit Value(const TypeId type): manage_data_(false),type_id_(type){size_.len_=1024; }
        Value(TypeId type,int8_t i);

        inline TypeId GetTypeId() const  {return type_id_;}
        //获取varchar数据的长度
        inline uint32_t GetLength() const { return Type::GetInstance(type_id_)->GetLength(*this); }
        //获取varchar数据的值
        inline const char *GetData() const { return Type::GetInstance(type_id_)->GetData(*this); }
        inline std::string ToString() const{return Type::GetInstance(type_id_)->ToString(*this);}
    protected:
        union Val{
        int8_t            boolean_;
        int32_t          integer_;
        double           decimal_;
        uint64_t         timestamp_;
        /**
         * 存储可变长度字符串
         */
        char                            *varlen_;
        /**
         * 存储不可变长字符串
         */
        const char                   *const_varlen_;
        } value_;
        // 用于存储长度信息或元素类型信息的联合体
        union {
            uint32_t len_;
            TypeId elem_type_id_;
        } size_;
        TypeId                         type_id_;
        bool                             manage_data_;
    };
}

