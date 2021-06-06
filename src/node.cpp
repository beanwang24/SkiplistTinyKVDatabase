#include "node.h"

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
{
    this->key = k;
    this->value = v;
    this->node_level = level;

    // level + 1, because array index is from 0 - level
    this->forward = new Node<K, V> * [level + 1];

    // Fill forward array with 0(NULL)
    memset(this->forward, 0, sizeof(Node<K, V >*)*(level + 1));
};

template<typename K, typename V>
Node<K, V>::~Node()
{
    delete []forward;
};

template<typename K, typename V>
K Node<K, V>::get_key() const
{
    return key;
};

template<typename K, typename V>
void Node<K, V>::set_value(V value)
{
    this->value=value;
};