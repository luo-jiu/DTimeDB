//
// Created by illumwang on 12/6/23.
//

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

// 定义行的数据结构
struct Row {
    int timestamp;
    std::string data;

    Row(int _timestamp, const std::string& _data) : timestamp(_timestamp), data(_data) {}
};

// 定义页头的数据结构
struct PageHeader {
    uint32_t pageId;        // 页标识符
    uint16_t pageType;      // 页类型
    uint16_t pageSize;      // 页大小
    uint32_t dataOffset;    // 数据偏移
    uint16_t recordCount;   // 数据记录数
    uint32_t checksum;      // 校验和
    uint64_t timestamp;     // 时间戳
};

// 定义页尾的数据结构
struct PageFooter {
    uint32_t checksum;      // 校验和
};

// 定义页的数据结构
struct Page {
    static const size_t PageSize = 4096;  // 页的大小为4KB
    PageHeader header;                     // 页头
    // ... 其他数据成员
    PageFooter footer;                     // 页尾

    // 计算页的校验和
    uint32_t calculateChecksum() const {
        uint32_t checksum = 0;
        const uint8_t* dataPtr = reinterpret_cast<const uint8_t*>(this);
        for (size_t i = 0; i < PageSize; ++i) {
            checksum += dataPtr[i];
        }
        return checksum;
    }

    // 验证页的完整性
    bool validateIntegrity() const {
        return header.checksum == calculateChecksum();
    }

    // 添加行到页
    void addRow(const Row& row) {
        // ... 添加行到页的逻辑

        // 更新页头信息
        header.recordCount++;
        header.dataOffset += row.data.size();

        // 更新页尾的校验和
        footer.checksum = calculateChecksum();
    }
};

// 定义环形链表的节点
struct ListNode {
    Page data;          // 数据库页
    ListNode* next;     // 下一个节点

    ListNode(const Page& _data) : data(_data), next(nullptr) {}
};

// 定义环形链表的结构体
class CircularLinkedList {
private:
    ListNode* head;    // 链表头
    size_t maxSize;    // 环形链表的最大大小

public:
    CircularLinkedList(size_t _maxSize) : head(nullptr), maxSize(_maxSize) {}

    // 添加行到环形链表
    void addRowToCircularLinkedList(const Row& row) {
        if (!head || head->data.header.recordCount >= Page::PageSize) {
            // 如果链表为空或者当前页已满，创建新的节点
            ListNode* newNode = new ListNode(Page());
            newNode->next = head;

            // 更新链表头
            head = newNode;
        }

        // 添加行到当前页
        head->data.addRow(row);

        // 检查是否超过最大大小，如果是，移除最后一个节点
        if (size() > maxSize) {
            ListNode* lastNode = head;
            while (lastNode->next != nullptr && lastNode->next != head) {
                lastNode = lastNode->next;
            }

            if (lastNode->next != nullptr) {
                ListNode* temp = lastNode->next;
                lastNode->next = temp->next;
                delete temp;
            }
        }
    }

    // 输出所有行数据
    void printAllRows() const {
        std::cout << "All Rows:\n";
        ListNode* current = head;

        do {
            // 输出当前页的所有行数据
            for (const auto& row : current->data.header.recordCount) {
                std::cout << "Timestamp: " << row.timestamp << ", Data: " << row.data << "\n";
            }

            // 移动到下一个节点
            current = current->next;

        } while (current != head);  // 循环链表
    }

    // 获取环形链表的大小
    size_t size() const {
        size_t count = 0;
        ListNode* current = head;

        do {
            ++count;
            current = current->next;
        } while (current != head);

        return count;
    }
};

int main() {
    Page page;
    Row row1(1, "Data 1");
    Row row2(2, "Data 2");

    page.addRow(row1);
    page.addRow(row2);

    // 获取页头数据
    PageHeader pageHeader = page.header;
    std::cout << "Page ID: " << pageHeader.pageId << "\n";
    std::cout << "Page Type: " << pageHeader.pageType << "\n";
    // ... 其他页头数据

    // 获取页尾数据
    PageFooter pageFooter = page.footer;
    std::cout << "Footer Checksum: " << pageFooter.checksum << "\n";

    // 验证页的完整性
    if (page.validateIntegrity()) {
        std::cout << "Page integrity is valid.\n";
    } else {
        std::cerr << "Page integrity check failed.\n";
    }

    return 0;
}
