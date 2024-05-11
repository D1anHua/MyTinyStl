#ifndef DHSTINYSTL_HEAP_ALGO_H_
#define DHSTINYSTL_HEAP_ALGO_H_

/**
 * 这个头文件包含了 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap
*/

#include "iterator.h"

namespace dhsstl{

template <typename RandomIter, typename Distance, typename T>
void __push_heap(RandomIter first, Distance holdIndex, Distance topIndex, T value){
    auto parent = (holdIndex - 1) / 2; // 找出父节点
    while(holdIndex > topIndex && *(first + parent) < value){
        // 使用 operator <, 默认 heap 为 max-heap
        *(first + holdIndex) = *(first + parent);
        holdIndex = parent;
        parent = (holdIndex - 1) / 2;
    }
    *(first + holdIndex) = value;
}

template <typename RandomIter, typename Distance> 
inline void __push_heap_aux(RandomIter first, RandomIter last, Distance*){
    dhsstl::__push_heap(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

/**
 * @brief 接受两个迭代器, 表示一个 heap 容器的首尾, 并且新元素已经插入到底部容器的最底端
 *        如果不符合这两个条件, 执行结果未可预期.
 * @param first RandomIter
 * @param last RandomIter
*/
template <typename RandomIter>
void push_heap(RandomIter first, RandomIter last){
    // 注意 : 此函数被调用时, 新元素应该已经置于底部容器的最尾端
    dhsstl::__push_heap_aux(first, last, distance_type(first));
}


template <typename RandomIter, typename Distance, typename T, typename Compare>
void __push_heap(RandomIter first, Distance holdIntex, Distance topIndex, T value, Compare comp){
    auto parent = (holdIntex - 1) / 2;
    while(holdIntex > topIndex && comp(*(first + parent), value)){
        *(first + holdIntex) = *(first + parent); 
        holdIntex = parent;
        parent = (holdIntex - 1) / 2;
    }
    *(first + holdIntex) = value;
}

template <typename RandomIter, typename Distance, typename Compare>
inline void __push_heap_aux(RandomIter first, RandomIter last, Distance*, Compare comp){
    dhsstl::__push_heap(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
}

/**
 * @brief push_heap 的重载版本
 * @param first RandomIter
 * @param last RandomIter
 * @param comp 比较操作
*/
template <typename RandomIter, typename Compare>
void push_heap(RandomIter first, RandomIter last, Compare comp){
    dhsstl::__push_heap_aux(first, last, distance_type(first), comp);
}

template <typename RandomIter, typename T, typename Distance>
void __adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value){
    // 先进行下溯(precolate down)的操作
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    while(rchild < len){
        if(*(first + rchild) < *(first + rchild - 1))
            --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    if(rchild == len){
        // 如果没有右子节点
        *(first + holeIndex) = *(first + (rchild - 1));
        holeIndex = rchild - 1;
    }
    // 在执行一次上溯(percolate up)过程
    dhsstl::__push_heap(first, holeIndex, topIndex, value);
}

// 这里Distance* 参数的作用是让函数估计模板的类型
template <typename RandomIter, typename T, typename Distance>
void __pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*){
    // 先将首值调整之为节点, 然后调整[first, last - 1] 使之重新成为一个 max_heap
    *result = *first;
    dhsstl::__adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

/**
 * @brief 接受两个迭代器, 表示 heap 容器的首尾, 将 heap 的根节点取出放到容器尾部, 调整heap
 * @param first RandomIter
 * @param last RandomIter
*/
template <typename RandomIter>
void pop_heap(RandomIter first, RandomIter last){
    dhsstl::__pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
}

template <typename RandomIter, typename T, typename Distance, typename Compare>
void __adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compare comp){
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    while(rchild < len){
        if(comp(*(first + rchild), *(first + rchild - 1)))
            --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    if(rchild == len){
        *(first + holeIndex) = *(first + (rchild - 1));
        holeIndex = rchild - 1;
    }
    dhsstl::__push_heap(first, holeIndex, topIndex, value, comp);
}

template <typename RandomIter, typename T, typename Distance, typename Compare>
void __pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*, Compare comp){
    *result = *first;
    dhsstl::__adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
}

/**
 * @brief 接受两个迭代器, 表示 heap 容器的首尾, 将 heap 的根节点取出放到容器尾部, 调整heap
 *        重载版本, 使用函数对象 comp 代替比较操作
 * @param first first RandomIter
 * @param last last RandomIter
 * @param comp 比较操作类
*/
template <typename RandomIter, typename Compare>
void pop_heap(RandomIter first, RandomIter last, Compare comp){
    dhsstl::__pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first) ,comp);
}


/**
 * @brief 该函数接受两个迭代器, 表示 heap 容器的首尾, 不断执行 pop_heap 操作, 知道收尾最多相差1
 * @param first first RandomIter
 * @param last last RandomIter
 * 我的疑问是 这个 重载版本为什么不用默认模板来实现, 因为有less的仿函数
*/
template <typename RandomIter>
void sort_heap(RandomIter first, RandomIter last){
    // 每执行一次 pop_heap, 最大的元素都被放到尾部, 直到容器最多只有一个元素, 完成排序
    while(last - first > 1){
        dhsstl::pop_heap(first, last--);
    }
}

/**
 * @brief 该函数接受两个迭代器, 表示 heap 容器的首尾, 不断执行 pop_heap 操作, 知道收尾最多相差1
 *        重载比较版本
 * @param first first RandomIter
 * @param last last RandomIter
 * @param comp comp operator 决定大根堆还是小根堆
*/
template <typename RandomIter, typename Compare>
void sort_heap(RandomIter first, RandomIter last, Compare comp){
    while(last - first > 1){
        dhsstl::pop_heap(first, last--, comp);
    }
}

template <typename RandomIter, typename Distance>
void __make_heap_aux(RandomIter first, RandomIter last, Distance*){
    if(last - first < 2)
        return;
    auto len = last - first;
    // 这个为啥是 - 2
    // 找出第一个需要重排的子树头部, 以 holeIndex 标出
    auto holeIndex = (len - 2) / 2;
    while(true){
        dhsstl::__adjust_heap(first, holeIndex, len, *(first + holeIndex));
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

/**
 * @brief 该函数接受两个迭代器, 表示heap容器的首尾, 把容器内的数据变为一个heap
 * @param first first RandomIter
 * @param last last RandomIter
*/
template <typename RandomIter>
void make_heap(RandomIter first, RandomIter last){
    dhsstl::__make_heap_aux(first, last, distance_type(first));
}

template<typename RandomIter, typename Distance, typename Compare>
void __make_heap_aux(RandomIter first, RandomIter last, Distance*, Compare comp){
    if(last - first < 2)
        return; 
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    while(true){
        //重排以holeIndex为首的子树
        dhsstl::__adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

/**
 * @brief 该函数接受两个迭代器, 表示heap容器的首尾, 把容器内的数据变为一个heap, 重载版本
 * @param first RandomIter
 * @param last RandomIter
 * @param comp comp
*/
template <typename RandomIter, typename Compare>
void make_heap(RandomIter first, RandomIter last, Compare comp){
    dhsstl::__make_heap_aux(first, last, distance_type(first), comp);
}
}
#endif