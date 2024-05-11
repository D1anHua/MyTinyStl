#ifndef DHSTINYSTL_TEST_DEQUE_H_
#define DHSTINYSTL_TEST_DEQUE_H_

#include <iostream>
#include <deque>

#include "deque.h"
#include "test.h"


namespace dhsstl {
namespace test {
//! @brief Test dhsstl::deque
void deque_test() {
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test  : deque -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5 };
    dhsstl::deque<int> d1;
    dhsstl::deque<int> d2(5);
    dhsstl::deque<int> d3(5, 1);
    dhsstl::deque<int> d4(a, a + 5);
    dhsstl::deque<int> d5(d2);
    dhsstl::deque<int> d6(std::move(d2));
    dhsstl::deque<int> d7;
    d7 = d3;
    dhsstl::deque<int> d8;
    d8 = std::move(d3);
    dhsstl::deque<int> d9{ 1,2,3,4,5,6,7,8,9 };
    dhsstl::deque<int> d10;
    d10 = { 1,2,3,4,5,6,7,8,9 };

    FUN_AFTER(d1, d1.assign(5, 1));
    FUN_AFTER(d1, d1.assign(8, 8));
    FUN_AFTER(d1, d1.assign(a, a + 5));
    FUN_AFTER(d1, d1.assign({ 1,2,3,4,5 }));
    FUN_AFTER(d1, d1.insert(d1.end(), 6));
    FUN_AFTER(d1, d1.insert(d1.end() - 1, 2, 7));
    FUN_AFTER(d1, d1.insert(d1.begin(), a, a + 5));
    FUN_AFTER(d1, d1.erase(d1.begin()));
    FUN_AFTER(d1, d1.erase(d1.begin(), d1.begin() + 4));
    FUN_AFTER(d1, d1.emplace_back(8));
    FUN_AFTER(d1, d1.emplace_front(8));
    FUN_AFTER(d1, d1.emplace(d1.begin() + 1, 9));
    FUN_AFTER(d1, d1.push_front(1));
    FUN_AFTER(d1, d1.push_back(2));
    FUN_AFTER(d1, d1.pop_back());
    FUN_AFTER(d1, d1.pop_front());
    FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.resize(5));
    FUN_AFTER(d1, d1.resize(8, 8));
    FUN_AFTER(d1, d1.clear());
    FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.swap(d4));
    FUN_VALUE(*(d1.begin()));
    FUN_VALUE(*(d1.end() - 1));
    FUN_VALUE(*(d1.rbegin()));
    FUN_VALUE(*(d1.rend() - 1));
    FUN_VALUE(d1.front());
    FUN_VALUE(d1.back());
    FUN_VALUE(d1.at(1));
    FUN_VALUE(d1[2]);
    FUN_VALUE(d1.empty());
    FUN_VALUE(d1.size());
    FUN_VALUE(d1.max_size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

//! @brief Std dhsstl::deque
void deque_std() {
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container std  :  deque -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API Begin -------- -------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5 };
    std::deque<int> d1;
    std::deque<int> d2(5);
    std::deque<int> d3(5, 1);
    std::deque<int> d4(a, a + 5);
    std::deque<int> d5(d2);
    std::deque<int> d6(std::move(d2));
    std::deque<int> d7;
    d7 = d3;
    std::deque<int> d8;
    d8 = std::move(d3);
    std::deque<int> d9{ 1,2,3,4,5,6,7,8,9 };
    std::deque<int> d10;
    d10 = { 1,2,3,4,5,6,7,8,9 };

    FUN_AFTER(d1, d1.assign(5, 1));
    FUN_AFTER(d1, d1.assign(8, 8));
    FUN_AFTER(d1, d1.assign(a, a + 5));
    FUN_AFTER(d1, d1.assign({ 1,2,3,4,5 }));
    FUN_AFTER(d1, d1.insert(d1.end(), 6));
    FUN_AFTER(d1, d1.insert(d1.end() - 1, 2, 7));
    FUN_AFTER(d1, d1.insert(d1.begin(), a, a + 5));
    FUN_AFTER(d1, d1.erase(d1.begin()));
    FUN_AFTER(d1, d1.erase(d1.begin(), d1.begin() + 4));
    FUN_AFTER(d1, d1.emplace_back(8));
    FUN_AFTER(d1, d1.emplace_front(8));
    FUN_AFTER(d1, d1.emplace(d1.begin() + 1, 9));
    FUN_AFTER(d1, d1.push_front(1));
    FUN_AFTER(d1, d1.push_back(2));
    FUN_AFTER(d1, d1.pop_back());
    FUN_AFTER(d1, d1.pop_front());
    FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.resize(5));
    FUN_AFTER(d1, d1.resize(8, 8));
    FUN_AFTER(d1, d1.clear());
    FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.swap(d4));
    FUN_VALUE(*(d1.begin()));
    FUN_VALUE(*(d1.end() - 1));
    FUN_VALUE(*(d1.rbegin()));
    FUN_VALUE(*(d1.rend() - 1));
    FUN_VALUE(d1.front());
    FUN_VALUE(d1.back());
    FUN_VALUE(d1.at(1));
    FUN_VALUE(d1[2]);
    FUN_VALUE(d1.empty());
    FUN_VALUE(d1.size());
    FUN_VALUE(d1.max_size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

} // namespace test
} // namespace dhsstl

#endif
