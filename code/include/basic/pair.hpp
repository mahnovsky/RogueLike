#pragma once

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

}
