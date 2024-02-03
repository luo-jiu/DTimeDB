#include <iostream>
#include <snappy.h>
#include <fstream>

class CompressedData {
public:
    CompressedData(const std::string& data) : originalData(data) {
        compressData();
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(compressedData.data(), compressedData.size());
            file.close();
            std::cout << "Data saved to file: " << filename << std::endl;
        } else {
            std::cerr << "Error opening file for writing." << std::endl;
        }
    }

    void readFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            compressedData.resize(static_cast<size_t>(size));
            if (file.read(compressedData.data(), size)) {
                std::cout << "Data read from file: " << filename << std::endl;
                decompressData();
            } else {
                std::cerr << "Error reading from file." << std::endl;
            }

            file.close();
        } else {
            std::cerr << "Error opening file for reading." << std::endl;
        }
    }

    void printData() const {
        std::cout << "Original Data: " << originalData << std::endl;
        std::cout << "Decompressed Data: " << decompressedData << std::endl;
    }

private:
    std::string originalData;
    std::string compressedData;
    std::string decompressedData;

    void compressData() {
        snappy::Compress(originalData.data(), originalData.size(), &compressedData);
    }

    void decompressData() {
        snappy::Uncompress(compressedData.data(), compressedData.size(), &decompressedData);
    }
};

int main() {
    std::string originalText = "1001 1002 1003 1004 1005 1006 1007 1008 1009 1010";

    // Create CompressedData object with Snappy compression
    CompressedData compressedData(originalText);

    // Save compressed data to file
    compressedData.saveToFile("compressed_data.snappy");

    // Read compressed data from file and decompress
    CompressedData readData("");
    readData.readFromFile("compressed_data.snappy");

    // Print original and decompressed data
    readData.printData();

    return 0;
}

