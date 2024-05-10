#ifndef DHSTINYSTL_UNINITIALIZED_H_
#define DHSTINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素
#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace dhsstl{

// ------------------------------------------------
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间, 返回复制结束位置
// ------------------------------------------------
// 给下 标准 c++ 对该函数的 描述
// Copies elements from the range [first, last) to an uninitialized memory area beginning at d_first as if...
// 这个 uninit 的意思是 这个将要被拷贝到的区间是没有初始化过的,
// 而 普通的 copy, 所有元素都是初始化过的.
template <typename InputIter, typename ForwardIter>
ForwardIter
unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type){
    return dhsstl::copy(first, last, result);
}

template <typename InputIter, typename ForwardIter>
ForwardIter
unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type){
    // 如果失败了, 就一个也不能够被construct
    auto cur = result;
    try{
        for(; first != last; ++first, ++cur){
            dhsstl::construct(&*cur, *first);
        }
    }catch (...){
        for(; result != cur; --cur)
            dhsstl::destory(&*cur);
    }
    return cur;
}

template <typename InputIter, typename ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result){
    return dhsstl::unchecked_uninit_copy(first, last, result,
                                         std::is_trivially_copy_assignable<
                                         typename iterator_traits<ForwardIter>::
                                         value_type 
                                         >{});
}
// ------------------------------------------------
// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result 为起始处的空间, 返回复制结束位置
// ------------------------------------------------
template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter
unchecked_uninit_copy_n(InputIter first, Size n ,ForwardIter result, std::true_type){
    return dhsstl::copy_n(first, n, result).second;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter
unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type){
    auto cur = result;
    try{
        for(; n > 0; ++first, ++cur){
            // 传得是一个 右值 first
            dhsstl::construct(&*cur, *first);
        }
    }catch (...){
        for(; result != cur; --cur){
            dhsstl::destory(&*cur);
        }
    }
    return cur;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result){
    return dhsstl::unchecked_uninit_copy_n(first, n, result,
                                           std::is_trivially_copy_assignable<
                                           typename iterator_traits<InputIter>::
                                           value_type 
                                           >{});
}
// ------------------------------------------------
// uninitialized_fill
// 在 [first, last) 区间内填充元素值
// ------------------------------------------------
template <typename ForwardIter, typename T>
void
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type){
    dhsstl::fill(first, last, value);
}

template <typename ForwardIter, typename T>
void
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type){
    auto cur = first;
    try{
        for(; cur != last; ++cur){
            dhsstl::construct(&*cur, value);
        }
    }catch(...){
        for(; first != cur; ++first){
            dhsstl::destory(&*first);
        }
    }
}

template <typename ForwardIter, typename T>
void
uninitialized_fill(ForwardIter first, ForwardIter last, const T& value){
    return dhsstl::unchecked_uninit_fill(first, last, value,
                                          std::is_trivially_copy_assignable<
                                          typename iterator_traits<ForwardIter>::
                                          value_type 
                                          >{});
}
// ------------------------------------------------
// uninitialized_fill_n
// 从 first 位置开始, 填充元素值
// ------------------------------------------------
template <typename ForwardIter, typename Size, typename T>
ForwardIter
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type){
    return dhsstl::fill_n(first, n, value);
}

template <typename ForwardIter, typename Size, typename T>
ForwardIter
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type){
    auto cur = first;
    try{
        for(; n > 0; --n, ++cur){
            dhsstl::construct(&*cur, value);
        }
    } catch(...){
        for(; first != cur; ++first){
            dhsstl::destory(&*first);
        }
    }
    return cur;
}

template <typename ForwardIter, typename Size, typename T>
ForwardIter
uninitialized_fill_n(ForwardIter first, Size n, const T& value){
    return unchecked_uninit_fill_n(first, n, value,
                                   std::is_trivially_copy_assignable<
                                   typename iterator_traits<ForwardIter>::
                                   value_type 
                                   >{});
}
// ------------------------------------------------
// uninitialized_move
// 把[first, last)上的内容移动到以result为起始处的空间, 返回移动结束的位置
// ------------------------------------------------
template <typename InputIter, typename ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type){
    return dhsstl::move(first, last, result);
}

template <typename InputIter, typename ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type){
    ForwardIter cur = result;
    try{
        for(; first != last; ++first, ++cur){
            dhsstl::construct(&*cur, dhsstl::move(*first));
        }
    }catch(...){
        dhsstl::destory(result, cur);
    }
    // 这里不用throw吗
    return cur;
}

template <typename InputIter, typename ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result){
    return dhsstl::unchecked_uninit_move(first, last, result,
                                         std::is_trivially_move_assignable<
                                         typename iterator_traits<InputIter>:: 
                                         value_type>{}
                                        );
}

// ------------------------------------------------
// uninitialized_move_n
// 把[first, first+n)上的内容移动到以result为起始处的空间, 返回移动结束的位置
// ------------------------------------------------
template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type){
    return dhsstl::move(first, first+n, result);
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type){
    auto cur = result;
    try{
        for(; n > 0; --n, ++first, ++cur){
            dhsstl::construct(&*cur, dhsstl::move(*first));
        }
    }catch(...){
        for(; result != cur; ++result){
            dhsstl::destory(&*result);
        }
        throw;
    }
    return cur;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result){
    return dhsstl::unchecked_uninit_move_n(first, n, result,
                                           std::is_trivially_move_assignable<
                                           typename iterator_traits<InputIter>::
                                           value_type
                                           >{}
    );
}

} // namespace dhsstl
#endif // !DHSTINYSTL_UNINITIALIZED_H_