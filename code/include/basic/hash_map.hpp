#pragma once

#include "pair.hpp"
#include "vector.hpp"
#include "string.hpp"

namespace basic
{

template<typename K>
struct HashFn
{
    static uint32 hash(const K& key){return 0;}
};

template<>
struct HashFn<uint32>
{
    static uint32 hash(const uint32& in)
    {
        uint32 res = (in + 32) ^ in;

        return (res >> 1);
    }
};

template<>
struct HashFn<int32>
{
    static uint32 hash(const int32& in)
    {
        uint32 res = (in + 32) ^ in;

        return (res >> 1);
    }
};

template<>
struct HashFn<String>
{
    static uint32 hash(const String& in)
    {
        uint32 res = 0;
        for(uint32 i = 0; i < in.get_size(); ++i)
        {
            uint32 ch = in[i];
            res += ((ch + 32 * i) ^ ch);
        }
        return res;
    }
};

template <typename K, typename V, typename HASH_FN=HashFn<K>>
class HashMap
{
    using InternalPair = Pair<K, V>;

    struct Bucket
    {
        static constexpr uint32 MAX_ELEMENTS = 4;
        uint32 count = 0;
        InternalPair elements[MAX_ELEMENTS];

        uint32 find_index(const K& key)
        {
            for(uint32 i = 0; i < count; ++i)
            {
                if(key == elements[i].key)
                {
                    return i;
                }
            }
            return MAX_ELEMENTS;
        }

        bool insert(InternalPair& p)
        {
            uint32 index = find_index(p.key);
            bool result = (index + 1) < MAX_ELEMENTS;

            if(result)
            {
               elements[index] = std::move(p);
            }

            if(!result && (count + 1) < MAX_ELEMENTS)
            {
                elements[count++] = std::move(p);
                result = true;
            }

            return result;
        }

        void remove(uint32 index)
        {
            uint32 last = count - 1;
            if(index < last)
            {
                elements[index] = elements[last];
            }

            --count;
        }

        bool find_by_index(uint32 index, Pair<K, V>** out)
        {
            for(uint32 i = 0; i < count; ++i)
            {
                if(elements[i].index == index)
                {
                    (*out) = &elements[i].p;
                    return true;
                }
            }
            return false;
        }
    };

    void rebuild()
    {
        basic::Vector<Bucket> new_table;
        basic::uint32 len = m_table.get_size();
        new_table.resize(len * 2);

        for(uint32 i = 0; i < m_table.get_size(); ++i)
        {
            Bucket& b = m_table[i];
            for(uint32 j = 0; j < b.count; ++j)
            {
                internal_insert(new_table, std::move(m_table[i].elements[j]));
            }
        }
        m_table = std::move(new_table);
    }

    void internal_insert(basic::Vector<Bucket>& table, InternalPair&& p)
    {
        uint32 pos = get_pos(p.key, table.get_size());

        if( table[pos].insert( p ) )
        {
            ++m_size;
        }
        else
        {
            rebuild();
            internal_insert(table, std::move(p));
        }
    }

    uint32 get_pos(const K& key, uint32 table_len)
    {
        uint32 hash_val = HASH_FN::hash(key);
        uint32 pos = hash_val % table_len;

        return pos;
    }

public:
    HashMap()
        :m_size(0)
        ,m_table()
    {
        m_table.resize(8);
    }

    uint32 get_size() const
    {
        return m_size;
    }

    void insert(const K& key, const V& val)
    {
        InternalPair p {key, val};
        internal_insert(m_table, std::move(p));
    }

    bool find(const K& key, V& out)
    {
        uint32 pos = get_pos(key, m_table.get_size());

        Bucket& b = m_table[pos];
        uint32 index = b.find_index(key);

        if(index != Bucket::MAX_ELEMENTS)
        {
            out = b.elements[index].value;

            return true;
        }

        return false;
    }

    bool contains(const K& key)
    {
        uint32 pos = get_pos(key, m_table.get_size());

        Bucket& b = m_table[pos];
        uint32 index = b.find_index(key);

        return index != Bucket::MAX_ELEMENTS;
    }

    void remove(const K& key)
    {
        uint32 pos = get_pos(key, m_table.get_size());

        Bucket& b = m_table[pos];
        uint32 index = b.find_index(key);

        if( index != Bucket::MAX_ELEMENTS )
        {
            b.remove(index);
            --m_size;
        }
    }

private:
    basic::uint32 m_size;
    basic::Vector<Bucket> m_table;
};

}
