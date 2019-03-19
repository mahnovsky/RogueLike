#pragma once

#include <gtest/gtest.h>

#include "basic/hash_map.hpp"

class TestHashMap : public ::testing::Test {
public:
    TestHashMap() { /* init protected members here */ }
    ~TestHashMap() { /* free protected members here */ }
    void SetUp() { /* called before every test */ }
    void TearDown() { /* called after every test */ }

protected:
    /* none yet */
};

