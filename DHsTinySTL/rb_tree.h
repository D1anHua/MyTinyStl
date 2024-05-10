#ifndef DHSTINYSTL_RB_TREE_H_
#define DHSTINYSTL_RB_TREE_H_

// RB Tree implementation -*- C++ -*-

#include <initializer_list>
#include <cassert>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"

namespace dhsstl{

// enum _Rb_tree_color_type { _S_red = false, _S_black = true };
typedef bool _Rb_tree_color_type;

static constexpr _Rb_tree_color_type _Rb_tree_red   = false;
static constexpr _Rb_tree_color_type _Rb_tree_black = true;

// forward declaration
struct _Rb_tree_node_base;
template<typename T> struct _Rb_tree_node;

template<typename T> struct _Rb_tree_iterator;
template<typename T> struct _Rb_tree_const_iterator;

// rb tree value traits
template <typename T, bool>
struct _Rb_tree_value_traits_imp{
    typedef T   key_type;
    typedef T   mapped_type;
    typedef T   value_type;
    
    template <typename Ty>
    static const key_type& get_key(const Ty& value){
        return value;
    }

    template <typename Ty>
    static const value_type& get_value(const Ty& value){
        return value;
    }
};

template <typename T>
struct _Rb_tree_value_traits_imp<T, true>
{
    // remove_cv:remove const-volatile
    typedef typename std::remove_cv<typename T::first_type>::type   key_type;
    typedef typename T::seconde_type                                mapped_type;
    typedef T                                                       value_type;

    template <typename Ty>
    static const key_type& get_key(const Ty& value){
        return value.first;
    }

    template <typename Ty>
    static const value_type& get_value(const Ty& value){
        return value;
    }
};

template <typename T>
struct _Rb_tree_value_traits
{
    static constexpr bool is_map = dhsstl::is_pair<T>::value;

    typedef _Rb_tree_value_traits_imp<T, is_map>    value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template <typename Ty>
    static const key_type& get_key(const Ty& value){
        return value_traits_type::get_key(value);
    }

    template <typename Ty>
    static const value_type& get_key(const Ty& value){
        return value_traits_type::get_value(value);
    }
};

/**
 * @brief rb_tree node traits
*/
template <typename T>
struct _Rb_tree_node_traits
{
    typedef _Rb_tree_color_type                 color_type;

    typedef _Rb_tree_value_traits<T>            value_traits;
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;

    typedef _Rb_tree_node_base*                 base_ptr;
    typedef _Rb_tree_node<T>*                   node_ptr;
};

/**
 * @brief rb_tree的节点设置
*/
struct _Rb_tree_node_base
{
    typedef _Rb_tree_color_type          color_type;
    typedef _Rb_tree_node_base*          base_ptr;
    typedef const _Rb_tree_node_base*    const_base_prt;

    base_ptr        parent;
    base_ptr        left;
    base_ptr        right;
    color_type      color;

    static base_ptr
    minimun(base_ptr x){
        while(x -> left != nullptr) x = x->left;
        return x;
    }

    static const_base_prt
    minimun(const_base_prt x){
        while(x-> left != nullptr) x = x->left;
        return x;
    }

    static base_ptr
    maximum(base_ptr x){
        while(x -> right != nullptr) x = x->right;
        return x;
    }

    static const_base_prt
    maximum(const_base_prt x){
        while(x -> right != nullptr) x = x->right;
        return x;
    }
};

template <typename Value>
struct _Rb_tree_node : public _Rb_tree_node_base{
    typedef _Rb_tree_node<Value>*    node_type;
    Value value_field;
};

/**
 * @brief rb tree traits
*/
template <typename T>
struct _Rb_tree_traits
{
    typedef _Rb_tree_value_traits<T>           value_traits;

    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;

    typedef value_type*                         pointer;
    typedef value_type&                         reference;
    typedef const value_type*                   const_pointer;
    typedef const value_type&                   const_reference;

    typedef _Rb_tree_node_base                  base_type;
    typedef _Rb_tree_node<T>                    node_type;

    typedef base_type*                          base_ptr;
    typedef node_type*                          node_ptr;
};

template <typename T>
struct _Rb_tree_base_iterator : public dhsstl::iterator<dhsstl::bidirectional_iterator_tag, T>
{
    typedef _Rb_tree_node_base::base_ptr       base_ptr;
    typedef bidirectional_iterator_tag         iterator_category;
    typedef ptrdiff_t                          different_type;
    
    base_ptr                                   node; // 它用来与容器之间产生一个连结关系(make a reference)

    _Rb_tree_base_iterator() : node(nullptr) {}

    // 使迭代器前进
    void inc() {
        if(node -> right != nullptr){
            // 尝试调用node的函数
            node = _Rb_tree_node_base::minimun(node->right);
            // while (node -> left != nullptr)
            //     node = node -> left; 
        }else{
            auto y = node->parent;
            while(y -> right == node){
                node = y;
                y = y -> parent;
            }
            if(node->right != y) // 为了应对"寻找根节点的下一节点, 而根节点没有右子节点"的特殊情况
                node = y;        // 此时, 根节点的父节点是 nullptr 而 node 的 右子节点也是 nullptr
        }
    }

    void dec(){
        if(node->parent->parent == node && node->color == _Rb_tree_red){
            node = node->right;
        }else if(node -> left != 0){
            node = _Rb_tree_node_base::maximum(node -> left);
            // base_ptr y = node->left;
            // while(y->right != 0)
            //     y = y->right;
            // node = y;
        }else{
            base_ptr y = node->parent;
            while(node == y->left){
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }

    bool operator==(const _Rb_tree_base_iterator& rhs){ return node == rhs.node; }
    bool operator!=(const _Rb_tree_base_iterator& rhs){ return node != rhs.node; }
};

template <typename T>
struct _Rb_tree_iterator : public _Rb_tree_base_iterator{
    typedef T                   value_type;
    typedef T*                  pointer; 
    typedef T&                  reference;

    typedef _Rb_tree_iterator<T>            iterator;
    typedef _Rb_tree_const_iterator<T>      const_iterator;
    typedef _Rb_tree_iterator<T>            self;
    typedef _Rb_tree_node_base::base_ptr    base_ptr;
    typedef _Rb_tree_node<T>*               node_ptr;

    using _Rb_tree_base_iterator::node;

    _Rb_tree_iterator() {}
    _Rb_tree_iterator(base_ptr x) { node = x; }
    _Rb_tree_iterator(node_ptr x) { node = x; }
    _Rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    _Rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    reference   operator*()     const { return static_cast<node_ptr>(node)->value_field; }
    pointer     operator->()    const { return &(operator*()); }

    self& operator++(){
        this->inc();
        return *this;
    }

    self operator++(int){
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--(){
        this->dec();
        return *this;
    }
    self operator--(int){
        self tmp(*this);
        this->dec();
        return tmp;
    }
};

template<typename T>
struct _Rb_tree_const_iterator : public _Rb_tree_base_iterator{
    typedef T                               value_type;
    typedef const T*                        pointer;
    typedef const T&                        reference;

    typedef _Rb_tree_iterator<T>            iterator;
    typedef _Rb_tree_const_iterator<T>      const_iterator;
    typedef _Rb_tree_const_iterator<T>      self;
    typedef _Rb_tree_node_base::base_ptr    base_ptr;
    typedef _Rb_tree_node<T>*               node_ptr;

    using _Rb_tree_base_iterator::node;

    // 构造函数
    _Rb_tree_const_iterator() {}
    _Rb_tree_const_iterator(base_ptr x) { node = x; }
    _Rb_tree_const_iterator(node_ptr x) { node = x; }
    _Rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
    _Rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    // MyTinySTL使用的是 reinterpret_cast,
    reference   operator*()  const { return static_cast<node_ptr>(node)->value_field; }
    pointer     operator->() const { return &(operator*()); }

    self& operator++(){
        this->inc();
        return *this;
    }

    self operator++(int){
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--(){
        this->dec();
        return *this;
    }

    self operator--(int){
        self tmp(*this);
        this->dec();
        return *this;
    }
};

//---------------------------------------------------------------------------
// tree algorithm

/**
 * @brief 判断一个节点是否是左子节点
 * @param node 需要判断的节点
*/
template <typename BasePtr>
BasePtr _rb_tree_is_lchild(BasePtr node) noexcept{
    return node == node->parent->left;
}

/**
 * @brief 判断一个节点颜色是否是红色
 * @param node 需要判断的节点
*/
template <typename BasePtr>
bool _rb_tree_is_red(BasePtr node) noexcept{
    return node->color == _Rb_tree_red;
}

/**
 * @brief 将一个节点的颜色设置为黑色
 * @param node 需要设置的节点
*/
template <typename BasePtr>
void _rb_tree_set_black(BasePtr& node) noexcept{
    node->color = _Rb_tree_black;
}

/**
 * @brief 将一个节点的颜色设置为红色
 * @param node 需要设置的节点
*/
template <typename BasePtr>
BasePtr _rb_tree_set_red(BasePtr& node) noexcept{
    node->color = _Rb_tree_red;
}

/**
 * @brief 查找某节点的下一节点
 * @param node 需要查找的节点
*/
template <typename BasePtr>
BasePtr _rb_tree_next(BasePtr node) noexcept{
    if(node -> right != nullptr)
        return _Rb_tree_node_base::minimun(node->right);
    while( !_rb_tree_is_lchild(node))
        node = node -> parent;
    return node -> parent;
}

/**
 * @brief 左旋
 * @param x 左旋点 
 * @param root 根节点
 * /*---------------------------------------*\
 * |       p                         p       |
 * |      / \                       / \      |
 * |     x   d    rotate left      y   d     |
 * |    / \       ===========>    / \        |
 * |   a   y                     x   c       |
 * |      / \                   / \          |
 * |     b   c                 a   b         |
 * /*---------------------------------------*\
*/
template <typename BasePtr>
void _rb_tree_rotate_left(BasePtr x, BasePtr& root) noexcept{
    // 将 x 的 右子节点 当作 x 的父节点
    // 原来 x 的 右子节点 的 左子节点 变为 x 现在的 右结点
    // x 的 右子节点 的 父节点 换为 x 的 父节点
    auto y = x->right;
    x->right = y -> left;
    if(y -> left != nullptr)
        y -> left -> parent = x;
    y->parent = x->parent;
    // 接下来需要改变 x 的 父节点的指引关系
    if(x == root){
        // 如果x是根节点, 则让y顶替x成为根节点
        root = y;
    }else if(_rb_tree_is_lchild(x)){
        // 如果 x 是左子节点
        x->parent->left = y;
    }else{
        // 如果 x 是右子节点
        x->parent->right = y;
    }
    // 调整 x 与 y 的关系
    y->left = x;
    x->parent = y;
}

/**
 * @brief 右旋
 * @param x 右旋点
 * @param root 根节点
 *|---------------------------------------- |
 *|     p                         p          |
 *|    / \                       / \         |
 *|   d   x      rotate right   d   y        |
 *|      / \     ===========>      / \       |
 *|     y   a                     b   x      |
 *|    / \                           / \     |
 *|   b   c                         c   a    |
 *| ---------------------------------------- |
*/
template <typename BasePtr>
void _Rb_tree_rotate_right(BasePtr x, BasePtr& root) noexcept{
    auto y = x -> left;
    x -> left = y -> right;
    if(y -> right)
        y->right->parent = x;
    y->parent = x -> parent;

    if(x == root){
        // 如果 x 为根节点, 让 y 顶替 x 成为根节点
        root = y;
    }else if(_rb_tree_is_lchild(x)){
        // 如果 x 是右子节点
        x->parent->left = y;
    }else{
        // 如果 x 是左子节点
        x->parent->right = y;
    }
    // 调整 x 与 y 的关系
    y -> right = x;
    x -> parent = y;
}

/**
 * 插入节点后使 rb tree 重新平衡, 参数一为新增节点, 参数二为根节点
 * 
 * case 1 : 新增节点位于根节点, 令新增节点为黑
 * case 2 : 新增节点的父节点为黑, 没有破坏平衡, 直接返回
 * case 3 : 父节点和叔叔节点都为红, 令父节点和叔叔节点为黑, 祖父节点为红
 *          然后令祖父节点为当前节点, 继续处理
 * case 4 : 父节点为红, 叔叔节点为 NIL 或黑色, 父节点为左(右)孩子, 当前节点为右(左)孩子,
 *          让父节点成为当前节点, 再一当前节点为支点左旋
 * case 5 : 父节点为红, 叔叔节点为 NIL 或黑色, 父节点为左(右)孩子, 当前节点为左(右)孩子,
 *          让父节点变为黑色, 祖父节点变为红色, 以祖父节点为支点右(左)旋
*/
template <typename BasePtr>
void _Rb_tree_insert_rebalance(BasePtr x, BasePtr& root) noexcept{
    _rb_tree_set_red(x); // 新增节点为红色
    while(x != root && _rb_tree_is_red(x -> parent)){
        if(_rb_tree_is_lchild(x->parent)){
            // 如果父节点是左子节点
            auto uncle = x->parent->parent->right;
            if(uncle != nullptr && _rb_tree_is_red(uncle)){
                // case 3 父亲节点和叔叔节点都为红
                _rb_tree_set_black(x->parent);
                _rb_tree_set_black(uncle);
                x = x -> parent -> parent;
                _rb_tree_set_red(x);
            }else{
               // 无叔叔节点或者叔叔节点为黑 
               if(!_rb_tree_is_lchild(x)){
                // case 4: 当前节点 x 为右子节点
                x = x -> parent;
                _rb_tree_rotate_left(x, root);
               }
               // 都转换为case5 : 当前节点为左子节点
               _rb_tree_set_black(x->parent);
               _rb_tree_set_red(x->parent->parent);
               _rb_tree_rotate_right(x->parent->parent, root);
               break;
            } // 如果父节点是右子节点, 对称处理
        }else{
            // 如果父节点是右子节点, 对称处理
            auto uncle = x -> parent -> parent -> left;
            if(uncle != nullptr && _rb_tree_is_red(uncle)){
                // case 3 : 父节点和叔叔节点都为红
                _rb_tree_set_black(x->parent);
                _rb_tree_set_black(uncle);
                x = x -> parent -> parent;
                _rb_tree_set_red(x);
                // 此时祖父节点为红, 可能会破坏红黑树的性质, 另当前节点为祖父节点, 继续处理
            }else{
                // 无叔叔节点或这叔叔节点为黑
                if(_rb_tree_is_lchild(x)){
                    // case 4: 当前节点 x 为左子节点
                    x = x -> parent;
                    _rb_tree_rotate_right(x, root);
                }
                // 都转换为 case 5: 当前节点为左子节点
                _rb_tree_set_black(x -> parent);
                _rb_tree_set_red(x->parent->parent);
                _rb_tree_rotate_left(x->parent->parent, root);
                break;
            }
        }
    }
    _rb_tree_set_black(root); // 跟节点永远为黑
}

/**
 * @brief 删除节点后使 Rb_tree 重新平衡
 * @param 要删除的节点
 * @param 根节点
 * @param 最小节点
 * @param 最大节点
*/
template <typename BasePtr>
BasePtr _rb_tree_erase_rebalance(BasePtr z, BasePtr& root, BasePtr& leftmost, BasePtr& rightmost){
    // y 是可能的替换节点, 指向最终要删除的节点
    auto y = (z->left == nullptr || z->right == nullptr) ? z : _rb_tree_next(z);
    // x 是 y 的一个独子节点或者NULL节点
    auto x = y->left != nullptr ? y->left : y->right;
    // xp 为 x 的父节点
    BasePtr xp = nullptr;

    // y != z 说明 z 有两个非空子节点, 此时 y 指向 z 右子树的最左节点, x 指向 y 的右子节点
    // 用 y 顶替 z 的位置, 用 x 顶替 y 的位置, 最后用 y 指向 z
    if(y != z){
        z->left->parent = y;
        y->left = z->left;
        
        // 如果 y 不是 z 的右子节点, 那么 z 的右子节点一定有左孩子
        if( y != z->right){
            // x 替换 y 的 位置
            xp = y -> parent;
            if(x != nullptr)
                x->parent = y->parent;
            
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        }else{
            xp = y;
        }

        // 连接 y 与 z 的父节点
        if( root == z )
            root = y;
        else if(_rb_tree_is_lchild(z))
            z->parent->left = y;
        else
            z->parent->right = y;
        y->parent = z->parent;
        dhsstl::swap(y->color, z->color);
        y = z;
    }else{
        // y == z 说明 z 至多只有一个孩子
        xp = y->parent;
        if(x)
            x->parent = y->parent;

        // 连接 x 与 z 的父节点
        if(root == z)
            root = x;
        else if(_rb_tree_is_lchild(z))
            z->parent->left = x;
        else
            z->parent->right = x;
        
        // 此时 z 有可能是最左节点或最右节点, 更新数据
        if(leftmost == z)
            leftmost = x == nullptr ? xp : _Rb_tree_node_base::minimun(x);
        if(rightmost == z){
            leftmost = x == nullptr ? xp : _Rb_tree_node_base::maximum(x);
        }
    }
    // 此时, y 指向要删除的节点, x 为替代节点, 从 x 节点开始调整.
    // 如果删除的节点为红色, 树的性质没有被破坏, 否则按照以下情况调整(x 为左子节点为例):
    // case 1 : 兄弟节点为红色, 令父节点为红, 兄弟节点为黑, 进行左(右)旋, 继续处理
    // case 2 : 兄弟节点为黑色, 且两个子节点都为黑色或者 NTL, 另兄弟节点为红, 父节点成为当前节点, 继续处理
    // case 3 : 兄弟节点为黑色, 左子节点为红色 或 NIL, 右子节点为黑色或者 NIL
    //          令兄弟节点为红, 兄弟节点的左子节点为黑, 一兄弟节点为支点右旋, 继续处理
    // case 4 : 兄弟节点为黑色, 右子节点为红色, 令兄弟节点为父节点的颜色, 父节点为黑色, 兄弟节点的右子节点
    //          为黑色, 以父节点为支点左(右)旋, 树的性质调整完成, 算法结束

    if(!_Rb_tree_red(y)){
        // x 为 黑色时, 调整, 否则直接将 x 变为黑色即可
        while(x != root && (x == nullptr || !_rb_tree_is_red(x))){
            if(x == xp->left){
                // 如果 x 为左子节点
                auto brother = xp->right;
                if(_rb_tree_is_red(brother)){
                    // case 1
                    _rb_tree_set_black(brother);
                    _rb_tree_set_red(xp);
                    _rb_tree_rotate_left(xp, root);
                    brother = xp -> right;
                }
                // case 1 转为了 case 2 3 4 中的一种
                if((brother->left == nullptr || !_rb_tree_is_red(brother->left))&&
                    (brother->right == nullptr || !_rb_tree_is_red(brother->right))){
                    // case 2
                    _rb_tree_set_red(brother);
                    x = xp;
                    xp = xp -> parent;
                }else{
                    if(brother->right == nullptr || !_rb_tree_is_red(brother->right)){
                        // case 3
                        if(brother->left != nullptr)
                            _rb_tree_set_black(brother->left);
                        _rb_tree_set_red(brother);
                        _Rb_tree_rotate_right(brother, root);
                        brother = xp->right;
                    }
                    // 转为 case 4
                    brother->color = xp->color;
                    _rb_tree_set_black(xp);
                    if(brother -> right != nullptr)
                        _rb_tree_set_black(brother->right);
                    _rb_tree_rotate_left(xp, root);
                    break;
                }
            }else{
                // x 为 右子节点, 对称处理
                auto brother = xp -> left;
                if(_rb_tree_is_red(brother)){
                    // case 1
                    _rb_tree_set_black(brother);
                    _rb_tree_set_red(xp);
                    _Rb_tree_rotate_right(xp, root);
                    brother = xp -> left;
                }
                if((brother->left == nullptr || !_rb_tree_is_red(brother->left))&&
                    (brother->right == nullptr || !_rb_tree_is_red(brother->right)))
                {
                    // case 2
                    _rb_tree_set_red(brother);
                    x = xp;
                    xp = xp -> parent;
                }else{
                    if(brother->left == nullptr || !_rb_tree_is_red(brother->left)){
                        // case 3
                        if(brother->right != nullptr)
                            _rb_tree_set_black(brother->right);
                        _rb_tree_set_red(brother);
                        _rb_tree_rotate_left(brother, root);
                        brother = xp->left;
                    }
                    // 转为 case 4
                    brother->color = xp->color;
                    _rb_tree_set_black(xp);
                    if(brother->left != nullptr)
                        _rb_tree_set_black(brother->left);
                    _rb_tree_rotate_right(xp, root);
                    break;
                }
            }
        }
        if(x != nullptr)
            _rb_tree_set_black(x);
    }
    return y;
}

/**
 * @brief 模板类 rb_tree
 * @tparam T Value类型
 * @tparam Compare 键值比较类型
*/
template <typename T, typename Compare>
class rb_tree{
    
public:
    typedef _Rb_tree_traits<T>                          tree_traits;
    typedef _Rb_tree_value_traits<T>                    value_traits;
    // rb_tree 的嵌套型别定义
    typedef _Rb_tree_node_base                          base_type;
    typedef _Rb_tree_node_base*                         base_ptr;
    typedef _Rb_tree_node<T>                            node_type;
    typedef _Rb_tree_node<T>*                           node_ptr;
    typedef typename tree_traits::key_type              key_type;
    typedef typename tree_traits::mapped_type           mapped_type;
    typedef typename tree_traits::value_type            value_type;
    typedef Compare                                     key_compare;

    typedef dhsstl::allocator<T>                        allocator_type;
    typedef dhsstl::allocator<T>                        data_allocator;
    typedef dhsstl::allocator<base_type>                base_allocator;
    typedef dhsstl::allocator<node_type>                node_allocator;

    typedef typename allocator_type::pointer             pointer;
    typedef typename allocator_type::const_pointer       const_pointer;
    typedef typename allocator_type::reference           reference;    
    typedef typename allocator_type::const_reference     const_reference;
    typedef typename allocator_type::size_type           size_type;
    typedef typename allocator_type::difference_type     difference_type;

    typedef _Rb_tree_iterator<T>                        iterator;
    typedef _Rb_tree_const_iterator<T>                  const_iterator;
    typedef dhsstl::reverse_iterator<iterator>          reverse_iterator;
    typedef dhsstl::reverse_iterator<const_iterator>    const_reverse_iterator;

    allocator_type  get_allocator()  const { return node_allocator(); }
    key_compare     key_comp()       const { return key_comp_; }

private:
    // 用一下三个数据表现 rb_tree
    base_ptr        header_;        // 特殊节点, 与根节点互为对方的父节点
    size_type       node_count_;    // 节点数
    key_compare     key_comp_;      // 节点键值比较的准则

private:
    /**
     * @brief 用于取得根节点, 最小节点和最大节点
    */
    base_ptr& root()        const { return header_-> parent; }
    base_ptr& leftmost()    const { return header_ -> left;}
    base_ptr& rightmost()   const { return header_ -> right; } 

public:
    /**
     * @brief 构造 复制 析构函数
    */
    rb_tree() { rb_tree_init(); }

    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs) noexcept;

    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs);

    ~rb_tree() { clear(); }

public:
    // 迭代器相关操作

    iterator                begin()             noexcept
    { return leftmost(); }

    const_iterator          begin()       const noexcept
    { return leftmost(); }

    iterator                end()               noexcept
    { return header_; }
    const_iterator          end()         const noexcept
    { return header_; }

    reverse_iterator        rbegin()            noexcept
    { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin()      const noexcept
    { return const_reverse_iterator(end()); }

    reverse_iterator        rend()              noexcept
    { return reverse_iterator(begin()); }
    const_reverse_iterator  rend()        const noexcept
    { return const_reverse_iterator(begin()); }

    const_iterator          cbegin()      const noexcept
    { return begin(); }
    const_iterator          cend()        const noexcept
    { return end(); }
    const_reverse_iterator  crbegin()     const noexcept
    { return rbegin(); }
    const_reverse_iterator  crend()       const noexcept
    { return rend(); }

    // 容量相关操作
    /**
     * @brief 容量相关操作
    */ 
    bool        empty()     const noexcept { return node_count_ == 0; }
    size_type   size()      const noexcept { return node_count_; }
    size_type   max_size()  const noexcept { return static_cast<size_type>(-1); }

    /**
     * @brief 在容器中插入或者删除元素
    */
    template <typename ...Args>
    iterator emplace_multi(Args&& ...args);

    template <typename ...Args>
    dhsstl::pair<iterator, bool> emplace_unique(Args&& ...args);

    template <typename ...Args>
    iterator emplace_multi_use_hint(iterator hint, Args&& ...args);

    template <typename ...Args>
    iterator emplace_unique_use_hint(iterator hint, Args&& ...args);

    iterator insert_multi(const value_type& value);
    iterator insert_multi(value_type&& value){
        return emplace_multi(dhsstl::move(value));
    }

    iterator insert_multi(iterator hint, const value_type& value){
        return emplace_multi_use_hint(hint, value);
    }
    iterator insert_multi(iterator hint, value_type&& value){
        return emplace_multi_use_hint(hint, dhsstl::move(value));
    }

    template <typename InputIterator>
    void insert_multi(InputIterator first, InputIterator last){
        size_type n = dhsstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for(; n > 0; --n, ++first)
            insert_multi(end(), *first);
    }

    dhsstl::pair<iterator, bool> insert_unique(const value_type& value);
    dhsstl::pair<iterator, bool> insert_unique(value_type&& value){
        return emplace_unique(dhsstl::move(value));
    }

    iterator  insert_unique(iterator hint, const value_type& value){
        return emplace_unique_use_hint(hint, value);
    }
    iterator  insert_unique(iterator hint, value_type&& value){
        return emplace_unique_use_hint(hint, dhsstl::move(value));
    }

    template <typename InputIterator>
    void      insert_unique(InputIterator first, InputIterator last){
        size_type n = dhsstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for(; n > 0; --n, ++first)
            insert_unique(end(), *first);
    }

    /**
     * @brief 删除操作
    */
    iterator   erase(iterator hint);
    size_type  erase_multi(const key_type& key);
    size_type  erase_unique(const key_type& key);

    void       erase(iterator first, iterator last);
    void       clear();

    /**
     * @brief 给定key值, 返回对应的迭代器
     * @param 想要查找的key值
    */
    iterator  find(const key_type& key);
    const_iterator find(const key_type& key) const;

    /**
     * @brief 给定key值, 计算key的数目
     * @param 想要查找的key值
    */
    size_type       count_multi(const key_type& key) const{
        auto p = equal_range_multi(key);
        return static_cast<size_type>(dhsstl::distance(p.first, p.second));
    }
    size_type       count_unique(const key_type& key) const{
        return find(key) != end() ? 1 : 0;
    }

    iterator        lower_bound(const key_type& key);
    const_iterator  lower_bound(const key_type& key) const;

    iterator        upper_bound(const key_type& key);
    const_iterator  upper_bound(const key_type& key) const;

    dhsstl::pair<iterator, iterator>
    equal_range_multi(const key_type& key){
        return dhsstl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
    }

    dhsstl::pair<const_iterator, const_iterator>
    equal_range_multi(const key_type& key) const{
        return dhsstl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
    }

    dhsstl::pair<iterator, iterator>
    equal_range_unique(const key_type& key){
        iterator it = find(key);        
        auto next = it;
        return it == end() ? dhsstl::make_pair(it, it) : dhsstl::make_pair(it, ++next);
    }

    dhsstl::pair<const_iterator, const_iterator>
    equal_range_unique(const key_type& key) const{
        const_iterator it = find(key);
        auto next = it;
        return it == end() ? dhsstl::make_pair(it, it) : dhsstl::make_pair(it ,++next);
    }

    void swap(rb_tree& rhs) noexcept;

private:
    // node related
    template <typename ...Args>
    node_ptr create_node(Args&& ...args);
    node_ptr clone_node(base_ptr x);
    void     destory_node(node_ptr p);

    // init / reset
    void     rb_tree_init();
    void     reset();

    // get insert pos
    dhsstl::pair<base_ptr, bool>
    get_insert_multi_pos(const key_type& key);

    dhsstl::pair<dhsstl::pair<base_ptr, bool>, bool>
    get_insert_unique_pos(const key_type& key);

    // insert value / insert node
    iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

    // insert use hint
    iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

    // copy tree / erase tree
    base_ptr copy_from(base_ptr x, base_ptr p);
    void     erase_since(base_ptr x);
};

// ------------------------------------------------------------

/**
 * @brief 复制构造函数
*/
template <typename T, typename Compare>
rb_tree<T, Compare>::
rb_tree(const rb_tree& rhs){
    rb_tree_init();
    if(rhs.node_count_ != 0){
        root() = copy_from(rhs.root(), header_);
        leftmost() = _Rb_tree_node_base::minimun(root());
        rightmost() = _Rb_tree_node_base::maximum(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

/**
 * @brief 移动构造函数
*/
template <typename T, typename Compare>
rb_tree<T, Compare>::
rb_tree(rb_tree&& rhs) noexcept
 : header_(dhsstl::move(rhs.header_)),
   node_count_(rhs.node_count_),
   key_comp_(rhs.key_comp_)
{
    rhs.reset();
}

/**
 * @brief 复制赋值操作符
*/
template <typename T, typename Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(const rb_tree& rhs){
    if(this != &rhs){
        clear();
        
        if(rhs.node_count_ != 0){
            root() = copy_from(rhs.root(), header_);
            leftmost() = _Rb_tree_node_base::minimun(root());
            rightmost() = _Rb_tree_node_base::maximum(root());
        }

        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}

/**
 * @brief 移动赋值操作符
*/
template <typename T, typename Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(rb_tree&& rhs){
    clear();
    header_ = dhsstl::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.reset();
    return *this;
}

/**
 * @brief 就地插入元素, 并且允许键值的重复
*/
template <typename T, typename Compare>
template <typename ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi(Args&& ...args){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(dhsstl::forward<Args>(args)...);
    auto res = get_insert_multi_pos(value_traits::get_key(np->value));
    return insert_node_at(res.first, np, res.second);
}

/**
 * @brief 就地插入元素, 键值不允许重复
*/
template <typename T, typename Compare>
template <typename ...Args>
dhsstl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
emplace_unique(Args&& ...args){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(dhsstl::forward<Args>(args)...);
    auto res = get_insert_unique_pos(value_traits::get_key(np->value));
    if(res.second){
        // 插入成功
        return dhsstl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
    }
    destory_node(np);
    return dhsstl::make_pair(iterator(res.first.first), false);
}

/**
 * @brief 就地插入元素, 键值允许重复, 当 hint 位置与插入位置接近时, 插入操作的时间复杂度可以降低
*/
template <typename T, typename Compare>
template <typename ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi_use_hint(iterator hint, Args&& ...args){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(dhsstl::forward<Args>(args)...);
    if(node_count_ == 0){
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if(hint == begin()){
        // 位于 begin 处
        if(key_comp_(key, value_traits::get_key(*hint))){
            return insert_node_at(hint.node, np, true);
        }else{
            auto pos = get_insert_multi_pos(key);
            return insert_node_at(hint.node, np, true);
        }
    }else if(hint == end()){
        // 位于 end 处
        if(!key_comp_(key, value_traits::get_key(static_cast<node_ptr>(rightmost())->value_field))){
            return insert_node_at(rightmost(), np, false);
        }else{
            auto pos = get_insert_multi_pos(key);
            return insert_node_at(pos.first, np, pos.second);
        }
    }
    return insert_multi_use_hint(hint, key, np);
}

/**
 * @brief 就地插入元素, 键值不允许重复, 当 hint 位置与插入位置接近时, 插入操作的时间复杂度可以降低
*/
template <typename T, typename Compare>
template <typename ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_unique_use_hint(iterator hint, Args&& ...args){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(dhsstl::forward<Args>(args)...);
    if(node_count_ == 0){
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if(hint == begin()){
        // 位于begin()处
        if(key_comp_(key, value_traits::get_key(*hint))){
            return insert_node_at(hint.node, np, true);
        }else{
            auto pos = get_insert_unique_pos(key);
            if(!pos.second){
                destory_node(np);
                return pos.first.first;
            }
            return insert_node_at(pos.first.first, np, pos.first.second);
        }
    }else if(hint == end()){
        // 位于 end 处
        if(key_comp_(value_traits::get_key(static_cast<node_ptr>(rightmost())->value_field), key)){
            return insert_node_at(rightmost(), np, false);
        }else{
            auto pos = get_insert_unique_pos(key);
            if(!pos.second){
                destory_node(np);
                return pos.first.first;
            }
            return insert_node_at(pos.first.first, np, pos.first.second);
        }
    }
    return insert_unique_use_hint(hint, key, np);
}

/**
 * @brief 插入元素, 节点键值允许重复
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi(const value_type& value){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    auto res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

/**
 * @brief 插入新值, 节点键值不允许重复,
 * @return pair: 若插入成功, pair.second 为 true, 反之为 false
*/
template <typename T, typename Compare>
dhsstl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
insert_unique(const value_type& value){
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");    
    auto res = get_insert_unique_pos(value_traits::get_key(value));
    if(res.second){
        // 插入成功
        return dhsstl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    }
    return dhsstl::make_pair(res.first.first, false);
}

/** 
 * @brief 删除 hint 位置的节点
 * @param hint iterator 需要删除节点的迭代器
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
erase(iterator hint){
    auto node = static_cast<node_ptr>(hint.node);
    iterator next(node);
    ++next;

    _rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
    destory_node(node);
    --node_count_;
    return next;
}

/**
 * @brief  删除键值等于 key 的元素
 * @param key 要被删除的键值
 * @return 删除的个数
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_multi(const key_type& key){
    auto p = equal_range_multi(key);
    size_type n = dhsstl::distance(p.first, p.second);
    erase(p.first, p.second);
    return n;
}

/**
 * @brief 删除键值等于 key 的元素
 * @param key 要被删除的键值
 * @return 删除的个数
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_unique(const key_type& key){
    auto it = find(key);
    if(it != end()){
        erase(it);
        return 1;
    }
    return 0;
}

/**
 * @brief 删除[first, last) 区间内的元素
 * @param first first 迭代器
 * @param last last 迭代器
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
erase(iterator first, iterator last){
    if(first == begin() && last == end()){
        clear();
    }else{
        while(first != last)
            erase(first++);
    }
}

/**
 * @brief 清空 rb_tree
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
clear(){
    if(node_count_ != 0){
        erase_since(root());
        leftmost() = header_;
        root() = nullptr;
        rightmost() = header_;
        node_count_ = 0;
    }
}

/**
 * @brief 查找键值为 k 的节点
 * @param key 要查找的键值
 * @return 指向key的iterator
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
find(const key_type& key){
    auto y = header_;
    auto x = root();
    while(x != nullptr){
        if(!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value_field), key)){
            // key 小于等于 x 键值, 向左走
            y = x, x = x->left;
        }else{
            // key 大于 x 的键值, 向右走
            x = x->right;
        }
    }
    iterator j = iterator(y);
    return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

template <typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
find(const key_type& key) const{
    auto y = header_;
    auto x = root();
    while( x != nullptr){
        if(!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value_field), key)){
            // key 小于等于 x 键值, 向左走
            y = x, x = x->left;
        }else{
            // key 大于 x 键值, 向右走
            x = x->right;
        }
    }
    const_iterator j = const_iterator(y);
    return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

/**
 * @brief 键值不小于 key 的第一个位置
 * @param key 键值
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
lower_bound(const key_type& key){
    auto y = header_;
    auto x = root();
    while( x != nullptr ){
        if(!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value_field), key)){
            // key <= x
            y = x, x = x->left;
        }else{
            x = x -> right;
        }
    }
    return iterator(y);
}

template <typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
lower_bound(const key_type& key) const{
    auto y = header_;
    auto x = root();
    while( x != nullptr){
        if( !key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value_field), key)){
            // key <= x
            y = x, x = x->left;
        }else{
            x = x->right;
        }
    }
    return const_iterator(y);
}

/**
 * @brief 键值不小于 key 的最后一个位置
 * @param key 键值
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
upper_bound(const key_type& key){
    auto y = header_;
    auto x = root();
    while(x != nullptr){
        if(key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value_field))){
            // key < x
            y = x, x = x->left;
        }else{
            x = x->right;
        }
    }
    return iterator(y);
}

template <typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
upper_bound(const key_type& key) const{
    auto y = header_;
    auto x = root();
    while( x != nullptr ){
        if(key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value_field))){
            // key < x
            y = x, x = x -> left;
        }else{
            x = x->right;
        }
    }
    return const_iterator(y);
}

/**
 * @brief 交换 rb tree
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
swap(rb_tree& rhs) noexcept{
    if(this != & rhs){
        dhsstl::swap(header_, rhs.header_);
        dhsstl::swap(node_count_, rhs.node_count_);
        dhsstl::swap(key_comp_, rhs.key_comp_);
    }
}

//-----------------------------------------------------------
// helper function

/**
 * @brief 创建一个节点
*/
template<typename T, typename Compare>
template<typename ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
create_node(Args&& ...args){
    auto tmp = node_allocator::allocate(1);
    try{
        data_allocator::constrcut(dhsstl::address_of(tmp->value), dhsstl::forward<Args>(args)...);
        tmp->left = nullptr;
        tmp->right = nullptr;
        tmp->parent = nullptr;
    }catch(...){
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

/**
 * @brief 复制一个节点
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
clone_node(base_ptr x){
    node_ptr tmp = create_node(static_cast<node_ptr>(x)->value_field);
    tmp->color = x -> color;
    tmp->left = nullptr;
    tmp->right = nullptr;
    return tmp;
}

/**
 * @brief 销毁一个节点
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
destory_node(node_ptr p){
    data_allocator::destory(&p->value_field);
    node_allocator::deallocate(p);
}

/**
 * @brief 初始化容器
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
rb_tree_init(){
    header_ = base_allocator::allocate(1);
    header_->color = _Rb_tree_red; // header_ 节点颜色为红, 与root区分
    root() = nullptr;
    leftmost() = header_;
    rightmost() = header_;
    node_count_ = 0;
}

template <typename T, typename Compare>
void rb_tree<T, Compare>::reset(){
    header_ = nullptr;
    node_count_ = 0;
}

template<typename T, typename Compare>
dhsstl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key){
    auto x = root();
    auto y = header_;
    bool add_to_left = true;
    while(x != nullptr){
        y = x;
        add_to_left = key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value_field));
        x = add_to_left ? x->left : x->right;
    }
    return dhsstl::make_pair(y, add_to_left);
}

/**
 * @brief get_insert_unique_pos
 * @param key 键值参数
 * @return 返回一个pair类型, 第一个值为一个pair, 表示插入点的父节点和一个 bool 表示是否在左边插入
 *         第二个值为一个bool, 表示是否插入成功
*/
template <typename T, typename Compare>
dhsstl::pair<dhsstl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key){
    auto x = root(); 
    auto y = header_;
    bool add_to_left = true; // 树为空时也在 header_ 左边插入
    while(x != nullptr){
        y = x;
        add_to_left = key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value_field));
        x = add_to_left ? x->left : x->right;
    }
    iterator j = iterator(y);   // 此时 y 为插入点的父节点
    if(add_to_left){
        if(y == header_ || j = begin()){
            // 如果树为空树或者插入点在最左节点处, 肯定可以插入新的节点
            return dhsstl::make_pair(dhsstl::make_pair(y, true), true);
        }else{
            --j; // 否则, 如果存在重复节点, 那么 --j 就是重复的值
        }
    }
    if(key_comp_(value_traits::get_key(*j), key)){
        // 表明新节点没有重复
        return dhsstl::make_pair(dhsstl::make_pair(y, add_to_left), true);
    }
    // 进行至此, 表示新节点与现有节点键值重复
    return dhsstl::make_pair(dhsstl::make_pair(y, add_to_left), false);
}

/**
 * @brief insert_value_at 函数
 * @param key x 为插入的父节点
 * @param value 要插入的值
 * @param add_to_left 表示是否在左边插入
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_value_at(base_ptr x, const value_type& value, bool add_to_left){
    node_ptr node = create_node(value);
    node->parent = x;
    auto base_node = static_cast<base_ptr>(node);
    if( x == header_ ){
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }else if(add_to_left){
        x->left = base_node;
        if(leftmost() == x)
            leftmost() = base_node;
    }else{
        x->right = base_node;
        if(rightmost() == x)
            rightmost() = base_node;
    }
    _Rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/**
 * @brief 在 x 节点处插入新的节点
 * @param x 插入的父节点
 * @param node 要插入的节点
 * @param add_to_left 表示是否要在左边插入
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_node_at(base_ptr x, node_ptr node, bool add_to_left){
    node->parent = x;
    auto base_node = static_cast<base_ptr>(node);
    if(x == header_){
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }else if(add_to_left){
        x->left = base_node;
        if(leftmost() == x)
            leftmost() = base_node;
    }else{
        x->right = base_node;
        if(rightmost() == x)
            rightmost() = base_node;
    }
    _Rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/**
 * @brief 插入元素, 键值允许重复, 使用hint
 * @param hint iterator迭代器
 * @param key  键值
 * @param node node_ptr
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi_use_hint(iterator hint, key_type key, node_ptr node){
    // 在 hint 附近寻找可以插入的位置
    auto np = hint.node;
    auto before = hint;
    --before;
    auto bnp = before.node;
    if(!key_comp_(key, value_traits::get_key(*before)) &&
       !key_comp_(value_traits::get_key(*hint), key)){
        // before <= node <= hint
        if(bnp->right == nullptr){
            return insert_node_at(bnp, node, false);
        }else if(np->left == nullptr){
            return insert_node_at(np, node, true);
        }
    }
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

/**
 * @brief 插入元素. 键值不允许重复, 使用 hint
 * @param hint iterator迭代器
 * @param key  键值
 * @param node node_ptr
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_unique_use_hint(iterator hint, key_type key, node_ptr node){
    // 在 hint 附近云找可以插入的元素
    auto np = hint.node;
    auto before = hint;
    --before;
    auto bnp = before.node;
    if(key_comp_(value_traits::get_key(*before), key) &&
       key_comp_(key, value_traits::get_key(*hint))){
        // before < node < hint
        if(bnp-> right == nullptr){
            return insert_node_at(bnp, node, false);
        }else if(np->left == nullptr){
            return insert_node_at(np, node, true);
        }
    }
    auto pos = get_insert_unique_pos(key);
    if(!pos.second){
        destory_node(node);
        return pos.first.first;
    }
    return insert_node_at(pos.first.first, node, pos.first.second);
}

/**
 * @brief copy_from 函数: 递归复制一棵树, 节点从 x 开始, p 为 x 的父节点
*/
template <typename T, typename Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p){
    auto top = clone_node(x);
    top->parent = p;
    try
    {
        if(x->right)
            top->right = copy_from(x->right, top);
        p = top;
        x = x->left;
        while(x != nullptr){
            auto y = clone_node(x);
            p->left = y;
            y->parent = p;
            if(x->right)
                y->right = copy_from(x->right, y);
            p = y;
            x = x->left;
        }
    }
    catch(...)
    {
        erase_since(top);
        throw;
    }
    return top;
}

/**
 * @brief erase_since 函数 从 x 节点开始删除该节点及其子树
*/
template <typename T, typename Compare>
void rb_tree<T, Compare>::
erase_since(base_ptr x){
    while(x != nullptr){
        erase_since(x->right);
        auto y = x->left;
        destory_node(static_cast<node_ptr>(x));
        x = y;
    }
}

// 重载比较操作符
template <typename T, typename Compare>
bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return lhs.size() == rhs.size() && dhsstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return dhsstl::lexicograhical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Compare>
bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return !(lhs == rhs);
}

template <typename T, typename Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return rhs > lhs;
}

template <typename T, typename Compare>
bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return !(rhs < lhs);
}

template <typename T, typename Compare>
bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs){
    return !(lhs < rhs);
}

// 重载 dhsstl 的 swap
template <typename T, typename Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept{
    lhs.swap(rhs);
}

} // namespace dhsstl

#endif // !DHSTINYSTL_RB_TREE_H_