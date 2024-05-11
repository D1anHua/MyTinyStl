#ifndef DHSTINYSTL_TEST_LIST_H_
#define DHSTINYSTL_TEST_LIST_H_

#include <iostream>
#include <list>

#include "test.h"
#include "list.h"

namespace dhsstl {
namespace test {
//! @brief Test dhsstl::list
void list_test(){
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test  :  list -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = {1, 2, 3, 4, 5};
    dhsstl::list<int> l1;
    dhsstl::list<int> l2(10);
    dhsstl::list<int> l3(10, 1);
    dhsstl::list<int> l4(a, a + 5);
    dhsstl::list<int> l5(l4);
    dhsstl::list<int> l6 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    dhsstl::list<int> l7(l6.begin(), l6.end());
    l1 = l3;
    PRINT(l1);
    PRINT(l2);
    PRINT(l3);
    PRINT(l4);
    PRINT(l5);
    PRINT(l6);
    PRINT(l7);
    FUN_AFTER(l1, l1.push_back(6));
    FUN_AFTER(l1, l1.push_front(8));
    FUN_AFTER(l1, l1.insert(l1.end(), 7));
    FUN_AFTER(l1, l1.insert(l1.begin(), 2, 3));
    FUN_AFTER(l1, l1.pop_back());
    FUN_AFTER(l1, l1.pop_front());
    FUN_AFTER(l1, l1.erase(l1.begin()));
    FUN_AFTER(l1, l1.erase(l1.begin(), ++l1.begin()));
    FUN_AFTER(l1, l1.swap(l4));
    FUN_VALUE(*l1.begin());
    FUN_VALUE(*l1.cbegin());
    FUN_VALUE(*(--l1.end()));
    FUN_VALUE(*(--l1.cend()));
    FUN_VALUE(*l1.rbegin());
    FUN_VALUE(*(--l1.rend()));
    FUN_VALUE(l1.front());
    FUN_VALUE(l1.back());
    FUN_AFTER(l1, l1.splice(l1.begin(), l2));
    FUN_AFTER(l1, l1.splice(l1.begin(), l3, l3.begin()));
    FUN_AFTER(l1, l1.splice(l1.begin(), l4, l4.begin(), l4.end()));
    FUN_VALUE(l1.size());
    FUN_VALUE(l1.max_size());
    FUN_VALUE(l1.empty());
    FUN_AFTER(l1, l1.reverse());
    FUN_AFTER(l1, l1.sort());
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.resize(30, 5));
    FUN_AFTER(l1, l1.clear());
    FUN_VALUE(l1.size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}


//! @brief Test dhsstl::vector
void list_stl(){
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container real  :  list -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = {1, 2, 3, 4, 5};
    std::list<int> l1;
    std::list<int> l2(10);
    std::list<int> l3(10, 1);
    std::list<int> l4(a, a + 5);
    std::list<int> l5(l4);
    std::list<int> l6 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::list<int> l7(l6.begin(), l6.end());
    l1 = l3;
    PRINT(l1);
    PRINT(l2);
    PRINT(l3);
    PRINT(l4);
    PRINT(l5);
    PRINT(l6);
    PRINT(l7);
    FUN_AFTER(l1, l1.push_back(6));
    FUN_AFTER(l1, l1.push_front(8));
    FUN_AFTER(l1, l1.insert(l1.end(), 7));
    FUN_AFTER(l1, l1.insert(l1.begin(), 2, 3));
    FUN_AFTER(l1, l1.pop_back());
    FUN_AFTER(l1, l1.pop_front());
    FUN_AFTER(l1, l1.erase(l1.begin()));
    FUN_AFTER(l1, l1.erase(l1.begin(), ++l1.begin()));
    FUN_AFTER(l1, l1.swap(l4));
    FUN_VALUE(*l1.begin());
    FUN_VALUE(*l1.cbegin());
    FUN_VALUE(*(--l1.end()));
    FUN_VALUE(*(--l1.cend()));
    FUN_VALUE(*l1.rbegin());
    FUN_VALUE(*(--l1.rend()));
    FUN_VALUE(l1.front());
    FUN_VALUE(l1.back());
    FUN_AFTER(l1, l1.splice(l1.begin(), l2));
    FUN_AFTER(l1, l1.splice(l1.begin(), l3, l3.begin()));
    FUN_AFTER(l1, l1.splice(l1.begin(), l4, l4.begin(), l4.end()));
    FUN_VALUE(l1.size());
    FUN_VALUE(l1.max_size());
    FUN_VALUE(l1.empty());
    FUN_AFTER(l1, l1.reverse());
    FUN_AFTER(l1, l1.sort());
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.resize(30, 5));
    FUN_AFTER(l1, l1.clear());
    FUN_VALUE(l1.size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

} // namespace test
} // namespace dhsstl
#endif