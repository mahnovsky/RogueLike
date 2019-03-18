#pragma once

#include "vector.hpp"

namespace basic
{

template<typename K, typename V>
struct Pair
{
    K key;
    V value;

    Pair()
        :key()
        ,value()
    {}

    Pair(const K& k, const V& v)
        :key(k)
        ,value(v)
    {}
};

template<typename K, typename V>
class Map
{
    struct Node
    {
        basic::int32 parent = -1;
        basic::int32 left = -1;
        basic::int32 right = -1;
        basic::uint32 index : 30, is_empty : 1, is_red : 1;

        Pair<K, V> item;
    };

    Node* parentn(Node* n)
    {
        return &m_nodes[n->parent];
    }

    Node* leftn(Node* n)
    {
        return &m_nodes[n->left];
    }

    Node* rightn(Node* n)
    {
        return &m_nodes[n->right];
    }
public:
    Map()
        : m_root(nullptr)
        , m_nodes()
    {
        m_root = create_node(nullptr);
    }

    void insert(const Pair<K, V>& item)
    {
        Node* insert_node = find_last(m_root, item.key);
        ASSERT(insert_node);

        if(!insert_node->is_empty)
        {
            insert_node = create_node(insert_node);
        }

        insert_node->is_empty = false;
        insert_node->item = item;
    }

    template<typename ... Args>
    void emplace(Args ... args)
    {
        insert(Pair<K, V>(args ...));
    }

    bool find(const K& key, V& out)
    {
        Node* node = find_last(&m_nodes[0], key);

        if( node->item.key == key )
        {
            out = node->item.value;
            return true;
        }
        return false;
    }
private:
    Node* create_node(Node* parent)
    {
        Node n;
        n.is_red = 0;
        if(parent)
        {
            n.parent = parent->index;
            n.is_red = !parent->is_red;
        }
        n.index = m_nodes.get_size();
        m_nodes.push(n);

        return &m_nodes.back();
    }

    Node* find_last(Node* from, const K& key)
    {
        if(from->is_empty)
        {
            return from;
        }

        if(key < from->item.key && from->left > 0)
        {
            return find_last(leftn(from), key);
        }
        else if(from->right > 0)
        {
            return find_last(rightn(from), key);
        }

        return from;
    }

    Node* grandparent(Node* n)
    {
        Node* p = parentn(n);
        if(p)
        {
            return parentn(p);
        }

        return nullptr;
    }

    Node* uncle(Node* n)
    {
        Node* g = grandparent(n);
        if(!g)
        {
            return  nullptr;
        }
        if(leftn(g) == parentn(n))
        {
            return rightn(g);
        }
        return leftn(g);
    }
private:
    Node* m_root;
    basic::Vector<Node> m_nodes;
};

}
