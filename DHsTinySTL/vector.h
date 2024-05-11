#ifndef DHSTINYSTL_VECTOR_H_
#define DHSTINYSTL_VECTOR_H_

// 这个头文件包含一个模板类 vector
// vector: 向量

// notes:
//
// 异常保证:
// dhsstl::vector<T> 满足基本的异常保证, 部分函数无异常保证, 并对以下函数做强异常安全保证
//      * emplace
//      * emplace_back
//      * push_back
// 当 dhsstl::is_nothrow_move_assignable<T>::value == true 时, 以下函数也满足强异常保证
//      * reserver
//      * resize
//      * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"
#include "algobase.h"

namespace dhsstl{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif

// 模板类: vector
// 模板参数 T 代表类型
template <typename T>
class vector{

    static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in dhsstl");
public:
    // vector 的嵌套型别定义
    typedef dhsstl::allocator<T>                     allocator_type;
    typedef dhsstl::allocator<T>                     data_allocator;

    typedef typename allocator_type::value_type      value_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef value_type*                              iterator;
    typedef const value_type*                        const_iterator;
    typedef dhsstl::reverse_iterator<iterator>       reverse_iterator;
    typedef dhsstl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return data_allocator(); }

private:
    iterator begin_;        // 表示目前使用空间的头部
    iterator end_;          // 表示目前使用空间的尾部
    iterator cap_;          // 表示目前存储空间的尾部

public:
    // 构造, 复制, 移动, 析构函数
    vector() noexcept{
        try_init();
    }

    explicit vector(size_type n){
        fill_init(n, value_type());
    }

    vector(size_type n, const value_type& value){
        fill_init(n, value);
    }

    template <typename Iter, typename std::enable_if<
        dhsstl::is_input_iterator<Iter>::value, int>::type = 0
    >
    vector(Iter first, Iter last){
        DHSSTL_DEBUG(!(last < first));
        range_init(first, last);
    }
    
    vector(const vector& rhs){
        range_init(rhs.begin_, rhs.end_);
    }

    vector(vector&& rhs) noexcept
        : begin_(rhs.begin_),
        end_(rhs.end_),
        cap_(rhs.cap_)
    {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }

    vector(std::initializer_list<value_type> ilist)
    {
        range_init(ilist.begin(), ilist.end());
    }

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs) noexcept;

    vector& operator=(std::initializer_list<value_type> ilist)
    {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~vector(){
        destory_and_recorver(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }
public:
    // 迭代器相关操作
    iterator                begin()         noexcept{
        return begin_;
    }
    const_iterator          begin()   const noexcept{
        return begin_;
    }
    iterator                end()           noexcept{
        return end_;
    }
    const_iterator          end()     const noexcept{
        return end_;
    }

    reverse_iterator        rbegin()        noexcept{
        return reverse_iterator(end());
    }
    const_reverse_iterator  rbegin()  const noexcept{
        return const_reverse_iterator(end());
    }
    reverse_iterator        rend()          noexcept{
        return reverse_iterator(begin());
    }
    const_reverse_iterator  rend()    const noexcept{
        return const_reverse_iterator(begin());
    }

    const_iterator          cbegin()   const noexcept{
        return begin();
    }
    const_iterator          cend()     const noexcept{
        return end();
    }
    const_reverse_iterator  crbegin()  const noexcept{
        return rbegin();
    }
    const_reverse_iterator  crend()    const noexcept{
        return rend();
    }

    // 容器相关操作
    bool                    empty()    const noexcept{
        return begin_ == end_;
    }
    size_type               size()     const noexcept{
        return static_cast<size_type>(end_ - begin_);
    }
    size_type               max_size() const noexcept{
        return static_cast<size_type>(-1) / sizeof(T);
    }
    size_type               capacity() const noexcept{
        return static_cast<size_type>(cap_ - begin_);
    }
    void                    reserve(size_type n);
    void                    shrink_to_fit();

    // 访问元素相关操作
    reference               operator[](size_type n){
        DHSSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    const_reference         operator[](size_type n) const{
        DHSSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    reference               at(size_type n){
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference         at(size_type n) const{
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
        return (*this)[n];
    }               

    reference               front(){
        DHSSTL_DEBUG(!empty());
        return *begin_;
    }
    const_reference         front() const{
        DHSSTL_DEBUG(!empty());
        return *begin_;
    }
    reference               back(){
        DHSSTL_DEBUG(!empty());
        return *(end_ - 1);
    }
    const_reference         back() const{
        DHSSTL_DEBUG(!empty());
        return *(end_ - 1);
    }

    pointer                 data()       noexcept{ return begin_; }
    const_pointer           data() const noexcept{ return begin_; }

    // 修改容器相关操作

    // assign
    void assign(size_type n, const value_type& value){
        fill_assign(n, value);
    }

    template <typename Iter, typename std::enable_if<
        dhsstl::is_input_iterator<Iter>::value, int 
    >::type = 0>
    void assign(Iter first, Iter last){
        DHSSTL_DEBUG(!(last < first));
        copy_assign(first, last, iterator_category(first));
    }

    void assign(std::initializer_list<value_type> il){
        copy_assign(il.begin(), il.end(), dhsstl::forward_iterator_tag{});
    }

    // emplace / emplace_back
    template <typename...Args>
    iterator emplace(const_iterator pos, Args&& ...args);

    template <typename...Args>
    void emplace_back(Args&& ...args);

    // push_back / pop_back
    void push_back(const value_type& value);
    void push_back(value_type&& value){
        emplace_back(dhsstl::move(value));
    }
    void pop_back();
    // insert
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value){
        return emplace(pos, dhsstl::move(value));
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value){
        DHSSTL_DEBUG(pos >= begin() && pos <= end());
        return fill_insert(const_cast<iterator>(pos), n, value);
    }

    template <typename Iter, typename std::enable_if<
        dhsstl::is_input_iterator<Iter>::value, int 
    >::type = 0>
    void insert(const_iterator pos, Iter first, Iter last){
        DHSSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
        copy_insert(const_cast<iterator>(pos), first, last);
    }

    // erase / clear
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void     clear(){ erase(begin(), end()); }

    // resieze / reverse
    void     resize(size_type new_size){ return resize(new_size, value_type());}
    void     resize(size_type new_size, const value_type& value);
    
    void     reverse(){ dhsstl::reverse(begin(), end()); }

    void     swap(vector<T>& rhs)   noexcept;
private:    
    // helper functions

    // initialize / destory
    void        try_init() noexcept;
    void        init_space(size_type size, size_type cap);
    void        fill_init(size_type n, const value_type& value);

    template <typename Iter>
    void        range_init(Iter first, Iter last);

    void        destory_and_recorver(iterator first, iterator last, size_type n);

    // calculate the growth size
    size_type   get_new_cap(size_type add_size);

    // assign
    void        fill_assign(size_type n, const value_type& value);

    template <typename IIter>
    void        copy_assign(IIter first, IIter last, input_iterator_tag);
    
    template <typename FIter>
    void        copy_assign(FIter first, FIter last, forward_iterator_tag);

    // reallocate
    template <typename... Args>
    void        reallocate_emplace(iterator pos, Args&& ...args);

    void        reallocate_insert(iterator pos, const value_type& value);

    // insert
    iterator    fill_insert(iterator pos, size_type n, const value_type& value);
    
    template <typename IIter>
    void        copy_insert(iterator pos, IIter frist, IIter last);

    // shrink_to_fit
    void        reinsert(size_type size);
};


// ------------------------------------------------------------------------------

// 复制赋值操作符 
template <typename T>
vector<T>& vector<T>::operator=(const vector& rhs){
    if(this != &rhs){
        const auto len = rhs.size();
        if(len > capacity()){
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        }else if(size() >= len){
            auto i = dhsstl::copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        }else{
            dhsstl::copy(rhs.begin(), rhs.begin() + size(), begin_);
            dhsstl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            cap_ = end_ = begin_ + len;
        }
    }
    return *this;
}

// 移动赋值操作符
template <typename T>
vector<T>& vector<T>::operator=(vector&& rhs) noexcept{
    destory_and_recorver(begin_, end_, cap_ - begin_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ =rhs.cap_;
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;
}

// 预留空间大小, 当原容量小于要求大小时, 才会重新分配
template<typename T>
void vector<T>::reserve(size_type n){
    if(capacity() < n){
        THROW_LENGTH_ERROR_IF(n > max_size(),
            "n can not larger than max_size() in vector<T>::reserve(n)" 
        );
        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);
        dhsstl::uninitialized_move(begin_, end_, tmp);
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

// 放弃多余的容量
template <typename T>
void vector<T>::shrink_to_fit(){
    if(end_ < cap_){
        reinsert(size());
    }
}

// 在pos位置就地构造元素, 避免额外的赋值或者移动开销
// !!!
template <typename T>
template <typename ...Args>
typename vector<T>::iterator
vector<T>::emplace(const_iterator pos, Args&& ...args){
    DHSSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    if(end_ != cap_ && end_ == xpos){
        data_allocator::construct(dhsstl::address_of(*end_), dhsstl::forward<Args>(args)...);
        ++end_;
    }else if(end_ != cap_){
        auto new_end = end_;
        data_allocator::construct(dhsstl::address_of(*end_), *(end_ - 1));
        ++new_end;
        // 这里为什么不用move_backward
        // dhsstl::move_backward(xpos, end_ - 1, end_);
        dhsstl::copy_backward(xpos, end_ - 1, end_);
        *xpos = value_type(dhsstl::forward<Args>(args)...);
        end_ = new_end;
    }else{
        reallocate_emplace(xpos, dhsstl::forward<Args>(args)...);
    }
    return begin() + n;
}

// 在尾部就地构造元素, 避免额外的复制或者移动开销
template <typename T>
template <typename ...Args>
void vector<T>::emplace_back(Args&& ...args){
    if(end_ < cap_){
        data_allocator::construct(dhsstl::address_of(*end_), dhsstl::forward<Args>(args)...);
        ++end_;
    }else{
        reallocate_emplace(end_, dhsstl::forward<Args>(args)...);
    }
}

// 在尾部插入元素
template <typename T>
void vector<T>::push_back(const value_type& value){
    if(end_ != cap_){
        data_allocator::construct(dhsstl::address_of(*end_), value);
        ++end_;
    }else{
        reallocate_emplace(end_, value);
    }
}

// 弹出尾部元素
template<typename T>
void vector<T>::pop_back(){
    DHSSTL_DEBUG(!empty());
    data_allocator::destroy(end_ - 1);
    --end_;
}

// 在 pos 处插入元素
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, const value_type& value){
    DHSSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = pos - begin_;
    if(end_ != cap_ && xpos == end_){
        data_allocator::construct(dhsstl::address_of(*end_), value);
        ++end_;
    }else if(end_ != cap_){
        auto new_end = end_;
        data_allocator::construct(dhsstl::address_of(*end_), *(end_ - 1));
        ++new_end;
        auto value_copy = value; // 避免元素因为以下复制操作而被改变
        dhsstl::copy_backward(xpos, end_ - 1, end_);
        *xpos = dhsstl::move(value_copy);
        end_ = new_end;
    }else{
        reallocate_insert(xpos, value);
    }
    return begin_ + n;
}

// 删除pos位置上的元素
template <typename T>
typename vector<T>::iterator
vector<T>::erase(const_iterator pos){
    DHSSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = begin_ + (pos - begin());
    dhsstl::move(xpos + 1, end_, xpos);
    data_allocator::destroy(end_ - 1);
    --end_;
    return xpos;
}

// 删除[first, last)上的元素
template <typename T>
typename vector<T>::iterator
vector<T>::erase(const_iterator first, const_iterator last){
    DHSSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
    const auto n = first - begin();
    iterator r = begin_ + (first - begin());
    data_allocator::destroy(dhsstl::move(r + (last - first), end_, r), end_);
    end_ = end_ - (last - first);
    return begin_ + n;
}

// 重置容器大小
template <typename T>
void vector<T>::resize(size_type new_size, const value_type& value){
    if(new_size < size()){
        erase(begin() + new_size, end());
    }else{
        insert(end(), new_size - size(), value);
    }
}

// 与另一个 vector 交换
template <typename T>
void vector<T>::swap(vector<T>& rhs) noexcept{
    if(this != &rhs){
        dhsstl::swap(begin_, rhs.begin_);
        dhsstl::swap(end_, rhs.end_);
        dhsstl::swap(cap_, rhs.cap_);
    }
}
// ------------------------------------------------------------------------------
// helper function

// try_init(), 若分配失败则忽略, 不抛出异常 
template <typename T>
void vector<T>::try_init() noexcept{
    try
    {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    }
    catch(...)
    {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

// init_space 函数
template <typename T>
void vector<T>::init_space(size_type size, size_type cap){
    try
    {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    }
    catch(...)
    {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}

// fill_init 函数
template <typename T>
void vector<T>::fill_init(size_type n, const value_type& value){
    const size_type init_size = dhsstl::max(static_cast<size_type>(16), n);
    init_space(n, init_size);
    dhsstl::uninitialized_fill_n(begin_, n, value);
}

// range_init 函数
template <typename T>
template <typename Iter>
void vector<T>::range_init(Iter first, Iter last){
    const size_type len = dhsstl::distance(first, last);
    const size_type init_size = dhsstl::max(len, static_cast<size_type>(16));
    init_space(len, init_size);
    dhsstl::uninitialized_copy(first, last, begin_);
}

// destory_and_recover()
template <typename T>
void vector<T>::destory_and_recorver(iterator first, iterator last, size_type n){
    data_allocator::destroy(first, last);
    data_allocator::deallocate(first, n);
}

// get_new_cap()
template <typename T>
typename vector<T>::size_type
vector<T>::get_new_cap(size_type add_size){
    // 这个函数要分三种情况:
    //  1. add_size 装不下: 也就是 old_size > max_size() - add_size
    //  2. add_size 可以装的下
    //      因为 c++ 默认一次可以装 old_size / 2 这样扩
    //      但加入 已经装不下, 那就只能按照 old_size / 2 这样了
    //      若可以装的下, 就判断一下
    const auto old_size = capacity();
    THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, "vector<T>'s size too big");
    if(old_size > max_size() - old_size / 2){
        return old_size + add_size > max_size() - 16
            ? old_size + add_size : old_size + add_size + 16;
    }
    const size_type new_size = old_size == 0 ?
        dhsstl::max(add_size, static_cast<size_type>(16)) :
        dhsstl::max(old_size + old_size / 2, old_size + add_size);
    return new_size;
}

// fill_assign()
template <typename T>
void vector<T>::fill_assign(size_type n, const value_type& value){
    if (n > capacity()){
        vector tmp(n, value);
        swap(tmp);
    }else if(n > size()){
        dhsstl::fill(begin(), end(), value); 
        end_ = dhsstl::uninitialized_fill_n(end_, n - size(), value);
    }else{
        erase(dhsstl::fill_n(begin_, n, value), end_);
    }
}

// copy_assign()
template<typename T>
template<typename IIter>
void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag){
    auto cur = begin_; 
    for(; first != last && cur != end_; ++first, ++cur){
        *cur = *first;
    }
    if(first == last){
        erase(cur, end_);
    }else{
        insert(end_, first, last);
    }
}

// 用[first, last) 为容器赋值
template<typename T>
template<typename FIter>
void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag){
    const size_type len = dhsstl::distance(first, last);
    if(len > capacity()){
        vector tmp(first, last);
        swap(tmp);
    }else if(size() >= len){
        auto new_end = dhsstl::copy(first, last, begin_);
        data_allocator::destroy(new_end, end_);
        end_ = new_end;
    }else{
        auto mid = first;
        dhsstl::advance(mid, size());
        dhsstl::copy(first, mid, begin_);
        auto new_end = dhsstl::uninitialized_copy(mid, last, end_);
        end_ = new_end;
    }
}

// 重新分配空间并在 pos 出就地构造元素
template <typename T>
template <typename... Args>
void vector<T>::reallocate_emplace(iterator pos, Args&& ...args){
    // 注: 这个get_new_cap的参数是加的个数
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    try{
        new_end = dhsstl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(dhsstl::address_of(*new_end), dhsstl::forward<Args>(args)...);
        ++new_end;
        new_end = dhsstl::uninitialized_move(pos, end_, new_end);
    }catch(...){
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destory_and_recorver(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// 重新分配空间并在 pos 处插入元素
template<typename T>
void vector<T>::reallocate_insert(iterator pos, const value_type& value){
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size); 
    auto new_end = new_begin;
    const value_type& value_copy = value;
    try{
        new_end = dhsstl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(dhsstl::address_of(*new_end), value_copy);
        ++new_end;
        new_end = dhsstl::uninitialized_move(end_, new_end);
    }catch(...){
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destory_and_recorver(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// fill_insert()
template <typename T>
typename vector<T>::iterator
vector<T>::
fill_insert(iterator pos, size_type n, const value_type& value){
    if(n == 0)
        return pos;
    const size_type xpos = pos - begin_;
    const value_type value_copy = value; // 避免被覆盖 
    if(static_cast<size_type>(cap_ - end_) >= n){
        // 如果备用空间大于等于增加的空间
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try{
            new_end = dhsstl::uninitialized_move(begin_, pos, new_begin);
            new_end = dhsstl::uninitialized_fill_n(new_end, n, value);
            new_end = dhsstl::uninitialized_move(pos, end_, new_end);
        }catch(...){
            destory_and_recorver(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
    return begin_ + xpos;
}

// copy_insert 函数
template <typename T>
template <typename IIter>
void vector<T>::
copy_insert(iterator pos, IIter first, IIter last){
    if(first == last)
        return;
    const auto n = dhsstl::distance(first, last);
    if((cap_ - end_) > n){
        // 如果备用空间大小足够
        const auto after_elems = end_ - pos;
        auto old_end = end_;
        if(after_elems > n){
            end_ = dhsstl::uninitialized_copy(end_ - n, end_, end_);
            dhsstl::move_backward(pos, old_end - n, old_end);
            dhsstl::uninitialized_copy(first, last, pos);
        }else{
            auto mid = first;
            dhsstl::advance(mid, after_elems);
            end_ = dhsstl::uninitialized_copy(mid, last, end_);
            end_ = dhsstl::uninitialized_move(pos, old_end, end_);
            dhsstl::uninitialized_copy(first, mid, pos);
        }
    }else{
        // 如果备用空间不足
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try{
            new_end = dhsstl::uninitialized_move(begin_, pos, new_begin);
            new_end = dhsstl::uninitialized_copy(first, last, new_end);
            new_end = dhsstl::uninitialized_move(pos, end_, new_end);
        }catch(...){
            destory_and_recorver(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
}

// reinsert 函数
template <typename T>
void vector<T>::reinsert(size_type size){
    auto new_begin = data_allocator::allocate(size);
    try{
        dhsstl::uninitialized_move(begin_, end_, new_begin);
    }catch(...){
        data_allocator::deallocate(new_begin, size);
        throw;
    }
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = new_begin;
    end_ = begin_ + size;
    cap_ = begin_ + size;
}

// --------------------------------------------------------------
// 重载比较操作符

template <typename T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs){
    return lhs.size() == rhs.size() &&
        dhsstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs){
    return dhsstl::lexicograhical_compare(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs){
    return !(lhs == rhs);
}

template <typename T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs){
    return rhs < lhs;
} 

template <typename T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs){
    return !(rhs > lhs);
}

template <typename T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs){
    return !(lhs < rhs);
}

// 重载 dhsstl 的 swap
template <typename T>
void swap(vector<T>& lhs, vector<T>& rhs){
    lhs.swap(rhs);
}

} // namespace dhsstl
#endif // !DHSTINYSTL_VECTOR_H_