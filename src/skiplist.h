#ifndef SKIPLIST_CPP_MASTER_SKIPLIST_H
#define SKIPLIST_CPP_MASTER_SKIPLIST_H

#include "node.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define STORE_FILE "../storefile/dumpFile"
std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

// 跳跃链表
template <typename K, typename V>
class SkipList
{
public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:
    // 跳表的最大level
    int _max_level;

    // 跳表的当前level
    int _skip_list_level;

    //头结点
    Node<K, V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // 跳表当前元素数量
    int _element_count;
};

#endif
