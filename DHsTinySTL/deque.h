#ifndef DHSTINYSTL_DEQUE_H_
#define DHSTINYSTL_DEQUE_H_

// 这个头文件包含了一个模板类  deque
// deque: 双端队列


// notes:
//
// 异常保证:
// dhsstl::deque<T> 满足基本异常保证, 部分函数无异常保证, 并对以下等函数做强异常安全保证
//  *emplace_front
//  *emplace_back
//  *emplace
//  *push_front
//  *push_back
//  *insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"

namespace dhsstl{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// deque map 初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template<typename T>
struct deque_buf_size{
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque 的迭代器设计
template<typename T, typename Ref, typename Ptr>
struct deque_iterator : public iterator<random_access_iterator_tag, T>{
    typedef deque_iterator<T, T&, T*>                   iterator;
    typedef deque_iterator<T, const T&, const T*>       const_iterator;
    typedef deque_iterator                              self;

    typedef T               value_type;
    typedef Ptr             pointer;
    typedef Ref             reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              value_pointer;
    typedef T**             map_pointer;

    static const size_type buffer_size = deque_buf_size<T>::value;

    // 迭代器所含成员数据
    value_pointer cur;      // 指向所在缓冲区的当前元素
    value_pointer first;    // 指向所在缓冲区的头部
    value_pointer last;     // 指向所在缓冲区的尾部
    map_pointer   node;     // 缓冲区所在节点

    // 构造, 复制, 移动函数
    deque_iterator() noexcept
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    deque_iterator(value_pointer v, map_pointer n)
        : cur(v), first(*n), last(*n + buffer_size), node(n) {}
    
    deque_iterator(const iterator& rhs)
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    
    deque_iterator(iterator&& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    deque_iterator(const const_iterator& rhs)
        :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    
    self& operator=(const iterator& rhs){
        if(this != &rhs){
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    // 转到另一个缓冲区
    void set_node(map_pointer new_node){
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }

    // 重载运算符
    reference operator*()  const { return *cur; }
    pointer   operator->() const { return cur;  }

    difference_type operator-(const self& x) const{
        return static_cast<difference_type>(buffer_size) * (node - x.node)
            + (cur - first) - (x.cur -x.first);
    }

    self& operator++(){
        ++cur;
        if(cur == last){
            // 如果到达缓冲区的尾
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }
    
    self& operator--(){
        if(cur == first){
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n){
        const auto offset = n + (cur - first);
        if(offset >= 0 && offset < static_cast<difference_type>(buffer_size)){
            // 仍在当前缓冲区
            cur += n;
        }else{
            // 要跳到其他的缓冲区
            const auto node_offset = offset > 0
                ? offset / static_cast<difference_type>(buffer_size)
                : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1; 
                // 一层是从 0 - k-1, 但是如果offset是负数的话 一层就是 -1 - -k ,所以需要减1
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        } 
        return *this;
    }
    self operator+(difference_type n) const{
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(difference_type n){
        return *this += -n;
    }
    self operator-(difference_type n) const{
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    // 重载比较操作符
    // 这个相等不用比较node节点吗?
    // 不用, 因为这里比较的是地址值, 如果这两个地址相同, 其实也就是全部相同了
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator< (const self& rhs) const{
        return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
    }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator> (const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
};

// 模板类 deque
// 模板参数代表数据类型
template<typename T>
class deque{
public:
    // deque 的型别定义
    typedef dhsstl::allocator<T>                        allocator_type;
    typedef dhsstl::allocator<T>                        data_allocator;
    typedef dhsstl::allocator<T*>                       map_allocator;
    
    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;
    typedef pointer*                                    map_pointer;
    typedef const_pointer*                              const_map_pointer;

    typedef deque_iterator<T, T&, T*>                   iterator;
    typedef deque_iterator<T, const T&, const T*>       const_iterator;
    typedef dhsstl::reverse_iterator<iterator>          reverse_iterator;
    typedef dhsstl::reverse_iterator<const_iterator>    const_reverse_iterator;

    allocator_type get_allocator() { return allocator_type(); }

    static const size_type buffer_size = deque_buf_size<T>::value;

private:
    // 用以下四个数据来表现一个 deque
    // 注: map_ 不一定和 begin_.node的地址相同
    //     map_ 所指的内存块一定包含 bgein_ -> end_ , 但是首和尾不能保证没有空余
    iterator        begin_;         // 指向第一个节点
    iterator        end_;           // 指向最后一个节点
    map_pointer     map_;           // 指向一块map, map中的每一个元素都是一个指针, 指向一个缓冲区
    size_type       map_size_;      // map 内指针的数目

public:
    // 构造, 复制, 移动, 析构函数

    deque()
    { fill_init(0, value_type()); }

    explicit deque(size_type n)
    { fill_init(n, value_type()); }

    deque(size_type n, const value_type& value)
    { fill_init(n, value); }

    template<typename IIter, typename std::enable_if<
        dhsstl::is_input_iterator<IIter>::value, int>::type = 0>
    deque(IIter first, IIter last)
    { copy_init(first, last, iterator_category(first)); }

    deque(std::initializer_list<value_type> ilist){
        copy_init(ilist.begin(), ilist.end(), dhsstl::forward_iterator_tag());
    }
    
    deque(const deque& rhs){
        copy_init(rhs.begin(), rhs.end(), dhsstl::forward_iterator_tag());
    }
    // 由于 begin_ 和 end_ 并不简单的是一个指针, 他是一个迭代器, 所以需要用move操作
    // 并且由于他是一个类, 所以之后会调用析构函数, 重点是 这个rhs之后不能被继续使用
    deque(deque&& rhs) noexcept
        :begin_(dhsstl::move(rhs.begin_)),
        end_(dhsstl::move(rhs.end_)),
        map_(rhs.map_),
        map_size_(rhs.map_size_)
    {
        rhs.map_ = nullptr;        
        rhs.map_size_ = 0;
    }

    deque& operator=(const deque& rhs);
    deque& operator=(deque&& rhs);

    deque& operator=(std::initializer_list<value_type> ilist){
        deque tmp(ilist);
        swap(tmp);
        return *this;
    }

    ~deque(){
        if(map_ != nullptr){
            clear();
            data_allocator::deallocate(*begin_.node, buffer_size);
            *begin_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }

public:
    // 迭代器相关操作 iterators

    iterator                begin()             noexcept
    { return begin_; }
    const_iterator          begin()       const noexcept            
    { return begin_; }
    iterator                end()               noexcept
    { return end_; }
    const_iterator          end()         const noexcept
    { return end_; }

    reverse_iterator        rbegin()            noexcept
    { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin()      const noexcept
    { return reverse_iterator(end()); }
    reverse_iterator        rend()              noexcept
    { return reverse_iterator(begin()); }
    const_reverse_iterator  rend()        const noexcept
    { return reverse_iterator(begin()); }

    const_iterator          cbegin()      const noexcept
    { return begin(); }
    const_iterator          cend()        const noexcept
    { return end(); }
    const_reverse_iterator  crbegin()     const noexcept
    { return rbegin(); }
    const_reverse_iterator  crend()       const noexcept
    { return rend(); }

    // 容量相关操作 Capacity
    bool        empty()     const noexcept { return begin() == end(); }
    size_type   size()      const noexcept { return end_ - begin_; } 
    size_type   max_size()  const noexcept { return static_cast<size_type>(-1); }
    void        resize(size_type new_size) { resize(new_size, value_type()); }
    void        resize(size_type new_size, const value_type& value);
    // reduces memory usage by freeing unused memory
    void        shrink_to_fit() noexcept;

    // 元素访问相关操作 Element access
    reference       operator[](size_type n){
        DHSSTL_DEBUG(n < size());
        return begin_[n];
    }
    const_reference operator[](size_type n) const{
        DHSSTL_DEBUG(n < size());
        return begin_[n];
    }

    reference       at(size_type n){
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const{
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }

    reference       front(){
        DHSSTL_DEBUG(!empty());
        return *begin();
    }
    const_reference front() const{
        DHSSTL_DEBUG(!empty());
        return *begin();
    }
    reference        back(){
        DHSSTL_DEBUG(!empty());
        return *(end() - 1);
    }
    const_reference  back() const{
        DHSSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    // Modifiers 修改容器相关操作
    // assign: assigns values to the container
    void    assign(size_type n, const value_type& value)
    { fill_assign(n, value); } 

    template <typename IIter, typename std::enable_if<
        dhsstl::is_input_iterator<IIter>::value, int>::type = 0>
    void    assign(IIter first, IIter last)
    { copy_assign(first, last, iterator_category(first)); }

    void    assign(std::initializer_list<value_type> ilist)
    { copy_assign(ilist.begin(), ilist.end(), dhsstl::forward_iterator_tag{}); }

    // emplace_front / emplace_back / emplace: constructs element in-place
    template <typename ...Args>
    void    emplace_front(Args&& ...args);
    template <typename ...Args>
    void    emplace_back(Args&& ...args);
    template <typename ...Args>
    iterator emplace(iterator pos, Args&& ...args);

    // push_front / push_back : adds an element to the front / end
    void    push_front(const value_type& value);
    void    push_back(const value_type& value);

    void    push_front(value_type&& value) { emplace_front(dhsstl::move(value)); }
    void    push_back(value_type&& value){ emplace_back(dhsstl::move(value)); }

    // pop_back / pop_front : removes the last/first element
    void    pop_front();
    void    pop_back();

    // insert : insert elempents
    iterator insert(iterator position, const value_type& value);
    iterator insert(iterator position, value_type&& value);
    void     insert(iterator position, size_type n, const value_type& value);
    template <typename IIter, typename std::enable_if<
        dhsstl::is_input_iterator<IIter>::value, int>::type = 0>
    void     insert(iterator position, IIter first, IIter last)
    { insert_dispatch(position, first, last, iterator_category(first)); }

    // erase : erase elements
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);

    // clear : clears the contents
    void     clear();

    // swap : swaps the contents
    void     swap(deque &rhs) noexcept;

private:
    // helper functions

    // create node / destroy node
    map_pointer     create_map(size_type size);
    void            create_buffer(map_pointer nstart, map_pointer nfinish);
    void            destroy_buffer(map_pointer nstart, map_pointer nfinish);
    
    // initialize
    void            map_init(size_type nelem);
    void            fill_init(size_type n, const value_type& value);
    template <typename IIter>
    void            copy_init(IIter, IIter, input_iterator_tag);
    template <typename FIter>
    void            copy_init(FIter, FIter, forward_iterator_tag);

    // assign
    void            fill_assign(size_type n, const value_type& value);
    template <typename IIter>
    void            copy_assign(IIter first, IIter last, input_iterator_tag);
    template <typename FIter>
    void            copy_assign(FIter first, FIter last, forward_iterator_tag);
    
    // insert
    template <typename ...Args>
    iterator        insert_aux(iterator position, Args&& ...args);
    void            fill_insert(iterator position, size_type n, const value_type& x);
    template <typename FIter>
    void            copy_insert(iterator, FIter, FIter, size_type);
    template <typename IIter>
    void            insert_dispatch(iterator, IIter, IIter, input_iterator_tag);
    template <typename FIter>
    void            insert_dispatch(iterator, FIter, FIter, forward_iterator_tag);

    // reallocate
    void            require_capacity(size_type n, bool front);
    void            reallocate_map_at_front(size_type need);
    void            reallocate_map_at_back(size_type need);
};

// -----------------------------------------------------------------------------

// 复制赋值运算符
template <typename T>
deque<T>& deque<T>::operator=(const deque& rhs){
    if(this != &rhs){
        const auto len = size();
        if(len >= rhs,size()){
            erase(dhsstl::copy(rhs.begin_, rhs.end_, begin_), end_);
        }else{
            iterator mid = rhs.begin() + static_cast<difference_type>(len);
            dhsstl::copy(rhs.begin_, mid, begin_);
            insert(end_, mid, rhs.end_);
        }
    }
    return *this;
}

// 移动赋值运算符
template <typename T>
deque<T>&   deque<T>::operator=(deque&& rhs){
    clear();
    begin_ = dhsstl::move(rhs.begin_);
    end_ = dhsstl::move(rhs.end_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;
    return *this;
}

// 重置容器大小
template <typename T>
void deque<T>::resize(size_type new_size, const value_type& value){
    const auto len = size();
    if(new_size < len){
        erase(begin_ + new_size, end_);
    }else{
        insert(end_, new_size - len, value);
    }
}

// 减小容器容量
template <typename T>
void deque<T>::shrink_to_fit() noexcept{
    // 注: map_ 没变, 只不过map_中的多余缓冲区全给删除了
    // 至少会留下头部缓冲区
    for(auto cur = map_; cur < begin_.node; ++cur){
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    for(auto cur = end_.node + 1; cur < map_ + map_size_; ++cur){
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

// 在头部就地构建元素
template <typename T>
template <typename ...Args>
void deque<T>::emplace_front(Args&& ...args){
    if(begin_.cur != begin_.first){
        data_allocator::construct(begin_.cur - 1, dhsstl::forward<Args>(args)...);
        --begin_.cur;
    }else{
        require_capacity(1, true);
        try{
            --begin_;
            data_allocator::construct(begin_.cur, dhsstl::forward<Args>(args)...);
        }catch(...){
            ++begin_;
            throw;
        }
    }
}

// 在尾部就地构建元素
template <typename T>
template <typename ...Args>
void deque<T>::emplace_back(Args&& ...args){
    if(end_.cur != end_.last - 1){
        data_allocator::construct(end_.cur, dhsstl::forward<Args>(args)...);
        ++end_.cur;
    }else{
        require_capacity(1, false);
        data_allocator::construct(end_.cur, dhsstl::forward<Args>(args)...);
        ++end_;
    }
}

// 在 pos 位置前就地构建元素, 返回这个元素所在位置的迭代器
template <typename T>
template <typename ...Args>
typename deque<T>::iterator deque<T>::emplace(iterator pos, Args&& ...args){
    if(pos.cur == begin_.cur){
        emplace_front(dhsstl::forward<Args>(args)...);
        return begin_;
    }else if(pos.cur == end_.cur){
        emplace_back(dhsstl::forward<Args>(args)...);
        return end_ - 1;
    }
    return insert_aux(pos, dhsstl::forward<Args>(args)...);
}

// 在头部插入元素
template <typename T>
void deque<T>::push_front(const value_type& value){
    if(begin_.cur != begin_.first){
        data_allocator::construct(begin_.cur - 1, value);
        --begin_.cur;
    }else{
        require_capacity(1, true);
        try{
            --begin_;
            data_allocator::construct(begin_.cur, value);
        }catch(...){
            ++begin_;
            throw;
        }
    }
}

// 在尾部插入元素
template <typename T>
void deque<T>::push_back(const value_type& value){
    if(end_.cur != end_.last -1){
        data_allocator::construct(end_.cur, value);
        ++end_.cur;
    }else{
        require_capacity(1, false);
        data_allocator::construct(end_.cur, value);
        ++end_;
    }
}

// 弹出头部元素
template <typename T>
void deque<T>::pop_front(){
    DHSSTL_DEBUG(!empty());
    if(begin_.cur != begin_.last - 1){
        data_allocator::destroy(begin_.cur);
        ++begin_.cur;
    }else{
        data_allocator::destroy(begin_.cur);
        ++begin_;
        destroy_buffer(begin_.node - 1, begin_.node - 1);
    }
}

// 弹出尾部元素
template <typename T>
void deque<T>::pop_back(){
    DHSSTL_DEBUG(!empty());
    if(end_.cur != end_.first){
        --end_.cur;
        data_allocator::destroy(end_.cur);
    }else{
        --end_;
        data_allocator::destroy(end_.cur);
        destroy_buffer(end_.node + 1, end_.node + 1);
    }
}

// 在position处插入元素
template <typename T>
typename deque<T>::iterator
deque<T>::insert(iterator position, const value_type& value){
    if(position.cur == begin_.cur){
        push_front(value);
        return begin_;
    }else if(position.cur == end_.cur){
        push_back(value);
        auto tmp = end_;
        --tmp;
        return tmp;
    }else{
        return insert_aux(position, value);
    }
}

template <typename T>
typename deque<T>::iterator
deque<T>::insert(iterator position, value_type&& value){
    if(position.cur == begin_.cur){
        emplace_front(dhsstl::move(value));
        return begin_;
    }else if(position.cur == end_.cur){
        emplace_back(dhsstl::move(value));
        auto tmp = end_;
        --tmp;
        return tmp;
    }else{
        return insert_aux(position, dhsstl::move(value));
    }
}

// 在 position 位置处插入 n 个元素
template <typename T>
void deque<T>::insert(iterator position, size_type n, const value_type& value){
    if(position.cur == begin_.cur){
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        dhsstl::uninitialized_fill_n(new_begin, n, value);
        begin_ = new_begin;
    }else if(position.cur == end_.cur){
        require_capacity(n, false);
        auto new_end = end_ + n;
        dhsstl::uninitialized_fill_n(end_, n, value);
        end_ = new_end;
    }else{
        fill_insert(position, n, value);
    }
}

// 删除 position 处的元素
template <typename T>
typename deque<T>::iterator
deque<T>::erase(iterator position){
    auto next = position;
    ++next;
    const size_type elems_before = position - begin_;
    if(elems_before < (size() / 2)){
        dhsstl::copy_backward(begin_, position, next);
        pop_front();
    }else{
        dhsstl::copy(next, end_, position);
        pop_back();
    }
    return begin_ + elems_before;
}

// 删除[first, last)上的元素
template <typename T>
typename deque<T>::iterator
deque<T>::erase(iterator first, iterator last){
    if(first == begin_ && last == end_){
        clear();
        return end_;
    }else{
        const size_type len = last - first;
        const size_type elems_before = first - begin_;
        if(elems_before < (size() - len / 2)){
            dhsstl::copy_backward(begin_, first, last);
            auto new_begin = begin_ + len;
            data_allocator::destroy(begin_.cur, new_begin.cur);
            begin_ = new_begin;
        }else{
            dhsstl::copy(last, end_, first);
            auto new_end = end_ - len;
            data_allocator::destroy(new_end.cur, end_.cur);
            end_ = new_end;
        }
        return begin_ + elems_before;
    }
}

// 清空 deque
template <typename T>
void deque<T>::clear(){
    // clear 会保留头部的缓冲区
    for(map_pointer cur = begin_.node + 1; cur < end_.node; ++cur){
        data_allocator::destory(*cur, *cur + buffer_size);
    }
    if(begin_.node != end_.node){
        // 有两个以上的缓冲区
        dhsstl::destory(begin_.cur, begin_.last);
        dhsstl::destory(end_.first, end_.cur);
    }else{
        dhsstl::destory(begin_.cur, end_.cur);
    }
    shrink_to_fit();
    end_ = begin_;
}

// 交换两个deque
template <typename T>
void deque<T>::swap(deque& rhs)noexcept{
    if(this != &rhs){
        dhsstl::swap(begin_, rhs.begin_);
        dhsstl::swap(end_, rhs.end_);
        dhsstl::swap(map_, rhs.map_);
        dhsstl::swap(map_size_, rhs.map_size_);
    }
}

// -----------------------------------------------------------------------------
// helper function

// create_map
template <typename T>
typename deque<T>::map_pointer
deque<T>::create_map(size_type size){
    map_pointer mp = nullptr;
    mp = map_allocator::allocate(size);
    for(size_type i = 0; i < size; ++i)
        *(mp + i) = nullptr;
    return mp;
}

// create_buffer
template <typename T>
void deque<T>::
create_buffer(map_pointer nstart, map_pointer nfinish){
    map_pointer cur;
    try{
        for(cur = nstart; cur <= nfinish; ++cur){
            *cur = data_allocator::allocate(buffer_size);
        }
    }catch(...){
        while(cur != nstart){
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw;
    }
}

// destory_buffer函数
template <typename T>
void deque<T>::
destroy_buffer(map_pointer nstart, map_pointer nfinish){
    for(map_pointer n = nstart; n <= nfinish; ++n){
        data_allocator::deallocate(*n, buffer_size);
        *n = nullptr;
    }
}

// map_init函数
template <typename T>
void deque<T>::
map_init(size_type nElem){
    const size_type nNode = nElem / buffer_size + 1;    //  需要分配的缓冲区的个数
    map_size_ = dhsstl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    try{
        map_ = create_map(map_size_);
    }catch(...){
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    // 让 nstart 和 nfinish 都指向 map_ 最中间的区域, 方便向头尾扩充
    // [nstart, nfinish]
    map_pointer nstart = map_ + (map_size_ - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;
    try{
        create_buffer(nstart, nfinish);
    }catch(...){
        map_allocator::deallocate(map_, map_size_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    begin_.set_node(nstart);
    end_.set_node(nfinish);
    begin_.cur = begin_.first;
    end_.cur = end_.first + (nElem % buffer_size);
}


// fill_init()
template <typename T>
void deque<T>::
fill_init(size_type n, const value_type& value){
    map_init(n);
    if(n != 0){
        for(auto cur = begin_.node; cur < end_.node; ++cur){
            dhsstl::uninitialized_fill(*cur, *cur + buffer_size, value);
        }
        dhsstl::uninitialized_fill(end_.first, end_.cur, value);
    }
}

// copy_init()
template <typename T>
template <typename IIter>
void deque<T>::
copy_init(IIter first, IIter last, input_iterator_tag){
    // const size_type n = dhsstl::distance(first, last);
    map_init(0);
    for(; first != last; ++first)
        emplace_back(*first);
}

template <typename T>
template <typename FIter>
void deque<T>::
copy_init(FIter first, FIter last, forward_iterator_tag){
    const size_type n = dhsstl::distance(first, last);
    map_init(n);
    for(auto cur = begin_.node; cur < end_.node; ++cur){
        auto next = first;
        dhsstl::advance(next, buffer_size);
        dhsstl::uninitialized_copy(first, next, *cur);
        first = next;
    }
    dhsstl::uninitialized_copy(first, last, end_.first);
}

// fill_assign 函数
template <typename T>
void deque<T>::
fill_assign(size_type n, const value_type& value){
    if(n > size()){
        dhsstl::fill(begin(), end(), value);
        insert(end(), n - size(), value);
    }else{
        erase(begin() + n, end());
        dhsstl::fill(begin(), end(), value);
    }
}

// copy_assgin 函数
template <typename T>
template <typename IIter>
void deque<T>::
copy_assign(IIter first, IIter last, input_iterator_tag){
    auto first1 = begin();
    auto last1 = end();
    for(; first != last && first1 != last1; ++first, ++first1){
        *first1 = *first;
    }
    if(first1 != last1){
        erase(first1, last1);
    }else{
        insert_dispatch(end_, first, input_iterator_tag{});
    }
}

// insert_aux 函数
template <typename T>
template <typename ...Args>
typename deque<T>::iterator
deque<T>::
insert_aux(iterator position, Args&& ...args){
    const size_type elems_before = position - begin_;
    value_type value_copy = value_type(dhsstl::forward<Args>(args)...);
    if(elems_before < (size() / 2)){
        emplace_front(front());
        auto front1 = begin_;
        ++front1;
        auto front2 = front1;
        ++front2;
        // 因为在前面加了一个元素, 所以position的位置变了 
        position = begin_ + elems_before;
        auto pos = position;
        ++pos;
        // 这里的copy其实是不包含原来的 begin的, 因为添加的元素就是begin
        dhsstl::copy(front2, pos, front1);
    }else{
        emplace_back(back());
        auto back1 = end_;
        --back1;
        auto back2 = back1;
        --back2;
        position = begin_ + elems_before;
        dhsstl::copy_backward(position, back2, back1); 
    }
    *position = dhsstl::move(value_copy);
    return position;
}

// fill_insert函数
template <typename T>
void deque<T>::
fill_insert(iterator position, size_type n, const value_type& value){
    const size_type elems_before = position - begin_;
    const size_type len = size();
    auto value_copy = value;
    if(elems_before < (len / 2)){
        require_capacity(n, true);
        // 原来的迭代器可能会失效
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        position = begin_ + elems_before;
        try{
            if(elems_before >= n){
                auto begin_n = begin_ + n;
                // 这里需要注意 uninitialized_copy 与 copy 的区别
                // 也就是说 其伴随着初始化的过程
                dhsstl::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                dhsstl::copy(begin_n, position, old_begin);
                dhsstl::fill(position - n, position, value_copy);
            }else{
                dhsstl::uninitialized_fill(
                    dhsstl::uninitialized_copy(begin_, position, new_begin), begin_, value_copy);
                begin_ = new_begin;
                dhsstl::fill(old_begin, position, value_copy);
            }
        }catch(...){
            // 前面的操作都包含了回滚, 所以这里不用回滚
            if(new_begin.node != begin_.node)
                destory_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    }else{
        require_capacity(n, false);
        // 原来的迭代器可能会失效
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        position = end_ - elems_after;
        try
        {
            if(elems_after > n){
                auto end_n = end_ - n;
                dhsstl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                dhsstl::copy_backward(position, end_n, old_end);
                dhsstl::fill(position, position + n, value_copy);
            }else{
                dhsstl::uninitialized_fill(end_, position + n, value_copy);
                dhsstl::uninitialized_copy(position, end_, position + n);
                end_ = new_end;
                dhsstl::fill(position, old_end, value_copy);
            }
        }
        catch(...){
            if(new_end.node != end_.node)
                destory_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }
}
        
// copy_insert()
template <typename T>
template <typename FIter>
void deque<T>::
copy_insert(iterator position, FIter first, FIter last, size_type n){
    const size_type elems_before = position - begin_;
    auto len = size();
    if(elems_before < (len / 2)){
        require_capacity(n, true);
        // 原来的迭代器可能会失效
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        position = begin_ + elems_before;
        try{
            if(elems_before >= n){
                auto begin_n = begin_ + n;
                dhsstl::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                dhsstl::copy(begin_n, position, old_begin);
                dhsstl::copy(first, last, position - n);
            }else{
                auto mid = first;
                dhsstl::advance(mid, n - elems_before);
                dhsstl::uninitialized_copy(first, mid,
                                           dhsstl::uninitialized_copy(begin_, position, new_begin));
                begin_ = new_begin;
                dhsstl::copy(mid, last, old_begin);
            }
        }catch(...){
            if(new_begin.node != begin_.node)
                destroy_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    }else{
        require_capacity(n, false);
        // 原来的迭代器有可能失效, 对于 try  catch 来说
        auto old_end = end_;
        auto new_end = end_ + n;
        const auto elems_after = len - elems_before;
        position = end_ - elems_after;
        try{
            if(elems_after > n){
                auto end_n = end_ - n;
                dhsstl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                dhsstl::copy_backward(position, end_n, old_end);
                dhsstl::copy(first, last, position);
            }else{
                auto mid = first;
                dhsstl::advance(mid, elems_after);
                dhsstl::uninitialized_copy(position, end_,
                                           dhsstl::uninitialized_copy(mid, last, end_));
                end_ = new_end;
                dhsstl::copy(first, mid, position);
            }
        }catch(...){
            if(new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }
}

// insert_dispatch 函数
template <typename T>
template <typename IIter>
void deque<T>::
insert_dispatch(iterator position, IIter first, IIter last, input_iterator_tag){
    if(last <= first) return;
    const size_type n = dhsstl::distance(first, last);
    const size_type elems_before = position - begin_;
    if(elems_before < (size() / 2)){
        require_capacity(n, true);
    }else{
        require_capacity(n, false);
    }
    position = begin_ + elems_before;
    // 这里存疑????  标准库的写法也不跟这个不一样
    auto cur = --last;
    // 这里为啥这样, input_iterator_tag为啥需要这么麻烦
    for(size_type i = 0; i < n; ++i, --cur){
        insert(position, *cur);
    }
}

template <typename T>
template <typename FIter>
void deque<T>::
insert_dispatch(iterator position, FIter first, FIter last, forward_iterator_tag){
    if(last <= first) return;
    const size_type n = dhsstl::distance(first, last);
    if(position.cur == begin_.cur){
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        try{
            dhsstl::uninitialized_copy(first, last, new_begin);
            begin_ = new_begin;
        }catch(...){
            if(new_begin.node != begin_.node)
                destory_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    }else if(position.cur == end_.cur){
        require_capacity(n, false);
        auto new_end = end_ + n;
        try{
            dhsstl::uninitialized_copy(first, last, end_);
            end_ = new_end;
        }catch(...){
            if(new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }else{
        copy_insert(position, first, last, n);
    }
}

// require_capacity 函数
template <typename T>
void deque<T>::require_capacity(size_type n, bool front){
    if(front && (static_cast<size_type>(begin_.cur - begin_.first) < n)){
        const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
        if(need_buffer > static_cast<size_type>(begin_.node - map_)){
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    }else if(!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)){
        const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
        if(need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)){
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_buffer(end_.node + 1, end_.node + need_buffer);
    }
}

// reallocate_map_at_front()
template <typename T>
void deque<T>::reallocate_map_at_front(size_type need_buffer){
    const size_type new_map_size = dhsstl::max(
        map_size_ << 1,
        map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE
    );
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buller = old_buffer + need_buffer;
    
    // 新的 map 中的指针指向原来的 buffer, 并开辟新的 buffer
    auto begin = new_map + (new_map_size - new_buller) / 2;
    auto mid = begin + need_buffer;
    auto end = mid + old_buffer;
    create_buffer(begin, mid - 1);
    for(auto begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2){
        *begin1 = *begin2;
    }
    
    // 更新数据
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
    end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
}

// reallocate_map_at_back()
template <typename T>
void deque<T>::reallocate_map_at_back(size_type need_buffer){
    const size_type new_map_size = dhsstl::max(
        map_size_ << 1,
        map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE
    );
    map_pointer new_map = create_buffer(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;

    // 从新的 map 中的指针指向原来的 buffer, 并开辟新的 buffer
    auto begin = new_map + ((new_map_size - new_buffer) / 2);
    auto mid = begin + old_buffer;
    auto end = mid + need_buffer;
    for(auto begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2){
        *begin1 = *begin2;
    }
    create_buffer(mid, end - 1);

    // update date
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
    end_   = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
}

// 重载操作符
template <typename T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs){
    return lhs.size() == rhs.size() &&
        dhsstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs){
    return dhsstl::lexicograhical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
    );
}

template <typename T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs) { return !(lhs == rhs); }

template <typename T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs) { return rhs < lhs; }

template <typename T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs) { return !(rhs < lhs); }

template <typename T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs) { return !(lhs < rhs); }

// 重载 dhsstl 的 swap
template <typename T>
void swap(deque<T>& lhs, deque<T>& rhs) { lhs.swap(rhs); }

} // namespace dhsstl
#endif // DHSTINYSTL_DEQUE_H_