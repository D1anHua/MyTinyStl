#ifndef DHSTINYSTL_ALGO_H_
#define DHSTINYSTL_ALGO_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

#include <cstddef>
#include <ctime>
#include "algobase.h"
#include "memory.h"
#include "functional.h"


namespace dhsstl
{

/*!
 * @brief 将[first, last) 区间内的元素反转
 */
//! reverse_dispatch 的 bidirectional_iterator_tag 版本
template<typename BidirectionalIter>
void reverse_dispatch(BidirectionalIter first, BidirectionalIter last, bidirectional_iterator_tag){
    // 注: 这里这样设计的原因是, 双向迭代器不支持比较大小.
    // 双向迭代器仅仅只支持前进操作, 后退操作, 等于不等于
    while(true){
        if(first == last || first == --last){
            return;
        }
        dhsstl::iter_swap(first++, last);
    }
}

//! reverse_dispatch 的 random_access_iterator_tag 版本
template<typename RandomIter>
void reverse_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag){
    while(first < last){
        dhsstl::iter_swap(first++, --last);
    }
}

template <typename BidirectionalIter>
void reverse(BidirectionalIter first, BidirectionalIter last){
    dhsstl::reverse_dispatch(first, last, iterator_category(first));
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif