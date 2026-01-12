#include "gtest/gtest.h"
#include <fstream>


TEST(FooTest, Test1) {
    EXPECT_EQ(1, 1);

    // 写文件
    std::ofstream file("FOO.txt");
    ASSERT_TRUE(file.is_open());
    file << "FOO";
}
