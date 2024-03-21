#include "table_engine_map.h"
using namespace dt::file;

#include <iostream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <shared_mutex>
#include <vector>

namespace fs = std::filesystem;

TableEngineManager::TableEngineManager(): m_default_base_path("./../dbs"), m_io_file(4)
{
    m_option_map["engine"] = &TableEngineManager::table_engine_type;
}

std::string TableEngineManager::create_database(const std::string & db_name)
{
    // 检查数据库是否存在
    fs::path db_path = m_default_base_path + "/" + db_name;
    if (fs::exists(db_path))
    {
        if (fs::is_directory(db_path))
        {
            std::cout << "Database already exists\n";
            return db_path;  // 存在直接返回
        }
    }
    else
    {
        // 创建数据库
        fs::create_directory(db_path);
        std::string sys_file_path = m_default_base_path + "/" + db_name + "/sys-metadata";
        int fd = open(sys_file_path.c_str(), O_CREAT | O_WRONLY, 0666);
        if (fd == -1)
        {
            std::cerr << "Error : Failed to crate file " << sys_file_path << std::endl;
            return "";
        }
        close(fd);
        return db_path;
    }
    return "";
}

/**
 * 加载数据库元数据
 */
std::unordered_map<std::string, std::vector<std::string>> TableEngineManager::load_database_metadata(
        const std::string & db_name)
{
    std::unordered_map<std::string, std::vector<std::string>> engine_tables;

    std::lock_guard<std::mutex> lock(m_mutex);

    auto db_it = m_tab_eng_map.find(db_name);
    if (db_it != m_tab_eng_map.end())
    {
        // 如果已经加载过直接退出
        std::cout << "use: " << db_name << "\n";
        return {};
    }

    std::string db_metadata_path = m_default_base_path + "/" + db_name + "/sys-metadata";
    auto file = m_io_file.get_file_stream(db_metadata_path, "");  // 只读模式打开
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return {};
    }

    file->seekg(std::ios::beg);  // 文件指针跳转到开头
    std::string line;
    while (std::getline(*file, line))
    {
        std::istringstream iss(line);
        std::string tb_name;
        std::string engine;
        if (!(iss >> tb_name >> engine))
        {
            continue;  // 处理出错 或跳过无用行
        }
        m_tab_eng_map[db_name][tb_name] = engine;
        engine_tables[engine].push_back(tb_name);
    }
    m_io_file.release_file_stream(db_metadata_path);

    if (engine_tables.empty())
    {
        std::cout << "use: " << db_name << "\n";
    }
    return engine_tables;
}

bool TableEngineManager::create_table(
        const std::string & db_name,
        const std::string & tb_name,
        std::string & engine)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (db_name.empty() || tb_name.empty())
    {
        std::cout << "database or table name is empty\n";
        return false;
    }
    if (engine.empty())  // 默认引擎
    {
        engine = "tsm";
    }

    // 检查是否存在
    auto db_it = m_tab_eng_map.find(db_name);
    if (db_it != m_tab_eng_map.end())
    {
        if (db_it->second.find(tb_name) != db_it->second.end())
        {
            std::cout << "Database does not exist\n";
            return false;  // 已经存在
        }
    }

    std::string db_metadata_path = m_default_base_path + "/" + db_name + "/sys-metadata";
    auto file = m_io_file.get_file_stream(db_metadata_path, "append");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }

    *file << tb_name << " " << engine << std::endl;
    file->flush();
    m_io_file.release_file_stream(db_metadata_path);

    m_tab_eng_map[db_name][tb_name] = engine;
    return true;
}

/**
 * 移除表
 * 需要将移除后的map重新刷写到元数据文件里面
 */
bool TableEngineManager::remove_table(
        const std::string & db_name,
        const std::string & tb_name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string db_metadata_path = m_default_base_path + "/" + db_name + "/sys-metadata";
    // 以只读模式打开文件以获取当前的内容
    auto file = m_io_file.get_file_stream(db_metadata_path, "");  // 只读
    if (!file || !file->is_open()) {
        std::cerr << "Error: Could not open file for reading" << std::endl;
        return false;
    }

    std::string line, content;
    bool found = false;
    while (std::getline(*file, line)) {
        std::istringstream iss(line);
        std::string existing_tb_name, engine;
        if (iss >> existing_tb_name >> engine) {
            if (existing_tb_name != tb_name) {
                content += line + "\n";
            } else {
                found = true; // 标记找到了需要删除的表
            }
        }
    }
    m_io_file.release_file_stream(db_metadata_path); // 释放只读流

    // 如果找到表，则以覆盖模式打开文件并写入修改后的内容
    if (found) {
        file = m_io_file.get_file_stream(db_metadata_path, "trunc");
        if (!file || !file->is_open()) {
            std::cerr << "Error: Could not open file for writing" << std::endl;
            return false;
        }
        *file << content; // 写入修改后的内容
        file->flush();
        m_io_file.release_file_stream(db_metadata_path); // 释放写入流
        m_tab_eng_map[db_name].erase(tb_name); // 从内存中删除表信息
    }
    return found;
}

bool TableEngineManager::database_exist(const std::string & db_name) const
{
    // 检查数据库是否存在
    fs::path db_path = m_default_base_path + "/" + db_name;
    if (fs::exists(db_path))
    {
        if (fs::is_directory(db_path))
        {
            return true;  // 存在直接返回
        }
    }
    return false;
}

void TableEngineManager::table_engine_type(
        const std::string & db_name,
        const std::string & tb_name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto db_it = m_tab_eng_map.find(db_name);
    if (db_it != m_tab_eng_map.end())
    {
        auto tb_it = db_it->second.find(tb_name);
        if (tb_it != db_it->second.end())
        {
            std::cout << "table:" << tb_name << ", engine:" << tb_it->second << "\n";
        }
    }
}

bool TableEngineManager::table_engine_is(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & engine_type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto db_it = m_tab_eng_map.find(db_name);
    if (db_it != m_tab_eng_map.end())
    {
        auto tb_it = db_it->second.find(tb_name);
        if (tb_it != db_it->second.end())
        {
            return engine_type == tb_it->second;
        }
    }
    return false;
}
