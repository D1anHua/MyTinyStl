---
title: STL--Move & Forward & SFINAE
date: 2023-09-12 16:30:49
tags: tinySTL
categories:
 - C++
 - STL
---

### 0. 序

**Uility.h**: 这个头文件主要包含一些工具, 包含`move`, `forward`, `swap`等函数, 以及一个`pair`类

> 这篇文档用来当作自己对C++ STL的部分阅读笔记, 主要参考以下几个文章以及仓库
>
> 1. MyTinySTL --[Github 链接](https://github.com/Alinshans/MyTinySTL)
> 2. STL源码剖析一书
> 3. C++ STL 部分源码

<!-- more -->

### 1. move
首先列出`std::move`的实现:
``` cpp
/*!
* @brief Convert a value to an rvalue.
* @param __t  A thing of arbitrary(任意的) type.
* @return The param cast to an rvalue-reference to allow moving it.
*/
template<typename _Tp>
constexpr typename std::remove_reference<_Tp>::type&& 
move(_Tp&& __t) noexcept
{ return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }
```
对move的理解:

1. 从实现的角度来说: `std::move`就是一个`static_cast`而已, 它存在的目的就是帮助编译器选择重载函数(对应的移动构造函数和移动赋值函数)

2. `move`的实际作用是: 转移所有权, 而不是移动元素. 可以用深浅拷贝的情况来理解该问题:

    1. 假设有一个 数组, 其需要在堆区开辟一系列元素

    2. 然后我们想要有一个新的变量 与 这个 数组相等. 那我们就需要在相应的拷贝构造函数或者拷贝赋值函数中手动开辟堆中的元素, 并挨个复制过去.

    3. 但现在假如, 你现在想要有一个新的变量 想要 与这个数组相等, 然后原来这个数组就不需要了, 这个时候不需要再开辟新的元素了, 可以直接接手 原来数组 在堆区中的元素.

这里还有一些概念上的区别, 可以查看以下参考链接. 还需要理解一些 右值 亡值 纯右值等等..

从我自己的角度理解: `move`的存在是帮助编译器选择重载函数(比如说某些类(List)的移动构造函数是浅拷贝实现的, 而拷贝构造函数是深拷贝而实现的, 这个例子中就深刻体现了`move`的作用).
然而, 对于移动构造函数与拷贝构造函数没有区别的数据结构, 或者简单的说没有深拷贝以及浅拷贝之分的数据结构, `move`操作是作用不大的.

>注:
> `T&& v`中`v`仍然是一个**左值**, 其是一个右值引用, 是对 `rvalue` 的引用, 但这个引用本身是一个`lvalue`. 代码中如果要调用T类型的移动函数需要使用`std::move(v);`

这里给出几个知乎的链接:

[1. c++ move函数到底是什么意思？](https://www.zhihu.com/question/64205844)

[2. 什么是move？理解C++ Value categories，move， move in Rust一文读懂C++右值引用和std::move](https://zhuanlan.zhihu.com/p/374392832)

----

### 2. forward
首先列出`std::forward`的实现:

``` cpp
  /*!
   *  @brief  Forward an lvalue.
   *  @return The parameter cast to the specified type.
   *
   *  This function is used to implement "perfect forwarding".
   */
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
    {
      static_assert(!std::is_lvalue_reference<_Tp>::value,
	  "std::forward must not be used to convert an rvalue to an lvalue");
      return static_cast<_Tp&&>(__t);
    }
```
在`forward`的注释中, 写明了这个函数被用来实现**完美转发**.

1. 从实现上来讲, 也可以将`forward`看作是强制类型转换, 其存在也是为了帮助编译器选择重载函数.

2. `forward<Arg>(arg)`使用时必须指定模板参数`Arg`.

    1. 如果传入的`arg`是`Arg`的左值, 就返回左值(引用折叠)

    2. 如果传入的`arg`是`Arg`的右值, 就返回右值

> 注: `forward` 和 `move` 如果想要深入理解, 就不能仅仅从实现的角度来考虑, 而必须深入理解右值等的概念, 参见`move`部分的参考文件

----

### 3. swap

这里之给出部分`swap`的实现, 迭代器的`swap`算法是调用了该部分的代码
```cpp
template <typename Tp>
void swap(Tp& lhs, Tp& rhs){
  auto tmp = dhsstl::move(lhs);
  lhs = dhsstl::move(rhs);
  rhs = dhsstl::move(tmp);
}
// 如果 一个类没有移动操作, 通过正常的函数匹配, 会使用对应的拷贝操作来代替移动操作
// 
// 注意对比与下列写法的区别
// template <typename Tp>
// void swap(Tp& lhs, Tp& rhs){
//     auto tmp = lhs;
//     lhs = rhs;
//     rhs = tmp;
// }
// 这里如果 Tp 类型有堆区元素的话, 每一步都将花费大量的时间
// 并且需要拷贝一份堆区元素, 十分浪费时间与空间
```
这里第三行`auto tmp = dhsstl::move(lhs);`, `tmp`是一个右值引用, 但它仍然是一个左值, 再使用时依然需要通过`std::move(tmp)`来告诉编辑器, 来做一个强制类型转换

---

### 4. SFINAE --> pair
在`C11`中, `pair`的实现运用了`SFINAE(Substitution failure is not an erros)`, 也是用来在模板编程中辅助编译器进行对应的 函数选择.

> 首先来看一下 `enable_if` 的实现[参考链接](https://zhuanlan.zhihu.com/p/21314708)
> ```cpp
>   template<bool, typename _Tp = void>
>     struct enable_if
>     { };
> 
>   // 模板特例化
>   template<typename _Tp>
>     struct enable_if<true, _Tp>
>     { typedef _Tp type; };
> ```
> 
> 对于`enable_if`这个类来说, 需要两个模板参数, 第一个为`bool`类型, 第二个为`_Tp`, 并对`bool`为`true`的情况 提供了 特例化的版本, 假如有如下的情况:
> ``` cpp
> typename std::enable_if<true, int>::type t; //正确
> typename std::enable_if<true>::type; //可以通过编译，没有实际用处，推导的模板是偏特化版本，第一模板参数是true，第二模板参数是通常版本中定义的默认类型即void
> typename std::enable_if<false>::type; //无法通过编译，type类型没有定义
> typename std::enable_if<false, int>::type t2; //同上
> ```
> 
> 接下来来看`enable_if`的使用:
> ```cpp
> // enable_if 用法
> // 1. the return type (bool) is only valid if T is an integral type:
> template <typename T>
> typename std::enable_if<has_reserve<T>::value,void>::type
>   reserve_test1 () {cout << "reserve_test1"<< endl;}
> 
> // 2. the second template argument is only valid if T is an integral type:
> template < typename T,
>            typename = typename std::enable_if<has_reserve<T>::value>::type>
> void reserve_test2 () {cout <<"reserve_test2" << endl;}
> 
> int main() {   
>     reserve_test1<TestReserve>();
>     reserve_test2<TestReserve>();
>     return 0;
> } 
> ```
> 上面展示了`enable_if`的两种惯用方法[参考链接](https://www.luozhiyun.com/archives/744):
> 
>     1. 函数参数或者返回值使用`enable_if`用来帮助函数重载 
> 
>     2. 模板参数额外指定一个默认的参数
> 
> 使用`enable_if`可以控制函数只接受某种类型, 这种类型对应于`enable_if`使得其第一个参数为`true`, 此时才会有内嵌类型`type`, 已完成函数匹配

通过结合`SFINAE`来看这个`pair`代码(部分):
```cpp
  _Ty1 first;
  _Ty2 second;
  // default constructiable
  template <
    typename Other1 = Ty1, 
    typename Other2 = Ty2, 
    typename = typename std::enable_if<
      std::is_default_constructible<Other1>::value &&
      std::is_default_constructible<Other2>::value,
      void>::type 
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
  // ...
```
该代码利用了上述`SPINAE`的技巧, 并且该技巧也将在后面的`STL`代码中频繁使用.

回到`pair`类, 其实现了很多不同情况下的默认构造, 拷贝构造, 拷贝移动, 以及显式的构造, 赋值等等.  这里代码较多, 不一一展示了.