#ifndef _REF_H_
#define _REF_H_

#include <atomic>
#include <functional>
#include <memory>

namespace dhsstl{
    namespace Detail{
        template<typename T>
        struct _default_delete{
            void operator()(T* ptr){
                if(ptr){
                    delete ptr;
                }
            }
        };


        template<typename T>
        struct ref_t
        {
            using deleter_type = std::function <void(T*)>;

            std::atomic<size_t> ncount_;
            std::atomic<size_t> wcount_;
            T* data_;
            deleter_type deleter_;

            explicit ref_t(T* p = nullptr, deleter_type pfunc = deleter_type(_default_delete<T>()))
                : ncount_(0), wcount_(0) ,data_(p), deleter_(pfunc){
                    if(data_){
                        ncount_ = 1;
                    }
            }

            ref_t(const ref_t&) = delete;
            ref_t& operator = (const ref_t&) = delete;

            ~ref_t(){
                --ncount_;
                if(ncount_ == 0){
                    deleter_(data_);
                }
            }

            size_t count() const { return ncount_.load(); }
            T* get_data()  const { return data_; }

            ref_t& operator++(){
                ++ncount_;
                return *this;
            }

            ref_t operator++(int){
                auto t = *this;
                ++*this;
                return t;
            }

            ref_t& operator --(){
                --ncount_;
                return *this;
            }

            ref_t operator--(int){
                auto t = *this;
                --*this;
                return t;
            }
        };

        template<typename T>
        bool operator==(const ref_t<T>& lhs, const ref_t<T>& rhs){
            return lhs.get_data() == rhs.get_data();
        }

        template<typename T> 
        bool operator!=(const ref_t<T>& lhs, const ref_t<T>& rhs){
            return !(lhs == rhs);
        }

    }
}

#endif