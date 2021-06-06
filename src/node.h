#ifndef NODE_H
#define NODE_H

//存储结点
template<typename K, typename V>
class Node
{
public:

    Node() {}

    Node(K key, V value, int);

    ~Node();

    k get_key() const;

    v get_value() const;

    void set_value(V);

    //线性数组用于保存下一个不同level的数组指针
    Node<K, V> **forward;

    int node_level;

private:

    K key;
    V value;
};
#endif
