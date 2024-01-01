#include "file_io_manager.h"
using namespace dt::file;

/**
 * 获取文件io 流
 */
std::shared_ptr<std::fstream> FileIOManager::get_file_stream(
        const std::string &file_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end() && !it->second.in_use) {
        it->second.in_use = true;
        update_usage_order(file_path);
        return it->second.stream;  // 找到io 流且未被使用
    }

    // 未找到io 流，准备创建
    if (m_file_stream.size() >= m_max_size)  // 是否超过容积
    {
        evict_least_used();  // 移出链表尾部不常用的io 流
    }

    auto stream = std::make_shared<std::fstream>(file_path, std::ios::in | std::ios::out | std::ios::binary);
    if (!stream->is_open()) {
        std::cerr << "Error opening file " << file_path << std::endl;
        return nullptr;
    }
    m_usage_order.push_front(file_path);
    m_file_stream[file_path] = {stream, m_usage_order.begin(), true};
    return stream;
}

std::shared_ptr<std::fstream> FileIOManager::get_file_stream(
        const std::string & file_path,
        std::string mode)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end() && !it->second.in_use) {
        it->second.in_use = true;
        update_usage_order(file_path);
        return it->second.stream;  // 找到io 流且未被使用
    }

    // 未找到io 流，准备创建
    if (m_file_stream.size() >= m_max_size)  // 是否超过容积
    {
        evict_least_used();  // 移出链表尾部不常用的io 流
    }

    std::shared_ptr<std::fstream> stream;
    if (mode == "binary")
    {
        stream = std::make_shared<std::fstream>(file_path, std::ios::in | std::ios::out | std::ios::binary);
    }
    else if(mode == "trunc")
    {
        stream = std::make_shared<std::fstream>(file_path, std::ios::in | std::ios::out | std::ios::trunc);
    }
    else if (mode == "normal")
    {
        stream = std::make_shared<std::fstream>(file_path, std::ios::in | std::ios::out);
    }

    if (!stream->is_open()) {
        std::cerr << "Error opening file " << file_path << std::endl;
        return nullptr;
    }
    m_usage_order.push_front(file_path);
    m_file_stream[file_path] = {stream, m_usage_order.begin(), true};
    return stream;
}

void FileIOManager::release_file_stream(const std::string & file_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end())
    {
        it->second.in_use = false;
        update_usage_order(file_path);
    }
}

void FileIOManager::close_file_stream(const std::string & file_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end())
    {
        it->second.stream->close();
        m_usage_order.erase(it->second.it);
        m_file_stream.erase(file_path);
    }
}

/**
 * 重置文件指针位置
 * (没有则创建)
 */
void FileIOManager::reset_file_stream(const std::string & file_path)
{
    auto stream = get_file_stream(file_path);
    if (stream)
    {
        stream->seekg(0, std::ios::beg);
        stream->seekp(0, std::ios::beg);
        if (stream->fail())
        {
            std::cerr << "Failed to reset stream for " << file_path << std::endl;
        }
        release_file_stream(file_path);
    }
    else
    {
        std::cerr << "Failed to get file stream for " << file_path << std::endl;
    }
}

void FileIOManager::update_usage_order(const std::string & file_path)
{
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end())
    {
        m_usage_order.erase(it->second.it);
        m_usage_order.push_front(file_path);
        it->second.it = m_usage_order.begin();
    }
}

void FileIOManager::evict_least_used()
{
    if (!m_usage_order.empty())
    {
        const auto & least_used_path = m_usage_order.back();
        close_file_stream(least_used_path);
    }
}