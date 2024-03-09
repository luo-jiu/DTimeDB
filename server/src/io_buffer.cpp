//
// Created by illumwang on 24-2-26.
//
#include <unistd.h>
#include <sys/ioctl.h>
#include "server/include/server_header.h"
/**
 *  为缓存池申请空间
 * @param N
 * @return
 */
#define EXTRA_MEM_LIMITT (5U * 1024 * 1024)
io_buffer *buffer_pool::alloc(int N) {
    int index;
    if (N <= u4K)
        index = u4K;
    else if (N <= u16K)
        index = u16K;
    else if (N <= u64K)
        index = u64K;
    else if (N <= u256K)
        index = u256K;
    else if (N <= u1M)
        index = u1M;
    else if (N <= u4M)
        index = u4M;
    else if (N <= u8M)
        index = u8M;
    else
        return NULL;
    ::pthread_mutex_lock(& _mutex);
//    缓冲池中没有找到对应索引的缓冲区对象
    if (!_pool[index]){
        // 检查内存使用是否超过额定限制
        if (_total_mem + index /1024 >= EXTRA_MEM_LIMITT){
            exit_log("use too many memory");
            ::exit(1);
        }
        io_buffer* new_buffer = new io_buffer(index);
        exit_if(new_buffer == nullptr, "new buffer is Null");
        // 更新总内存使用量
        _total_mem += index /1024;
        ::pthread_mutex_unlock(&_mutex);
        // 返回新分配的缓冲区对象指针
        return new_buffer;
    }
    // 从池中获取缓冲区对象
    io_buffer* target = _pool[index];
    _pool[index] = target->next;
    ::pthread_mutex_unlock(&_mutex);
    target->next = nullptr;
    return target;
}
// 回收不再使用的缓冲区对象
void buffer_pool::revert(io_buffer *buffer) {
    int index = buffer->capacity;
    buffer->length = 0;
    buffer->head = 0;
    ::pthread_mutex_lock(&_mutex);
    assert(_pool.find(index) != _pool.end());
    buffer->next = _pool[index];
    _pool[index] = buffer;
    ::pthread_mutex_unlock(&_mutex);
}
// 构造函数，初始化缓冲池
buffer_pool::buffer_pool() :_total_mem(0){
    io_buffer* prev;
    // 分配不同大小的缓冲区对象并加入到缓冲池中
    _pool[u4K] = new io_buffer(u4K);
    exit_if(_pool[u4K] == NULL, "new io_buffer err");
    prev = _pool[u4K];
    // 4K缓冲区对象数量: 5000 ≈ 20MB
    for (int i = 1; i < 5000; ++i) {
        prev->next = new io_buffer(u4K);
        exit_if(prev->next == nullptr,"new io_buffer err");
        prev = prev->next;
    }
    _total_mem += 4 * 5000;
//    16k
    _pool[u16K] = new io_buffer(u16K);
    exit_if(_pool[u16K] == NULL, "new io_buffer");

    prev = _pool[u16K];
    //16K buffer count: 1000 ≈ 16MB
    for (int i = 1;i < 1000; ++i)
    {
        prev->next = new io_buffer(u16K);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 16 * 1000;
        //64K
    _pool[u64K] = new io_buffer(u64K);
    exit_if(_pool[u64K] == NULL, "new io_buffer");

    prev = _pool[u64K];
    //64K buffer count: 500 ≈ 32MB
    for (int i = 1;i < 500; ++i)
    {
        prev->next = new io_buffer(u64K);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 64 * 500;

    _pool[u256K] = new io_buffer(u256K);
    exit_if(_pool[u256K] == NULL, "new io_buffer");

    prev = _pool[u256K];
    //256K buffer count: 100 ≈ 25MB
    for (int i = 1;i < 200; ++i)
    {
        prev->next = new io_buffer(u256K);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 256 * 200;

    _pool[u1M] = new io_buffer(u1M);
    exit_if(_pool[u1M] == NULL, "new io_buffer");

    prev = _pool[u1M];
    //1M buffer count: 50 = 50MB
    for (int i = 1;i < 50; ++i)
    {
        prev->next = new io_buffer(u1M);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 1024 * 50;

    _pool[u4M] = new io_buffer(u4M);
    exit_if(_pool[u4M] == NULL, "new io_buffer");

    prev = _pool[u4M];
    //4M buffer count: 20 = 80MB
    for (int i = 1;i < 20; ++i)
    {
        prev->next = new io_buffer(u4M);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 4096 * 20;

    _pool[u8M] = new io_buffer(u8M);
    exit_if(_pool[u8M] == NULL, "new io_buffer");

    prev = _pool[u8M];
    //4M buffer count: 10 = 80MB
    for (int i = 1;i < 10; ++i)
    {
        prev->next = new io_buffer(u8M);
        exit_if(prev->next == NULL, "new io_buffer");
        prev = prev->next;
    }
    _total_mem += 8192 * 10;
}
//  信息初始化!!!

buffer_pool* buffer_pool::_ins = NULL;
pthread_mutex_t buffer_pool::_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_once_t buffer_pool::_once = PTHREAD_ONCE_INIT;

// 清空 TCP 缓冲区
void tcp_buffer::clear() {
    if (_buf)
    {
        buffer_pool::ins()->revert(_buf);
        _buf = NULL;
    }
}
// 弹出指定长度的数据
void tcp_buffer::pop(int len) {
    assert(_buf && len <= _buf->length);
    _buf->pop(len);
    if (!_buf->length){
        buffer_pool::ins()->revert(_buf);
        _buf = NULL;
    }
}
// 从文件描述符中读取数据到输入缓冲区
int input_buffer::read_data(int fd) {
    // 一次性读取所有数据
    int rn, ret;
    if (::ioctl(fd, FIONREAD, &rn) == -1)
    {
        error_log("ioctl FIONREAD");
        return -1;
    }
    if (!_buf)
    {
        _buf = buffer_pool::ins()->alloc(rn);
        if (!_buf)
        {
            error_log("no idle for alloc io_buffer");
            return -1;
        }
    }
    else
    {
        assert(_buf->head == 0);
        if (_buf->capacity - _buf->length < (int)rn)
        {
            // 如果缓冲区容量不足，需要获取一个新的缓冲区对象
            io_buffer* new_buf = buffer_pool::ins()->alloc(rn + _buf->length);
            if (!new_buf)
            {
                error_log("no idle for alloc io_buffer");
                return -1;
            }
            new_buf->copy(_buf);
            buffer_pool::ins()->revert(_buf);
            _buf = new_buf;
        }
    }

    // 从文件描述符中读取数据到缓冲区
    do
    {
        ret = ::read(fd, _buf->data + _buf->length, rn);
    } while (ret == -1 && errno == EINTR);

    if (ret > 0)
    {
        assert(ret == rn);
        _buf->length += ret;
    }
    return ret;
}
// 发送数据到输出缓冲区
int output_buffer::send_data(const char *data, int datlen) {
    if (!_buf)
    {
        _buf = buffer_pool::ins()->alloc(datlen);
        if (!_buf)
        {
            error_log("no idle for alloc io_buffer");
            return -1;
        }
    }
    else
    {
        assert(_buf->head == 0);
        if (_buf->capacity - _buf->length < datlen)
        {
            // 如果缓冲区容量不足，需要获取一个新的缓冲区对象
            io_buffer* new_buf = buffer_pool::ins()->alloc(datlen + _buf->length);
            if (!new_buf)
            {
                error_log("no idle for alloc io_buffer");
                return -1;
            }
            new_buf->copy(_buf);
            buffer_pool::ins()->revert(_buf);
            _buf = new_buf;
        }
    }

    // 将数据复制到缓冲区
    ::memcpy(_buf->data + _buf->length, data, datlen);
    _buf->length += datlen;
    return 0;
}
//将缓冲区数据写入文件描述符
int output_buffer::write_fd(int fd) {
    assert(_buf && _buf->head == 0);
    int writed;
    do
    {
        // 将缓冲区数据写入文件描述符
        writed = ::write(fd, _buf->data, _buf->length);
    } while (writed == -1 && errno == EINTR);
    if (writed > 0)
    {
        // 更新缓冲区状态
        _buf->pop(writed);
        _buf->adjust();
    }
    // 如果写入阻塞，则返回0
    if (writed == -1 && errno == EAGAIN)
    {
        writed = 0; // 不是错误，仅返回为0表示此时不可继续写
    }
    return writed;
}