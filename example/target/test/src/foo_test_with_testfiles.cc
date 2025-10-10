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

void check_file_content(const std::string& file_path, const std::string& expected_content,
                        testing::Test* const test_context)
{
    // 相对路径读取文件内容
    std::ifstream file(file_path);
    ASSERT_TRUE(file.is_open()) << "file [" << file_path << "] open failed";
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    EXPECT_EQ(expected_content, trim(content));
}

} // namespace

TEST(FooTest, Test1) {
    EXPECT_EQ(1, 1);

    check_file_content("foo.txt", "foo", this);
    check_file_content("bar/bar.txt", "bar", this);
    check_file_content("new_path.txt", "new_path", this);
    check_file_content("folder_1/folder_2/new_path.txt", "new_path", this);
    check_file_content("copy_foo.txt", "foo", this);
}
