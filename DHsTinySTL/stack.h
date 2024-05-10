#ifndef DHSTINYSTL_STACK_H_
#define DHSTINYSTL_STACK_H_

// 这个头文件包含了一个模板类 stack

#include "deque.h"

namespace dhsstl{

/**
 * @brief A standard container giving FILO behavior
 * 
 * @ingroup sequences
 * 
 * @tparam T  type of element
 * @tparam Container  Type of underlying sequence, defaults to deque<T>, 简单来说就是底层容器是啥
*/
template <typename T, typename Container = dhsstl::deque<T>>
class stack{

public:
    typedef Container                           container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    static_assert(std::is_same<T, value_type>::value,
                  "the value_type of Container should be same with T");


private:
    container_type c_; // 用底层容器实现 stack

public:
    // 构造 复制 移动函数
    /**
     * @brief Default constructor creates no elements
    */
    stack() = default;

    explicit stack(size_type n)
        :c_(n) {}

    stack(size_type n, const value_type& value)
        :c_(n, value){}

    template <typename IIter>
    stack(IIter first, IIter last)
        :c_(first, last){}
    
    stack(std::initializer_list<T> ilist)
        :c_(ilist.begin(), ilist.end()){}

    stack(const Container& c)
        :c_(c){}

    stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(dhsstl::move(c)){}

    stack(const stack& rhs)
        :c_(rhs.c_){}

    stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(dhsstl::move(rhs.c_)) {}
    
    stack& operator=(const stack& rhs){
        c_ = rhs.c_;
        return *this;
    }

    stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value){
        c_ = dhsstl::move(rhs.c_);
        return *this;
    }

    stack& operator=(std::initializer_list<T> ilist){
        c_ = ilist;
        return *this;
    } 

    ~stack() = default;

    // 访问元素相关操作
    /**
     *  Return a read/write reference to the data at the first
     *  element of the %stack
    */
    reference       top()       {return c_.back(); }
    
    /**
     *  Returns a read-only (constant) reference to the data at the first
     *  element of the %stack
    */
    const_reference top() const {return c_.back(); }

    // 容量相关操作
    /**
     *  Return true if the %stack is empty.
    */
    bool        empty()   const noexcept { return c_.empty(); }

    /** Returns the number of elements in the %stack. */
    size_type   size()    const noexcept { return c_.size(); }

    // 修改容器相关操作
    /**
     *  @brief Add data to the top of the %stack
     *  @param value Data to be added
    */
    void push(const value_type& value)
    { c_.push_back(value); }

    void push(value_type&& value)
    { c_.push_back(dhsstl::move(value)); }

    template <typename ...Args>
    void emplace(Args&& ...args)
    { c_.emplace_back(dhsstl::forward<Args>(args)...); }

    /** @brief Removes first element */
    void pop()
    { c_.pop_back(); }

    /**
     * @brief Clear this %stack
    */
    void clear(){
        while(!empty()){
            pop();
        }
    }

    void swap(stack& rhs) noexcept(noexcept(dhsstl::swap(c_, rhs.c_)))
    { dhsstl::swap(c_, rhs.c_); }

public:
    friend bool operator==(const stack& lhs, const stack& rhs){ return lhs.c_ == rhs.c_; }
    friend bool operator< (const stack& lhs, const stack& rhs){ return lhs.c_ <  rhs.c_; }
}; // stack

/**
 * @brief Stack equality comparison
 * @param lhs  A %stack
 * @param rhs  A %stack of the same type as @a lhs.
 * @return True or False
*/
// 重载比较操作符
template <typename T, typename Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return lhs == rhs; }

template <typename T, typename Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return lhs < rhs; }


template <typename T, typename Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return !(lhs == rhs); }


template <typename T, typename Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return rhs < lhs; }


template <typename T, typename Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return !(rhs < lhs); }

template <typename T, typename Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return !(lhs < rhs); }

// 重载 dhsstl 的 swap
template <typename T, typename Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{ return lhs.swap(rhs); }

}

#endif // DHSTINYSTL_STACK_H_