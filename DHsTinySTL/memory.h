#ifndef DHSTINYSTL_MEMORY_H_
#define DHSTINYSTL_MEMORY_H_

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数, 空间配置器, 未初始化的储存空间管理, 以及一个模板类 auto_ptr

#include <cstddef>
#include <cstdlib>
#include <climits>
#include <atomic>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "util.h"
//#include "Detail/ref.h"

namespace dhsstl{

// 获取对象地址
template <typename Tp>
constexpr Tp* address_of(Tp& value) noexcept{
    return &value;
}

// 获取 / 释放 临时缓冲区
// 这里 确定了一个缓冲区的 最大大小: INT_MAX 也就是说这个缓冲区不能占用
// INT_MAX的内存? 这是为什么?
// 估计是 malloc 的问题 !!! Q:
// 申请一个缓冲区
template <typename T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*){
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
        len = INT_MAX / sizeof(T);
    while (len > 0){
        T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
        if(tmp)
            return pair<T*, ptrdiff_t>(tmp, len);
        len /= 2; // 申请失败时减少len的大小
    }
    return pair<T*, ptrdiff_t>(nullptr, 0);
}

template <typename T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len){
    return get_buffer_helper(len, static_cast<T*>(0));
}

template <typename T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*){
    return get_buffer_helper(len, static_cast<T*>(0));
}

template <typename T>
void release_temporary_buffer(T* ptr){
    free(ptr);
}

// ---------------------------------------------------------
// 类模板: temporary_buffer
// 进行临时缓冲区的申请和释放
template <typename ForwardIterator, typename T>
class temporary_buffer
{
private:
    ptrdiff_t original_len;     // 缓冲区申请的大小
    ptrdiff_t len;              // 缓冲区实际的大小
    T*        buffer;           // 指向缓冲区的指针

public:
    temporary_buffer(ForwardIterator first, ForwardIterator last);
    ~temporary_buffer(){
        dhsstl::destroy(buffer, buffer+len);
        free(buffer);
    }

public:
    ptrdiff_t size()            const noexcept { return len; }
    ptrdiff_t requested_size()  const noexcept { return original_len; }
    T*        begin()                 noexcept { return buffer; }
    T*        end()                   noexcept { return buffer + len; }

private:
    void allocate_buffer();
    void initialize_buffer(const T&, std::true_type) {}
    void initialize_buffer(const T& value, std::false_type){
        dhsstl::uninitialized_fill_n(buffer, len, value);
    }

private:
    temporary_buffer(const temporary_buffer&);
    // 这个 operator= 的作用是啥
    void operator=(const temporary_buffer&);
};

// 构造函数
template <typename ForwardIterator, typename T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last){
    try{
        len = dhsstl::distance(first, last);
        allocate_buffer();
        if(len > 0){
            initialize_buffer(*first, std::is_trivially_default_constructible<T>());
        }
    }catch(...){
        free(buffer);
        buffer = nullptr;
        len = 0;
    }
}

// allocate_buffer 函数
template <typename ForwardIterator, typename T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer(){
    original_len = len;
    if(len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
        len = INT_MAX / sizeof(T);
    while(len > 0){
        buffer = static_cast<T*>(malloc(len * sizeof(T)));
        if(buffer)
            break;
        len /= 2; // 申请失败时减少申请空间
    }
}

// ------------------------------------------------------
// 模板类: auto_ptr 
// 一个具有严格对象所有权的小型智能指针
template <typename T>
class auto_ptr{
public:
    typedef T   elem_type;
    
private:
    T* m_ptr;   // 实际指针
    
public:
    // 构造, 复制, 析构函数
    explicit auto_ptr(T* p = nullptr) : m_ptr(p){}
    auto_ptr(auto_ptr& rhs) : m_ptr(rhs.release()){}
    template<typename U>
    auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()){}

    auto_ptr& operator=(auto_ptr& rhs){
        if(this != &rhs){
            delete m_ptr;
            m_ptr = rhs.release();
        }
        return *this;
    }

    template <typename U>
    auto_ptr& operator=(auto_ptr<U>& rhs){
        if(this->get() != rhs.get()){
            delete m_ptr;
            m_ptr = rhs.release();
        }
        return *this;
    }

    ~auto_ptr(){delete m_ptr;}

public:
    // 重载operator* 和 operator->
    T& operator*() const{
        return *m_ptr;
    }
    T* operator->() const{
        return m_ptr;
    }

    // 获得指针
    T* get() const{return m_ptr;}

    // 释放指针
    T* release(){
        T* tmp = m_ptr;
        m_ptr = nullptr;
        return tmp;
    }

    // 重置指针
    void reset(T* p = nullptr){
        if(m_ptr != p){
            delete m_ptr;
            m_ptr = p;
        }
    }
};


/*!
 * @brief Unique_ptr
 * 具有严格对象所有权的智能指针
 * @tparam T 只能指针所指的类型
 */
template<typename T>
class unique_ptr{

public:
    typedef T               element_type;
    typedef element_type*   pointer;

public:
    explicit unique_ptr(T* data = nullptr) : data_(data){}
    unique_ptr(unique_ptr&& up) noexcept   : data_(nullptr){
        dhsstl::swap(data_, up.data_);
    }

    unique_ptr& operator=(unique_ptr&& up) noexcept{
        if(&up != this){
            clean();
            dhsstl::swap(*this, up);
        }
        return *this;
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator = (const unique_ptr&) = delete;

    ~unique_ptr(){ clean(); }

    pointer       get()      { return data_; }

    // 重载bool操作: if(ptr)
    explicit operator bool() const{return get() != nullptr; }

    pointer release(){
        T *p = nullptr;
        dhsstl::swap(p, data_);
        return p;
    }
    void reset(pointer p = pointer()){
        clean();
        data_ = p;
    }
    void swap(unique_ptr& up){
        dhsstl::swap(data_, up.data);
    }

    element_type&       operator*() const { return *data_; }
    pointer             operator->() {return data_;}
private:
    inline void clean(){
        data_ = nullptr;
    }
private:
    element_type *data_;
};

template<typename T>
void swap(unique_ptr<T>& x, unique_ptr<T>& y){
    x.swap(y);
}

template<typename T1, typename T2>
bool operator==(const unique_ptr<T1>& lhs, const unique_ptr<T2>& rhs){
    return lhs.get() == rhs.get();
}
template<typename T1, typename T2>
bool operator!=(const unique_ptr<T1>& lhs, const unique_ptr<T2>& rhs){
    return !(lhs == rhs);
}
template<typename T>
bool operator==(const unique_ptr<T>& up, std::nullptr_t p){
    return up.get() == p;
}
template<typename T>
bool operator==(std::nullptr_t p ,const unique_ptr<T>& up){
    return up.get() == p;
}
template<typename T>
bool operator!=(const unique_ptr<T>& up, std::nullptr_t p){
    return up.get() != p;
}
template<typename T>
bool operator!=(std::nullptr_t p ,const unique_ptr<T>& up){
    return up.get() != p;
}

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args){
    return unique_ptr<T>(new T(dhsstl::forward<Args>(args)...));
}

// 计数类
struct prt_count{
    prt_count():ncount_(0), wcount_(0){}
    std::atomic<size_t> ncount_;
    std::atomic<size_t> wcount_;
};


template<typename T> class shared_ptr;
/**
 * @brief weak_ptr
 * 弱共享指针, 由shared_ptr初始化
 * 不增加管理资源的指针计数, 当管理资源的shared_ptr计数清零后将组织访问资源
 * @tparam T
 */
template<typename T>
class weak_ptr{
    template <typename U> friend class shared_ptr;

public:
    weak_ptr() = default;
    weak_ptr(const weak_ptr& rhs): _data(rhs._data), _ptr_count(rhs._ptr_count){
        if(_ptr_count) ++_ptr_count->wcount_;
    }

    weak_ptr(const shared_ptr<T>& rhs): _data(rhs._data), _ptr_count(rhs._ptr_count){
        if(_ptr_count) ++_ptr_count->wcount_;
    }

    weak_ptr(weak_ptr&& rhs) noexcept{
        swap(rhs);
        rhs.reset();
    }

    weak_ptr& operator=(const weak_ptr& rhs) noexcept{
        weak_ptr tmp(rhs);
        swap(tmp);
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& rhs) noexcept{
        weak_ptr tmp(dhsstl::move(this));
        swap(tmp);
        return *this;
    }

    ~weak_ptr(){
        release();
    }

    void reset() noexcept{
        release();
        _data = nullptr;
        _ptr_count = nullptr;
    }

    int use_count() const noexcept{
        return _data == nullptr ? 0 : _ptr_count->ncount_.load();
    }

    bool expired() const noexcept{ return use_count() == 0; }

    shared_ptr<T> lock(){
        return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
    }
private:
    void swap(weak_ptr& rhs) noexcept{
        dhsstl::swap(_data, rhs._data);
        dhsstl::swap(_ptr_count, rhs._ptr_count);
    }

    void release(){
        if(_ptr_count){
            if(_ptr_count->ncount_ == 0 && --_ptr_count->wcount_ == 0){
                delete _ptr_count;
                _ptr_count = nullptr;
            }
        }
    }
private:
    T* _data = nullptr;
    prt_count* _ptr_count = nullptr;
};
/*!
 * @brief shared_ptr
 * 允许多个智能指针同时指向一个底部资源
 * @tparam T 只能指针所指的类型
 */
template<typename T>
class shared_ptr{
    template<typename U> friend class weak_ptr;
public:
    typedef T              element_type;

public:
    explicit shared_ptr(T* p = nullptr) : _data(p){
        if(p){
            try{
                _ptr_count = new prt_count();
            }
            catch(...){
                delete p;
                throw;
            }
        }
    }

    // template<class D>
    // shared_ptr(T *p, D del) : ref_t(new ref_t<T>(p, del)){}

    shared_ptr(const shared_ptr& sp) : _data(sp._data), _ptr_count(sp._ptr_count){
        if(_ptr_count) ++_ptr_count->ncount_;
    }
    shared_ptr(shared_ptr&& sp) noexcept{
        swap(this);
        sp.reset();
    }
    explicit shared_ptr(const weak_ptr<T>& wp) : _data(wp._data), _ptr_count(wp._ptr_count){
        if(_ptr_count) ++_ptr_count->ncount_;
    }

    shared_ptr& operator=( const shared_ptr& sp){
        shared_ptr tmp(sp);
        swap(tmp);
        return *this;
    }
    shared_ptr& operator=(shared_ptr&& sp) noexcept{
        shared_ptr tmp(sp);
        swap(tmp);
        return *this;
    }

    ~shared_ptr() { release(); }

    bool unique() const noexcept{
        return _data != nullptr && _ptr_count->ncount_.load() == 1;
    }
    int use_count() const {
        return _data == nullptr ? 0 : _ptr_count->ncount_.load();
    }

    element_type&       operator*()             { assert(*this); return *(get()); }
    element_type*       operator->()            { return get(); }
    const element_type& operator*()  const      { return *(get()); }
    const element_type* operator->() const      { return get(); }

    element_type*       get()       { return _data; }
    const element_type* get() const { return  _data; }

    void reset(){
        release();
        _data = nullptr;
        _ptr_count = nullptr;
    }

    void reset(T* data){
        reset();
        shared_ptr tmp(data);
        swap(tmp);
    }

    operator bool() const { return get() != nullptr;}
private:
    void swap(shared_ptr& rhs){
        dhsstl::swap(_data, rhs._data);
        dhsstl::swap(_ptr_count, rhs._ptr_count);
    }

    void release(){
        if(_ptr_count){
            if(--_ptr_count->ncount_ == 0){
                delete _data;
                _data = nullptr;
                if(_ptr_count->wcount_.load() == 0){
                    delete _ptr_count;
                    _ptr_count = nullptr;
                }
            }
        }
    }

private:
    // ref_t<T> *ref_;
    T* _data = nullptr;
    prt_count* _ptr_count = nullptr;
};

template<class T1, class T2>
bool operator==(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs){
    return lhs.get() == rhs.get();
}
template<class T1, class T2>
bool operator!=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs){
    return !(lhs == rhs);
}
template<class T>
bool operator==(const shared_ptr<T>& sh, std::nullptr_t p){
    return sh.get() == p;
}
template<class T>
bool operator==(std::nullptr_t p, const shared_ptr<T>& sh){
    return sh.get() == p;
}
template<class T>
bool operator!=(const shared_ptr<T>& sh, std::nullptr_t p){
    return sh.get() != p;
}
template<class T>
bool operator!=(std::nullptr_t p, const shared_ptr<T>& sh){
    return sh.get() != p;
}

template<class T, class... Args>
shared_ptr<T> make_shared(Args... args){
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}


}//namespace dhsstl
#endif //!DHSTINYSTL_MEMORY_H_