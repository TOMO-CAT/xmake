#include <iostream>

// --- Strategy: Extreme Macro Generation and Recursive Macro Abuse ---
// This is pure preprocessor torture. It generates a massive amount of
// text that the compiler has to parse.
//
// 通过调整 x+x 的数量来增加编译时长
#define HUGE_EXPRESSION(x) x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x

#define EXPAND(x) HUGE_EXPRESSION(HUGE_EXPRESSION(HUGE_EXPRESSION(x)))

// Include a lot of large headers to ensure the preprocessor and parser are busy.
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <deque>
#include <forward_list>

// 构造的一个编译很慢的 cc 文件, 用于测试一些特殊场景
int main() {
    // Let's create an extreme amount of code to be parsed.
    int dummy = 4;
    int result = EXPAND(dummy); // This macro call will expand into a massive expression.
    std::cout << "Result of huge expression: " << result << std::endl;

    return 0;
}
