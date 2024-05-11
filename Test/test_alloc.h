#ifndef DHSTINYSTL_TEST_ALLOC_H_
#define DHSTINYSTL_TEST_ALLOC_H_

#include <ctime>
#include <list>
#include <iostream>
#include "allocator.h"

namespace dhsstl {
namespace test {
enum {NUMBERS = 100000000};

void test_alloc(){
    // Test of list
    // test of push_back
    std::list<int> list1;
    std::list<int, dhsstl::allocator<int>> list2;
    clock_t start = clock();
    for(size_t i = 0; i < NUMBERS; ++i){
        list1.push_back(1);
    }
    clock_t end = clock();
    std::cout << "Time to insert " << NUMBERS
              << " numbers in list with STL allocator: "
              << end - start << std::endl;

    start = clock();
    for(size_t i = 0; i < NUMBERS; ++i){
        list2.push_back(1);
    }
    end = clock();
    std::cout << "Time to insert " << NUMBERS
              << " numbers in list with DHS' STl allocator: "
              << end - start << std::endl;
}

} // namespace test
} // namespace dhsstl
#endif
