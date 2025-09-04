#include <iostream>

// --- Strategy 1: Extreme Template Recursion ---
// We'll create a factorial calculation at compile-time.
// Let's go to a very high number to force deep recursion.
template<int N>
struct Factorial {
    static const unsigned long long value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static const unsigned long long value = 1;
};

// --- Strategy 2: Massive Template Instantiation Loop ---
// Use a macro to generate hundreds or even thousands of unique template instantiations.
// We will fix the 'no object' error by creating a dummy object for each call.
template<int N>
struct ComplexType {
    int data[N];
    void do_something() {
        // A dummy function to ensure the compiler generates code for it.
        // We will make it slightly more complex to increase work.
        for (int i = 0; i < N; ++i) {
            data[i] = i * 1.5; // Use a float to force more complex operations.
        }
    }
};

// Define a macro to create an object and call its method.
// We'll declare a dummy variable of a unique type to force instantiation.
#define INSTANTIATE_TYPES(base_num) \
    ComplexType<base_num * 10> obj_##base_num##_0; \
    obj_##base_num##_0.do_something(); \
    ComplexType<base_num * 10 + 1> obj_##base_num##_1; \
    obj_##base_num##_1.do_something(); \
    ComplexType<base_num * 10 + 2> obj_##base_num##_2; \
    obj_##base_num##_2.do_something(); \
    ComplexType<base_num * 10 + 3> obj_##base_num##_3; \
    obj_##base_num##_3.do_something(); \
    ComplexType<base_num * 10 + 4> obj_##base_num##_4; \
    obj_##base_num##_4.do_something(); \
    ComplexType<base_num * 10 + 5> obj_##base_num##_5; \
    obj_##base_num##_5.do_something(); \
    ComplexType<base_num * 10 + 6> obj_##base_num##_6; \
    obj_##base_num##_6.do_something(); \
    ComplexType<base_num * 10 + 7> obj_##base_num##_7; \
    obj_##base_num##_7.do_something(); \
    ComplexType<base_num * 10 + 8> obj_##base_num##_8; \
    obj_##base_num##_8.do_something(); \
    ComplexType<base_num * 10 + 9> obj_##base_num##_9; \
    obj_##base_num##_9.do_something();

// --- Strategy 3: Extreme Macro Generation and Recursive Macro Abuse ---
// This is pure preprocessor torture. It generates a massive amount of
// text that the compiler has to parse.
#define HUGE_EXPRESSION(x) \
    x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x

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
    // Force the compiler to perform a very deep factorial calculation at compile-time.
    std::cout << "Compile-time Factorial(200) value: " << Factorial<200>::value << std::endl;

    // Call the macro loop to create thousands of unique instantiations.
    // We'll include a lot of these calls to generate a lot of code.
    INSTANTIATE_TYPES(1)
    INSTANTIATE_TYPES(2)
    INSTANTIATE_TYPES(3)
    INSTANTIATE_TYPES(4)
    INSTANTIATE_TYPES(5)
    INSTANTIATE_TYPES(6)
    INSTANTIATE_TYPES(7)
    INSTANTIATE_TYPES(8)
    INSTANTIATE_TYPES(9)
    INSTANTIATE_TYPES(10)
    // ... Copy and paste the above block 10 times to generate 1000 instantiations
    // You can also use a script to generate the calls.
    // For now, I'll just show the first few to demonstrate the fix.
    // To get a huge time, you'll want to have 100+ calls to INSTANTIATE_TYPES.

    // Let's create an extreme amount of code to be parsed.
    int dummy = 10;
    int result = EXPAND(dummy); // This macro call will expand into a massive expression.
    std::cout << "Result of huge expression: " << result << std::endl;

    // Use a very large vector to force the compiler to do more work.
    std::vector<long long> v(1000000);
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = i;
    }

    std::sort(v.begin(), v.end());

    std::map<double, int> m;
    m[1.2] = 1;
    m[3.4] = 2;

    return 0;
}
