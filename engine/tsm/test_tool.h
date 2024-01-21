#ifndef DTIMEDB_TEST_TOOL_H
#define DTIMEDB_TEST_TOOL_H

#include <random>
#include <thread>
#include <iostream>
using std::string;

#include <chrono>
using namespace std::chrono;

#include <engine/tsm/tsm_ingredient.h>
#include <engine/tsm/write.h>

namespace dt::tsm
{
    /**
     * 测试工具
     */
    class Tool
    {
    public:
        static system_clock::time_point random_time(bool mode);
        static void test_data(Write & m_write, int num, bool timer, bool pri,string & field_name, string & file_path);

        static int rand_int();

        static void write_string_skip_list(SkipList<string> & sl, int num, bool timer);
        static void write_integer_skip_list(SkipList<string> & sl, int num, bool timer);
        static void write_float_skip_list(SkipList<string> & sl, int num, bool timer);

        static void transfer(SkipList<string> & sl, std::shared_ptr<DataBlock> & data_block, bool timer);

        static void start();
        static void end();

    private:
        static high_resolution_clock::time_point m_start;
        static high_resolution_clock::time_point m_end;
    };

    high_resolution_clock::time_point Tool::m_start = system_clock::now();
    high_resolution_clock::time_point Tool::m_end = system_clock::now();

    /**
     * 生成随机时间戳
     * @param mode 是否需要打印
     */
    system_clock::time_point Tool::random_time(bool mode)
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();  // 获取当前时间点
        // 创建随机数生成器
        int random_seconds = rand_int();

        // 添加这些秒数到当前时间点
        std::chrono::system_clock::time_point random_time_point = now + std::chrono::seconds(random_seconds);
        if (mode)
        {
            // 将时间点转换为可读的时间格式（例如：C time）
            time_t random_time = std::chrono::system_clock::to_time_t(random_time_point);
            std::cout << "Random time point: " << std::ctime(&random_time);
        }

        return random_time_point;
    }

    int Tool::rand_int()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 864000);  // 定义时间范围（比如，240小时 = 864000秒）
        return dis(gen);  // 生成一个随机秒数
    }

    /**
     * 生成随机类型随机时间戳的数据
     */
    void Tool::test_data(
            Write & write,
            int num,
            bool timer,
            bool pri,
            string & field_name,
            string & file_path)
    {
        if (timer) start();
        for (size_t i = 0; i < num; ++i)
        {
            int random_seconds = rand_int();
            auto _data = std::to_string(random_seconds);
            write.write(random_time(pri), _data, DataBlock::DATA_STRING, field_name, file_path);  // 写入结束
        }

        // 跳表可能还有没写完的数据
//        write

        if (timer) end();
    }

    void Tool::write_string_skip_list(SkipList<string> & sl, int nums, bool timer)
    {
        if (timer) start();
        for (size_t i = 0; i < nums; ++i)
        {
            auto _num = rand_int();
            auto _data = std::to_string(_num);
            sl.put(random_time(false), _data);
        }
        if (timer) end();
    }

    void Tool::write_integer_skip_list(SkipList<string> & sl, int nums, bool timer)
    {
        if (timer) start();
        for (size_t i = 0; i < nums; ++i)
        {
            auto random_int = rand_int();
            auto _num = std::to_string(random_int);
            sl.put(random_time(false), _num);
        }
        if (timer) end();
    }

    void Tool::write_float_skip_list(SkipList<string> & sl, int nums, bool timer)
    {
        if (timer) start();
        for (size_t i = 0; i < nums; ++i)
        {
            auto random_int = rand_int();
            float min = 0.0f;
            float max = 10.0f;
            float random_float = min + static_cast<float>(random_int) / (RAND_MAX / (max - min));
            auto _num = std::to_string(random_float);

            sl.put(random_time(false), _num);
        }
        if (timer) end();
    }

    void Tool::transfer(
            SkipList<string> & sl,
            std::shared_ptr<DataBlock> & data_block,
            bool timer)
    {
        if (timer) start();
        for (auto it = sl.begin(); it != sl.end(); ++it)  // 迭代器
        {
            if (it != sl.end())  // 校验
            {
                auto key_value = *it;
                data_block->write(key_value.first, key_value.second);
            }
        }
        if (timer) end();
    }

    /**
     * 计算执行时间
     */
    void Tool::start()
    {
        m_start = system_clock::now();
    }

    void Tool::end()
    {
        m_end = system_clock::now();
        // 计算时间差
        auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start);
        double seconds = static_cast<double>(duration_ms.count()) / 1000000.0;
        // 打印运行时间
        std::cout << "代码块运行时间: " << seconds << " 秒; " << duration_ms.count() << " 微秒" << std::endl;
    }
}

#endif //DTIMEDB_TEST_TOOL_H
