#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 
#include <cstddef>
#include "type_traits.h"

namespace dhsstl
{
  
// move
// move: 获得一个绑定到左值上的右值引用

/*
一般情况下, C++假定通过作用域运算符'::'所得到的名字不是类型, 而是一个数据成员
因此, 若要说明他得到的是一个类型, 就需要加上typename关键字来显示的告诉C++这是一个类型
*/

/*
remove_reference:是定义在<type_traits>头文件中的一组 标准库的类型转换模板
其有一个模板类型参数 和 一个名为 type 的类型成员
注: type 是一个类型成员, 因此, 需要加上typename

另: 以remove_referenct举例,
其处理流程为:
  若: 类型T为引用类型, 则去掉引用, 获得其引用的类型
  反之, 若类型T不为引用类型, 则什么也不干, 过的T的类型, 类似于 decltype
*/
template<typename T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept{
  return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

/*
forward:
forward的作用是 转发, 将一个或者多个实参连同类型不变的转发给其他函数
forward的调用: std::forward<Type>(arg)
注: forward必须通过显式的模板实参来调用!!!, 这是由于编辑器无法推断出T的类型
    从下文定义来看, 返回值是T的右值引用, 参数是T remove之后的, 这是推断不出来的
forward可以保持给定实参的左值/ 右值属性

如果传入的arg是type的右值, 就返回右值
如果传入的arg是type的左值, 就返回左值
*/

// 应对arg是左值的情况
template <typename T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept{
  return static_cast<T&&>(arg);
}

// 应对arg是右值的情况
// 为什么这里需要判断一下:
// 如果说: 我们给定一个T 是 左值引用
// T&& 的 右值是 其实还是左值
template <typename T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept{
  // &arg:
  static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
  return static_cast<T&&>(arg);
}

// swap
// 注: 这里为什么要用move?
// https://blog.csdn.net/cxsmarkchan/article/details/50792007
// 避免过多的拷贝操作
template <typename Tp>
void swap(Tp& lhs, Tp& rhs){
  auto tmp = dhsstl::move(lhs);
  lhs = dhsstl::move(rhs);
  rhs = dhsstl::move(tmp);
}

template <typename ForwardIter1, typename ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2){
  for(; first1 != last1; ++first1, (void)++first2){
    dhsstl::swap(*first1, *first2);
  }
  return first2;
}

template <typename Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N]){
  dhsstl::swap_range(a, a+N, b);
}


// ------------------------------------------------
// pair
// 结构体模板: pair
// 两个模板参数分别表示两个数据的类型
// 用first和second来分别取出第一个数据和第二个数据
template <typename Ty1, typename Ty2>
struct pair{
  using first_type = Ty1;
  using second_type = Ty2;

  first_type first;
  second_type second;

  // default constructiable
  template <
    typename Other1 = Ty1, 
    typename Other2 = Ty2, 
    typename = typename std::enable_if<
      std::is_default_constructible<Other1>::value &&
      std::is_default_constructible<Other2>::value,
      void
    >::type 
    >
    constexpr pair() : first(), second()
  {
  }

  // implicit constructiable for this type 
  template <class U1 = Ty1, class U2 = Ty2,
    typename std::enable_if<
      std::is_copy_constructible<U1>::value &&
      std::is_copy_constructible<U2>::value &&
      std::is_convertible<const U1&, Ty1>::value &&
      std::is_convertible<const U2&, Ty2>::value,
      int>::type = 0
    >
    constexpr pair(const Ty1& a, const Ty2& b)
    :first(a), second(b)
  {
  }

  // explicit constructible for this type
  template <class U1 = Ty1, class U2 = Ty2,
    typename std::enable_if<
      std::is_copy_constructible<U1>::value &&
      std::is_copy_constructible<U2>::value &&
      (
        !std::is_convertible<const U1&, Ty1>::value ||
        !std::is_convertible<const U2&, Ty2>::value
      ),
      int>::type = 0
    >
    explicit constexpr pair(const Ty1& a, const Ty2& b)
    :first(a), second(b)
  {
  }
  
  pair(const pair& rhs) = default;
  pair(pair&& rhs) = default;

  // template <class T, class... Args> struct is_constructible;
  // Trait class that identifies whether T is a constructible type with the set of argument types specified by Arg.
  // implicit constructiable for other type
  template<typename Other1, typename Other2,
           typename std::enable_if<
           std::is_constructible<Ty1, Other1>::value &&
           std::is_constructible<Ty2, Other2>::value &&
           std::is_convertible<Other1&&, Ty1>::value &&
           std::is_convertible<Other2&&, Ty2>::value,
           int>::type = 0
           >
           constexpr pair(Other1&& a, Other2&& b) : 
           first(dhsstl::forward<Other1>(a)),
           second(dhsstl::forward<Other2>(b)) 
  {
  }

  // explicit constructiable for other type
  template <typename Other1, typename Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    (
      !std::is_convertible<Other1, Ty1>::value ||
      !std::is_convertible<Other2, Ty2>::value
    ),int
    >::type = 0
    >
  explicit constexpr pair(Other1&& a, Other2&& b)
  : first(dhsstl::forward<Other1>(a)),
  second(dhsstl::forward<Other2>(b))
  {
  }

  // implicit construciable for other pair
  template <typename Other1, typename Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, const Other1&>::value &&
    std::is_constructible<Ty2, const Other2&>::value && 
    std::is_convertible<const Other1&, Ty1>::value &&
    std::is_convertible<const Other2&, Ty2>::value, int
    >::type = 0
    >
  constexpr pair(const pair<Other1, Other2>& other)
  : first(other.first),
  second(other.second)
  {
  }

  // explicit constructiable for other pair
  template <typename Other1, typename Other2,
   typename std::enable_if<
   std::is_constructible<Ty1, const Other1&>::value &&
   std::is_constructible<Ty2, const Other2&>::value &&
   (
    !std::is_convertible<const Other1&, Ty1>::value ||
    !std::is_convertible<const Other2&, Ty2>::value
   ), int 
   >::type = 0
   >
  explicit constexpr pair(const pair<Other1, Other2>& other) 
  : first(other.first),
  second(other.second) 
  {
  }

  // implicit constructible for other pair
  template <typename Other1, typename Other2,
   typename std::enable_if<
   std::is_constructible<Ty1, Other1>::value &&
   std::is_constructible<Ty2, Other2>::value &&
   std::is_convertible<Other1, Ty1>::value &&
   std::is_convertible<Other2, Ty2>::value, int 
   >::type = 0  
  >
  constexpr pair(pair<Other1, Other2>&& other)
  : first(dhsstl::forward<Other1>(other.first)),
  second(dhsstl::forward<Other2>(other.second))
  {
  }

  // explicit constructible for other pair
  template <typename Other1, typename Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    (
      !std::is_convertible<Other1, Ty1>::value ||
      !std::is_convertible<Other2, Ty2>::value
    ),int 
    >::type = 0
  >
  explicit constexpr pair(pair<Other1, Other2>&& other)
  : first(dhsstl::forward<Other1>(other.first)),
  second(dhsstl::forward<Other2>(other.second))
  {
  }

  // copy assign for this pair
  pair& operator=(const pair& rhs){
    if(this != &rhs){
      first = rhs.first;
      second = rhs.second;
    }
    return *this;
  } 

  // move assign for this pair
  pair& operator=(pair&& rhs){
    if(this != &rhs){
      first = dhsstl::move(rhs.first);
      second = dhsstl::move(rhs.second);
    }
    return *this;
  }

  // copy assign for other pair
  template <typename Other1, typename Other2>
  pair& operator=(const pair<Other1, Other2>& other){
    first = other.first;
    second = other.second;
    return *this;
  }

  // move assign for other pair
  template <typename Other1, typename Other2>
  pair& operator=(pair<Other1, Other2>&& other){
    first = dhsstl::forward<Other1>(other.first);
    second = dhsstl::forward<Other2>(other.second);
    return *this;
  }

  ~pair() = default;

  void swap(pair &other){
    if(this != &other){
      dhsstl::swap(first, other.first);
      dhsstl::swap(second, other.second);
    }
  }
};

// 重载比较操作符
template <typename Ty1, typename Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename Ty1, typename Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return !(lhs == rhs);
}

template <typename Ty1, typename Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <typename Ty1, typename Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return !(lhs < rhs);
}

template <typename Ty1, typename Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return rhs < lhs;
}

template <typename Ty1, typename Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
  return !(rhs < lhs);
}

// 重写 dhsstl 的 swap
template <typename Ty1, typename Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs){
  lhs.swap(rhs);
}

// 全局函数, 让两个数据成为一个pair
template <typename Ty1, typename Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second){
  return pair<Ty1, Ty2>(dhsstl::forward<Ty1>(first), dhsstl::forward<Ty2>(second));
}
} // namespace dhsstl

#endif