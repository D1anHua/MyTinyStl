#ifndef DHSTINYSTL_TEST_SET_H_
#define DHSTINYSTL_TEST_SET_H_
#include <iostream>
#include <set>
#include <functional>

#include "set.h"
#include "test.h"


namespace dhsstl {
namespace test {

// ---------------------------------------------------------------------------------------------------------------------
//! @brief Test dhsstl::set
void set_test(){
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test : Set ----------------------------]" << std::endl;
    std::cout << "[--------------------------- ------- API test ------- ----------------------------]" << std::endl;
        int a[] = { 5,4,3,2,1 };
    dhsstl::set<int> s1;
    dhsstl::set<int, std::greater<int>> s2;
    dhsstl::set<int> s3(a, a + 5);
    dhsstl::set<int> s4(a, a + 5);
    dhsstl::set<int> s5(s3);
    dhsstl::set<int> s6(std::move(s3));
    dhsstl::set<int> s7;
    s7 = s4;
    dhsstl::set<int> s8;
    s8 = std::move(s4);
    dhsstl::set<int> s9{ 1,2,3,4,5 };
    dhsstl::set<int> s10;
    s10 = { 1,2,3,4,5 };

    for (int i = 5; i > 0; --i)
    {
        FUN_AFTER(s1, s1.emplace(i));
    }
    FUN_AFTER(s1, s1.emplace_hint(s1.begin(), 0));
    FUN_AFTER(s1, s1.erase(s1.begin()));
    FUN_AFTER(s1, s1.erase(0));
    FUN_AFTER(s1, s1.erase(1));
    FUN_AFTER(s1, s1.erase(s1.begin(), s1.end()));
    for (int i = 0; i < 5; ++i)
    {
        FUN_AFTER(s1, s1.insert(i));
    }
    FUN_AFTER(s1, s1.insert(a, a + 5));
    FUN_AFTER(s1, s1.insert(5));
    FUN_AFTER(s1, s1.insert(s1.end(), 5));
    FUN_VALUE(s1.count(5));
    FUN_VALUE(*s1.find(3));
    FUN_VALUE(*s1.lower_bound(3));
    FUN_VALUE(*s1.upper_bound(3));
    auto first = *s1.equal_range(3).first;
    auto second = *s1.equal_range(3).second;
    std::cout << " s1.equal_range(3) : from " << first << " to " << second << std::endl;
    FUN_AFTER(s1, s1.erase(s1.begin()));
    FUN_AFTER(s1, s1.erase(1));
    FUN_AFTER(s1, s1.erase(s1.begin(), s1.find(3)));
    FUN_AFTER(s1, s1.clear());
    FUN_AFTER(s1, s1.swap(s5));
    FUN_VALUE(*s1.begin());
    FUN_VALUE(*s1.rbegin());
    FUN_VALUE(s1.empty());
    FUN_VALUE(s1.size());
    FUN_VALUE(s1.max_size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------
//! @brief Test dhsstl::multiset
    void multiset_test(){
        std::cout << "[=================================================================================]" << std::endl;
        std::cout << "[-------------------------- Run container test : multiset ------------------------]" << std::endl;
        std::cout << "[-------------------------- -------    API test   ------- ------------------------]" << std::endl;
        int a[] = { 5,4,3,2,1 };
        dhsstl::set<int> s1;
        dhsstl::set<int, std::greater<int>> s2;
        dhsstl::set<int> s3(a, a + 5);
        dhsstl::set<int> s4(a, a + 5);
        dhsstl::set<int> s5(s3);
        dhsstl::set<int> s6(std::move(s3));
        dhsstl::set<int> s7;
        s7 = s4;
        dhsstl::set<int> s8;
        s8 = std::move(s4);
        dhsstl::set<int> s9{ 1,2,3,4,5 };
        dhsstl::set<int> s10;
        s10 = { 1,2,3,4,5 };

        for (int i = 5; i > 0; --i)
        {
            FUN_AFTER(s1, s1.emplace(i));
        }
        FUN_AFTER(s1, s1.emplace_hint(s1.begin(), 0));
        FUN_AFTER(s1, s1.erase(s1.begin()));
        FUN_AFTER(s1, s1.erase(0));
        FUN_AFTER(s1, s1.erase(1));
        FUN_AFTER(s1, s1.erase(s1.begin(), s1.end()));
        for (int i = 0; i < 5; ++i)
        {
            FUN_AFTER(s1, s1.insert(i));
        }
        FUN_AFTER(s1, s1.insert(a, a + 5));
        FUN_AFTER(s1, s1.insert(5));
        FUN_AFTER(s1, s1.insert(s1.end(), 5));
        FUN_VALUE(s1.count(5));
        FUN_VALUE(*s1.find(3));
        FUN_VALUE(*s1.lower_bound(3));
        FUN_VALUE(*s1.upper_bound(3));
        auto first = *s1.equal_range(3).first;
        auto second = *s1.equal_range(3).second;
        std::cout << " s1.equal_range(3) : from " << first << " to " << second << std::endl;
        FUN_AFTER(s1, s1.erase(s1.begin()));
        FUN_AFTER(s1, s1.erase(1));
        FUN_AFTER(s1, s1.erase(s1.begin(), s1.find(3)));
        FUN_AFTER(s1, s1.clear());
        FUN_AFTER(s1, s1.swap(s5));
        FUN_VALUE(*s1.begin());
        FUN_VALUE(*s1.rbegin());
        FUN_VALUE(s1.empty());
        FUN_VALUE(s1.size());
        FUN_VALUE(s1.max_size());
        std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
    }
} // namespace test
} // namespace dhsstl

#endif
