#ifndef DHSTINYSTL_TEST_QUEUE_H_
#define DHSTINYSTL_TEST_QUEUE_H_

#include <iostream>
#include <queue>

#include "queue.h"
#include "test.h"

namespace dhsstl {
namespace test {
// ---------------------------------------------------------------------------------------------------------------------
// test helper function
        void queue_print(dhsstl::queue<int> q) {
            while (!q.empty()) {
                std::cout << " " << q.front();
                q.pop();
            }
            std::cout << std::endl;
        }

        void p_queue_print(dhsstl::priority_queue<int> p) {
            while (!p.empty()) {
                std::cout << " " << p.top();
                p.pop();
            }
            std::cout << std::endl;
        }

//  queue 的遍历输出
#define QUEUE_COUT(q) do {                       \
    std::string q_name = #q;                     \
    std::cout << " " << q_name << " :";          \
    queue_print(q);                              \
} while(0)

// priority_queue 的遍历输出
#define P_QUEUE_COUT(p) do {                     \
    std::string p_name = #p;                     \
    std::cout << " " << p_name << " :";          \
    p_queue_print(p);                            \
} while(0)

#define QUEUE_FUN_AFTER(con, fun) do {           \
  std::string fun_name = #fun;                   \
  std::cout << " After " << fun_name << " :\n";  \
  fun;                                           \
  QUEUE_COUT(con);                               \
} while(0)

#define P_QUEUE_FUN_AFTER(con, fun) do {         \
  std::string fun_name = #fun;                   \
  std::cout << " After " << fun_name << " :\n";  \
  fun;                                           \
  P_QUEUE_COUT(con);                             \
} while(0)

// ---------------------------------------------------------------------------------------------------------------------
//! @brief Test dhsstl::queue
void queue_test() {
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[--------------------------- Run container test :  queue -------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5 };
    dhsstl::deque<int> d1(5);
    dhsstl::queue<int> q1;
    dhsstl::queue<int> q2(5);
    dhsstl::queue<int> q3(5, 1);
    dhsstl::queue<int> q4(a, a + 5);
    dhsstl::queue<int> q5(d1);
    dhsstl::queue<int> q6(std::move(d1));
    dhsstl::queue<int> q7(q2);
    dhsstl::queue<int> q8(std::move(q2));
    dhsstl::queue<int> q9;
    q9 = q3;
    dhsstl::queue<int> q10;
    q10 = std::move(q3);
    dhsstl::queue<int> q11{ 1,2,3,4,5 };
    dhsstl::queue<int> q12;
    q12 = { 1,2,3,4,5 };

    QUEUE_FUN_AFTER(q1, q1.push(1));
    QUEUE_FUN_AFTER(q1, q1.push(2));
    QUEUE_FUN_AFTER(q1, q1.push(3));
    QUEUE_FUN_AFTER(q1, q1.pop());
    QUEUE_FUN_AFTER(q1, q1.emplace(4));
    QUEUE_FUN_AFTER(q1, q1.emplace(5));
    FUN_VALUE(q1.empty());
    FUN_VALUE(q1.size());
    FUN_VALUE(q1.front());
    FUN_VALUE(q1.back());
    while (!q1.empty())
    {
        QUEUE_FUN_AFTER(q1, q1.pop());
    }
    QUEUE_FUN_AFTER(q1, q1.swap(q4));
    QUEUE_FUN_AFTER(q1, q1.clear());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------
//! @brief Test dhsstl::stack
void priority_queue_test() {
    std::cout << "[=================================================================================]" << std::endl;
    std::cout << "[-------------------------- Run container test : priority ------------------------]" << std::endl;
    std::cout << "[--------------------------- -------- API test --------- -------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5 };
    dhsstl::vector<int> v1(5);
    dhsstl::priority_queue<int> p1;
    dhsstl::priority_queue<int> p2(5);
    dhsstl::priority_queue<int> p3(5, 1);
    dhsstl::priority_queue<int> p4(a, a + 5);
    dhsstl::priority_queue<int> p5(v1);
    dhsstl::priority_queue<int> p6(std::move(v1));
    dhsstl::priority_queue<int> p7(p2);
    dhsstl::priority_queue<int> p8(std::move(p2));
    dhsstl::priority_queue<int> p9;
    p9 = p3;
    dhsstl::priority_queue<int> p10;
    p10 = std::move(p3);
    dhsstl::priority_queue<int> p11{ 1,2,3,4,5 };
    dhsstl::priority_queue<int> p12;
    p12 = { 1,2,3,4,5 };

    P_QUEUE_FUN_AFTER(p1, p1.push(1));
    P_QUEUE_FUN_AFTER(p1, p1.push(5));
    P_QUEUE_FUN_AFTER(p1, p1.push(3));
    P_QUEUE_FUN_AFTER(p1, p1.pop());
    P_QUEUE_FUN_AFTER(p1, p1.emplace(7));
    P_QUEUE_FUN_AFTER(p1, p1.emplace(2));
    P_QUEUE_FUN_AFTER(p1, p1.emplace(8));
    FUN_VALUE(p1.empty());
    FUN_VALUE(p1.size());
    FUN_VALUE(p1.top());
    while (!p1.empty())
    {
        P_QUEUE_FUN_AFTER(p1, p1.pop());
    }
    P_QUEUE_FUN_AFTER(p1, p1.swap(p4));
    P_QUEUE_FUN_AFTER(p1, p1.clear());
    std::cout << "[--------------------------- ------ END API test ------- -------------------------]" << std::endl;
}

} // namespace test
} // namespace dhsstl
#endif
