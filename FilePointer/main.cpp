#include <iostream>
#include <vector>
#include <fstream>
#include "page_block.h"


int main() {

    auto streams = get_new_page("area.bin", static_cast<std::streampos>(0));
    std::ofstream& output= streams.first;
    std::ifstream& input = streams.second;

    char *data[] = {"luojiu","luonai","yanyin","heinai","bainai"};
    write_page(std::move(output), std::move(input), data, 5);

    read_page("area.bin",static_cast<std::streampos>(0));
}


