#ifndef DHSTINYSTL_ALGOBASE_H_
#define DHSTINYSTL_ALGOBASE_H_

// 这个头文件主要包含了该tinystl的基本算法
#include <cstring>
#include "iterator.h"
#include "util.h"

namespace dhsstl{

#ifdef max
#pragma message("#undefing marco max")
#undef
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef
#endif // min


// ---------------------------------------------------
// max
// 取二者中的较大值, 若语义相等时保证返回第一个参数
// ---------------------------------------------------
// 为什么用 const T&
template <typename T>
const T& max(const T& lhs, const T& rhs){
    return lhs < rhs ? rhs : lhs;
}

// 注: C++的默认比较操作一般都是返回 return lhs < rhs;
// C11一书中写道:
//      编写泛型代码的两个重要原则:
//          1. 模板中的函数参数是const的引用
//          2. 函数体中的条件判断仅适用 < 比较划算
//          原因: 1. const的引用的作用是为了 保证函数用于不能拷贝的类型
//                2. 只用小于号是为了 就要求 所有类型可以只支持 小于 而不用 支持 大于
template <typename T, typename Compare>
const T& max(const T& lhs, const T& rhs, Compare comp){
    return comp(lhs, rhs) ? rhs : lhs;
}


// ---------------------------------------------------
// min
// 取二者中的较小值, 若语义相等时保证返回第一个参数
// ---------------------------------------------------
template <typename T>
const T& min(const T& lhs, const T& rhs){
    return rhs < lhs ? rhs : lhs;
}

template <typename T, typename Compare>
const T& min(const T& lhs, const T& rhs, Compare comp){
    return comp(rhs, lhs) ? rhs : lhs;
}


// ---------------------------------------------------
// iter_swap
// 将两个迭代器所指的对象对调 
// ---------------------------------------------------
template <typename FIter1, typename FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs){
    dhsstl::swap(*lhs, *rhs);
}


// ---------------------------------------------------
// copy
// 把[first, last)区间内的元素拷贝到[result, result + (last - first))内 
// ---------------------------------------------------
// input_iterator_tag版本
template<typename InputIter, typename OutputIter>
OutputIter
unchecked_copy_cat(InputIter first, InputIter last, OutputIter result, dhsstl::input_iterator_tag){
    for(;first != last; ++first, ++result){
        *result = *first;
    }
    return result; 
}

// random_access_iterator_tag 版本
template <typename RandomIter, typename OutputIter>
OutputIter
unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, dhsstl::random_access_iterator_tag){
    for(auto n = last - first; n > 0; --n, ++first, ++result){
        *result = *first;
    }
    return result;
}

template <typename InputIter, typename OutputIter>
OutputIter
unchecked_copy(InputIter first, InputIter last, OutputIter result){
    return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 的类型 提供特化版本
// 这个应该是重载, 不是模板特例化
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*
>::type
unchecked_copy(Tp *first, Tp *last, Up *result){
    const auto n = static_cast<size_t>(last - first);
    if(n != 0)
        std::memmove(result, first, n * sizeof(Up));
    return result + n;
}

template<typename InputIter, typename OutputIter>
OutputIter copy(InputIter fisrt, InputIter last, OutputIter result){
    return unchecked_copy(fisrt, last, result);
}
// ---------------------------------------------------
// copy_bcakward
// 把[first, last)区间内的元素拷贝到[result - (last - first), result)内 
// ---------------------------------------------------
// unchecked_copy_backward_cat 的 bidireactional_iterator_tag 版本 
template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, dhsstl::bidirectional_iterator_tag){
    while(first != last)
        *--result = *--last;
    return result;
}

template <typename RandomIter1, typename RandomIter2>
RandomIter2
unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last, 
                            RandomIter2 result, dhsstl::random_access_iterator_tag){
    for(auto n = last - first; n > 0; --n)
        *--result = *--last;
    return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*
>::type
unchecked_copy_backward(Tp* first, Tp* last, Up* result){
    const auto n = static_cast<size_t>(last - first);
    if( n!= 0 ){
        result -= n;
        std::memmove(result, first, n*sizeof(Up));
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_copy_backward(first, last, result);
}
// ---------------------------------------------------
// copy_if
// 把[first, last)区间内的满足一元操作 unary_pred 的元素移动到以 result 为起始的位置上 
// ---------------------------------------------------
template <typename InputIter, typename OutputIter, typename UnaryPredicate>
OutputIter
copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred){
    for(; first != last; ++first){
        if(unary_pred(*first))
            *result++ = *first;
    }
    return result;
}
// 注: 这个函数没有 为 trivially 类型 提供特化版本
// 原因, 个人理解:
//  为什么需要对trivially类型提供特例版本呢?  
//  因为对于trivially类型可以使用 memmove 来移动内存区间
//  而 fill_if 并不能 memmove, 所以没有必要设计特例化版本
// 
// 其次, 也没有 random_iter的版本, 但目前我并不知道 first - last 和 first != last 有什么区别

// ---------------------------------------------------
// copy_n
// 把[first, first+n)区间内的元素拷贝到[result, result + n)内 
// 返回一个 pair 分别指向拷贝结束的尾部
// ---------------------------------------------------
// 这里我的理解是, 没有输入 InputIter的结束位置, 所以返回一下
// 这个也没有提供 trivally 类型的特例化版本:
// 但是默认会调用特例化版本:
// 因为 一般认为: trivally 类型的 指针 是 randomIter
// 所以其会调用 unchecked_copy_n
// 而 其 会调用 copy, copy应对了trivally的情况
template <typename InputIter, typename Size, typename OutputIter>
dhsstl::pair<InputIter, OutputIter>
unchecked_copy_n(InputIter first, Size n, OutputIter result, dhsstl::input_iterator_tag){
    for(; n > 0; --n, ++first, ++result){
        *result = *first;
    }
    return dhsstl::pair<InputIter, OutputIter>(first, result);
}

template <typename InputIter, typename Size, typename OutputIter>
dhsstl::pair<InputIter, OutputIter>
unchecked_copy_n(InputIter first, Size n, OutputIter result, dhsstl::random_access_iterator_tag){
    auto last = first + n;
    return dhsstl::pair<InputIter, OutputIter>(last, dhsstl::copy(first, last, result));
}

template <typename InputIter, typename Size, typename OutputIter>
dhsstl::pair<InputIter, OutputIter>
copy_n(InputIter first, Size n, OutputIter result){
    return unchecked_copy_n(first, n, result, iterator_category(first));
}
// ---------------------------------------------------
// move
// 把[first, last)区间内的元素移动到[result - (last - first), result)内 
// ---------------------------------------------------
// input_iterator_tag版本
template <typename InputIter, typename OutputIter>
OutputIter
unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                   dhsstl::input_iterator_tag){
    for(; first != last; ++first, ++result)
        *result = dhsstl::move(*first);
    return result;
}

// random_access_iterator_tag版本
template <typename RandomIter, typename OutputIter>
OutputIter
unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
                   dhsstl::random_access_iterator_tag){
    for(auto n = last - first; n > 0; --n, ++first, ++result){
        *result = dhsstl::move(*first);
    }
    return result;
}

template <typename InputIter, typename OutputIter>
OutputIter
unchecked_move(InputIter first, InputIter last, OutputIter result){
    return unchecked_move_cat(first, last, iterator_category(first));
}
// 为 trivially_copy_assignable 类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_move_assignable<Up>::value,
    Up*
>::type
unchecked_move(Tp* first, Tp* last, Up* result){
    const size_t n = static_cast<size_t>(last - first);
    if(n!=0)
        std::memmove(result, first, n * sizeof(Up));
    return result + n;
}

template <typename InputIter, typename OutputIter>
OutputIter
move(InputIter first,InputIter last, OutputIter result){
    return unchecked_move(first, last, result);
}
// ---------------------------------------------------
// move_backward
// 把[first, last)区间内的元素move到[result - (last - first), result)内 
// ---------------------------------------------------
// unchecked_copy_backward_cat 的 bidireactional_iterator_tag 版本 
template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, dhsstl::bidirectional_iterator_tag){
    while(first != last)
        *--result = dhsstl::move(*--last);
    return result;
}

template <typename RandomIter1, typename RandomIter2>
RandomIter2
unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last, 
                            RandomIter2 result, dhsstl::random_access_iterator_tag){
    for(auto n = last - first; n > 0; --n)
        *--result = dhsstl::move(*--last);
    return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_move_backward_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_move_assignable<Up>::value,
    Up*
>::type
unchecked_move_backward(Tp* first, Tp* last, Up* result){
    const auto n = static_cast<size_t>(last - first);
    if( n!= 0 ){
        result -= n;
        std::memmove(result, first, n * sizeof(Up));
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_move_backward(first, last, result);
}
// ---------------------------------------------------
// equal
// 比较第一序列在 [first, last) 区间上的元素值是否和第二序列相等
// ---------------------------------------------------
template<typename InputIter1, typename InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2){
    for(; first1 != last1; ++first1, ++first2){
        if(*first1 != !first2){
            return false;
        }
    }
    return true;
}

template <typename InputIter1, typename InputIter2, typename Compared>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp){
    for(; first1 != last1; ++first1, ++first2){
        if(!comp(*first1, *first2))
            return false;
    }
    return true;
}
// ---------------------------------------------------
// fill_n
// 从 first位置开始填充n个值
// ---------------------------------------------------
template <typename OutputIter, typename Size, typename T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T &value){
    for(; n > 0; --n, ++first){
        *first = value;
    }
    return first;
}

// 为one-byte类型提供特化版本
// is_integral<T> :
// Checks whether T is an integral type. Provides the member constant value which is equal to true, if T is the type
// bool, char, char8_t (since C++20), char16_t, char32_t, wchar_t, short, int, long, long long, or any implementation
// -defined extended integer types, including any signed, unsigned, and cv-qualified variants. Otherwise, value is 
// equal to false.
// 检查一下Tp是不是基本数据类型
template<typename Tp, typename Size, typename Up>
typename std::enable_if<
    std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
    std::is_same<Tp, bool>::value &&
    std::is_integral<Up>::value && sizeof(Up) == 1,
    Tp*
>::type
unchecked_fill_n(Tp* first, Size n , Up value){
    if(n > 0){
        std::memmove(first, (unsigned char)value, (size_t)(n));
    }
    return first + n;
}
template <typename OutputIter, typename Size, typename T>
OutputIter fill_n(OutputIter first, Size n, const T& value){
    return unchecked_fill_n(first, n, value);
}
// ---------------------------------------------------
// fill
// 为 [first, last) 区间内的所有元素填充新值
// ---------------------------------------------------
template <typename ForwardIter, typename T>
void fill_cat(ForwardIter first, ForwardIter last, const T &value,
              dhsstl::forward_iterator_tag){
    for(; first != last; ++first){
        *first = value;
    }
}

template <typename RandomIter, typename T>
void flii_cat(RandomIter first, RandomIter last, const T &value,
              dhsstl::random_access_iterator_tag){
    fill_n(first, last - first, value);
}

template <typename ForwardIter, typename T>
void fill(ForwardIter first, ForwardIter last, const T &value){
    fill_cat(first, last, value, iterator_category(first));
}
// ---------------------------------------------------
// lexicographical_compare
// 以字典序排列对两个序列进行比较, 当在某个位置发现第一组不想等元素时, 有以下几种情况:
// (1) 如果第一序列的元素较小, 返回true, 否则返回false
// (2) 如果到达 last1 而未到达 last2 返回 true
// (3) 如果到达 last2 而未到达 last1 返回 false
// (4) 如果同时到达 last1 和 last2 返回 false
// ---------------------------------------------------
template <typename InputIter1, typename InputIter2>
bool lexicograhical_compare(InputIter1 first1, InputIter1 last1,
                            InputIter2 first2, InputIter2 last2){
    for(; first1 != last1 && first2 != last2; ++first1, ++first2){
        if(*first1 < *first2) 
            return true;
        if(*first2 < *first1) 
            return false;
    }
    return first1 == last1 && first2 != last2;
}

// 重载版本使用函数对象 comp 操作代替比较操作
template <typename InputIter1, typename InputIter2, typename Compred>
bool lexicograhical_compare(InputIter1 first1, InputIter1 last1,
                           InputIter2 first2, InputIter2 last2,
                           Compred comp){
    for(; first1 != last1 && first2 != last2; ++first1, ++first2){
        if(comp(*first1, *first2))
            return true;
        if(comp(*first2, *first1))
            return false;
    }
    return first1 == last1 && first2 != last2; 
}

// 针对 const unsigned char* 的特化版本
bool lexicograhical_compare(const unsigned char* first1,
                           const unsigned char* last1,
                           const unsigned char* first2,
                           const unsigned char* last2){
    const auto len1 = last1 - first1;
    const auto len2 = last2 - first2;
    // 先比较相同长度的部分
    const auto result = std::memcmp(first1, first2, dhsstl::min(len1, len2));
    // 若相等, 长度较长的比较大
    return result != 0 ? result < 0 : len1 < len2;
}
// ---------------------------------------------------
// mismatch
// 平行比较两个序列, 找到第一处失配的元素, 返回一对迭代器, 分别指向两个序列中失配的元素
// ---------------------------------------------------
template <typename InputIter1, typename InputIter2>
dhsstl::pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter2 last1, InputIter2 first2){
    while(first1 != last1 && *first1 == *first2){
        ++first1;
        ++first2;
    }
    return dhsstl::pair<InputIter1, InputIter2>(first1, first2);
}

// 重载版本使用函数对象 comp 代替比较操作
template <typename InputIter1, typename InputIter2, typename Compared>
dhsstl::pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp){
    while(first1 != last1 && comp(*first1, *first2)){
        ++first1;
        ++first2;
    }
    return dhsstl::pair<InputIter1, InputIter2>(first1, first2);
}

} // dhsstl

#endif