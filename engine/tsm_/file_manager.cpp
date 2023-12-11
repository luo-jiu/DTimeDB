//#include <engine/tsm_/file_manager.h>
//using namespace dt::tsm;
//
//std::unordered_map<std::string, std::ifstream> FileManager::m_input_stream = {
//};
//
//std::unordered_map<std::string, std::ofstream> FileManager::m_output_stream = {
//};
//
//std::mutex FileManager::m_mutex;
//
///**
// * 获取输入流
// */
//std::ifstream & FileManager::get_input_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if (m_input_stream.find(file_path) == m_input_stream.end())  // 不存在该io 流
//    {
//        m_input_stream[file_path] = std::ifstream(file_path, std::ios::binary);  // 创建
//    }
//    return m_input_stream[file_path];
//}
//
///**
// * 获取输出流
// */
//std::ofstream & FileManager::get_output_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if (m_output_stream.find(file_path) == m_output_stream.end())  // 不存在该io 流
//    {
//        m_output_stream[file_path] = std::ofstream(file_path, std::ios::binary);  // 创建
//    }
//    return m_output_stream[file_path];
//}
//
///*
// * 创建输入输出流
// */
//void FileManager::create_input_and_output_stream(
//        const std::string & file_path)
//{
//    create_input_stream(file_path);
//    create_output_stream(file_path);
//}
//
//bool FileManager::create_input_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    auto it = m_input_stream.find(file_path);
//    if (it == m_input_stream.end())  // 没有打开
//    {
//        try
//        {
//            m_input_stream[file_path].open(file_path);
//            if (!m_input_stream[file_path].is_open())  // 检验文件是否被打开
//            {
//                throw std::runtime_error("Error opening file: " + file_path);
//            }
//        }
//        catch (const std::exception & e)
//        {
//            std::cerr << e.what() << std::endl;
//            return false;
//        }
//    }
//    return true;
//}
//
//bool FileManager::create_output_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    auto it = m_output_stream.find(file_path);
//    if (it == m_output_stream.end())
//    {
//        try
//        {
//            m_output_stream[file_path].open(file_path);
//            if (!m_output_stream[file_path].is_open())
//            {
//                throw std::runtime_error("Error opening file: " + file_path);
//            }
//        }
//        catch (const std::exception & e)
//        {
//            std::cerr << e.what() << std::endl;
//            return false;
//        }
//    }
//    return true;
//}
//
//void FileManager::release_input_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    m_input_stream.erase(file_path);
//}
//
//void FileManager::release_output_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    m_output_stream.erase(file_path);
//}
//
//void FileManager::close_input_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    m_input_stream[file_path].close();
//}
//
//void FileManager::close_output_stream(const std::string & file_path)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    m_output_stream[file_path].close();
//}
