#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string inputString = "hello";

    // 打开一个二进制文件以写入模式
    std::ofstream outputFile("binary_file.bin", std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return 1;
    }

    // 将字符串以二进制形式写入文件
    outputFile.write(inputString.c_str(), inputString.size());

    // 关闭文件
    outputFile.close();

    std::cout << "String written to binary file." << std::endl;

    return 0;
}
