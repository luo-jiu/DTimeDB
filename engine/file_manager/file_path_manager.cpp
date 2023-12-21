#include <engine/file_manager/file_path_manager.h>
using namespace dt::tsm;

/**
 * 创建数据库(也就是文件夹)
 */
string FilePathManager::create_database(
        string & table_name,
        string & database_name)
{
    string db_path = m_default_base_path + "/" + table_name;

//    // 先判断是否已经创建了
//    auto it = m_database_list_map.find(db_path);
//    if (it == m_database_list_map.end())
//    {
//        // 已经创建则返回
//        return db_path;
//    }
//
//    // 没创建
//    int res = mkdir(db_path.c_str(), 0755);
//    if (res == 0)  // 创建失败
//    {
//        std::cerr << "Error : Failed to create database '" << db_path << "' " << std::endl;
//    }
//    else
//    {
//        // 创建成功，存入数据库map
//        std::map<string, string>
//        m_database_list_map[db_path] = std::vector<string>();
//    }
//
//    return db_path;
}

string FilePathManager::create_file(
        string & table_name,
        string & database_name,
        string & engine_abbrev) {

//    // 拿取map 中映射的计数器
//    auto it = m_table_count_map.find(database_name);
//    if (it != m_table_count_map.end())  // 找到list
//    {
//
//    }
//    else
//    {
//
//    }
//    string file_path = m_default_base_path + "/" + table_name + "-" + std::to_string(count--) + "." + engine_abbrev;
//    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0666);
//    if (fd == -1)
//    {
//        std::cerr << "Error : Failed to crate file " << file_path << std::endl;
//        return "";
//    }
//    close(fd);
//
//    m_table_count_map[table_name] = count;
//    return file_path;
}

bool FilePathManager::delete_file(const string & file_path)
{
//    if (remove(file_path.c_str()) != 0)
//    {
//        std::cerr << "Error : Filed to delete file " << file_path << std::endl;
//        return false;
//    }
//    m_file_map.erase(file_path);
}