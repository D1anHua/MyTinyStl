### 0. 写在前面
迭代器模式:
设计模式一书中对`iterator`模式的定义:
> 提供一种方法, 使之能够按照顺序寻访某个聚合物(容器)所含的各个元素, 而又无需暴露该聚合物的内部表达形式.

> 这篇文档用来当作自己对C++ STL的部分阅读笔记, 主要参考以下几个文章以及仓库
>
> 1. MyTinySTL --[Github 链接](https://github.com/Alinshans/MyTinySTL)
> 2. STL源码剖析一书
> 3. C++ STL 部分源码

<!-- more -->


### 1. `iterator`
迭代器的设计的中心思想在于: 提供一种胶合剂, 用来将数据容器`containers`和算法`algorithms`分开, 以方便其设计

迭代器是一种类似指针的对象, 对于指针来说最常见的也是最重要的便是内容提领`dereference`和成员访问`member access`. 因此, 在迭代器的设计过程中, 最重要的编程工作就是对`operator*` 和 `operator->`进行重载

---

### 2. 迭代器相应型别以及Traits编程技法

根据移动特性和施行操作, 迭代器被分为五类:

>  `input iterator`: 这种迭代器所指的对象, 不允许外界改变. 只读(read only)\
>  `Output iterator`: 唯写(write only)\
>  `forward Iterator`: 允许"写入型"的算法(例如replace()) 在这种迭代所形成的区间上进行读写操作\
>  `Bidirectional Iterator`: 可双向移动, 某些算法需要逆向走访某个迭代器区间(例如逆向拷贝某范围内的元素) 可以使用Bidirectional Iterator\
>  `Random Access Iterator`: 前四种迭代器都只是提供一部分的指针算术能力(前三种支持 operator++, 第四种增加了一个 operator--), 第五种则涵盖了所有的指针算术能力, 包括 +n ...

```cpp
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

> 上述给出的`iterator`模板仅仅用来给出用于萃取的 5个相对型别. 
>
> 而具体迭代器的实现只需要继承这个基类就可以, 而且这个基类也并没有任何的危害.纯粹就是型别的定义, 继承它不会招致任何的负担
>
> 另外, 由于为了符合规范, 任何迭代器都应该提供这五个内嵌的相应型别, 以利于萃取
> 
> 为了避免写代码的时候忘记了, 因此设计这样的一个iterator类别, 让每个新设计的迭代器继承它, 就可简化工作, 避免出错

![2023-09-13-21-42-45](https://hexoblog-1304281944.cos.ap-hongkong.myqcloud.com/hexo/pic/2023-09-13-21-42-45.jpeg)
`iterator_traits`萃取各个迭代器的相应型别, 方便迭代器使用时的模板选择. 另外也可以通过模板特例化来支持原生指针等等.
