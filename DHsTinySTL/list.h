#ifndef DHSTINYSTL_LIST_H_
#define DHSTINYSTL_LIST_H_

/*
这个头文件包含了一个模板类: list
list: 双向链表

// notes:
// 
// 异常保证:
// dhsstl::list<T> 满足基本宜昌保证, 部分函数无异常保证, 并对以下等函数做强异常安全保证
//  * emplace_front
//  * emplace_back
//  * emplace
//  * push_front
//  * push_bcak
//  * insert

https://en.cppreference.com/w/cpp/container/list 中显示
template<
    class T,
    class Allocator = std::allocator<T>
> class list;

namespace pmr {
    template <class T>
    using list = std::list<T, std::pmr::polymorphic_allocator<T>>;
}
*/

/*
SGI list 不仅是一个双向链表, 而且还是一个环状的双向链表, 所以其只需要一个指针, 便可以完整表现整个链表
*/
#include <initializer_list>
#include "iterator.h"
#include "memory.h"
#include <functional>
#include "util.h"
#include "exceptdef.h"

namespace dhsstl{

template <typename T> struct list_node_base;
template <typename T> struct list_node;

template <typename T>
struct node_traits
{
    // base_ptr 就是一个没有值的节点, 就类似于带有头节点的链表, 而这个头节点用base来表示
    // node_ptr 就是正常的有值的节点
    // list 是一个 带有头节点的双向循环链表, 一般就用头节点来指代这条链表, c++同理
    // 其中就用 这个 base_ptr 表示 链表
    // 因此 List中的元素是 base_ptr
    typedef list_node_base<T>*      base_ptr;
    typedef list_node<T>*           node_ptr;
};

// list的节点结构
template <typename T>
struct list_node_base
{  
    typedef typename node_traits<T>::base_ptr   base_ptr;
    typedef typename node_traits<T>::node_ptr   node_ptr;

    base_ptr prev;
    base_ptr next;

    list_node_base() = default;

    inline node_ptr as_node(){ return static_cast<node_ptr>(self()); }

    inline void unlink(){ prev = next = self(); }

    // 这句话的作用是啥
    // 我觉得只是封装一下这句话
    inline base_ptr self(){ return static_cast<base_ptr>(&*this); }
};

template <typename T>
struct list_node : public list_node_base<T>{
    typedef typename node_traits<T>::base_ptr   base_ptr;
    typedef typename node_traits<T>::node_ptr   node_ptr;

    T value;
    list_node() = default;
    list_node(const T& v): value(v){}

    inline base_ptr as_base(){ return static_cast<base_ptr>(&*this); }

    inline self(){ return static_cast<node_ptr>(&*this); }
};

// List 的迭代器设计
template <typename T>
struct list_iterator : public dhsstl::iterator<dhsstl::bidirectional_iterator_tag, T>{
    typedef T                                   value_type;
    typedef T*                                  pointer;
    typedef T&                                  reference;
    typedef typename node_traits<T>::base_ptr   base_ptr;
    typedef typename node_traits<T>::node_ptr   node_ptr;
    typedef list_iterator<T>                    self;

    base_ptr node_; // 指向当前节点

    // 构造函数
    list_iterator() = default;
    list_iterator(base_ptr x) : node_(x){}
    list_iterator(node_ptr x) : node_(x->as_base()){}
    list_iterator(const list_iterator& rhs):node_(rhs.node_){}

    // 重载操作符
    reference operator*() const{ return node_->as_node()->value; }
    pointer   operator->() const{ return &(operator*()); }

    self& operator++(){
        DHSSTL_DEBUG(node_ != nullptr);
        node_ = node_ -> next;
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--(){
        DHSSTL_DEBUG(node_ != nullptr);
        node_ = node_ -> prev;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 重载比较操作符
    bool operator==(const self& rhs) const { return node_ == rhs.node_ ; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_ ; }
};

template<typename T>
struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>{
    typedef T                                           value_type;
    typedef const T*                                    pointer;
    typedef const T&                                    reference;
    typedef typename node_traits<T>::base_ptr           base_ptr;
    typedef typename node_traits<T>::node_ptr           node_ptr;
    typedef list_const_iterator<T>                      self;

    base_ptr node_;

    list_const_iterator() = default;
    list_const_iterator(base_ptr x)                       :node_(x){}
    list_const_iterator(node_ptr x)                       :node_(x->as_base()){}
    list_const_iterator(const list_iterator<T>& rhs)      :node_(rhs.node_){}
    list_const_iterator(const list_const_iterator<T>& rhs):node_(rhs.node_){}

    reference operator*()     const{ return node_->as_node()->value; }
    pointer   operator->()    const{ return &(operator*()); }

    self& operator++(){
        DHSSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++*this; 
        return tmp;
    }

    self& operator--(){
        DHSSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
    
    // 重载比较操作符
    bool operator==(const self& rhs) const{ return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const{ return node_ != rhs.node_; }
};

// 模板类: list
// 模板参数 T 代表数据类型
template <typename T>
class list{

public:
    // list 的嵌套型别定义
    typedef dhsstl::allocator<T>                        allocator_type;
    typedef dhsstl::allocator<T>                        data_allocator;
    typedef dhsstl::allocator<list_node_base<T>>        base_allocator;
    typedef dhsstl::allocator<list_node<T>>             node_allocator;

    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;

    typedef list_iterator<T>                            iterator;
    typedef list_const_iterator<T>                      const_iterator;
    typedef dhsstl::reverse_iterator<iterator>          reverse_iterator;
    typedef dhsstl::reverse_iterator<const_iterator>    const_reverse_iterator;

    typedef typename node_traits<T>::base_ptr           base_ptr;
    typedef typename node_traits<T>::node_ptr           node_ptr;

    // get_allocator: returns the associated allocator
    allocator_type get_allocator(){ return node_allocator(); }

private:
    base_ptr  node_;
    size_type size_;

public:
    // 构造\ 复制 移动 析构函数
    // Member functions
    // constructor
    // destructor
    // operator=
    // assign
    // assign_range(C++23)
    // get_allocator: returns the associated allocator
    list()                                              { fill_init(0, value_type()); }
    explicit list(size_type n)                          { fill_init(n, value_type()); }
    list(size_type n, const T& value)                   { fill_init(n, value_type(value)); }

    template<typename Iter, typename std::enable_if<
        dhsstl::is_input_iterator<Iter>::value, int>::type = 0
    >
    list(Iter first, Iter last)                         { copy_init(first, last); }
    
    list(std::initializer_list<T> ilist)                { copy_init(ilist.begin(), ilist.end()); }
    list(const list& rhs)                               { copy_init(rhs.cbegin(), rhs.cend()); }

    list(list&& rhs) noexcept 
    : node_(rhs.node_), size_(rhs.size_){
        rhs.node_ = nullptr;
        rhs.size_ = 0;
    }

    // 复制
    list& operator=(const list& rhs){
        if(this != rhs){
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }

    // 移动
    list& operator=(list&& rhs) noexcept{
        clear();
        splice(end(), rhs);
        return *this;
    }

    list& operator=(std::initializer_list<T> ilist){
        list tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~list(){
        if(node_){
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            size_ = 0;
        }
    }

public:
    // 迭代器相关操作
    // Iterators
    // begin  cbegin(C++11)
    // end    cend(C++11)
    // rbegin crbegin(C++11)
    // rend   crend(c++11)
    iterator                begin()                noexcept
    { return node_->next; }
    const_iterator          begin()         const noexcept
    { return node_->next; }

    iterator                end()                  noexcept
    { return node_; }
    const_iterator          end()           const noexcept
    { return node_; }

    reverse_iterator        rbegin()               noexcept
    { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin()        const noexcept
    { return reverse_iterator(end()); }
    reverse_iterator        rend()                 noexcept
    { return reverse_iterator(begin()); }
    const_reverse_iterator  rend()          const noexcept
    { return reverse_iterator(begin()); }

    const_iterator          cbegin()         const noexcept
    { return begin(); }
    const_iterator          cend()           const noexcept
    { return end(); }
    const_reverse_iterator  crbegin()        const noexcept
    { return rbegin(); }
    const_reverse_iterator  crend()          const noexcept
    { return rend(); }

    // Member funciton
    // Capacity
    // empty
    bool                    empty()          const noexcept
    { return node_->next == node_; }
    // size
    size_type               size()           const noexcept
    { return size_; }

    size_type               max_size()       const noexcept
    { return static_cast<size_type>(-1); }
    // max_size: returns the maximum possible number of elements

    // Element access
    // front
    reference               front(){
        DHSSTL_DEBUG(!empty());
        return *begin();
    }
    const_reference         front()          const{
        DHSSTL_DEBUG(!empty());
        return *begin();
    }
    // bcak
    reference               back(){
        DHSSTL_DEBUG(!empty());
        return *(--end());
    }
    const_reference         back()           const{
        DHSSTL_DEBUG(!empty());
        return *(--end());
    }

    // Modifiers
    // clear
    // insert
    // insert_range(C++23)
    // emplace(C++11)
    // erase
    // push_back
    // emplace_back(C++11)
    // append_range(C++23)
    // pop_back
    // push_front
    // emplace_front(C++11)
    // emplace_range(C++23)
    // pop_front
    // resize
    // swap
    // 调整容器相关操作
    void assign(size_type n, const value_type& value)
    { fill_assign(n, value); }

    template<typename Iter, typename std::enable_if<
     dhsstl::is_input_iterator<Iter>::value, int
    >::type = 0>
    void assign(Iter first, Iter last)
    { copy_assign(first, last); }

    void assign(std::initializer_list<T> ilist)
    { copy_assign(ilist.begin(), ilist.end()); }

    template <typename ...Args>
    void emplace_front(Args&& ...args){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(dhsstl::forward<Args>(args)...);
        link_nodes_at_front(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    template <typename ...Args>
    void emplace_back(Args&& ...args){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(dhsstl::forward<Args>(args)...);
        link_nodes_at_back(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    template <typename ...Args>
    iterator emplace(const_iterator pos, Args&& ...args){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(dhsstl::forward<Args>(args)...);
        link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
        ++size_;
        return iterator(link_node);
    }

    iterator insert(const_iterator pos, const value_type& value){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        ++size_;
        return link_iter_node(pos, link_node->as_base());
    }

    iterator insert(const_iterator pos, const value_type&& value){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(dhsstl::move(value));
        ++size_;
        return link_iter_node(pos, link_node->as_base());
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
        return fill_insert(pos, n, value);
    }

    template<typename Iter, typename std::enable_if<
     dhsstl::is_input_iterator<Iter>::value, int 
    >::type = 0>
    iterator insert(const_iterator pos, Iter first, Iter last){
        size_type n = dhsstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
        return copy_insert(pos, n, first); 
    }

    // push_front / push_back
    void push_front(const value_type& value){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        link_nodes_at_front(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    void push_front(value_type&& value){
        emplace_front(dhsstl::move(value)); 
    }

    void push_back(const value_type& value){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        link_nodes_at_back(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    void push_back(value_type&& value){
        emplace_back(dhsstl::move(value));
    }

    // pop_front / pop_back
    void pop_front(){
        DHSSTL_DEBUG(!empty());
        auto n = node_->next;
        unlink_nodes(n, n);
        destory_node(n->as_node());
        --size_;
    }

    void pop_back(){
        DHSSTL_DEBUG(!empty());
        auto n = node_->prev;
        unlink_nodes(n, n);
        destory_node(n->as_node());
        --size_;
    }

    // erase / clear
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void     clear();
    
    // resize
    void     resize(size_type new_size){ resize(new_size, value_type()); }
    void     resize(size_type new_size, const value_type& value);

    void     swap(list& rhs) noexcept{
        dhsstl::swap(node_, rhs.node_);
        dhsstl::swap(size_, rhs.size_);
    }
    // Operators
    // merge
    void     merge(list& x)
    { merge(x, dhsstl::less<T>()); }
    template <typename Compare>
    void     merge(list& x, Compare comp);
    // splice
    void     splice(const_iterator pos, list& other);
    void     splice(const_iterator pos, list& other, const_iterator it);
    void     splice(const_iterator pos, list& other, const_iterator first, const_iterator last);
    // remove remove_if 
    void     remove(const value_type& value)
    { remove_if([&](const value_type& v) {return v == value; }); }
    template <typename UnaryPredicate>
    void     remove_if(UnaryPredicate pred);
    // unique removes consecutive duplicate elements
    // unique
    void     unique()
    { unique(dhsstl::equal_to<T>()); }
    template<typename BinaryPredicate>
    void     unique(BinaryPredicate pred);
    // sort
    void     sort()
    { list_sort(begin(), end(), size(), dhsstl::less<T>()); }
    template <typename Compare>
    void     sort(Compare comp)
    { list_sort(begin(), end(), size(), comp);}

    void reverse();

private:
    // helper functions

    // create / destory node
    template<typename ...Args>
    node_ptr create_node(Args&& ...args);
    void     destory_node(node_ptr p);

    // initialize
    void    fill_init(size_type n, const value_type& value);
    template<typename Iter>
    void    copy_init(Iter first, Iter last);

    // link / unlink
    iterator    link_iter_node(const_iterator pos, base_ptr node);
    void        link_nodes(base_ptr p, base_ptr first, base_ptr last);
    void        link_nodes_at_front(base_ptr first, base_ptr last);
    void        link_nodes_at_back(base_ptr first, base_ptr last);
    void        unlink_nodes(base_ptr f, base_ptr l);

    // assign
    void        fill_assign(size_type n, const value_type& value);
    template<typename Iter>
    void        copy_assign(Iter first, Iter last);

    // insert
    iterator    fill_insert(const_iterator pos, size_type n, const value_type& value);
    template<typename Iter>
    iterator    copy_insert(const_iterator pos, size_type n, Iter first);

    // sort
    template <typename Compared>
    iterator    list_sort(iterator first, iterator last, size_type n, Compared comp);

};

// ----------------------------------------------------
// Non-member functions

// 删除pos处的元素
template <typename T>
typename list<T>::iterator
list<T>::erase(const_iterator pos){
    DHSSTL_DEBUG(pos != cend());
    auto n = pos.node_;
    auto next = n->next;
    unlink_nodes(n, n);
    destory_node(n->as_node());
    --size_;
    return iterator(next);
}

// 删除[first, last) 内的元素
template <typename T>
typename list<T>::iterator
list<T>::erase(const_iterator first, const_iterator last){
    if(first != last){
        unlink_nodes(first.node_, last.node_->prev);
        while(first != last){
            auto cur = first.node_;
            ++first;
            destory_node(cur->as_node());
            --size_;
        }
    }
    return iterator(last.node_);
}

// 清空list
template <typename T>
void list<T>::clear(){
    if(size_ != 0){
        auto cur = node_->next;
        // 因为删除了 cur, 所以这里首先记录一下 cur->next;
        for(base_ptr next = cur->next; cur != node_; cur = next, next = cur->next){
            destory_node(cur->as_node());
        }
        node_->unlink();
        size_ = 0;
    }
}

// 重置容器大小
template <typename T>
void list<T>::resize(size_type new_size, const value_type& value){
    auto i = begin();
    size_type len = 0;
    while(i != end() && len < new_size){
        ++i; ++len;
    }
    if(len == new_size){
        erase(i, node_);
    }else{
        insert(node_, new_size - len, value);
    }
}

// 将list x 接合于 pos 之前
template <typename T>
void list<T>::splice(const_iterator pos, list& x){
    DHSSTL_DEBUG(this != &x);
    if(!x.empty()){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

        auto f = x.node_->next;
        auto l = x.node_->prev;

        x.unlink_nodes(f, l);
        link_nodes(pos.node_, f, l);

        size_ += x.size_;
        x.size_ = 0;
    }
}

// 将 it 所指的节点结合与 pos 之前
template <typename T>
void list<T>::splice(const_iterator pos, list& x, const_iterator it){
    // 这里排除两种情况: 第一中 it所指的节点本来就是pos节点, 或者it所指的节点本来就在pos节点前面
    if(pos.node_ != it.node_ && pos.node_ != it.node_->next){
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");

        auto f = it.node_;

        x.unlink_nodes(f, f);
        link_nodes(pos.node_, f, f);

        ++size_;
        --x.size_;
    }
}

// 将 list x 的 [first, last) 内的节点结合于 pos 之前
template <typename T>
void list<T>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last){
    if(first != last && this != &x){
        size_type n =dhsstl::distance(first, last);
        THROW_OUT_OF_RANGE_IF(size_ > max_size() - n, "list<T>'s size too big");
        auto f = first.node_;
        auto l = last.node_->prev;

        x.unlink_nodes(f, l);
        link_nodes(pos.node_, f, l);

        size_ += n;
        x.size_ -= n;
    }
}

// 将另一元操作 pred 为 true 的所有元素移除
// Unary predicate 一元谓词
template <typename T>
template <typename UnaryPredicate>
void list<T>::remove_if(UnaryPredicate pred){
    auto f = begin();
    auto l = end();
    for(auto next = f; f != l; f = next){
        ++next;
        if(pred(*f)){
            erase(f);
        }
    }
}

// 移除 list 中满足 pred 为 true 重复元素
// 删除容器中相邻的重复元素, 只保留一个
template <typename T>
template <typename BinaryPredicate>
void list<T>::unique(BinaryPredicate pred){
    auto i = begin();
    auto e = end();
    auto j = i;
    ++j;
    while (j != e){
        if(pred(*i, *j)){
            erase(j);
        }else{
            i = j;
        }
        j = i;
        ++j;
    }
}

// 与另一个 list 合并, 按照 comp 为 true 的顺序
template <typename T>
template <typename Compare>
void list<T>::merge(list &x, Compare comp){
    if(this != &x){
        THROW_LENGTH_ERROR_IF(size > max_size() - x.size_, "list<T>'s size too big");

        auto f1 = begin();
        auto l1 = end();
        auto f2 = x.begin();
        auto l2 = x.end();

        while(f1 != l1 && f2 != l2){
            if(comp(*f2, *f1)){
                // 使 comp 为 true 的一段区间
                auto next = f2;
                ++next;
                for(; next != l2 && comp(*next, *f1); ++next);
                auto f = f2.node_;
                auto l = next.node_->prev;

                // link node
                x.unlink_nodes(f, l);
                link_nodes(f1.node_, f, l);
                ++f1;
            }else{
                ++f1;
            }
        }
        // 剩余部分
        if(f2 != l2){
            auto f = f2.node_;
            auto l = ls.node_->prev;
            x.unlink_nodes(f, l);
            link_nodes(l1.node_, f, l);
        }

        size_ += x.size_;
        x.size_ = 0;
    }
}

// 将 list 反转
template <typename T>
void list<T>::reverse(){
    if(size_ <= 1){
        return;
    }
    auto i = begin();
    auto e = end();
    while(i.node_ != e.node_){
        // 注意: 按我的理解 swap 只是交换的地址的值, 其他值都没变, 只是地址互相换了一下
        dhsstl::swap(i.node_->prev, i.node_->next);
        i.node_ = i.node_->prev;
    }
    dhsstl::swap(e.node_->prev, e.node_->next);
}

// ----------------------------------------------------------------
// helper function 

// 创建节点
template<typename T>
template<typename ...Args>
typename list<T>::node_ptr
list<T>::create_node(Args&& ...args){
    node_ptr p = node_allocator::allocate(1);
    try{
        data_allocator::construct(dhsstl::address_of(p->value), dhsstl::forward<Args>(args)...);
        p->prev = nullptr;
        p->next = nullptr;
    }catch(...){
        node_allocator::deallocate(p);
        throw;
    }
    return p;
}

// 销毁节点
template <typename T>
void list<T>::destory_node(node_ptr p){
    data_allocator::destory(dhsstl::address_of(p->value));
    node_allocator::deallocate(p);
}

// 用 n 个元素初始化容器
template <typename T>
void list<T>::fill_init(size_type n, const value_type& value){
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_ = n;
    try{
        for(; n > 0; --n){
            auto node = create_node(value);
            link_nodes_at_back(node_->as_base(), node_->as_base());
        }
    }catch(...){
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

// 以[first, last) 初始化容器
template <typename T>
template <typename Iter>
void list<T>::copy_init(Iter first, Iter last){
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_type n = dhsstl::distance(first, last);
    size_ = n;
    try
    {
        for(; n > 0; --n, ++first){
            auto node = create_node(*first);
            link_nodes_at_back(node->as_base(), node_->as_base());
        }
    }
    catch(...)
    {
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

// 在 pos 处链接一个节点
template <typename T>
typename list<T>::iterator
list<T>::link_iter_node(const_iterator pos, base_ptr link_node){
    if(pos == node_->next){
        link_nodes_at_front(link_node, link_node);
    }else if(pos == node_){
        link_nodes_at_back(link_node, link_node);
    }else{
        link_nodes(pos.node_, link_node, link_node);
    }
    return iterator(link_node);
}

// 在pos 前链接 [first, last]的节点
template <typename T>
void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last){
    pos->prev->next = first;
    first->prev = pos->prev;
    pos->prev = last;
    last->next = pos;
}

// 在头部添加 [first, last]的节点
template <typename T>
void list<T>::link_nodes_at_front(base_ptr first, base_ptr last){
    first->prev = node_;
    last->next = node_->next;
    last->next->prev = last;
    node_->next = first;
}

// 在尾部添加 [first, last]的节点
template <typename T>
void list<T>::link_nodes_at_back(base_ptr first, base_ptr last){
    last->next = node_;
    first->prev = node_->prev;
    first->prev->next = first;
    node_->prev = last;
}

// 容器与 [first, last] 节点断开连接
template <typename T>
void list<T>::unlink_nodes(base_ptr first, base_ptr last){
    first->prev->next = last->next;
    last->next->prev = first->prev;
}

// 用 n 个元素为容器赋值
template <typename T>
void list<T>::fill_assign(size_type n, const value_type& value){
    auto i = begin();
    auto e = end();
    for(; n > 0 && i != e; --n, ++i){
        *i = value;
    }
    if(n > 0){
        insert(e, n, value);
    }else{
        erase(i, e);
    }
}

// 赋值[f2, l2)为容器赋值
template <typename T>
template <typename Iter>
void list<T>::copy_assign(Iter f2, Iter l2){
    auto f1 = begin();
    auto l1 = end();
    for(; f1 != l1 && f2 != ls; ++f1, ++f2){
        *f1 = *f2;
    }
    if(f2 == l2){
        erase(f1, l1);
    }else{
        insert(l1, f2, l2);
    }
}

// 在 pos 处插入 n 个元素
template <typename T>
typename list<T>::iterator
list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value){
    iterator r(pos.node_);    
    if(n != 0){
        const auto add_size = n;
        auto node = create_node(vlaue);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try{
            // 前面已经创建了一个节点, 还需要n-1个
            for(--n; n > 0; --n, ++end){
                auto next = create_node(value);
                end.node_->next = next->as_base();
                next->prev = end.node_;
            }
            size_ += add_size;
        }
        catch(...){
            auto enode = end.node_;
            while(true){
                auto prev = enode->prev;
                destory_node(enode->as_node());
                if(prev == nullptr)
                    break;
                enode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

// 在 pos 处插入[first, last)的元素
template <typename T>
template <typename Iter>
typename list<T>::iterator
list<T>::copy_insert(const_iterator pos, size_type n, Iter first){
    iterator r(pos.node_);
    if(n != 0){
        const auto add_size = n;
        auto node = create_node(*first);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try{
            for(--n, ++first; n > 0; --n, ++first, ++end){
                auto next = create_node(*first);
                end.node_->next = next->as_base();
                next->prev = end.node_;
            }
            size_ += add_size;
        }catch(...){
            auto enode = end.node_;
            while (true)
            {
                auto prev = enode->prev;
                destory_node(enode->as_node());
                if(prev == nullptr)  break;
                enode = prev;
            }
            throw; 
        }
        link_nodes(pos->node_, r.node_, end.node_);
    }
    return r;
}

// 对list 进行归并排序, 返回一个迭代器指向区间最小元素的位置
template <typename T>
template <typename Compared>
typename list<T>::iterator
list<T>::list_sort(iterator f1, iterator l2, size_type n, Compared comp){
    if(n < 2)
        return f1;
    
    if(n == 2){
        if(comp(*--l2, *f1)){
            auto ln == l2.node_;
            unlink_nodes(ln, ln);
            link_nodes(f1.node_, ln, ln);
            return l2;
        }
        return l1;
    }

    auto n2 = n / 2;    
    auto l1 = f1;
    dhsstl::advance(l1, n2);
    auto result = f1 = list_sort(f1, l1, n2, comp);
    auto f2 = l1 = list_sort(l1, l2, n - n2, comp);
    
    // 把较小的一段区间移动到前面
    // 不把这一部分放到循环里的原因是: 
    //  假如存在这样的情况的话, result得更新
    //  而后面部分的reuslt不需要更新
    if(comp(*f2, *f1)){
        auto m = f2;
        ++m;
        for(; m != l2 && comp(*m, *f1); ++m);
        auto f = f2.node_;
        auto l = m.node_->prev;
        result = f2;
        // 这里l1 需要变的原因是 原来的l1 被移动到链表头部了
        // 可以和下面对比: 如果
        // l1 == f2. 那么就会出现这种问题
        l1 = f2 = m;
        unlink_nodes(f, l);
        m = f1;
        ++m;
        link_nodes(f1.node_, f, l);
        f1 = m;
    }else{
        ++f1;
    }
    
    // 合成两端有序区间
    while(f1 != l1 && f2 != l2){
        if(comp(*f2, *f1)){
            auto m = f2;
            ++m;
            for(; m != l2 && comp(*m, *f1); ++m) ;
            auto f = f2.node_;
            auto l = m.node_->prev;
            if(l1 == f2) l1 = m;
            f2 = m;
            unlink_nodes(f, l);
            m = f1;
            // 这里我觉得, 在f1.node_前面添加元素,应该不会改变++f1的值的吧
            ++m;
            link_nodes(f1.node_, f, l);
            f1 = m;
        }else{
            ++f1;
        }
    }
    return result;
}

// 重载比较操作符
// operator== (removed in C++20)
template <typename T>
bool operator==(const list<T>& lhs, const list<T>& rhs){
    auto f1 = lhs.cbegin();
    auto f2 = rhs.cbegin();
    auto l1 = lhs.cend();
    auto l2 = rhs.cend();
    for(; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2);
    return f1 == l1 && f2 == l2;
}

// operator!= 同上
template <typename T>
bool operator!=(const list<T>& lhs, const list<T>& rhs){
    return !(lhs == rhs);
}

// operator< 同上
template <typename T>
bool operator<(const list<T>& lhs, const list<T>& rhs){
    return dhsstl::lexicograhical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cen());
}

// operator<=
template <typename T>
bool operator<=(const list<T>& lhs, const list<T>& rhs){
    return !(rhs < lhs);
}

// operator>
template <typename T>
bool operator>(const list<T>& lhs, const list<T>& rhs){
    return rhs < lhs;
}

// operator>=
template <typename T>
bool operator>=(const list<T>& lhs, const list<T>& rhs){
    return !(lhs < rhs);
}
// operator<=> (C++20)
//      lexicographically compares the values in the list 

// std::swap(std::list) :: specializes the std::swap algo
template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept{
    lhs.swap(rhs);
}
// erase(std::list) erase_if :: erases all elements satisfying specific criteria
} // namespace dhsstl

#endif // DHSTINYSTL_LIST_H_