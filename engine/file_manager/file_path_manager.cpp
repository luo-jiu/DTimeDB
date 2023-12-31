#include <engine/file_manager/file_path_manager.h>
using namespace dt::file;

#include <algorithm>

#include <filesystem>
namespace fs = std::filesystem;

/**
 * 创建数据库(文件夹)
 * @return 文件夹路径
 */
string FilePathManager::create_database(
        const string & db_name)
{
    // 数据库路径
    string db_path = m_default_base_path + "/" + db_name;

    // 检查路径是否已经存在
    if (!fs::exists(db_path))
    {
        // 创建文件夹
        fs::create_directory(db_path);

        // 初始化数据库条目,如果已经存在,这行代码不会有任何影响
        m_map[db_name] = std::map<string, TableInfo>();

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
 * @return 返回表名
 */
bool FilePathManager::create_table(
        const string & tb_name,
        const string & db_name,
        const string & engine)
{
    // 检查数据库是否存在
    if (m_map.find(db_name) == m_map.end())
    {
        std::cout << "Database does not exist\n";
        return false;
    }

    // 创建表(创建系统表文件就行)
    string sys_file_path;
    if (engine == "tsm")
    {
        sys_file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + ".tsm";
    }
    else
    {
        sys_file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + ".clt";
    }
    int fd = open(sys_file_path.c_str(), O_CREAT | O_WRONLY, 0666);
    if (fd == -1)
    {
        std::cerr << "Error : Failed to crate file " << sys_file_path << std::endl;
        return false;
    }
    close(fd);

    auto file = m_io_file.get_file_stream(sys_file_path, "trunc");
    *file << 147483647 << std::endl;
    m_io_file.release_file_stream(sys_file_path);

    m_map[db_name][tb_name] = TableInfo{INT64_MAX, engine, std::list<string>()};
    return true;
}

/**
 * 判断表是否存在
 *
 * 其实还可以顺手判断数据库是否存在
 */
bool FilePathManager::exists_table(
        const string & tb_name,
        const string & db_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_info_it = db_it->second.find(tb_name);
        if (tb_info_it != db_it->second.end())
        {
            return true;
        }
//        else
//        {
//            std::cout << "cannot find table:'" << tb_name << "'" << std::endl;
//        }
    }
    else
    {
        std::cout << "cannot find database:'" << db_name << "'" << std::endl;
    }
    return false;
}

/**
 * 创建表文件
 * @param engine_abbrev 引擎名
 * @return 文件路径
 */
string FilePathManager::create_file(
        const string & tb_name,
        const string & db_name,
        const string & engine_abbrev)
{
    // 先判断库是否存在
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())  // 数据库存在
    {
        // 判断表是否存在
        auto tb_info_it = db_it->second.find(tb_name);
        if (tb_info_it != db_it->second.end())  // 表存在
        {
            auto & tb_info = tb_info_it->second;
            tb_info.m_counter--;
            string file_name = tb_name + "-" + std::to_string(tb_info.m_counter) + "." + engine_abbrev;
            string file_path = m_default_base_path + "/" + file_name;

            // 创建文件
            int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0666);
            if (fd == -1)
            {
                std::cerr << "Error : Failed to crate file " << file_path << std::endl;
                return "";
            }

            // 把文件名存入对应表目录下
            m_map[db_name][tb_name].m_files.push_back(file_name);
            return file_path;
        }
        else
        {
            // 表不存在
            std::cerr << "Error : cannot find table '" << tb_name << "' " << std:: endl;
        }
    }
    else
    {
        // 数据库不存在
        std::cerr << "Error : cannot find database '" << db_name << "' " << std:: endl;
    }
    return "";
}

/**
 * 该文件用来记录单个表的计数器信息，以及溢写其他的
 * 可能的数据
 */
string FilePathManager::create_sys_tfile(
        const string & tb_name,
        const string & db_name,
        const string & engine_abbrev)
{

}

string FilePathManager::create_sys_dfile(
        const string & tb_name,
        const string & db_name,
        const string & engine_abbrev)
{

}

/**
 * 删除数据库
 */
bool FilePathManager::delete_database(
        const string & db_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        // 数据库存在
        fs::path db_path = m_default_base_path + "/" + db_name;
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
    return m_map.erase(db_name) == 1;
}

/**
 * 删除表
 */
bool FilePathManager::delete_table(
        const string & tb_name,
        const string & db_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())  // 数据库存在
    {
        auto tb_it = db_it->second.find(tb_name);
        if (tb_it != db_it->second.end())  // 表也存在
        {
            // 拿取list 开始删除所有表对应文件
            auto & file_list = tb_it->second.m_files;
            string file_path;
            for (string & item : file_list)
            {
                try
                {
                    file_path = m_default_base_path + "/" + tb_name + "/" + item;
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
    return m_map[db_name].erase(tb_name) == 1;
}


/**
 * 删除文件
 */
bool FilePathManager::delete_file(
        const string & file_name,
        const string & tb_name,
        const string & db_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.find(tb_name);
        if (tb_it != db_it->second.end())
        {
            auto & file_list = tb_it->second.m_files;
            auto file_it = std::find(file_list.begin(), file_list.end(), file_name);

            if (file_it != file_list.end())  // 文件存在
            {
                try
                {
                    string file_path = m_default_base_path + "/" + tb_name + "/" + file_name;
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
            std::cerr << "Error : cannot find table '" << tb_name << "' " << std:: endl;
        }
    }
    else
    {
        std::cerr << "Error : cannot find database '" << db_name << "' " << std:: endl;
    }
    return false;
}

/**
 * 以数据库为单位将表信息加载到内存
 *
 * 调用use 的时候需要调用该方法加载上下文
 * @param db_name
 * @return
 */
bool FilePathManager::load_database(
        const string & db_name,
        const string & engine)
{
    // 读取基路径下的所有数据库
    string _db_name;
    for (const auto & entry : fs::directory_iterator(m_default_base_path))
    {
        if (entry.is_directory())
        {
            _db_name = entry.path().filename().string();
            m_map[_db_name] = std::map<string, TableInfo>();
        }
    }

    /**
     * 开始加载数据库的信息
     * 只用加载系统文件即可(因为和表一一对应)
     */
    string _abbrev;
    if (engine == "clt")
    {
        _abbrev = ".clt";
    }
    else
    {
        _abbrev = ".tsm";
    }

    string _db_path = m_default_base_path + "/" + db_name;
    string _file_name;
    string _start_part;
    std::map<string, TableInfo> _table_map;
    // 判断路径是否存在
    if (!fs::exists(_db_path)) {
        // 不存在
        std::cout << "Error: '" << db_name << "' database does not exist" << std::endl;
        return false;
    }
    for (const auto & entry : fs::directory_iterator(_db_path))
    {
        if (entry.is_regular_file())
        {
            _file_name = entry.path().filename().string();
            size_t pos = _file_name.find('-');
            if (pos != std::string::npos)
            {
                _start_part = _file_name.substr(0, pos);
            }
            if (_start_part == "sys" && entry.path().extension() == _abbrev)  // 只加载sys 系统文件
            {
                string _table_part = _file_name.substr(pos + 1);
                size_t dot_pos = _table_part.find('.');
                string _table_name = _table_part.substr(0, dot_pos);
                _table_map[_table_name] = TableInfo{0, "tsm", std::list<string>()};  // 创建对象
            }
        }
    }

    // 开始加载计数器
    // 遍历_map 的key，去读取每个表的计数器
    for (auto & pair : _table_map)
    {
        string file_path = m_default_base_path + "/" + db_name + "/sys-" + pair.first + _abbrev;
        auto file = m_io_file.get_file_stream(file_path, "normal");
        if (!file->is_open())
        {
            std::cerr << "Error: Could not open file for writing" << std::endl;
            return false;
        }

        // 向前搜索换行符
        string last_line;
        string current_line;
        while (std::getline(*file, current_line))
        {
            last_line = current_line;
        }
        file->clear();
        file->seekp(0, std::ios::beg);
        *file << last_line << std::endl;
        file->flush();
//        m_io_file.release_file_stream(file_path);  // 归还io 流
        m_io_file.close_file_stream(file_path);
        // 将计数器写到_table_map 中
        std::cout << last_line << std::endl;
        int64_t _temp = std::stol(last_line);
        _table_map[pair.first].m_counter = _temp;
    }
    m_map[db_name] = _table_map;

    return true;
}

bool FilePathManager::show_data(const string & db_name)
{
    if (db_name.empty())
    {
        for (const auto & entry : fs::directory_iterator("./../dbs"))
        {
            if (entry.is_directory())
            {
                std::cout << entry.path().filename().string() << std::endl;
            }
        }
    }
    else
    {
//        string db_name = "Executor::get_database()";
        string db_path = "./../dbs/" + db_name;  // 数据库路径
        if (!fs::exists(db_path))
        {
            std::cout << "Error: '" << db_name << "' database does not exist" << std::endl;
            return false;
        }

        // 遍历所有表
        string _file_name;
        string _start_part;
        for (const auto & entry : fs::directory_iterator(db_path))
        {
            if (entry.is_regular_file())
            {
                _file_name = entry.path().filename().string();
                size_t pos = _file_name.find('-');
                if (pos != std::string::npos)
                {
                    _start_part = _file_name.substr(0, pos);
                }
                if (_start_part == "sys")  // 只加载sys 系统文件
                {
                    string _table_part = _file_name.substr(pos + 1);
                    size_t dot_pos = _table_part.find('.');
                    string _table_name = _table_part.substr(0, dot_pos);
                    std::cout << _table_name << std::endl;
                }
            }
        }
    }
}