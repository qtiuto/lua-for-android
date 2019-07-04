

#ifndef LUADROID_STRING_H
#define LUADROID_STRING_H

#include <stddef.h>
#include "initializer_list"
#include "string.h"
#include <stdlib.h>
namespace util{
    uint32_t Hash32(const char* s, size_t len);
}
namespace std{
    class MString
    {
    public:
        typedef uint32_t size_type;
        typedef char value_type ;
        typedef char& reference;
        typedef const char& const_reference;
        typedef char* pointer;
        typedef const char* const_pointer;
    private:
        static const size_type __short_mask = 0x01;
        static const size_type __long_mask  = 0x1ul;
        struct __long
        {
            size_type __cap_;
            size_type __size_;
            pointer   __data_;
        };

        enum {__min_cap = (sizeof(__long) - 1) / sizeof(value_type)
        };
        struct __short {
            union {
                unsigned char __size_;
                value_type __lx;
            };
            value_type __data_[__min_cap];
        };
        union __ulx{__long __lx; __short __lxx;};

        enum {__n_words = sizeof(__ulx) / sizeof(size_type)};
        struct __raw
        {
            size_type __words[__n_words];
        };

        struct __rep
        {
            union
            {
                __long  __l;
                __short __s;
                __raw   __r;
            };
        } __r_;
        bool __is_long() const noexcept
        {return bool(__r_.__s.__size_ & __short_mask);}
        void __zero() noexcept
        {
            size_type (&__a)[__n_words] = __r_.__r.__words;
            for (unsigned __i = 0; __i < __n_words; ++__i)
                __a[__i] = 0;
        }
        void __set_short_size(size_type __s) noexcept
        {__r_.__s.__size_ = (unsigned char)(__s << 1);}

        size_type __get_short_size() const noexcept
        {return __r_.__s.__size_ >> 1;}

        void __set_long_size(size_type __s) noexcept
        {__r_.__l.__size_ = __s;}

        size_type __get_long_size() const noexcept
        {return __r_.__l.__size_;}

        void __set_size(size_type __s) noexcept
        {if (__is_long()) __set_long_size(__s); else __set_short_size(__s);}

        void __set_long_cap(size_type __s) noexcept
        {__r_.__l.__cap_  = __long_mask | __s;}

        size_type __get_long_cap() const noexcept
        {return __r_.__l.__cap_ & size_type(~__long_mask);}


        void __set_long_pointer(pointer __p) noexcept
        {__r_.__l.__data_ = __p;}

        pointer __get_long_pointer() noexcept
        {return __r_.__l.__data_;}

        const_pointer __get_long_pointer() const noexcept
        {return __r_.__l.__data_;}

        pointer __get_short_pointer() noexcept
        {return &__r_.__s.__data_[0];}

        const_pointer __get_short_pointer() const noexcept
        {return &__r_.__s.__data_[0];}

        pointer __get_pointer() noexcept
        {return __is_long() ? __get_long_pointer() : __get_short_pointer();}
        const_pointer __get_pointer() const noexcept
        {return __is_long() ? __get_long_pointer() : __get_short_pointer();}
        template <size_type __a> static
        size_type __align_it(size_type __s) noexcept
        {return (__s + (__a-1)) & ~(__a-1);}
        enum {__alignment = 16};
        size_type __recommend(size_type __s) noexcept
        {
            if (__s < __min_cap) return static_cast<size_type>(__min_cap) - 1;
            size_type __guess = __align_it<sizeof(value_type) < __alignment ?
                                           __alignment/sizeof(value_type) : 1 > (__s+1) - 1;
            if (__guess == __min_cap) ++__guess;
            return __guess;
        }
        void __init(const value_type* __pointer,size_type __sz){
            pointer __p;
            if (__sz < __min_cap)
            {
                __set_short_size(__sz);
                __p = __get_short_pointer();
            }
            else
            {
                size_type __cap = __recommend(__sz);
                __p = new value_type[ __cap+1];
                __set_long_pointer(__p);
                __set_long_cap(__cap+1);
                __set_long_size(__sz);
            }
            memcpy(__p,__pointer,__sz);
            __p[__sz]=0;
        }
        void __init(size_type __n, value_type __c)
        {
            pointer __p;
            if (__n < __min_cap)
            {
                __set_short_size(__n);
                __p = __get_short_pointer();
            }
            else
            {
                size_type __cap = __recommend(__n);
                __p = new value_type[ __cap+1];
                __set_long_pointer(__p);
                __set_long_cap(__cap+1);
                __set_long_size(__n);
            }
            memset(__p,__c,__n);
            __p[__n]=0;
        }
        void __clear_and_shrink(){
            clear();
            if(__is_long())
            {
                delete [] __get_long_pointer();
                __set_long_cap(0);
                __set_short_size(0);
            }
        }
        void __grow_by_and_replace
                (size_type __old_cap, size_type __delta_cap, size_type __old_sz,
                 size_type __n_copy,  size_type __n_del,     size_type __n_add, const value_type* __p_new_stuff)
        {
            pointer __old_p = __get_pointer();
            size_type __cap = __recommend(std::max(__old_cap + __delta_cap, 2 * __old_cap)) ;

            pointer __p = new value_type[ __cap+1];
            if (__n_copy != 0)
                memcpy(__p, __old_p, __n_copy);
            if (__n_add != 0)
                memcpy(__p + __n_copy, __p_new_stuff, __n_add);
            size_type __sec_cp_sz = __old_sz - __n_del - __n_copy;
            if (__sec_cp_sz != 0)
                memcpy(__p + __n_copy + __n_add, __old_p + __n_copy + __n_del, __sec_cp_sz);
            if (__old_cap+1 != __min_cap)
                delete[] __old_p;
            __set_long_pointer(__p);
            __set_long_cap(__cap+1);
            __old_sz = __n_copy + __n_add + __sec_cp_sz;
            __set_long_size(__old_sz);
            __p[__old_sz]= value_type();
        }
        void __grow_by(size_type __old_cap, size_type __delta_cap, size_type __old_sz,
                       size_type __n_copy,  size_type __n_del,     size_type __n_add=0)
        {
            pointer __old_p = __get_pointer();
            size_type __cap =__recommend(std::max(__old_cap + __delta_cap, 2 * __old_cap));
            pointer __p = new value_type[ __cap+1];
            if (__n_copy != 0)
                memcpy(__p, __old_p, __n_copy);
            size_type __sec_cp_sz = __old_sz - __n_del - __n_copy;
            if (__sec_cp_sz != 0)
                memcpy(__p + __n_copy + __n_add, __old_p + __n_copy + __n_del, __sec_cp_sz);
            if (__old_cap+1 != __min_cap)
                delete [] __old_p;
            __set_long_pointer(__p);
            __set_long_cap(__cap+1);
        }
    public:

        static const size_type npos = -1;
        MString()noexcept{__zero();}
        MString(const MString& str)
        {
            if (!str.__is_long())
                __r_.__r = str.__r_.__r;
            else
                __init(str.__get_long_pointer(), str.__get_long_size());

        }
        MString(MString&& str)noexcept: __r_(std::move(str.__r_))
        {
            str.__zero();
        }
        MString(const MString& str, size_type pos){
            size_type __str_sz = str.size();
            __init(str.data() + pos, __str_sz - pos);
        }
        MString(const MString& str, size_type pos, size_type n){
            __init(str.data() + pos, n);
        }
        MString(const value_type* s){
            __init(s,strlen(s));
        }
        MString(const value_type* s, size_type n){
            __init(s,n);
        }
        MString(size_type n, value_type c){
            __init(n,c);
        }
        MString(std::initializer_list<value_type> list){
            __init(list.begin(),list.size());
        }

        ~MString(){
            if (__is_long())
                delete [] __get_long_pointer();
        }



        MString& operator=(const MString& str){
            return assign(str.data(),str.size());
        }
        MString& operator=(MString&& str)noexcept{
            __clear_and_shrink();
            __r_=str.__r_;
            str.__zero();
            return *this;
        }
        MString& operator=(const value_type* s){
            return assign(s);
        }
        MString& operator=(value_type c){
            return assign(&c,1);
        }
        MString& operator=(std::initializer_list<value_type> list){
            return assign(list.begin(),list.size());
        }


        size_type size() const noexcept{
            return __is_long() ? __get_long_size() : __get_short_size();
        };
        size_type length() const noexcept{
            return size();
        };
        size_type capacity() const noexcept{
            return (__is_long() ? __get_long_cap()
                                : static_cast<size_type>(__min_cap)) - 1;}

        void resize(size_type n, value_type c){
            size_type __sz = size();
            if (n > __sz)
                append(n - __sz, c);
            else{
                if (__is_long())
                {
                    *(__get_long_pointer() + n)= value_type();
                    __set_long_size(n);
                }
                else
                {
                    *(__get_short_pointer() + n)= value_type();
                    __set_short_size(n);
                }
            }
        }
        void resize(size_type n){
            resize(n,0);
        }

        void reserve(size_type __res_arg = 0){
            size_type __cap = capacity();
            size_type __sz = size();
            __res_arg = std::max(__res_arg, __sz);
            __res_arg = __recommend(__res_arg);
            if (__res_arg != __cap)
            {
                pointer __new_data, __p;
                bool __was_long, __now_long;
                if (__res_arg == __min_cap - 1)
                {
                    __was_long = true;
                    __now_long = false;
                    __new_data = __get_short_pointer();
                    __p = __get_long_pointer();
                }
                else
                {
                    if (__res_arg > __cap)
                        __new_data = new value_type[ __res_arg+1];
                    else
                    {
                        __new_data = new value_type[__res_arg+1];
                        if (__new_data == nullptr)
                            return;
                    }
                    __now_long = true;
                    __was_long = __is_long();
                    __p = __get_pointer();
                }
                memcpy(__new_data, __p, size()+1);
                if (__was_long)
                    delete[] __p;
                if (__now_long)
                {
                    __set_long_cap(__res_arg+1);
                    __set_long_size(__sz);
                    __set_long_pointer(__new_data);
                }
                else
                    __set_short_size(__sz);
            }
        }
        void shrink_to_fit(){reserve();}

        void clear() noexcept {
            if (__is_long()) {
                *__get_long_pointer()= value_type();
                __set_long_size(0);
            } else {
                *__get_short_pointer()= value_type();
                __set_short_size(0);
            }
        }
        bool empty() const noexcept{return size() == 0;}

        const_reference operator[](size_type pos) const{
            return data()[pos];
        };
        reference operator[](size_type pos){
            return data()[pos];
        };

        const_reference at(size_type n) const{
            return data()[n];
        };
        reference       at(size_type n){
            return data()[n];
        };

        MString& operator+=(const MString& str){
            return append(str);
        }
        MString& operator+=(const value_type* s){
            return append(s);
        }
        MString& operator+=(value_type c){
            push_back(c);
            return *this;
        }
        MString& operator+=(std::initializer_list<value_type> list){
            return append(list.begin(),list.size());
        }

        MString& append(const MString& str){
            return append(str.data(),str.length());
        }
        MString& append(const MString& str, size_type pos, size_type n=npos){
            size_type __sz = str.size();
            if (pos > __sz)
                abort();
            return append(str.data() + pos, std::min(n, __sz - pos));
        }
        MString& append(const value_type* __s, size_type __n){
            size_type __cap = capacity();
            size_type __sz = size();
            if (__cap - __sz >= __n)
            {
                if (__n)
                {
                    value_type* __p = __get_pointer();
                    memcpy(__p + __sz, __s, __n);
                    __sz += __n;
                    __set_size(__sz);
                    __p[__sz]= value_type();
                }
            }
            else
                __grow_by_and_replace(__cap, __sz + __n - __cap, __sz, __sz, 0, __n, __s);
            return *this;
        }
        MString& append(const value_type* s){
            return append(s,strlen(s));
        }
        MString& append(size_type __n, value_type __c){
            if (__n)
            {
                size_type __cap = capacity();
                size_type __sz = size();
                if (__cap - __sz < __n)
                    __grow_by(__cap, __sz + __n - __cap, __sz, __sz, 0);
                pointer __p = __get_pointer();
                memset(__p + __sz, __c, __n);
                __sz += __n;
                __set_size(__sz);
                __p[__sz]=value_type();
            }
            return *this;
        }
        MString& append(std::initializer_list<value_type> list){
            return assign(list.begin(),list.size());
        }

        void push_back(value_type __c){
            bool __is_short = !__is_long();
            size_type __cap;
            size_type __sz;
            if (__is_short)
            {
                __cap = __min_cap - 1;
                __sz = __get_short_size();
            }
            else
            {
                __cap = __get_long_cap() - 1;
                __sz = __get_long_size();
            }
            if (__sz == __cap)
            {
                __grow_by(__cap, 1, __sz, __sz, 0);
                __is_short = !__is_long();
            }
            pointer __p;
            if (__is_short)
            {
                __p = __get_short_pointer() + __sz;
                __set_short_size(__sz+1);
            }
            else
            {
                __p = __get_long_pointer() + __sz;
                __set_long_size(__sz+1);
            }
            *__p= __c;
            *++__p= value_type();
        }
        void pop_back(){
            size_type __sz;
            if (__is_long())
            {
                __sz = __get_long_size() - 1;
                __set_long_size(__sz);
                *(__get_long_pointer() + __sz)= value_type();
            }
            else
            {
                __sz = __get_short_size() - 1;
                __set_short_size(__sz);
                *(__get_short_pointer() + __sz)=value_type();
            }
        }
        reference       front(){
            return *data();
        }
        const_reference front() const{
            return *data();
        }
        reference       back(){
            return *(data() + size() - 1);
        }
        const_reference back() const{
            return *(data() + size() - 1);
        }

        MString& assign(const MString& str){
            return assign(str.data(),str.size());
        }
        MString& assign(MString&& str){*this = std::move(str); return *this;}
        MString& assign(const MString& str, size_type pos, size_type n=npos){
            size_type __sz = str.size();
            if (pos > __sz)
                abort();
            return assign(str.data() + pos, std::min(n, __sz - pos));
        }
        MString& assign(const value_type* __s, size_type __n){
            size_type __cap = capacity();
            if (__cap >= __n)
            {
                value_type* __p = __get_pointer();
                memmove(__p, __s, __n);
                __p[__n]= value_type();
                __set_size(__n);
            }
            else
            {
                size_type __sz = size();
                __grow_by_and_replace(__cap, __n - __cap, __sz, 0, __sz, __n, __s);
            }
            return *this;
        }
        MString& assign(const value_type* s){
            return assign(s,strlen(s));
        }
        MString& assign(size_type __n, value_type __c){
            size_type __cap = capacity();
            if (__cap < __n)
            {
                size_type __sz = size();
                __grow_by(__cap, __n - __cap, __sz, 0, __sz);
            }
            value_type* __p = __get_pointer();
            memset(__p,  __c,__n);
            __p[__n]= value_type();
            __set_size(__n);
            return *this;
        }
        MString& assign(std::initializer_list<value_type> list){
            return assign(list.begin(),list.size());
        }

        MString& insert(size_type pos1, const MString& str){
            return insert(pos1,str.data(),str.size());
        }
        MString& insert(size_type pos1, const MString& str,
                        size_type pos2, size_type n=npos){
            size_type __str_sz = str.size();
            if (pos2 > __str_sz)
                abort();
            return insert(pos1,str.data()+pos2,std::min(n,__str_sz-pos2));
        }
        MString& insert(size_type __pos, const value_type* __s, size_type __n){
            size_type __sz = size();
            if (__pos > __sz)
                abort();
            size_type __cap = capacity();
            if (__cap - __sz >= __n)
            {
                if (__n)
                {
                    value_type* __p = __get_pointer();
                    size_type __n_move = __sz - __pos;
                    if (__n_move != 0)
                    {
                        if (__p + __pos <= __s && __s < __p + __sz)
                            __s += __n;
                        memmove(__p + __pos + __n, __p + __pos, __n_move);
                    }
                    memmove(__p + __pos, __s, __n);
                    __sz += __n;
                    __set_size(__sz);
                    __p[__sz]=value_type();
                }
            }
            else
                __grow_by_and_replace(__cap, __sz + __n - __cap, __sz, __pos, 0, __n, __s);
            return *this;
        }
        MString& insert(size_type pos, const value_type* s){
            return insert(pos, s, strlen(s));
        }
        MString& insert(size_type __pos, size_type __n, value_type __c){
            size_type __sz = size();
            if (__pos > __sz)
                abort();
            if (__n)
            {
                size_type __cap = capacity();
                value_type* __p;
                if (__cap - __sz >= __n)
                {
                    __p = __get_pointer();
                    size_type __n_move = __sz - __pos;
                    if (__n_move != 0)
                        memmove(__p + __pos + __n, __p + __pos, __n_move);
                }
                else
                {
                    __grow_by(__cap, __sz + __n - __cap, __sz, __pos, 0, __n);
                    __p = __get_long_pointer();
                }
                memset(__p + __pos, __c, __n);
                __sz += __n;
                __set_size(__sz);
                __p[__sz]= value_type();
            }
            return *this;
        }

        MString& erase(size_type pos = 0, size_type n = npos){
            size_type __sz = size();
            if (pos > __sz)
                abort();
            if (n)
            {
                value_type* __p =__get_pointer();
                n = std::min(n, __sz - pos);
                size_type __n_move = __sz - pos - n;
                if (__n_move != 0)
                    memmove(__p + pos, __p + pos + n, __n_move);
                __sz -= n;
                __set_size(__sz);
                __p[__sz]= value_type();
            }
            return *this;
        }

        MString& replace(size_type pos1, size_type n1, const MString& str){
            return replace(pos1,n1,str.data(),str.size());
        }
        MString& replace(size_type pos1, size_type n1, const MString& str,
                         size_type pos2, size_type n2=npos){
            size_type __str_sz = str.size();
            if (pos2 > __str_sz)
                abort();
            return replace(pos1,n1,str.data()+pos2,std::min(n2,__str_sz - pos2));
        }
        MString& replace(size_type __pos, size_type __n1, const value_type* __s, size_type __n2){
            size_type __sz = size();
            if (__pos > __sz)
                abort();
            __n1 = std::min(__n1, __sz - __pos);
            size_type __cap = capacity();
            if (__cap - __sz + __n1 >= __n2)
            {
                value_type* __p = __get_pointer();
                if (__n1 != __n2)
                {
                    size_type __n_move = __sz - __pos - __n1;
                    if (__n_move != 0)
                    {
                        if (__n1 > __n2)
                        {
                            memmove(__p + __pos, __s, __n2);
                            memmove(__p + __pos + __n2, __p + __pos + __n1, __n_move);
                            goto __finish;
                        }
                        if (__p + __pos < __s && __s < __p + __sz)
                        {
                            if (__p + __pos + __n1 <= __s)
                                __s += __n2 - __n1;
                            else // __p + __pos < __s < __p + __pos + __n1
                            {
                                memmove(__p + __pos, __s, __n1);
                                __pos += __n1;
                                __s += __n2;
                                __n2 -= __n1;
                                __n1 = 0;
                            }
                        }
                        memmove(__p + __pos + __n2, __p + __pos + __n1, __n_move);
                    }
                }
                memmove(__p + __pos, __s, __n2);
                __finish:
// __sz += __n2 - __n1; in this and the below function below can cause unsigned integer overflow,
// but this is a safe operation, so we disable the check.
                __sz += __n2 - __n1;
                __set_size(__sz);
                __p[__sz]=value_type();
            }
            else
                __grow_by_and_replace(__cap, __sz - __n1 + __n2 - __cap, __sz, __pos, __n1, __n2, __s);
            return *this;
        }
        MString& replace(size_type pos, size_type n1, const value_type* s){
            return replace(pos,n1,s,strlen(s));
        }
        MString& replace(size_type __pos, size_type __n1, size_type __n2, value_type __c){
            size_type __sz = size();
            if (__pos > __sz)
                abort();
            __n1 = std::min(__n1, __sz - __pos);
            size_type __cap = capacity();
            value_type* __p;
            if (__cap - __sz + __n1 >= __n2)
            {
                __p = __get_pointer();
                if (__n1 != __n2)
                {
                    size_type __n_move = __sz - __pos - __n1;
                    if (__n_move != 0)
                        memmove(__p + __pos + __n2, __p + __pos + __n1, __n_move);
                }
            }
            else
            {
                __grow_by(__cap, __sz - __n1 + __n2 - __cap, __sz, __pos, __n1, __n2);
                __p = __get_long_pointer();
            }
            memset(__p + __pos, __c, __n2);
            __sz += __n2 - __n1;
            __set_size(__sz);
            __p[__sz]= value_type();
            return *this;
        }


        size_type copy(value_type* __s, size_type __n, size_type __pos = 0) const{
            size_type __sz = size();
            if (__pos > __sz)
                abort();
            size_type __rlen = std::min(__n, __sz - __pos);
            memcpy(__s, data() + __pos, __rlen);
            return __rlen;
        }
        MString substr(size_type pos = 0, size_type n = npos) const{
            return MString(*this, pos, n);
        }

        void swap(MString& str)noexcept {
            auto r=__r_;
            __r_=str.__r_;
            str.__r_=r;
        }

        const value_type* c_str() const noexcept{
            return data();
        }
        const value_type* data() const noexcept{
            return __get_pointer();
        }
        value_type* data()       noexcept{
            return __get_pointer();
        }  // C++17


        size_type find(const MString& str, size_type pos = 0) const noexcept{
            return find(str.data(),pos,str.size());
        }
        size_type find(const value_type* s, size_type pos, size_type n) const noexcept{
            size_type __sz=size();
            const_pointer p=data();
            if (pos > __sz)
                return npos;

            if (n == 0) // There is nothing to search, just return __pos.
                return pos;
            for (const value_type * __ps = p + pos; *__ps ;++__ps)
            {
                if(strncmp(__ps,s,n)==0)
                    return static_cast<size_type >(__ps - p);
            }
            return npos;
        }
        size_type find(const value_type* s, size_type pos = 0) const noexcept{
            return find(s,pos,strlen(s));
        }
        size_type find(value_type c, size_type pos = 0) const noexcept{
            size_type n=size();
            const_pointer s=data();
            if (n < 1)
                return npos;
            if (pos > n)
                pos = n;
            for (const value_type * __ps = s + pos; ;++__ps)
            {
                auto ch=*__ps;
                if (ch== c)
                    return static_cast<size_type >(__ps - s);
                else if(ch==0)break;
            }
            return npos;
        }

        size_type rfind(const MString& str, size_type pos = npos) const noexcept{
            return rfind(str.data(),pos,str.size());
        }
        size_type rfind(const value_type* s, size_type pos, size_type n) const noexcept{
            size_type __sz=size();
            const_pointer p=data();
            if (n == 0)
                return pos;
            if (pos >= __sz)
                pos = __sz-1;
            for (const value_type * __ps = p + pos-n+1; __ps<p ;--__ps)
            {
                if(strncmp(__ps,s,n)==0)
                    return static_cast<size_type >(__ps - p);
            }
            return npos;
        }
        size_type rfind(const value_type* s, size_type pos = npos) const noexcept{
            return rfind(s,pos,strlen(s));
        }

        size_type rfind(value_type c, size_type pos = npos) const noexcept{
            size_type n=size();
            const_pointer s=data();
            if (n == 0)
                return npos;
            if(pos>=n)
                pos = n-1;
            for (const value_type * __ps = s + pos; __ps!=s;--__ps)
            {
                if (*__ps== c)
                    return static_cast<size_type >(__ps - s);
            }
            return npos;
        }

       /* size_type find_first_of(const MString &str, size_type pos = 0) const noexcept;

        size_type find_first_of(const value_type *s, size_type pos, size_type n) const noexcept;

        size_type find_first_of(const value_type *s, size_type pos = 0) const noexcept;

        size_type find_first_of(value_type c, size_type pos = 0) const noexcept;*/

        size_type find_last_of(const MString &str, size_type pos = npos) const noexcept{
            return find_last_of(str.data(),npos,str.size());
        }

        size_type find_last_of(const value_type *s, size_type pos, size_type n) const noexcept{
            size_type __sz=size();
            const_pointer p=data();
            if (n == 0)
                return pos;
            else if(pos>=__sz){
                pos=__sz-1;
            }
            for (const value_type * __ps = p + pos; __ps>=p ;--__ps)
            {
                value_type c=*__ps;
                for (int i = n; i --; ) {
                    if(c==s[i])
                        return static_cast<size_type >(__ps - p);
                }
            }
            return npos;
        }

        size_type find_last_of(const value_type *s, size_type pos = npos) const noexcept{
            return find_last_of(s,pos,strlen(s));
        }

        size_type find_last_of(value_type c, size_type pos = npos) const noexcept{
            return rfind(c,pos);
        }

          /* size_type find_first_not_of(const MString& str, size_type pos = 0) const noexcept;
         size_type find_first_not_of(const value_type* s, size_type pos, size_type n) const noexcept;
         size_type find_first_not_of(const value_type* s, size_type pos = 0) const noexcept;
         size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept;

         size_type find_last_not_of(const MString& str, size_type pos = npos) const noexcept;
         size_type find_last_not_of(const value_type* s, size_type pos, size_type n) const noexcept;
         size_type find_last_not_of(const value_type* s, size_type pos = npos) const noexcept;
         size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept;

         int compare(const MString& str) const noexcept;
         int compare(size_type pos1, size_type n1, const MString& str) const;
         int compare(size_type pos1, size_type n1, const MString& str,
                     size_type pos2, size_type n2=npos) const; // C++14
         int compare(const value_type* s) const noexcept;
         int compare(size_type pos1, size_type n1, const value_type* s) const;
         int compare(size_type pos1, size_type n1, const value_type* s, size_type n2) const;

         bool starts_with(value_type c) const noexcept;                             // C++2a
         bool starts_with(const_pointer s) const;                      // C++2a
         bool ends_with(value_type c) const noexcept;                               // C++2a
         bool ends_with(const_pointer s) const;    */                             // C++2a
    };

    template <>
    struct hash<MString>
    {
        MString::size_type operator()(const MString& val) const noexcept {
            return util::Hash32(val.data(),val.size());
        };
    };

    inline MString operator+(const char c,const MString &f){
        MString tmp;
        tmp.reserve(f.size()+1);
        tmp+=c;
        tmp+=f;
        return tmp;
    }
    inline MString operator+(const char c, MString &&f){
        f.insert(0,&c,1);
        return std::move(f);
    }
    inline MString operator+(const MString &f,const char c){
        MString tmp;
        tmp.reserve(f.size()+1);
        tmp+=f;
        tmp+=c;
        return tmp;
    }
    inline MString operator+(const MString &f,const char* c){
        MString tmp;
        tmp.reserve(f.size()+strlen(c));
        tmp+=f;
        tmp+=c;
        return tmp;
    }
    inline MString operator+(const MString &f,const MString& c){
        MString tmp;
        tmp.reserve(f.size()+c.size());
        tmp+=f;
        tmp+=c;
        return tmp;
    }
    inline MString operator+(MString &&f,const char* c){
        f+=c;
        return std::move(f);
    }
    inline MString operator+(MString &&f,const char c){
        f+=c;
        return std::move(f);
    }
    inline MString operator+(MString &&f,const MString& c){
        f+=c;
        return std::move(f);
    }
    inline MString operator+(const MString &f, MString&& c){
        c.insert(0,f);
        return std::move(c);
    }
    inline MString operator+(const char* c, MString &&f){
        f.insert(0,c,strlen(c));
        return std::move(f);
    }
    inline MString operator+(const char* c,const MString &f){
        MString tmp;
        tmp.reserve(f.size()+strlen(c));
        tmp+=c;
        tmp+=f;
        return tmp;
    }


    inline bool operator==(const char *f, const MString& s){
        return strcmp(f,s.data())==0;
    }
    inline bool operator!=(const char *f, const MString& s){
        return strcmp(f,s.data())!=0;
    }
    inline bool operator==(const MString& s, const char *f){
        return strcmp(f,s.data())==0;
    }
    inline bool operator!=( const MString& s,const char *f){
        return strcmp(f,s.data())!=0;
    }

    inline bool operator==(const MString& f, const MString& s){
        return strcmp(f.data(),s.data())==0;
    }
    inline bool operator!=(const MString& f, const MString& s){
        return strcmp(f.data(),s.data())!=0;
    }

}

#endif //LUADROID_STRING_H
