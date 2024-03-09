//
// Created by illumwang on 24-2-26.
//
#include <list>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <pthread.h>
#include <ext/hash_map>
#ifndef DTIMEDB_IO_BUFFER_H
#define DTIMEDB_IO_BUFFER_H
#include "cassert"
#include <cstring>
// 定义一个输入/输出缓冲区结构体
struct io_buffer{
    io_buffer(int size)
    :capacity(size),length(0),head(0),next(nullptr)
    {
        // 分配缓冲区的数据存储空间，并进行断言检查
        data = new char [size];
        assert(data);
    }
    // 清空缓冲区
    void clear(){
        length = head = 0;
    }
    // 调整数据位置，将数据移动到缓冲区的开头
    void adjust(){
        if (head) {
            if (length) {
                ::memmove(data, data + head, length);
            }
            head = 0;
        }
    }
    // 复制另一个缓冲区的数据到本缓冲区
    void copy(const io_buffer* other){
        // 仅复制数据到自身缓冲区
        ::memcpy(data, other->data + other->head, other->length);
        head = 0;
        length = other->length;
    }
    // 弹出指定长度的数据
    void pop(int len) {
        length -= len;
        head += len;
    }
    // 缓冲区容量
    int capacity;
    // 缓冲区长度
    int length;
    // 缓冲区头部位置
    int head;
    // 下一个缓冲区指针
    io_buffer* next;
    // 数据存储区
    char *data;
};
// 缓冲区池类
class buffer_pool{
public:
    // 定义枚举类型，表示不同缓冲区大小的常量值
    enum MEM_CAP
    {
        u4K   = 4096,
        u16K  = 16384,
        u64K  = 65536,
        u256K = 262144,
        u1M   = 1048576,
        u4M   = 4194304,
        u8M   = 8388608
    };
    // 初始化缓冲区池的静态方法
    static void init()
    {
        _ins = new buffer_pool(); // 创建缓冲区池对象
        assert(_ins); // 断言确保对象创建成功
    }

    // 获取缓冲区池单例对象的静态方法
    static buffer_pool* ins()
    {
        pthread_once(&_once, init); // 确保初始化只执行一次
        return _ins; // 返回单例对象指针
    }
    // 分配指定大小的缓冲区对象
    io_buffer* alloc(int N);

    // 默认分配4KB大小的缓冲区对象
    io_buffer* alloc() { return alloc(u4K); }

    // 回收不再使用的缓冲区对象
    void revert(io_buffer* buffer);
private:
    buffer_pool();
    buffer_pool(const buffer_pool&);
    const buffer_pool &operator = (const buffer_pool&);
//管理分配的缓冲区对象，以便在需要时进行分配和回收
    typedef __gnu_cxx::hash_map<int, io_buffer*> pool_t;
    pool_t _pool;

//    缓冲区池的参数设定
    uint64_t _total_mem; // 总内存大小
    static buffer_pool* _ins; // 缓冲区池单例对象指针
    static pthread_mutex_t _mutex; // 互斥锁，保证线程安全，保护在多线程环境中对缓冲区池的并发访问
    static pthread_once_t _once; // 保证init函数只执行一次的控制变量
};
struct tcp_buffer{
    tcp_buffer():_buf(nullptr){}
    ~tcp_buffer(){clear();}
    // 获取缓冲区长度
    const int length() const {return _buf?_buf->length: 0;}
    void clear();
    // 弹出指定长度的数据
    void pop(int len);
protected:
    io_buffer* _buf;
};
// 输出缓冲区类
class output_buffer : public tcp_buffer{
public:
    // 向缓冲区写入数据
    //暂时用char代替，完整版需要使用数据包
    int send_data(const char *data,int datlen);
    // 将缓冲区的数据写入到文件描述符中
    int write_fd(int fd);
};
//输入缓冲区类
class input_buffer : public tcp_buffer{
public:
    int read_data(int fd);
    const char* data() const {return _buf? _buf->data + _buf->head: nullptr;}
    void adjust() { if (_buf) _buf->adjust();}
};

#endif //DTIMEDB_IO_BUFFER_H
