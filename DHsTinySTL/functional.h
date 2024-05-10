#ifndef DHSTINYSTL_FUNCTIONAL_H_
#define DHSTINYSTL_FUNCTIONAL_H_

// 这个头文件包含一些仿函数
#include <cstddef>
namespace dhsstl{
/**
 * @brief 定义一元函数的参数型别和返回值型别
 * @tparam Arg 参数型别
 * @tparam Result 返回值型别
 * */ 
template<typename Arg, typename Result>
struct unarg_function
{
    typedef Arg     argument_type;
    typedef Result  Result_type;
};

/**
 * @brief 定义二元函数的参数型别和返回值型别
 * @tparam Arg1 参数型别1
 * @tparam Args 参数型别2
 * @tparam Result 返回值型别
*/
template <typename Arg1, typename Arg2, typename Result>
struct binary_function
{
    typedef Arg1    first_argument_type;
    typedef Arg2    second_argument_type;
    typedef Result  result_type;
};

/**
 * @brief 一些运算 + - X /
 * @tparam T 参与运算的参数类型
*/
template <typename T>
struct plus : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x + y; }
};

template <typename T>
struct minus : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x - y; }
};

template <typename T>
struct multiplies : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const {return x * y; }
};

template <typename T>
struct  divides : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x / y; }
};

template <typename T>
struct modules : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x % y; }
};

template <typename T>
struct negate : public unarg_function<T, T> 
{
    T operator()(const T& x, const T& y) const { return -x; }
};

/**
 * @brief 加法的证同元素
*/
template <typename T>
T identity_element(plus<T>) { return T(0); }

/**
 * @brief 乘法的证同元素
*/
template <typename T>
T identity_element(multiplies<T>) { return T(1); }

/**
 * @brief 用于比较大小的仿函数
 * @tparam T 参与比较的参数类型
 * @param x 参与运算的第一个参数
 * @param y 参与运算的第二个参数, 如果有的话
*/
template <typename T>
struct equal_to : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x == y; }
};

template <typename T>
struct not_equal_to : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x != y; }
};

template <typename T>
struct greater : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x > y; }
};

template <typename T>
struct less : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x < y; }
};

template <typename T>
struct greater_equal : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

template <typename T>
struct less_equal : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x <= y; }
};

template <typename T>
struct logical_and : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x && y; }
};

template <typename T>
struct logical_or : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x || y; }
};

template <typename T>
struct logical_not : public unarg_function<T, bool>
{
    bool operator()(const T& x) const { return !x; }
};

/**
 * @brief 证同函数, 不会改变元素, 返回本身
 * 这个函数目前有点疑问
*/
template <typename T>
struct identity : public unarg_function<T, T>
{
    const T& operator()(const T& x) const { return x; }
};

/**
 * @brief 选择函数, 接受一个pair 返回第一个元素
*/
template <typename Pair>
struct selectfirst : public unarg_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type& operator()(const Pair& x) const{
        return x.first;
    }
};

template <typename Pair>
struct selectsecond : public unarg_function<Pair, typename Pair::second_type>
{
    const typename Pair::second_type& operator()(const Pair& x) const{
        return x.second;
    }
};

/**
 * @brief 投射函数, 返回第一个 \ 第二个 参数
 * @tparam Arg1 第一个参数
 * @tparam Arg2 第二个参数
*/
template <typename Arg1, typename Arg2>
struct projectfirst : public binary_function<Arg1, Arg2, Arg1>
{
    Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

template <typename Arg1, typename Arg2>
struct projectsecond : public binary_function<Arg1, Arg2, Arg2>
{
    Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};

/*************************************************************************/
// 哈希函数对象

// 对于大部分类型, hash function 什么都不做
template <typename Key>
struct hash {};

// 针对指针的偏特化版本
template <typename T>
struct  hash<T*>
{
    size_t  operator()(T* p) const noexcept{
        return reinterpret_cast<size_t>(p);
    }
};

// 对于整数类型, 只是返回原值
#define DHSTINY_TRIVIAL_HASH_FCN(Type)        \
template <> struct hash<Type>                 \
{                                             \
    size_t operator()(Type val) const noexcept\
    { return static_cast<size_t>(val); }      \
};

DHSTINY_TRIVIAL_HASH_FCN(bool)

DHSTINY_TRIVIAL_HASH_FCN(char)

DHSTINY_TRIVIAL_HASH_FCN(signed char)

DHSTINY_TRIVIAL_HASH_FCN(unsigned char)

DHSTINY_TRIVIAL_HASH_FCN(wchar_t)

DHSTINY_TRIVIAL_HASH_FCN(char16_t)

DHSTINY_TRIVIAL_HASH_FCN(char32_t)

DHSTINY_TRIVIAL_HASH_FCN(short)

DHSTINY_TRIVIAL_HASH_FCN(unsigned short)

DHSTINY_TRIVIAL_HASH_FCN(int)

DHSTINY_TRIVIAL_HASH_FCN(unsigned int)

DHSTINY_TRIVIAL_HASH_FCN(long)

DHSTINY_TRIVIAL_HASH_FCN(unsigned long)

DHSTINY_TRIVIAL_HASH_FCN(long long)

DHSTINY_TRIVIAL_HASH_FCN(unsigned long long)

#undef DHSTINY_TRIVIAL_HASH_FCN

// 对于浮点数, 逐位哈希
inline size_t bitwise_hash(const unsigned char* first, size_t count){

#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull;
    const size_t fnv_prime = 1099511628211ull;
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif
    size_t  result = fnv_offset;
    for(size_t i = 0; i < count; ++i){
        result ^= (size_t)first[i];
        result *= fnv_prime;
    }
    return result;
}

template <>
struct hash<float>
{
    size_t  operator()(const float& val){
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
    }
};

template <>
struct hash<double>
{
    size_t operator()(const double& val){
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
    }
};

template <>
struct hash<long double>
{
    size_t operator()(const long double& val){
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
    }
};

} // namespace dhsstl
#endif //DHSTINYSTL_FUNCTIONAL_H_