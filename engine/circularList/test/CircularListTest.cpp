//
// Created by illumwang on 12/6/23.
//
#include "iostream"
//#include "engine/circularList/include/circular_list_header.h"
using namespace std;
//int main(){
//    /**
//     * 分页存储测试
//     */
//    Meta meta(1,"row1");
//    Timestamp timestamp;
//    Row row(meta,timestamp);
//    row.add_field(DATA_TYPE::INTEGER,"1");
//    row.add_field(DATA_TYPE::STRING,"Myclass");
//
//    Page page(1,"example",PAGE_SIZE,PAGE_TYPE::DATA,42);
//    if (page.insert_row(row)){
//        cout<<"successfully"<<endl;
//        row.display_row();
//    } else{
//        cout<<"false"<<endl;
//    }
//    vector<Row> data=page.get_page_data();
//    for (const Row &r:data) {
//        r.display_row();
//    }
//    return 0;
//}
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

enum class DATA_TYPE { INTEGER, FLOAT, STRING };

class Column {
private:
    std::string name;
    DATA_TYPE type;

public:
    Column(const std::string& _name, DATA_TYPE _type) : name(_name), type(_type) {}

    const std::string& getName() const {
        return name;
    }

    DATA_TYPE getType() const {
        return type;
    }
};

class Row {
private:
    std::vector<std::string> values;
    std::chrono::system_clock::time_point timestamp; // 添加时间戳成员

public:
    Row(const std::vector<std::string>& _values) : values(_values), timestamp(std::chrono::system_clock::now()) {}

    const std::vector<std::string>& getValues() const {
        return values;
    }

    const std::chrono::system_clock::time_point& getTimestamp() const {
        return timestamp;
    }
};

class Table {
private:
    std::vector<Column> columns;
    std::vector<Row> rows;

public:
    void addColumn(const std::string& name, DATA_TYPE type) {
        columns.emplace_back(name, type);
    }

    void addRow(const std::vector<std::string>& values) {
        if (values.size() != columns.size()) {
            std::cerr << "Error: Number of values does not match the number of columns." << std::endl;
            return;
        }

        rows.emplace_back(values);
    }

    void displayTable() const {
        // 打印表头
        for (const auto& column : columns) {
            std::cout << column.getName() << "\t";
        }
        std::cout << "Timestamp" << std::endl;

        // 打印数据
        for (const auto& row : rows) {
            const auto& values = row.getValues();
            for (const auto& value : values) {
                std::cout << value << "\t";
            }
            // 打印时间戳tx
            std::cout << std::chrono::system_clock::to_time_t(row.getTimestamp());
            std::cout << std::endl;
        }
    }
};

int main() {
    // 创建表
    Table myTable;

    // 添加列
    myTable.addColumn("ID", DATA_TYPE::INTEGER);
    myTable.addColumn("Name", DATA_TYPE::STRING);
    myTable.addColumn("Age", DATA_TYPE::INTEGER);

    // 添加数据
    myTable.addRow({"1", "John Doe", "25"});
    myTable.addRow({"2", "Jane Smith", "30"});

    // 显示表格
    myTable.displayTable();

    return 0;
}
