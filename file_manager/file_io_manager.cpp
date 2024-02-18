#include "file_io_manager.h"
using namespace dt::file;

FileIOManager::~FileIOManager()
{
    std::lock_guard<std::mutex> lock(m_mutex); // 确保线程安全
    for (auto& pair : m_file_stream) {
        auto& streamInfo = pair.second.first; // 获取FileStreamInfo
        if (streamInfo.stream && streamInfo.stream->is_open()) {
            streamInfo.stream->close();
        }
    }
    // 清空容器
    m_file_stream.clear();
    m_usage_order.clear();
}

/**
 * 获取文件io 流
 */
//std::shared_ptr<std::fstream> FileIOManager::get_file_stream(
//        const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    auto it = m_file_stream.find(file_path);
//
//    // 检查文件流是否已经存在，并且模式匹配且现在并未被其他线程使用
//    if (it != m_file_stream.end())
//    {
//        FileStreamInfo & file_info = it->second.first;
//        std::string & existing_mode = it->second.second;
//        if (!file_info.in_use)
//        {
//            file_info.in_use = true;
//            update_usage_order(file_path);
//            return file_info.stream; // 找到io 流且未被使用
//        }
//    }
//
//    // 未找到io 流，准备创建
//    if (m_file_stream.size() >= m_max_size)  // 是否超过容积
//    {
//        evict_least_used();  // 移出链表尾部不常用的io 流
//    }
//
//    auto stream = std::make_shared<std::fstream>(file_path, std::ios::in | std::ios::out | std::ios::binary);
//    if (!stream->is_open()) {
//        std::cerr << "Error opening file " << file_path << std::endl;
//        return nullptr;
//    }
//    m_usage_order.push_front(file_path);
//    m_file_stream[file_path] = {stream, m_usage_order.begin(), true};
//    return stream;
//}

std::shared_ptr<std::fstream> FileIOManager::get_file_stream(
        const std::string & file_path,
        const std::string & mode)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);

    // 检查文件流是否已经存在，并且模式匹配且现在并未被其他线程使用
    if (it != m_file_stream.end())
    {
        FileStreamInfo & file_info = it->second.first;
        std::string& existing_mode = it->second.second;
        if (!file_info.in_use && existing_mode == mode)
        {
            file_info.in_use = true;
            update_usage_order(file_path);
            return file_info.stream; // 找到io 流且未被使用
        }
    }

    // 未找到io 流，准备创建
    if (m_file_stream.size() >= m_max_size)  // 是否超过容积
    {
        evict_least_used();  // 逐出最不常用的文件流
    }

    std::ios_base::openmode open_mode = std::ios::in; // 默认模式设置为只读
    if (mode == "binary") {
        open_mode = std::ios::in | std::ios::out | std::ios::binary; // 二进制写入模式，同时支持读取
    } else if (mode == "trunc") {
        open_mode = std::ios::in | std::ios::out | std::ios::trunc; // 覆盖写模式
    } else if (mode == "append") {
        open_mode = std::ios::in | std::ios::out | std::ios::app; // 追加模式，同时支持读取
    }

    auto stream = std::make_shared<std::fstream>(file_path, open_mode);
    if (!stream->is_open())
    {
        std::cerr << "Error opening file " << file_path << std::endl;
        return nullptr;
    }
    m_usage_order.push_front(file_path);
    m_file_stream[file_path] = {{stream, m_usage_order.begin(), true}, mode};
    return stream;
}

void FileIOManager::release_file_stream(const std::string & file_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end())
    {
        it->second.first.in_use = false;
        update_usage_order(file_path);
    }
}

void FileIOManager::close_file_stream(const std::string & file_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_file_stream.find(file_path);
    if (it != m_file_stream.end())
    {
        it->second.first.stream->close();
        m_usage_order.erase(it->second.first.it);
        m_file_stream.erase(file_path);
    }
}

/**
 * 重置文件指针位置
 * (没有则创建)
 */
void FileIOManager::reset_file_stream(const std::string & file_path)
{
    auto stream = get_file_stream(file_path, "binary");
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
        m_usage_order.erase(it->second.first.it);
        m_usage_order.push_front(file_path);
        it->second.first.it = m_usage_order.begin();
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