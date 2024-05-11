#ifndef DHSTINYSTL_TEST_VECTOR_H_
#define DHSTINYSTL_TEST_VECTOR_H_

#include <iostream>
#include <vector>
#include "vector.h"
#include "test.h"


namespace dhsstl {
namespace test {

template<typename T, typename Alloc1>
inline bool vec_equal(const std::vector<T, Alloc1> lhs,
                      const dhsstl::vector<T>& rhs){
    if(lhs.size() != rhs.size())
        return false;
    for(size_t i = 0; i != lhs.size(); ++i){
        if(lhs[i] != rhs[i])
            return false;
    }
    return true;
}

//! @brief Test dhsstl::vector
void vector_test(){
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test : vector -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = {1, 2, 3, 4, 5};
    dhsstl::vector<int> v1;
    dhsstl::vector<int> v2(10);
    dhsstl::vector<int> v3(10, 1);
    dhsstl::vector<int> v4(a, a + 5);
    dhsstl::vector<int> v5(v4);
    dhsstl::vector<int> v6 = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    v1 = v3;
    PRINT(v1);
    PRINT(v2);
    PRINT(v3);
    PRINT(v4);
    PRINT(v5);
    PRINT(v6);
    FUN_AFTER(v1, v1.push_back(6));
    FUN_AFTER(v1, v1.insert(v1.end(), 7));
    FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));
    FUN_AFTER(v1, v1.pop_back());
    FUN_AFTER(v1, v1.erase(v1.begin()));
    FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));
    FUN_AFTER(v1, v1.swap(v4));
    FUN_VALUE(*v1.begin());
    FUN_VALUE(*v1.cbegin());
    FUN_VALUE(*(v1.end() - 1));
    FUN_VALUE(*(v1.cend() - 1));
    FUN_VALUE(*v1.rbegin());
    FUN_VALUE(*v1.crbegin());
    FUN_VALUE(*(v1.rend() - 1));
    FUN_VALUE(v1.front());
    FUN_VALUE(v1.back());
    FUN_VALUE(v1[0]);
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.max_size());
    FUN_VALUE(v1.capacity());
    FUN_VALUE(v1.empty());
    FUN_AFTER(v1, v1.reserve(40));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.resize(20, 5));
    FUN_AFTER(v1, v1.clear());
    FUN_VALUE(v1.size());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;

//    FUN_AFTER(v1, v1.assign(8, 8));
//    FUN_AFTER(v1, v1.assign(a, a + 5));
//    FUN_AFTER(v1, v1.emplace(v1.begin(), 0));
//    FUN_AFTER(v1, v1.emplace_back(6));
//    FUN_AFTER(v1, v1.push_back(6));
//    FUN_AFTER(v1, v1.insert(v1.end(), 7));
//    FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));
//    FUN_AFTER(v1, v1.insert(v1.begin(), a, a + 5));
//    FUN_AFTER(v1, v1.pop_back());
//    FUN_AFTER(v1, v1.erase(v1.begin()));
//    FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));
//    FUN_AFTER(v1, v1.reverse());
//    FUN_AFTER(v1, v1.swap(v4));
//    FUN_VALUE(*v1.begin());
//    FUN_VALUE(*(v1.end() - 1));
//    FUN_VALUE(*v1.rbegin());
//    FUN_VALUE(*(v1.rend() - 1));
//    FUN_VALUE(v1.front());
//    FUN_VALUE(v1.back());
//    FUN_VALUE(v1[0]);
//    FUN_VALUE(v1.at(1));

}
} // namespace test
} // namespace dhsstl
#endif
