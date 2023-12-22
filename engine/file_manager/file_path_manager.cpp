#include <engine/file_manager/file_path_manager.h>
using namespace dt::tsm;

#include <filesystem>
namespace fs = std::filesystem;

/**
 * 创建数据库(文件夹)
 * @return 文件夹路径
 */
string FilePathManager::create_database(
        const string & database_name)
{
    // 数据库路径
    string db_path = m_default_base_path + "/" + database_name;

    // 检查路径是否已经存在
    if (!fs::exists(db_path))
    {
        // 创建文件夹
        fs::create_directory(db_path);

        // 初始化数据库条目,如果已经存在,这行代码不会有任何影响
        m_map[database_name] = std::map<string, TableInfo>();

        return db_path;
    }
    else
    {
        std::cerr << "Database already exists\n";
        return "";
    }
}

/**
 * 创建表
 * @param table_name
 * @param database_name
 * @return 返回表名
 */
bool FilePathManager::create_table(
        const string & table_name,
        const string & database_name)
{
    // 检查数据库是否存在
    if (m_map.find(database_name) == m_map.end())
    {
        std::cout << "Database does not exist\n";
        return false;
    }

    // 创建表(不用对真实的文件或文件夹进行操作, 是抽象出来的)
    m_map[database_name][table_name] = TableInfo{INT64_MAX, std::list<string>()};
    return true;
}

/**
 * 创建表文件
 * @param engine_abbrev 引擎名
 * @return 文件路径
 */
string FilePathManager::create_file(
        const string & table_name,
        const string & database_name,
        const string & engine_abbrev)
{
    // 先判断库是否存在
    int64_t count = 0;
    auto db_it = m_map.find(database_name);
    if (db_it != m_map.end())  // 数据库存在
    {
        // 判断表是否存在
        auto tb_info_it = db_it->second.find(table_name);
        if (tb_info_it != db_it->second.end())  // 表存在
        {
            auto & tb_info = tb_info_it->second;
            tb_info.m_counter--;
            string file_name = table_name + "-" +std::to_string(tb_info.m_counter) + "." + engine_abbrev;
            
        }
        else
        {
            // 表不存在
            std::cerr << "Error : cannot find table '" << table_name << "' " << std:: endl;
            return "";
        }
    }
//
//    // 开始创建文件
//    string file_name = table_name + "-" + std::to_string(count) + "." + engine_abbrev;
//    string file_path = m_default_base_path + "/" + file_name;
//    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0666);
//    if (fd == -1)
//    {
//        std::cerr << "Error : Failed to crate file " << file_path << std::endl;
//        return "";
//    }
//    // 添加到表
//    m_table_map[table_name].push_back(file_name);
//    close(fd);
//
//    return file_path;
}
//
//bool FilePathManager::delete_database(
//        const string & database_name)
//{
//    auto db_it = m_table_count_map.find(database_name);
//    if (db_it != m_table_count_map.end())
//    {
//        // 数据库存在
//        fs::path db_path = m_default_base_path + "/" + database_name;
//        if (fs::exists(db_path) && fs::is_directory(db_path))
//        {
//            fs::remove_all(db_path);
//        }
//    }
//
//    return m_table_count_map.erase(database_name) == 1;
//}
//
//bool FilePathManager::delete_table(
//        string & table_name,
//        string & databases_name)
//{
//
//}

bool FilePathManager::delete_file(const string & file_path)
{
//    if (remove(file_path.c_str()) != 0)
//    {
//        std::cerr << "Error : Filed to delete file " << file_path << std::endl;
//        return false;
//    }
//    m_file_map.erase(file_path);
}