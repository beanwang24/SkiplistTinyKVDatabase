#include "skiplist.h"


// 构造跳表
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level)
{
    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
};

//创建一个新的结点
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level)
{
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

//析构
template<typename K, typename V>
SkipList<K, V>::~SkipList()
{
    if (_file_writer.is_open())
    {
        _file_writer.close();
    }
    if (_file_reader.is_open())
    {
        _file_reader.close();
    }
    delete _header;
}

//从链表中存入数据，返回1为数据已经存在，0为存入成功
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
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value)
{
    mtx.lock();
    Node<K, V> *current = this->_header;

    //创建一个update数组并初始化
    //临时数组update[]来记录位置信息
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

    //如果当前结点的key等于查找的key，则存已在
    if (current != NULL && current->get_key() == key)
    {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    // 如果当前结点为NULL说明已经到了末尾
    //如果当前的key不等于需要插入的kry则需要在update[0]和当前结点之间插入
    if (current == NULL || current->get_key() != key )
    {
        //生成一个随即的level
        int random_level = get_random_level();

        //如果生成的随机level大于跳表的当前level，超出部分用head初始化
        if (random_level > _skip_list_level)
        {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // 创建一个新的结点，结点level为前面生成的random_level
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // 插入结点
        for (int i = 0; i <= random_level; i++)
        {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count ++;
    }
    mtx.unlock();
    return 0;
}

// 从链表中查找元素
template<typename K, typename V>
bool SkipList<K, V>::search_element(K key)
{
    std::cout << "search_element-----------------" << std::endl;
    Node<K, V> *current = _header;

    // 从链表的最高level开始
    for (int i = _skip_list_level; i >= 0; i--)
    {
        while (current->forward[i] && current->forward[i]->get_key() < key)
        {
            current = current->forward[i];
        }
    }

    //使用level0
    current = current->forward[0];

    // 如果当前结点的值等于key 则找到
    if (current and current->get_key() == key)
    {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

// 从跳表中删除元素
template<typename K, typename V>
void SkipList<K, V>::delete_element(K key)
{
    mtx.lock();
    Node<K, V> *current = this->_header;
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    // 从跳表的最高level开始
    for (int i = _skip_list_level; i >= 0; i--)
    {
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != NULL && current->get_key() == key)
    {
        // 从最低level开始删除
        for (int i = 0; i <= _skip_list_level; i++)
        {
            //如果指向的不是current了，退出
            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // 如果顶层level没有元素了，level--
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0)
        {
            _skip_list_level --;
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        _element_count --;
    }
    mtx.unlock();
    return;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level(){

    int k = 1;
    while (rand() % 2)
    {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};

template<typename K, typename V>
int SkipList<K, V>::size()
{
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value)
{
    if(!is_valid_string(str))
    {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str)
{
    if (str.empty())
    {
        return false;
    }
    if (str.find(delimiter) == std::string::npos)
    {
        return false;
    }
    return true;
}

// 显示跳表
template<typename K, typename V>
void SkipList<K, V>::display_list()
{
    std::cout << "\n*****Skip List*****"<<"\n";
    for (int i = 0; i <= _skip_list_level; i++)
    {
        Node<K, V> *node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != NULL)
        {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// 转储
template<typename K, typename V>
void SkipList<K, V>::dump_file()
{

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0];

    while (node != NULL)
    {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return ;
}

template<typename K, typename V>
void SkipList<K, V>::load_file()
{
    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line))
    {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty())
        {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
}
