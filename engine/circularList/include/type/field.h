//
// Created by illumwang on 23-12-24.
//
#pragma once
#include <cstdint>
#include <string>
#include "type_id.h"
#include "type.h"
#include "timestamp.h"
namespace ctl{
    //行值的抽象类，存储不同类型的数据
    class Field{
        friend class Type;
        friend class IntegerType;
        friend class TimestampType;
    public:
        ~Field();
        //构造函数，设置数值的生命周期管理
        explicit Field(const TypeId type): manage_data_(false), type_id_(type){ size_.len_=UINT_MAX; }
        //布尔值类型的构造函数
        Field(TypeId type, int8_t i);
//        浮点类型构造函数
        Field(TypeId type, double i);
        //整型构造函数
        Field(TypeId type, int32_t i);
//        时间戳类型
        Field(TypeId type, uint64_t i);
//        varchar
        Field(TypeId type,const char *data,uint32_t len,bool manage_data);
        Field(TypeId type,const std::string &data);
        //默认类型
        Field(): Field(TypeId::INVALID){}

        Field(const Field &value);
        //获取值的具体类型
        template<class T>
        inline T GetAs()const{
            return *reinterpret_cast<const T *>(&value_);
        }
        inline TypeId GetTypeId() const  {return type_id_;}
        //获取varchar数据的长度
        inline uint32_t GetLength() const { return Type::GetInstance(type_id_)->GetLength(*this); }
        //获取varchar数据的值
        inline const char *GetData() const { return Type::GetInstance(type_id_)->GetData(*this); }
        inline std::string ToString() const{return Type::GetInstance(type_id_)->ToString(*this);}

    protected:
        /**
         * value_ 的不同成员用于表示不同类型的数据。
         * 当 Value 实例表示可变长度数据时，会使用 varlen_ 来存储实际的数据。
         */
        union Val{
        int8_t            boolean_;
        int32_t          integer_;
        double           decimal_;
        uint64_t         timestamp_;
        /**
         * 存储可变长度字符串，
         * 它是一个指向字符数组的指针，用于存储字符串数据或其他可变长度的数据。
         */
        char                            *varlen_;
        /**
         * 存储不可变长字符串
         *
         */
        const char                   *const_varlen_;
        } value_;
        // 用于存储长度信息或元素类型信息的联合体
        union {
            uint32_t len_;
            TypeId elem_type_id_;
        } size_;
        TypeId                         type_id_;
        /**
         * 是否需要拷贝的标志
         * 当 manage_data_ 为 true 时，表示 Value 对象拥有并管理其内部数据的内存，负责释放这块内存。
         * 而当 manage_data_ 为 false 时，表示 Value 对象不负责管理内部数据的内存，通常是通过外部手段（例如，其他对象或模块）管理
         */
        bool                             manage_data_;
    };
}

