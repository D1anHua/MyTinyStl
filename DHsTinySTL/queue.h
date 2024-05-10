#ifndef DHSTINYSTL_QUEUE_H_
#define DHSTINYSTL_QUEUE_H_

// 这个头文件包含了两个模板类 queue 和 priority_queue
#include "deque.h"
#include "vector.h"
#include "functional.h"
#include "heap_algo.h"

namespace dhsstl{

/**
 * @brief A standard container giving FIFO behavior
 * 
 * @ingroup sequences
 * 
 * @tparam T  type of element
 * @tparam Container  type of underlying sequence, default to deque<T>
*/
template <typename T, typename Container = dhsstl::deque<T>>
class queue{
public:
    typedef Container                               container_type;
    typedef typename Container::value_type          value_type;
    typedef typename Container::size_type           size_type;
    typedef typename Container::reference           reference;
    typedef typename Container::const_reference     const_reference;

    static_assert(std::is_same<T, value_type>::value,
                  "the value_type of Container should be same with T");
private:
    container_type c_;

public:
    /** @brief Default constructor creates no elements */
    queue() = default;

    explicit queue(size_type n)
        :c_(n) {}

    queue(size_type n, const value_type& value)
        :c_(n, value) {}
    
    template <typename IIter>
    queue(IIter first, IIter last)
        :c_(first, last) {}
    
    queue(std::initializer_list<T> ilist)
        :c_(ilist.begin(), ilist.end()) {}
    
    queue(const Container& c)
        :c_(c) {}
    
    queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(dhsstl::move(c)) {}
    
    queue(const queue& rhs)
        :c_(rhs.c_) {}
    
    queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(dhsstl::move(c_)) {}

    queue& operator=(const queue& rhs){
        c_ = rhs.c_;
        return *this;
    }

    queue& operator=(std::initializer_list<T> ilist){
        c_ = ilist;
        return *this;
    }

    ~queue() = default;

    // 访问元素相关操作
    /**
     * Returns a read/write reference to the data at the first
     * element of the %queue.
    */
    reference           front()         { return c_.front(); }
    /** 
     * Returns a read-only(constant) reference to the data at the first
     * element of the %queue.
     */
    const_reference     front()   const { return c_.front(); }
    /**
     * Returns a read/write reference to the data at the last
     * element of the %queue.
    */
    reference           back()          { return c_.back(); }
    /** 
     * Returns a read-only(constant) reference to the data at the first
     * element of the %queue.
     */
    const_reference     back()   const { return c_.back(); }

    /**
     * Returns true if the %queue is empty.
    */
    bool        empty() const noexcept  { return c_.empty(); }
    /** Returns the number of elements in the %queue. */
    size_type   size()  const noexcept  { return c_.size(); }

    /**
     * @brief Add data to the end of the %queue.
     * @param value Data to be added.
    */
    void push(const value_type& value)
    { c_.push_back(value); }
    void push(value_type&& value)
    { c_.emplace_back(dhsstl::move(value)); }

    template <typename ...Args>
    void emplace(Args ...args)
    { c_.emplace_back(dhsstl::forward<Args>(args)...); }

    /**
     * @brief Removes first element.
    */
    void pop()
    { c_.pop_front(); }

    void clear(){
        while (!empty()){
            pop();
        }
    }

    void swap(queue& rhs) noexcept(noexcept(dhsstl::swap(c_, rhs.c_)))
    { dhsstl::swap(c_, rhs.c_); }

public:
    // 这两个友元有啥用
    // 为啥不合下面的写在一块
    friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }

};
/**
 * @brief Queue equality comparison
 * @param lhs A %queue.
 * @param rhs A %queue of the same type as @a lhs.
 * @return Ture or False
*/
template <typename T, typename Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return lhs == rhs; }

template <typename T, typename Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return !(lhs == rhs); }

template <typename T, typename Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return lhs < rhs; }

template <typename T, typename Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return rhs < lhs; }

template <typename T, typename Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return !(lhs < rhs); }

template <typename T, typename Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{ return !(rhs < lhs); }

template <typename T, typename Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{ lhs.swap(rhs); }

//--------------------------------------------------------------------------
/**
 * 
 * @brief priority_queue
 * @tparam T 参数类型
 * @tparam Container 容器, 默认使用 dhsstl::vector 作为底层容器
 * @tparam Compare 比较权值的方式, 缺省使用 dhsstl::less 作为比较方式
*/
template <typename T, typename Container = dhsstl::vector<T>,
          typename Compare = dhsstl::less<typename Container::value_type>>
class priority_queue
{
public:
    typedef Container                           container_type;
    typedef Compare                             value_compare;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    static_assert(std::is_same<T, value_type>::value,
                  "the value_type of Container should be same with T");
private:
    container_type c_;
    value_compare  comp_;
public:
    /**
     * @brief 构造函数
    */
    priority_queue() = default;

    priority_queue(const Compare& c)
        :c_(), comp_(c) {}
    
    explicit priority_queue(size_type n)
        :c_(n)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(size_type n, const value_type& value)
        :c_(n, value)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    template <typename IIter>
    priority_queue(IIter first, IIter last)
        :c_(first, last)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(std::initializer_list<T> ilist)
        :c_(ilist)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const Container& s)
        :c_(s)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(Container&& s)
        :c_(dhsstl::move(s))
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const priority_queue& rhs)
        :c_(rhs.c_), comp_(rhs.comp_)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(priority_queue&& rhs)
        :c_(dhsstl::move(rhs.c_)), comp_(rhs.comp_)
    {
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue& operator=(const priority_queue& rhs){
        c_ = rhs.c_;
        comp_ = rhs.comp_;
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs){
        c_ = dhsstl::move(rhs.c_);
        comp_ = rhs.comp_;
        dhsstl::make_heap(c_.begin(),c_.end(), comp_);
        return *this;
    }

    priority_queue& operator=(std::initializer_list<T> ilist){
        c_ = ilist;
        comp_ = value_compare();
        dhsstl::make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    ~priority_queue() = default;

public:
    /**
     * @brief 返回首元素的常量引用
    */
    const_reference top() const { return c_.front(); }

    /**
     * @brief 判断优先队列是否为空
     * @return 如果为空, 返回true
    */
    bool     empty() const noexcept { return c_.empty(); }
    /**
     * @brief 返回队列中元素的数目 
    */
    size_type size() const noexcept { return c_.size(); }

    /**
     * @brief 向优先队列中添加元素
    */
    template <typename ...Args>
    void emplace(Args&& ...args){
        c_.emplace_back(dhsstl::forward<Args>(args)...);
        dhsstl::push_heap(c_.begin(), c_.end(), comp_);
    }

    void push(const value_type& value){
        c_.push_bcak(value);
        dhsstl::push_heap(c_.begin(), c_.end(), comp_);
    }

    void push(value_type&& value){
        c_.push_back(dhsstl::move(value));
        dhsstl::push_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 删除优先队列中的对首元素
    */
    void pop(){
        dhsstl::pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }

    /** @brief 清空优先队列 */
    void clear(){
        while(!empty())
            pop();
    }

    void swap(priority_queue& rhs) noexcept(noexcept(dhsstl::swap(c_, rhs.c_))&&
                                            noexcept(dhsstl::swap(comp_, rhs.comp_))){
        dhsstl::swap(c_, rhs.c_);
        dhsstl::swap(comp_, rhs.comp_);
    }

public:
    friend bool operator==(const priority_queue& lhs, const priority_queue& rhs){
        return lhs.c_ == rhs.c_;
    }

    friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs){
        return lhs.c_ != rhs.c_;
    }
};

// 重载比较操作符
template <typename T, typename Container, typename Compare>
bool operator==(priority_queue<T, Container, Compare>& lhs,
                priority_queue<T, Container, Compare>& rhs)
{
    return lhs == rhs;
}

template <typename T, typename Container, typename Compare>
bool operator!=(priority_queue<T, Container, Compare>& lhs,
                priority_queue<T, Container, Compare>& rhs)
{
    return lhs != rhs;
}

template <typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare>& lhs,
                priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace dhsstl

#endif // DHSTINYSTL_QUEUE_H_