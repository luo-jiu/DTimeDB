#pragma once

#include <random>
#include <iostream>
#include <chrono>
using namespace std::chrono;

namespace dt
{
    namespace tsm
    {
        /**
         * 测试工具
         */
        class Tool
        {
        public:
            static system_clock::time_point random_time(bool mode);
        };

        /**
         * 生成随机时间戳
         * @param mode 是否需要打印
         */
        system_clock::time_point Tool::random_time(bool mode)
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();  // 获取当前时间点
            // 创建随机数生成器
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 86400);  // 定义时间范围（比如，24小时 = 86400秒）
            int random_seconds = dis(gen);  // 生成一个随机秒数

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
    }
}