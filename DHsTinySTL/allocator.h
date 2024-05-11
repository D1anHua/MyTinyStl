#ifndef DHSTINYSTL_ALLOCATOR_H_
#define DHSTINYSTL_ALLOCATOR_H_

// allocator 以及 construct
// 一般的new操作包括几步:
// 1. operator new
// 2. placemnet new
// 这个头文件包含一个模板类 allocator, 用于管理内存的分配以及释放, 对象的构造以及析构
#include "construct.h"
#include "util.h"

namespace dhsstl
{
// 模板类: allocator
// 模板函数代表数据类型
template <class T>
class allocator
{
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    static T*   allocate();
    static T*   allocate(size_type n);
    
    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type value);

    static void construct(T* ptr);
    static void construct(T* ptr, const T& value);
    static void construct(T* ptr, T&& value);

    template <class... Args>
    static void construct(T* ptr, Args&& ...args);
    
    static void destroy(T* ptr);
    static void destroy(T* first, T* last);
    
};

// 定义
template<class T>
T* allocator<T>::allocate(){
    return static_cast<T*>(::operator new(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(size_type n){
    if(n == 0)
        return nullptr;
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template<class T>
void allocator<T>::deallocate(T* ptr){
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template<class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/){
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template<class T>
void allocator<T>::construct(T* ptr){
    dhsstl::construct(ptr);
}

template<class T>
void allocator<T>::construct(T* ptr, const T& value){
    dhsstl::construct(ptr, value);
}

template<class T>
void allocator<T>::construct(T* ptr, T&& value){
    dhsstl::construct(ptr, dhsstl::move(value));
}

template<class T>
template<class ...Args>
void allocator<T>::construct(T* ptr, Args&& ...args){
    dhsstl::construct(ptr, dhsstl::forward<Args>(args)...);
}

template<class T>
void allocator<T>::destroy(T* ptr){
    dhsstl::destroy(ptr);
}

template<class T>
void allocator<T>::destroy(T* first, T* last){
    dhsstl::destroy(first, last);
}

} // namespace dhsstl

#endif // DHSTINYSTL_ALLOCATOR_H_