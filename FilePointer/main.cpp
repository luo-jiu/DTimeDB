#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>

struct Header {
    uint16_t num_blocks; // number of blocks
};

struct Data_block {
    uint16_t next_block_offset; // next block offset
    char data[255] = {};  // data
};


int main() {
    char *data[] = {"luojiu","luonai","yanyin","heinai","bainai"};

    std::vector<Data_block> memory_blocks;  // create container

    for(int i = 0; i < 5; ++i){
        Data_block block;
        // set offset
        block.next_block_offset = static_cast<uint16_t>((i + 1)* sizeof(Data_block) + sizeof (Header));

        //set data
        strcpy(block.data, data[i]);
        memory_blocks.push_back(block);
    }

    Header header{};  // create head
    header.num_blocks = static_cast<uint16_t>(memory_blocks.size());

    std::ofstream output_file("data.bin", std::ios::binary);
    // write header
    output_file.write(reinterpret_cast<const char*>(&header), sizeof(Header));
    // write data_blocks
    for(const Data_block& block : memory_blocks){
        output_file.write(reinterpret_cast<const char*>(&block), sizeof(Data_block));
    }

    output_file.close();
    std::cout << "落盘结束..." << std::endl;



    std::ifstream input_file("data.bin", std::ios::binary);
    Header read_header{};
    // read header
    input_file.read(reinterpret_cast<char*>(&read_header), sizeof(Header));

    std::cout << "Number of Data_Blocks:" << read_header.num_blocks << std::endl;
    // read data blocks
    for (int i = 0; i < read_header.num_blocks; ++i){
        Data_block read_block;
        input_file.read(reinterpret_cast<char*>(&read_block), sizeof(Data_block));

        std::cout << "Block Data:" << read_block.data << std::endl;
    }

    input_file.close();
    std::cout << "读取结束..." << std::endl;

    return 0;
}