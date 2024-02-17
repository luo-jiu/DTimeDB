#include <iostream>
#include <vector>
#include <map>

// 数据点结构
struct DataPoint {
    long long timestamp; // 时间戳
    double value;        // 数据值
};

// 模拟列存储的字段
struct Field {
    std::string name;
    std::vector<DataPoint> data; // 按时间顺序存储的数据点
};

// 时间戳对齐并合并数据
std::map<long long, std::map<std::string, double>> alignTimestamps(const std::vector<Field>& fields) {
    std::map<long long, std::map<std::string, double>> alignedData;

    // 遍历每个字段
    for (const auto& field : fields) {
        // 遍历每个数据点
        for (const auto& point : field.data) {
            alignedData[point.timestamp][field.name] = point.value;
        }
    }

    return alignedData;
}

int main() {
    // 示例：两个字段的时间序列数据
    Field temperature = {"Temperature", {{1, 100.0}, {2, 101.0}, {3, 102.0}}};
    Field humidity = {"Humidity", {{2, 40.5}, {3, 41.0}, {4, 42.0}}};

    // 模拟列式存储
    std::vector<Field> fields = {temperature, humidity};

    // 对齐时间戳并合并数据
    auto alignedData = alignTimestamps(fields);

    // 输出对齐的数据
    for (const auto& entry : alignedData) {
        std::cout << "Timestamp: " << entry.first;
        for (const auto& fieldData : entry.second) {
            std::cout << " | " << fieldData.first << ": " << fieldData.second;
        }
        std::cout << std::endl;
    }

    return 0;
}
