#include "skiplist.h"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

//创建一个新的结点
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level)
{
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}



//存入一个数据，返回1为数据已经存在，0为存入成功
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+
*/
int SkipList<K, V>::insert_element(const K key, const V value)
{
    mtx.lock();
    Node<K, V> *current = this->_header;

    //创建一个update数组并初始化
    Node<K, V> *update[_max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    //从跳表的最高level开始
    for(int i = _skip_list_level; i >= 0; i--)
    {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    //从level 0开始往右，找位置插入
    current = current->forward[0];
}