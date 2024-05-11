#ifndef DHSTINYSTL_TEST_STACK_H_
#define DHSTINYSTL_TEST_STACK_H_

#include <iostream>
#include <stack>

#include "stack.h"
#include "test.h"

namespace dhsstl {
namespace test {
void stack_print(dhsstl::stack<int> s){
    while(!s.empty()){
        std::cout << " " << s.top();
        s.pop();
    }
    std::cout << std::endl;
}

// stack 的遍历输出
#define STACK_COUT(s) do {                       \
    std::string s_name = #s;                     \
    std::cout << " " << s_name << " :";          \
    stack_print(s);                              \
} while(0)

#define STACK_FUN_AFTER(con, fun) do {           \
  std::string fun_name = #fun;                   \
  std::cout << " After " << fun_name << " :\n";  \
  fun;                                           \
  STACK_COUT(con);                               \
} while(0)

//! @brief Test dhsstl::stack
void stack_test(){
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test :  stack -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5 };
    dhsstl::deque<int> d1(5);
    dhsstl::stack<int> s1;
    dhsstl::stack<int> s2(5);
    dhsstl::stack<int> s3(5, 1);
    dhsstl::stack<int> s4(a, a + 5);
    dhsstl::stack<int> s5(d1);
    dhsstl::stack<int> s6(std::move(d1));
    dhsstl::stack<int> s7(s2);
    dhsstl::stack<int> s8(std::move(s2));
    dhsstl::stack<int> s9;
    s9 = s3;
    dhsstl::stack<int> s10;
    s10 = std::move(s3);
    dhsstl::stack<int> s11{ 1,2,3,4,5 };
    dhsstl::stack<int> s12;
    s12 = { 1,2,3,4,5 };

    STACK_FUN_AFTER(s1, s1.push(1));
    STACK_FUN_AFTER(s1, s1.push(2));
    STACK_FUN_AFTER(s1, s1.push(3));
    STACK_FUN_AFTER(s1, s1.pop());
    STACK_FUN_AFTER(s1, s1.emplace(4));
    STACK_FUN_AFTER(s1, s1.emplace(5));
    FUN_VALUE(s1.empty());
    FUN_VALUE(s1.size());
    FUN_VALUE(s1.top());
    while (!s1.empty())
    {
        STACK_FUN_AFTER(s1, s1.pop());
    }
    STACK_FUN_AFTER(s1, s1.swap(s4));
    STACK_FUN_AFTER(s1, s1.clear());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

} // namespace test
} // namespace dhsstl

#endif
