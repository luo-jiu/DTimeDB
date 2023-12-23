#include <engine/file_manager/file_path_manager.h>
using namespace dt::tsm;

#include <algorithm>

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
            string file_path = m_default_base_path + "/" + file_name;

            // 创建文件
            int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0666);
            if (fd == -1)
            {
                std::cerr << "Error : Failed to crate file " << file_path << std::endl;
                return "";
            }

            // 把文件名存入对应表目录下
            m_map[database_name][table_name].m_files.push_back(file_name);
            return file_path;
        }
        else
        {
            // 表不存在
            std::cerr << "Error : cannot find table '" << table_name << "' " << std:: endl;
            return "";
        }
    }
    else
    {
        // 数据库不存在
        std::cerr << "Error : cannot find database '" << database_name << "' " << std:: endl;
        return "";
    }
}

/**
 * 删除数据库
 */
bool FilePathManager::delete_database(
        const string & database_name)
{
    auto db_it = m_map.find(database_name);
    if (db_it != m_map.end())
    {
        // 数据库存在
        fs::path db_path = m_default_base_path + "/" + database_name;
        if (fs::exists(db_path) && fs::is_directory(db_path))
        {
            try
            {
                fs::remove_all(db_path);  // 移除文件夹以及里面的所有内容
                return true;
            }
            catch (const std::exception & e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                return false;
            }
        }
        else
        {
            std::cout << "Folder does not exist, no action taken." << std::endl;
            return false;
        }
    }

    // 移除库记录
    return m_map.erase(database_name) == 1;
}

/**
 * 删除表
 */
bool FilePathManager::delete_table(
        const string & table_name,
        const string & databases_name)
{
    auto db_it = m_map.find(databases_name);
    if (db_it != m_map.end())  // 数据库存在
    {
        auto tb_it = db_it->second.find(table_name);
        if (tb_it != db_it->second.end())  // 表也存在
        {
            // 拿取list 开始删除所有表对应文件
            auto & file_list = tb_it->second.m_files;
            string file_path;
            for (string & item : file_list)
            {
                try
                {
                    file_path = m_default_base_path + "/" + table_name + "/" + item;
                    if (fs::exists(file_path) && fs::is_regular_file(file_path))  // 存在且是文件
                    {
                        fs::remove(file_path);
                    }
                    else
                    {
                        std::cout << "File does not exist, no action taken." << std::endl;
                    }
                }
                catch (const std::exception & e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
        }
    }

    // 移除表记录
    return m_map[databases_name].erase(table_name) == 1;
}


/**
 * 删除文件
 */
bool FilePathManager::delete_file(
        const string & file_name,
        const string & table_name,
        const string & database_name)
{
    auto db_it = m_map.find(database_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.find(table_name);
        if (tb_it != db_it->second.end())
        {
            auto & file_list = tb_it->second.m_files;
            auto file_it = std::find(file_list.begin(), file_list.end(), file_name);

            if (file_it != file_list.end())  // 文件存在
            {
                try
                {
                    string file_path = m_default_base_path + "/" + table_name + "/" + file_name;
                    if (fs::exists(file_path) && fs::is_regular_file(file_path))  // 存在且是文件
                    {
                        fs::remove(file_path);  // 移除文件
                        return true;
                    }
                    else
                    {
                        std::cout << "File does not exist, no action taken." << std::endl;
                        return false;
                    }
                }
                catch (const std::exception & e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    return false;
                }
            }
            else
            {
                std::cout << "File does not exist, no action taken." << std::endl;
            }
        }
        else
        {
            std::cerr << "Error : cannot find table '" << table_name << "' " << std:: endl;
        }
    }
    else
    {
        std::cerr << "Error : cannot find database '" << database_name << "' " << std:: endl;
    }
    return false;
}
