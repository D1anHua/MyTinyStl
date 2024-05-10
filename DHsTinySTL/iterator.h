#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

/*
迭代器模式:
设计模式一书中对 iterator模式的定义:
    提供一种方法, 使之能够按照顺序寻访某个聚合物(容器)所含的各个元素, 而又无需暴露该聚合物的内部表达形式.

迭代器是一种类似指针的对象.
指针最重要的内容便是 内容提领(dereference'.')以及成员访问(member access '->') 的重载

迭代器的相对型别:
在算法运用迭代器时, 很有可能会用到相对型别(associated type), 所谓相对型别, 迭代器所指之物就是其中之一.

按照我的理解, 所谓的迭代器相对型别, 就是指代 迭代器所指对象的一些特征,
比如说: 我理解有两类:
1. 迭代器所指的对象相关: 比如 对象的value_type, 以及这个类型的指针类型 以及引用类型
    (这是在泛型算法中可能需要的, 知道这个value_type才能确定对应的模板, 才能进行相应的实例化
    另一方面, 为了返回左值, 必须要有引用类型
    注意: 引用类型和指针类型并不相同
    : 迭代器本身是指针
    从"迭代器所指之物的内容是否允许改变"的角度来看,
        如果是 不允许改变 所指之物的内容, 称为 constant iterators, 例如 const int* pic;
        反之, 则是 mutable iterators, 例如 int* pi
        当我们对 mutable iterators 的迭代器 进行 内容提领 的操作时,获得的不应该是一个右值, 而应该是一左值
        在C++中, 如果函数要传回左值, 都是以 by reference 的方式来进行的
        所以 如果 p 是一个 mutable iterators 那么 *p 的类别 就应该不是 T 而是 T&;
        同理, 如果 p 是一个 constant iterators 那么 *p 的类型 就应该是 const T&

        同理, 加入 我们需要返回一个左值, 另她代表p所指之物的地址, 也就是说, 我们可以传回一个 pointer
        注意, pointer就是地址, 这个 * 号 的作用不是 内容提领, 而是代表他是一个指针
)
2. 迭代器所指对象的存取方式: 容器都有对应的存取方式, 应对不同的存取方式, 泛型算法有不同的解决方案:
    这又包括两个方面: 存取方式以及 大小的表达, 迭代器需要有 end-front的操作, 因此相减操作,
    而对于不同的存取方式, 这种差值的意义也不太相同.

这五个相应型别分别为: value_type, difference_type, pointer, reference, iterator_category
*/

/*
根据移动特性和施行操作, 迭代器被分为五类:
  input iterator: 这种迭代器所指的对象, 不允许外界改变. 只读(read only)
  Output iterator: 维写(write only)
  forward Iterator: 允许"写入型"的算法(例如replace()) 在这种迭代所形成的区间上进行读写操作
  Bidirectional Iterator: 可双向移动, 某些算法需要逆向走访某个迭代器区间(例如逆向拷贝某范围内的元素) 可以使用Bidirectional Iterator
  Random Access Iterator: 前四种迭代器都只是提供一部分的指针算术能力(前三种支持 operator++, 第四种增加了一个 operator--), 第五种则
    涵盖了所有的指针算术能力, 包括 +n ...
*/

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数，
#include <cstddef>
#include "type_traits.h"

namespace dhsstl
{

// 五种迭代器类型
// 为了能够进行萃取, 如果traits能够萃取出迭代器的种类, 我们就可以将这个相应型别(iterator_category)作为某算法的第三个参数.
// 这个相应型别必须是一个 class type, 因为需要依靠这个(一个类型)来进行重载决议(overloaded resolution), 也就是判断函数的最优重载
// 这些class只是作为一个class使用, 因此不需要任何的成员, 继承机制的应用有助于多态的进行, 也即动态类型推断.
// 假如说, 某一个算法 对于 input_iter... 和 forward ... 和 bid... 三种迭代器所进行的操作都一样.
// 那么我们可以利用继承机制, 直接不覆盖基类的该函数, 因此也就在函数匹配的过程中可以进行更好的匹配, 从而减少工作量
// 比如说 distance算法:
// 其对于 1 3 4这三个迭代器都一样, 都是++1的操作, 因此这三个都用基类的版本就可
// 而random_iter则不同, 其可以直接 +n, 因此就需要特别写他自己的版本, 而其他迭代器类型就可以不进行重写, 而减少了工作量
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator 模板
// 仅仅用来给出用于萃取的 5个相对型别, 而具体迭代器的实现
// 只需要继承这个基类就可以, 而且这个基类也并没有任何的危害.纯粹就是型别的定义, 继承塔不会招致任何的负担
// 另外, 由于为了符合规范, 任何迭代器都应该提供这五个内嵌的相应型别, 以利于萃取
// 为了避免写代码的时候忘记了, 因此设计这样的一个iterator类别, 让每个新设计的迭代器继承它, 就可简化工作, 避免出错
// ************************************
// 新写的iterator 都需要继承该类
// 该类后三个相对型别有默认值, 故最少只需要给出两个参数即可(Category, T)
// 注: 具体的迭代器实现就交由容器来实现, 因为每个容器的操作不尽相同
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


// iterator traits 榨汁机
// 为什么要有 has_iterator_cat 和 is_convertible这两个类
// https://zhuanlan.zhihu.com/p/547313994
// 因为 iterator_traits可以萃取的iterator是有前提的, 只能对有iterator_category这个属性
// 并且 iterator_category 可以转换为 input_iterator_tag 和 output_iterator_tag 的 
// iterator萃取

// has_iterator_cat 利用了 SFINAE 的技巧
// SFINAE: https://zhuanlan.zhihu.com/p/21314708
template <class T>
struct has_iterator_cat
{
private:
  struct two { char a; char b; };
  template <class U> static two test(...);  //可变参数列表
  template <class U> static char test(typename U::iterator_category* = 0);
  // 运用了SFINAE的技巧, 若有iterator_category属性, 则会优先匹配到这个
public:
  // 这里给0是为了啥
  // 如果没有这个0, 则test<T>()将不知道调用那个重载版本, 因为哪个重载版本都一样匹配
  static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_impl {};

template <typename Iterator>
struct iterator_traits_impl<Iterator, true>
{
    typedef typename Iterator::iterator_category itrerator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::difference_type   difference_type;
};

template <typename Iterator, bool>
struct iterator_traits_helper {};

template <typename Iterator>
struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<Iterator,
    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
{
};

// iterator_traits: 萃取器
// 给他一个 迭代器T, 它能够萃取出 我们想从这个 迭代器 T 中所需要的 相对类型信息,
// 而不需要对 T 有任何的假设
// 萃取迭代器的特性
// 首先, 得有 iterator_category的属性, 所以 has_iterator_cat<>
// 然后, 这个 iterator_category还得能够转换为 input_iterator_tag 或者 output_iterator_tag
// 因为这tag就只有五类, 而其他的三类都继承的input
template <typename Iterator>
struct iterator_traits
    : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

// 针对原声指针的偏特化版本(模板特例化)
// 一下这个版本, 适配 T是原声指针的情况, 
template <typename T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag          iterator_category;
    typedef T                                   value_type;
    typedef T*                                  pointer;
    typedef T&                                  reference;
    typedef ptrdiff_t                           difference_type;
};

// 针对 const T* 的偏特化版本,
// 如果对于 const T* 也采用上述的 偏特化版本, 则推断出的 T 是 const T, 完全不是我们想要的, 因为其value type肯定不是 const T
// 因为 这个 value_type的作用是声明一个 可以赋值的版本, 而 这个声明出来的是不可赋值的暂时变量, 没有什么用
template <typename T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag          iterator_category;
    typedef T                                   value_type;
    typedef const T*                            pointer;
    typedef const T&                            reference;
    typedef ptrdiff_t                           difference_type;                     
};


template <typename T, typename U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
    : public m_bool_constant<std::is_convertible<
    typename iterator_traits<T>::iterator_categoery, U>::value>
{
};

template <typename T, typename U>
struct has_iterator_cat_of<T, U, false> : public m_false_type{};

// 萃取迭代器的类型
template <typename Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag>{};

template <typename Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag>{};

template <typename Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag>{};

template <typename Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag>{};

template <typename Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag>{};

template <typename Iterator>
struct is_iterator : 
    public m_bool_constant<is_input_iterator<Iterator>::value || 
    is_output_iterator<Iterator>::value >
{
};

// 这个函数可以很方便的决定某个迭代器的类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();
}

// 这个函数而可以很方便的决定某个迭代器的 difference_type
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator*){
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 这个函数可以很方便的决定某个迭代器的 value_type
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 以下函数用于计算迭代器之间的距离

// distance 的 input_iterator_tag 的版本
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type 
__distance(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last){
        ++first; ++n;
    }
    return n;
}
// distance 的 random_access_iterator_tag 的版本
template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag){
    return last - first;
}

// 整合版本
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last){
    return __distance(first, last, iterator_traits<InputIterator>::category(first));
}

// 以下是整组 advance 函数
// 也就是让迭代器 前进n个距离
template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag){
    while(n--) ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag){
    if(n >= 0)
        while (n--) i++;
    else
        while (n++) i--;
}

template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag){
    i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n){
    __advance(i, n, iterator_traits<InputIterator>::category(i));
}

// --------------------------------------------------------------------
// 模板类 : reverse_iterator
// 代表反向迭代器, 使前进为后退, 后退为前进
template <typename Iterator>
class reverse_iterator
{
private:
    Iterator current;   // 记录对应的正向迭代器

public:
    // 反向迭代器对性的五种相应型别
    typedef typename iterator_traits<Iterator>::iterator_category   iterator_category;
    typedef typename iterator_traits<Iterator>::value_type          value_type;
    typedef typename iterator_traits<Iterator>::difference_type     difference_type;
    typedef typename iterator_traits<Iterator>::pointer             pointer;
    typedef typename iterator_traits<Iterator>::reference           reference;

    typedef Iterator                                                iterator_type;
    typedef reverse_iterator<Iterator>                              self;

public:
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i) : current(i) {}
    reverse_iterator(const self& rhs) : current(rhs.current) {}


public:
    // 取出相应的正向迭代器
    // 在成员函数后面 + const 代表 该成员函数不能修改this指针. 也就是它不能修改成员变量
    iterator_type base() const{
        return this->current;
    }

    // 重载操作符
    reference operator*() const{
        // 实际对应正向迭代器的前一个位置
        auto tmp = current;
        return *--tmp;
    }

    pointer operator->() const{
        return &(operator*());
    }

    // 前向 ++ 变为 后退 --
    self& operator++(){
        --current;
        return *this;
    }
    // 注意: 前置++ 操作符 和 后置++ 操作符 的区分就是在形参列表里面加上一个不需要的int类型, 实际调用的时候以有所区分
    self operator++(int){
        self tmp = *this;
        --current;
        return tmp;
    }
    // 后退 -- 变为 前进 ++
    self& operator--(){
        ++current;
        return *this;
    }
    self operator--(int){
        self tmp = *this;
        --current;
        return tmp;
    }

    // 重载 += 运算符
    self& operator+=(difference_type n){
        current -= n;
        return *this;
    }

    self operator+(difference_type n) const{
        return self(current - n);
    }

    self& operator-=(difference_type n){
        current += n;
        return *this;
    }

    self operator-(difference_type n) const{
        return self(current + n);
    }

    reference operator[](difference_type n) const{
        return *(*this + n);
    }
};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs){
    return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs){
    return lhs.base() == rhs.base();
}
template <typename Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs){
    return !(lhs == rhs);
}
template <typename Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs){
    return lhs.base() < rhs.base();
}
template <typename Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs){
    return rhs < lhs; 
}
template <typename Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs){
    return !(lhs < rhs);
}
template <typename Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs){
    return !(rhs < lhs);
}

}// namespace dhsstl

#endif // !DHSTINYSTL_ITERATOR_H_