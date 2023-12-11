#ifndef DTIMEDB_FILE_MANAGER_H
#define DTIMEDB_FILE_MANAGER_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <mutex>

namespace dt
{
    namespace tsm
    {
        class FileManager
        {
        public:
            static std::ifstream & get_input_stream(const std::string & file_path);
            static std::ofstream & get_output_stream(const std::string & file_path);

            static void create_input_and_output_stream(const std::string & file_path);

            static bool create_input_stream(const std::string & file_path);
            static bool create_output_stream(const std::string & file_path);

            static void release_input_stream(const std::string & file_path);
            static void release_output_stream(const std::string & file_path);

            static void close_input_stream(const std::string & file_path);
            static void close_output_stream(const std::string & file_path);

        private:
            static std::unordered_map<std::string, std::ifstream> m_input_stream;
            static std::unordered_map<std::string, std::ofstream> m_output_stream;
            static std::mutex m_mutex;
        };
    }
}

#endif //DTIMEDB_FILE_MANAGER_H
