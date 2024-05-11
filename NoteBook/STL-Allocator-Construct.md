#### 0. 序

接下来分析STL的空间配置器(Allocator).

> 这篇文档用来当作自己对C++ STL的部分阅读笔记, 主要参考以下几个文章以及仓库
>
> 1. MyTinySTL --[Github 链接](https://github.com/Alinshans/MyTinySTL)
> 2. STL源码剖析一书
> 3. C++ STL 部分源码
<!-- more -->

我参考的这个仓库中仅仅实现了基层内存的配置/释放行为(也就是简单的 `::operator new` 和 `::operator delete`的封装), 并没有考虑效率上的强化, 也没有实现内存池等相关的数据结构.

STL`<memory>`中内存空间的配置/释放与对象内容的构造/析构是分别由两个文件来管理的.其中`<construct.h>`中定义由两个基本函数, 分别管理构造用的`construct()` 和 析构用的 `destory()`

![](https://hexoblog-1304281944.cos.ap-hongkong.myqcloud.com/hexo/pic/2024-03-18-21-28-05.png)

#### construct.h
实现的过程中有一些注意事项:

constroct()的实现注意对于有值的构造, 根据所给的值调用`forward()`来选择对应的移动构造函数或者是拷贝构造函数.

destory()总的来说有两个版本:

- 第一个版本接受一个指针, 讲该指针析构掉即可.

- 第二个版本接受一个迭代器, 由于迭代器中间的范围可能很大, 假如每个对象的析构函数都是无关痛痒的(`trivial destructor`), 那么没有必要一次次调用这些析构函数, 将造成很大的损失, 因此这里首先利用了`type_traits`做了判断.

> ```cpp
> // type_traites
> std::is_trivially_copy_assignable<
>   typename iterator_traits<InputIter>::value_type>{}
> std::is_trivially_move_assignable<
>   typename iterator_traits<InputIter>::value_type>{}
> ```

![](https://hexoblog-1304281944.cos.ap-hongkong.myqcloud.com/hexo/pic/2024-03-18-21-44-36.png)

#### alloc
关于这部分我没有实现, 因此跳过

#### uninitialized.h
实现一些作用于未初始化的空间上的一组函数, 对于容器的实现很有帮助.

**注:**

- 必须具有"commit or rollback"语义, 要么产生出所有必要元素, 要么不产生任何元素.\
- POD类型: 意为Plain Old Data, 也就是标量型别(`scalar types`)或者说传统的 C struct型别. POD 型别必然拥有`trivial ctor/ dtor/ copy/ assignment`函数, 因此, 我们可以针对POD型别采用最有效率的手法.\
- C++ STL库有个整体的原则, 就是在整个库的设计过程中, 要充分考虑到传统C struct型别.
(或者说需要考虑到针对不同的类型进行模板特例化以及function template的参数推导机制等操作来优化针对这些原始类型的优化)(type_traites)\

**uninitialized_copy:**
```cpp
/*!
 * @brief 未初始化区域内的范围copy 
 * 调用copy constructor, 给[first, last)范围内的每一个对象产生一份复制品, 放进输出范围内
 * @return 返回被拷贝区域的起始位置
 */
templete<typename InputIter, typename ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter rusult);
```

**uninitialized_fill:**
```cpp
/*!
 * @brief 像未初始化的区域中填充值
 */
template<typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);
```

**uninitialized_move:**
``` cpp
/*!
 * @brief 未初始化区域内的范围move
 * 调用move constructor, 给[first, last)范围内的每一个对象产生一份复制品, 放进输出范围内
 * @return 返回被拷贝区域的起始位置
 */
templete<typename InputIter, typename ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter rusult);
```