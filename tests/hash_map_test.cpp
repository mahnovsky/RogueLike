#include "hash_map_test.hpp"
#include "basic/string.hpp"

using namespace basic;

TEST_F(TestHashMap, Insert)
{
    HashMap<int, char> m;
    m.insert(10, 'a');
    m.insert(9, 'b');

    EXPECT_EQ(2, m.get_size());
    EXPECT_TRUE(m.contains(10));
    EXPECT_TRUE(m.contains(9));
    EXPECT_FALSE(m.contains(1));
}

TEST_F(TestHashMap, Insert_String)
{
    HashMap<String, String> m;
    m.insert("hello", "a");
    m.insert("world", "b");

    EXPECT_EQ(2, m.get_size());
    EXPECT_TRUE(m.contains("hello"));
    EXPECT_TRUE(m.contains("world"));
    EXPECT_FALSE(m.contains("hi"));
    String res;
    EXPECT_TRUE(m.find("hello", res));
    EXPECT_EQ(res, "a");
}

TEST_F(TestHashMap, Remove)
{
    HashMap<int, char> m;
    m.insert(10, 'a');
    m.insert(9, 'b');

    EXPECT_EQ(2, m.get_size());
    EXPECT_TRUE(m.contains(10));
    EXPECT_TRUE(m.contains(9));
    m.remove(10);
    EXPECT_FALSE(m.contains(10));
    EXPECT_EQ(1, m.get_size());
    m.remove(9);
    EXPECT_FALSE(m.contains(9));
    EXPECT_EQ(0, m.get_size());
}
