# 0 序
为了提升自己对C++语法的掌握程度, 参考了GitHub中[MyTinySTL的源码](https://github.com/Alinshans/MyTinySTL). 实现了一个小的STL库. 
其是C11 Stl的一个子集.

> 本项目主要参考以下几个文章以及仓库
>
> 1. MyTinySTL -- [Github 链接](https://github.com/Alinshans/MyTinySTL)
> 2. STL源码剖析一书
> 3. C++ STL 部分源码
> 4. [CppReference](https://cppreference.com/)

# 1 简介
本项目的主要完成如下:

总的来说 ,STL提供六大组件, 分别为: 
- [x] :heavy_check_mark:配置器(allocator): 负责动态空间的配置, 空间管理, 空间释放. 仅仅是对new的简单的封装(缺点: 没有实现对应的内存池/一级二级配置器);
- [x] :heavy_check_mark:容器(containers): 各种数据结构, 包括`vector, list, deque, set, map, unordered_map, unordered_set`均已实现;
- [x] :heavy_check_mark:迭代器(iterators): 五种迭代器类型`input/ output/ forward/ bidirectional/ random`, 以及对应的`iterator_traits`;
- [x] :heavy_check_mark:仿函数(functors): 实现了逻辑运算, 算数运算, 比较运算以及一系列trivial类型的hash函数;
- [ ] :o:配接器(adapters): 主要是实现了一些改变`container`接口的 `container adapter`
- [ ] :x:算法(algorithms): 仅仅实现了一小部分, 目前主要缺陷. :hourglass_flowing_sand: 


**需求:** 本项目使用`cmake`搭建.


## 在项目之前
[可以先阅读对util.h中相关函数的理解笔记(move/forward)](/NoteBook/STL-Move-Forward-SFINAE.md)

# 2. 配置器(allocator)
[有关配置器部分可以参考我的个人笔记](/NoteBook/STL-Allocator-Construct.md)
```Cpp
// allocator.h
#include "construct.h"
/*!
 * @brief 主要负责容器空间的开辟与回收, 容器的construct & destroy
 * @note  仅仅对new的简单封装
 * @note  接口: [cppreference](https://en.cppreference.com/w/cpp/memory/allocator)
 * @tparam T value_type
 */
templace<typename T> class allocator;
```

## Test allocator
使用标准库`stl::list<int>`对`allocator`的正确行进行了测试. 见`\Test\test_alloc.h`

# 3. 迭代器(iterator)
[有关迭代器详细说明可以参考我的个人笔记](/NoteBook/STL-iterator.md)

本头文件除了实现`iterator_tag`之外, 以及针对原生指针的特例化, 此外还实现了对应的`iterator_traits`.
```Cpp
// iterator.h
//! 对应接口请参考对应的[cppreference]
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <class Category,
          class T,
          class Distance = ptrdiff_t,
          class Pointer = T*,
          class Reference = T&>
struct iterator{
typedef Category                             iterator_category;
typedef T                                    value_type;
typedef Pointer                              pointer;
typedef Reference                            reference;
typedef Distance                             difference_type;
};
```

# 4. 序列式容器(sequence containers)

## 4.1 vector
```Cpp
// vector.h
//! vector迭代器为: random_access_iterator, 使用原生指针作为迭代器
/*!
 * @brief 可以动态分配空间的数组vector
 * @note  默认内存扩展倍数为2.0, 也即当达到容量上限后按照原大小两倍进行扩容.
 * @todo  默认使用dhsstl::allocator作为配置器, 并且也没有提供对应的自行定义接口
 * @note  接口: [cppreference](https://en.cppreference.com/w/cpp/container/vector)
 * @tparam T value_type
 */
templace<typename T> class vector;
```
### vector_test
按照所提供的接口对vector的功能进行了测试, 但并未进行性能方面的测试. 只进行了功能测试.
``` cpp
// test_vector.h
dhsstl::test::vector_test();
```

## 4.2 list
```cpp
// list.h
//! list迭代器为: bidirectional_iterator, 双向迭代器: 支持 iter++, iter--, == , !=, 但不支持比较大小或者数值加法操作
/*!
 * @brief 支持双向顺序访问的双向环形链表
 * @todo  默认使用dhsstl::allocator作为配置器, 并且也没有提供对应的自行定义接口
 * @note  接口: [cppreference](https://en.cppreference.com/w/cpp/container/list)
 * @tparam T value_type
 */
templace<typename T> class list;
```

### list_test
按照所提供的接口对list的功能进行了测试, 但并未进行性能方面的测试. 只进行了功能测试.
``` cpp
// test_list.h
dhsstl::test::list_test();
```

## 4.3 deque
```cpp
// deque.h
//! deque迭代器为: random_access_iterator
/*!
 * @brief 可以随机访问的双端输入输出队列
 * @todo  默认使用dhsstl::allocator作为配置器, 并且也没有提供对应的自行定义接口
 * @note  接口: [cppreference](https://en.cppreference.com/w/cpp/container/deque)
 * @tparam T value_type
 */
templace<typename T> class deque;
```

### deque_test
按照所提供的接口对deque的功能进行了测试, 但并未进行性能方面的测试. 只进行了功能测试.
``` cpp
// test_deque.h
dhsstl::test::deque_test();
```

## 4.4 stack
```cpp
// stack.h
/*!
 * @brief A standard container giving FILO behavior
 * @tparam T  type of element
 * @tparam Container  Type of underlying sequence, defaults to deque<T>, 简单来说就是底层容器是啥
 */
template <typename T, typename Container = dhsstl::deque<T>> class stack;

// test_stack.h
dhsstl::test::stack_test();
```

## 4.5 queue && priority_queue
```cpp
// queue.h
/*!
 * @brief A standard container giving FIFO behavior
 * @tparam T  type of element
 * @tparam Container  type of underlying sequence, default to deque<T>
 */
template <typename T, typename Container = dhsstl::deque<T>>
class queue;
/*!
 * @brief priority_queue
 * @tparam T 参数类型
 * @tparam Container 容器, 默认使用 dhsstl::vector 作为底层容器
 * @tparam Compare 比较权值的方式, 缺省使用 dhsstl::less 作为比较方式
 */
template <typename T, typename Container = dhsstl::vector<T>,
typename Compare = dhsstl::less<typename Container::value_type>>
class priority_queue;

// test_queue.h
dhsstl::test::queue_test();
dhsstl::test::priority_queue_test();
```

# 5. 关联式容器(associative containers)
## 5.1 Set/MultiSet, Map/MultiMap
```cpp
// set.h
#include "rb_tree"
//! 迭代器为: bidirectional_iterator, 双向迭代器: 支持 iter++, iter--, == , !=, 但不支持比较大小或者数值加法操作
/*!
 * @brief 模板类set, 键值不允许重复
 * @tparam Compare 键值比较方式, 缺省使用dhstl::less
 */
template <typename Key, typename Compare = dhsstl::less<Key>> class set;
//! @brief 模板类multiset, 允许键值重复
template <typename Key, typename Compare = dhsstl::less<Key>> class multiset;

// map.h
//! @brief 模板类 map , 不允许键值重复
template <typename Key, typename T, typename Compare = dhsstl::less<Key>> class map;
//! @brief 模板类 multimap , 允许键值重复
template <typename Key, typename T, typename Compare = dhsstl::less<Key>> class multimap;
```

## 5.2 HashTable
```cpp
// hashtable.h
//! unordered系列迭代器为: forward_iterator, 只支持 ++, ==, !=操作(原因: 每个桶使用的是单链表)
/*!
 * @brief hashtable
 */
template <typename T, typename Hash, typename KeyEqual> class hashtable;
```

# 6. 仿函数(functors)

- 算术类仿函数
  - `plus, minus, mutiplies, divides, modulus, negate`
- 逻辑类仿函数
  - `logical_and, logical_or, logical_not`
- 关系类仿函数
  - `equal_to, not_equal_to, less, less_equal, greater, greater_equal`
- 其他
  - `identity, select, project`

# 7. smart pointer
```cpp
// memory.h
//! @brief Unique_ptr 具有严格对象所有权的智能指针
template<typename T> class unique_ptr;

//! @brief shared_ptr 允许多个智能指针同时指向一个底部资源
template<typename T> class shared_ptr;
//! @brief weak_ptr 弱共享指针, 由shared_ptr初始化, 不增加管理资源的指针计数, 当管理资源的shared_ptr计数清零后将组织访问资源
template<typename T> class weak_ptr;
```