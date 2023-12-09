//
// Created by illumwang on 12/6/23.
//

#include <iostream>
#include <vector>
#include <fstream>

// 定义行的数据结构
struct Row {
    int id;
    std::string data;

    Row(int _id, const std::string& _data) : id(_id), data(_data) {}
};

// 定义数据库页的结构体
struct DatabasePage {
    static const size_t PageSize = 4096;  // 页的大小为4KB
    std::vector<Row> rows;  // 页内存储的行数据

    // 添加行到页
    bool addRow(const Row& row) {
        // 检查是否有足够的空间添加行
        if (getSize() + sizeof(Row) <= PageSize) {
            rows.push_back(row);
            return true;
        }
        return false;
    }

    // 获取页的大小
    size_t getSize() const {
        return rows.size() * sizeof(Row);
    }

    // 将页的数据写入磁盘
    void writePageToDisk(size_t pageIndex) {
        // 假设每个页写入一个文件
        std::ofstream file("page_" + std::to_string(pageIndex) + ".bin", std::ios::binary);
        if (file.is_open()) {
            // 写入页的数据
            for (const auto& row : rows) {
                file.write(reinterpret_cast<const char*>(&row), sizeof(Row));
            }
            file.close();
            std::cout << "Page " << pageIndex << " written to disk.\n";
        } else {
            std::cerr << "Error writing to disk.\n";
        }
    }
};

// 定义环形链表的节点
struct ListNode {
    DatabasePage data;    // 数据库页
    ListNode* next;       // 下一个节点

    ListNode(const DatabasePage& _data) : data(_data), next(nullptr) {}
};

// 定义环形链表的结构体
struct CircularLinkedList {
    static const size_t MaxPages = 5;  // 环形链表的最大页数
    ListNode* head;                     // 链表头
    size_t pageCount;                   // 当前链表中的页数

    CircularLinkedList() : head(nullptr), pageCount(0) {}

    // 添加行到环形链表
    void addRowToCircularLinkedList(const Row& row) {
        if (!head || !head->data.addRow(row)) {
            // 如果链表为空或者当前页已满，创建新的节点
            ListNode* newNode = new ListNode(DatabasePage());
            newNode->next = head;

            // 更新链表头和页数
            head = newNode;
            ++pageCount;
        }

        // 添加行到当前页
        head->data.addRow(row);
    }

    // 将环形链表的数据写入磁盘
    void writeCircularLinkedListToDisk() {
        ListNode* current = head;
        size_t pageIndex = 0;

        do {
            // 将页的数据写入磁盘
            current->data.writePageToDisk(pageIndex);

            // 移动到下一个节点
            current = current->next;
            ++pageIndex;

        } while (current != head);  // 循环链表

        // 释放链表节点的内存
        while (head) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main() {
    // 创建环形链表存储引擎
    CircularLinkedList circularLinkedList;

    // 添加一些行到环形链表
    for (int i = 0; i < 20; ++i) {
        Row row(i, "Data for row " + std::to_string(i));
        circularLinkedList.addRowToCircularLinkedList(row);
    }

    // 将环形链表的数据写入磁盘
    circularLinkedList.writeCircularLinkedListToDisk();

    return 0;
}
