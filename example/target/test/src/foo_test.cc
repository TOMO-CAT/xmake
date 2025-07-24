#include "gtest/gtest.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace
{
std::string ltrim(const std::string& s)
{
    auto it = std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace(static_cast<unsigned char>(c)); });
    return std::string(it, s.end());
}

std::string rtrim(const std::string& s)
{
    auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace(static_cast<unsigned char>(c)); });
    return std::string(s.begin(), it.base());
}

std::string trim(const std::string& s) { return rtrim(ltrim(s)); }
} // namespace

TEST(FooTest, Test1)
{
    EXPECT_EQ(1, 1);

    // 相对路径读取文件内容
    std::ifstream file("foo.txt");
    ASSERT_TRUE(file.is_open());
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    EXPECT_EQ(trim(content), "foo");
}
