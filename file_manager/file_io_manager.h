#ifndef DTIMEDB_FILE_IO_MANAGER_H
#define DTIMEDB_FILE_IO_MANAGER_H

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <stdexcept>
#include <optional>
#include <memory>
#include <list>

namespace dt::file
{
    class FileIOManager
    {
    public:
        explicit FileIOManager(size_t max_size) : m_max_size(max_size) {}
        ~FileIOManager();

        std::shared_ptr<std::fstream> get_file_stream(const std::string & file_path, const std::string & mode);
        void release_file_stream(const std::string & file_path);
        void close_file_stream(const std::string & file_path);
        void reset_file_stream(const std::string & file_path);

    private:
        void update_usage_order(const std::string & file_path);
        void evict_least_used();

    private:
        struct FileStreamInfo
        {
            std::shared_ptr<std::fstream> stream;
            std::list<std::string>::iterator it;
            bool in_use;
        };

        size_t m_max_size;  // io 流数量上限
        std::mutex m_mutex;

        std::unordered_map<std::string, std::pair<FileStreamInfo, std::string>> m_file_stream;
        std::list<std::string> m_usage_order;
    };
}

#endif //DTIMEDB_FILE_IO_MANAGER_H
