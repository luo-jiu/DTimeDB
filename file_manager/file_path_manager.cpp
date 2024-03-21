#include "file_path_manager.h"
using namespace dt::file;
using std::string;

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
    if (fs::exists(db_path))
    {
        // 初始化数据库条目,如果已经存在,这行代码不会有任何影响
        m_map[db_name] = std::map<string, TableInfo>();

        return db_path;
    }
    else
    {
        std::cerr << "Database not exists\n";
        return "";
    }
}

/**
 * 创建表
 */
bool FilePathManager::create_table(
        const std::string & tb_name,
        const std::string & db_name)
{
    std::shared_lock<std::shared_mutex> map_lock(m_mutex);  // 先锁整个m_map[读锁]

    // 检查数据库是否存在
    auto db_it = m_map.find(db_name);
    if (db_it == m_map.end())
    {
        std::cout << "Database does not exist\n";
        return false;
    }

    // 放全局锁 细化粒度到表锁
    map_lock.unlock();
    std::unique_lock<std::shared_mutex> tb_lock(m_table_locks[tb_name]);

    // 创建表(创建系统表文件就行)
    string sys_file_path;
    // 这里需要修改为map映射，以便于未来多个引擎不会造成if-else if-else 逻辑冗余

    sys_file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;

    int fd = open(sys_file_path.c_str(), O_CREAT | O_WRONLY, 0666);
    if (fd == -1)
    {
        std::cerr << "Error : Failed to crate file " << sys_file_path << std::endl;
        return false;
    }
    close(fd);
    sys_clear_file(db_name, tb_name);
    db_it->second[tb_name] = TableInfo{std::list<string>()};
    return true;
}

/**
 * 判断表是否存在
 *
 * 其实还可以顺手判断数据库是否存在
 */
bool FilePathManager::exists_table(
        const string & db_name,
        const string & tb_name,
        bool print)
{
    std::shared_lock<std::shared_mutex> read_map_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_info_it = db_it->second.find(tb_name);
        if (tb_info_it != db_it->second.end())
        {
            return true;
        }
        else
        {
            if (print)
            {
                std::cout << "cannot find table:'" << tb_name << "'" << std::endl;
            }
        }
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
    if (exists_table(db_name, tb_name, false))
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        string file_name = tb_name + "-" + std::to_string(current_time.time_since_epoch().count()) + "." + engine_abbrev;
        string file_path = m_default_base_path + "/" + db_name + "/" + file_name;

        // 创建文件
        int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0666);
        if (fd == -1)
        {
            std::cerr << "Error : Failed to crate file " << file_path << std::endl;
            return "";
        }

        // 把文件名存入对应表目录下
        return file_path;
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
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);

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
    std::unique_lock<std::shared_mutex> write_lock(m_mutex); // 直接获取写锁

    // 检查数据库和表是否存在
    auto db_it = m_map.find(db_name);
    if (db_it == m_map.end()) {
        std::cout << "cannot find database:'" << db_name << "'" << std::endl;
        return false; // 数据库不存在
    }

    auto &db = db_it->second;
    auto tb_info_it = db.find(tb_name);
    if (tb_info_it == db.end()) {
        std::cout << "cannot find table:'" << tb_name << "'" << std::endl;
        return false; // 表不存在
    }

    // 删除所有表对应文件
    auto &file_list = tb_info_it->second.m_files;
    string file_path;
    for (string &item : file_list) {
        try {
            file_path = m_default_base_path + "/" + db_name + "/" + tb_name + "/" + item; // 注意路径拼接包含数据库名
            if (fs::exists(file_path) && fs::is_regular_file(file_path)) { // 存在且是文件
                fs::remove(file_path);
            } else {
                std::cout << "File does not exist, no action taken." << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error deleting file: " << e.what() << std::endl;
        }
    }

    // 移除表记录
    bool result = db.erase(tb_name) == 1;
    return result;
}


/**
 * 删除文件
 */
bool FilePathManager::delete_file(
        const string & file_name,
        const string & tb_name,
        const string & db_name)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mutex); // 获取写锁

    // 直接检查数据库和表是否存在
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto &db = db_it->second;
        auto tb_it = db.find(tb_name);
        if (tb_it != db.end())
        {
            // 现在我们已经确认表确实存在，可以继续进行文件删除操作
            auto &file_list = tb_it->second.m_files;
            auto file_it = std::find(file_list.begin(), file_list.end(), file_name);
            if (file_it != file_list.end())  // 文件存在于列表中
            {
                try {
                    string file_path = m_default_base_path + "/" + db_name + "/" + tb_name + "/" + file_name; // 确保路径是正确的
                    if (fs::exists(file_path) && fs::is_regular_file(file_path))  // 存在且是文件
                    {
                        fs::remove(file_path);  // 移除文件

                        // 从列表中移除文件名
                        file_list.erase(file_it);

                        return true;
                    } else {
                        std::cout << "File does not exist, no action taken." << std::endl;
                        return false;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    return false;
                }
            }
        }
        else
        {
            std::cout << "Table '" << tb_name << "' does not exist in database '" << db_name << "'." << std::endl;
        }
    }
    else
    {
        std::cout << "Database '" << db_name << "' does not exist." << std::endl;
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
        std::vector<std::string> & tables)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);

    // 读取过
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        // 直接返回
        return true;
    }

    // 没读取过
    // 读取基路径下的所有数据库
    string db_path = m_default_base_path + "/" + db_name;
    if (fs::exists(db_path) && fs::is_directory(db_path))
    {
        m_map[db_name] = std::map<string, TableInfo>();
    }
    else
    {
        std::cout << "Error: The database does not exist." << "\n";
        return false;
    }

    /**
     * 开始加载数据库的信息
     * 只用加载系统文件即可(因为和表一一对应)
     */
    string file_name;
    string start_part;
    std::map<string, TableInfo> table_map;
    for (auto & table_name : tables)
    {
        table_map[table_name] = TableInfo{std::list<string>()};
    }

    m_map[db_name] = table_map;
    return true;
}

bool FilePathManager::show_data(const string & db_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    if (db_name.empty())
    {
        for (const auto & entry : fs::directory_iterator(m_default_base_path))
        {
            if (entry.is_directory())
            {
                std::cout << entry.path().filename().string() << std::endl;
            }
        }
    }
    else
    {
        // 展示数据库中的表之间遍历map 的key即可
        auto & tb_map = m_map[db_name];
        for (const auto & entry : tb_map)
        {
            std::cout << entry.first << "\n";
        }
    }
    return true;
}

/**
 * 加载系统文件信息
 * @param db_name
 * @param tb_name
 * @return
 */
dt::tsm::SystemInfo FilePathManager::load_system_info(
        const string & db_name,
        const string & tb_name)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return {};
    }
    file->seekg(std::ios::beg);

    int64_t head_offset, tail_offset;
    uint64_t margin;
    file->read(reinterpret_cast<char*>(&head_offset), sizeof(head_offset));
    file->read(reinterpret_cast<char*>(&tail_offset), sizeof(tail_offset));
    file->read(reinterpret_cast<char*>(&margin), sizeof(margin));
    int16_t length;
    file->read(reinterpret_cast<char*>(&length), sizeof(length));
    std::vector<char> file_name(length + 1);
    if (file->good())
    {
        file->read(file_name.data(), length);
        file_name[length] = '\0';
    }

    m_io_file.release_file_stream(file_path);
    string read_file_path = "./../dbs/" + db_name + "/" + string(file_name.data());
    std::cout << "head_offset:" << head_offset << ", tail_offset:" << tail_offset << ", margin:" << margin << ", file_name:" << file_name.data() << "\n";
    std::cout << "read_file_path:" << read_file_path + "\n";
    return dt::tsm::SystemInfo{head_offset, tail_offset, margin, read_file_path};
}

/**
 * 初始化新创建的tsm file 文件信息
 */
bool FilePathManager::create_tsm_file_update_sys_info(
        const string & db_name,
        const string & tb_name,
        const string & file_name,
        uint64_t margin)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    file->seekp(std::ios::beg);
    auto length = static_cast<uint16_t>(file_name.length());

    // 写m_head_offset 和m_tail_offset
    int64_t head_offset = 5, tail_offset = margin;
    file->write(reinterpret_cast<const char*>(&head_offset), sizeof(int64_t));
    file->write(reinterpret_cast<const char*>(&tail_offset), sizeof(int64_t));
    // 写margin
    file->write(reinterpret_cast<const char*>(&margin), sizeof(uint64_t));
    // 写file_name
    file->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    file->write(file_name.c_str(), length);  // 写字符串
    file->flush();

    m_io_file.release_file_stream(file_path);
    std::cout << "sys更新成功" << std::endl;
    return true;
}

/**
 * 修改系统文件中待写入文件名
 */
bool FilePathManager::update_system_file_name(
        const string & db_name,
        const string & tb_name,
        const string & file_name)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    file->seekp(24);
    auto length = static_cast<uint16_t>(file_name.length());

    // 写file_name
    file->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    file->write(file_name.c_str(), length);  // 写字符串
    file->flush();

    m_io_file.release_file_stream(file_path);
    return true;
}

/**
 * 修改系统文件中待写入文件剩余容量大小
 */
bool FilePathManager::update_system_file_margin(
        const string & db_name,
        const string & tb_name,
        uint64_t margin)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    file->seekp(16);
    file->write(reinterpret_cast<const char*>(&margin), sizeof(uint64_t));
    file->flush();

    m_io_file.release_file_stream(file_path);
    return true;
}

/**
 * 修改系统文件中待写入文件指针偏移量
 */
bool FilePathManager::update_system_file_offset(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & type,
        int64_t offset)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    if (type == "head")
    {
        file->seekp(std::ios::beg);
    }
    else if (type == "tail")
    {
        file->seekp(8);
    }
    else if (type == "field")
    {
        file->seekp(200);
    }
    else
    {
        m_io_file.release_file_stream(file_path);
        return false;
    }

    file->write(reinterpret_cast<const char*>(&offset), sizeof(int64_t));
    file->flush();

    m_io_file.release_file_stream(file_path);
    return true;
}

bool FilePathManager::sys_show_file(
        const string & db_name,
        const string & tb_name)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    file->seekg(std::ios::beg);

    int64_t head_offset, tail_offset;
    uint64_t margin;
    file->read(reinterpret_cast<char*>(&head_offset), sizeof(head_offset));
    file->read(reinterpret_cast<char*>(&tail_offset), sizeof(tail_offset));
    file->read(reinterpret_cast<char*>(&margin), sizeof(margin));
    int16_t length;
    file->read(reinterpret_cast<char*>(&length), sizeof(length));
    std::vector<char> file_name(length + 1);
    if (file->good())
    {
        file->read(file_name.data(), length);
        file_name[length] = '\0';
    }
    m_io_file.release_file_stream(file_path);
    std::cout << "table:" << tb_name << ", filename:" << file_name.data()
        << ", margin:" << margin << ", head_offset:" << head_offset << ", tail_offset:" << tail_offset << "\n";
    return true;
}

bool FilePathManager::sys_clear_file(
        const string & db_name,
        const string & tb_name)
{
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
    }
    int64_t offset = 0;
    uint16_t length = 0;
    file->seekp(std::ios::beg);

    offset = 10;  // 偏移量
    file->write(reinterpret_cast<const char*>(&offset), sizeof(int64_t));
    file->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    file->flush();

    m_io_file.release_file_stream(file_path);
    return true;
}

std::set<std::string> FilePathManager::load_mea_fields(
        const std::string & db_name,
        const std::string & tb_name)
{
    std::lock_guard<std::mutex> lock(m_table_meta_mutex);
    std::string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading" << std::endl;
        return {};
    }

    // 定位到文件中存储字段数量的位置，即跳过偏移量存储的200字节加上8字节的偏移量
    file.seekg(sizeof(int64_t));
    uint16_t num;
    file.read(reinterpret_cast<char*>(&num), sizeof(num));

    // 读取所有字段
    std::set<std::string> fields;
    for (uint16_t i = 0; i < num; ++i)
    {
        uint16_t length;
        file.read(reinterpret_cast<char*>(&length), sizeof(length)); // 读取字段长度

        std::string field(length, '\0');
        file.read(&field[0], length); // 根据长度读取字段

        fields.insert(field); // 添加到字段列表中
    }

    // 打印读取的字段
//    for (const auto& field : fields)
//    {
//        std::cout << "Field: " << field << std::endl;
//    }
    return fields;
}

void FilePathManager::insert_field_to_file(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & field)
{
    std::lock_guard<std::mutex> lock(m_table_meta_mutex);
    string file_path = m_default_base_path + "/" + db_name + "/sys-" + tb_name + "." + m_engine;
    auto file = m_io_file.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return;
    }
    file->seekg(std::ios::beg);
    int64_t offset;
    uint16_t num;
    file->read(reinterpret_cast<char*>(&offset), sizeof(offset));
    file->read(reinterpret_cast<char*>(&num), sizeof(num));
    std::cout << "offset:" << offset << "num:" << num << std::endl;
    file->seekp(offset);
    auto length = static_cast<uint16_t>(field.length());

    // 写file_name
    file->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    file->write(field.c_str(), length);  // 写字符串

    // 获取当前末尾位置的偏移量
    offset = file->tellp();
    num += 1;

    file->seekp(std::ios::beg);
    file->write(reinterpret_cast<char*>(&offset), sizeof(offset));
    file->write(reinterpret_cast<char*>(&num), sizeof(num));

    file->flush();
    m_io_file.release_file_stream(file_path);
}