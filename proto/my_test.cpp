#include "my_test.h"
#include "Shard.pb.h"

void dt::test::MyTest::test()
{
    Shard shard;
    // 设置单个值字段
    shard.set_retention_policy("retentionPolicy");
    shard.set_shard_id("shardID123");
    shard.set_database_name("exampleDB");
    shard.set_measurement("temperature");
    shard.set_curr_file_path("/path/to/current/file");
    shard.set_curr_file_margin(100);
    shard.set_curr_file_head_offset(0);
    shard.set_curr_file_tail_offset(2048);

    // 添加repeated字段的值
    shard.add_tsm_file("file1.tsm");
    shard.add_tsm_file("file2.tsm");
    shard.add_tsm_file("file3.tsm");

    // 此时，shard对象已经包含了你设置的所有数据

    // 可以进行序列化或其他操作
    std::string serialized_data;
    shard.SerializeToString(&serialized_data);

    // 如果需要，还可以遍历repeated字段
    for (int i = 0; i < shard.tsm_file_size(); ++i)
    {
        const auto& file = shard.tsm_file(i); // 获取第i个tsm文件的名称
        // 可以在这里处理每个tsm文件的名称
        std::cout << file << std::endl;
    }
}
