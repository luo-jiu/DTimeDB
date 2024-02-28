#include "my_test.h"
#include "Shard.pb.h"
#include "Person.pb.h"

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
    std::cout << shard.DebugString();
    // 可以进行序列化或其他操作
    std::string serialized_data;
    shard.SerializeToString(&serialized_data);
    Shard ss;
    ss.ParseFromString(serialized_data);
    std::cout << ss.DebugString();

//    Person p;
//    // 设置单个值字段
//    p.set_retention_policy("retentionPolicy");
//    p.set_shard_id("shardID123");
//    p.set_database_name("exampleDB");
//    p.set_measurement("temperature");
//    p.set_curr_file_path("/path/to/current/file");
//    p.set_curr_file_margin(100);
//    p.set_curr_file_head_offset(0);
//    p.set_curr_file_tail_offset(2048);
//
//    // 添加repeated字段的值
//    p.add_tsm_file("file1.tsm");
//    p.add_tsm_file("file2.tsm");
//    p.add_tsm_file("file3.tsm");


//    p.add_id(10);
//    p.add_id(10);
//    p.add_id(10);
//    p.set_age(32);
//    p.set_sex("girl");
//    p.add_name("xxx");
//    p.add_name("yyy");


//    // 序列化person
//    std::string output;
//    p.SerializeToString(&output);
//
//    // 反序列化person
//    Person pp;
//    pp.ParseFromString(output);
//
//    std::cout << pp.DebugString() << std::endl;

}
