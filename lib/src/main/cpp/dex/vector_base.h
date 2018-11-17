

#ifndef LUADROID_VECTOR_BASE_H
#define LUADROID_VECTOR_BASE_H

#include "stdint.h"
#include <new>

template<class _Tp>
class vector_base {
public:
    typedef size_t size_type;
    typedef _Tp value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef _Tp *pointer;
    typedef const _Tp *const_pointer;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

protected:
    pointer __begin_;
    pointer __end_;
    pointer __end_cap_;

    pointer &__end_cap() { return __end_cap_; }

    const pointer &__end_cap() const { return __end_cap_; }


    void destruct_at_end(pointer __new_last) noexcept {
        pointer soon_to_be_end = __end_;
        while (__new_last != soon_to_be_end)
            (--soon_to_be_end)->~_Tp();
        __end_ = __new_last;
    }

public:
    vector_base() noexcept:__begin_(nullptr), __end_(nullptr), __end_cap_(nullptr) {}
    
    vector_base(vector_base&& o) noexcept: __begin_(o.__begin_), __end_(o.__end_), __end_cap_(o.__end_cap_) {
        o.__begin_=o.__end_=o.__end_cap_= nullptr;
    }
    
    vector_base(const vector_base& other) {
        copyForm(other);
    };

    ~vector_base(){
        if (__begin_ != nullptr) {
            clear();
            deallocate();
        }
    }

    void clear() noexcept { destruct_at_end(__begin_); }

    size_type capacity() const noexcept { return static_cast<size_type>(__end_cap() - __begin_); }
    
    size_type size() const noexcept { return __end_-__begin_;}
    
    pointer begin() const { return __begin_;}
    
    pointer end() const { return __end_; }
    
    vector_base& operator=(const vector_base& other){
        clean();
        copyForm(other);
        return *this;
    }

    void push_back(_Tp &&val) {
        reserve(size() + 1);
        new (__end_++)_Tp(std::move(val));
    }

    void push_back(const _Tp &val) {
        reserve(size() + 1);
        new (__end_++)_Tp(val);
    }

    _Tp& operator[](size_type idx)const {
        return __begin_[idx];
    }

    _Tp& at(size_type idx) const {
        return __begin_[idx];
    }

    void reserve(size_type len) {
        if (len <= capacity()) return;
        size_type _capacity = binaryCeil(len);
        _Tp *newArr =allocate(_capacity);
        size_type count = size();
        arrayMove(__begin_, newArr, count);
        deallocate();
        __begin_ = newArr;
        __end_=newArr+count;
        __end_cap_=newArr+_capacity;

    }
private:
    static inline size_type binaryCeil(size_type i) {
        i |= (i >> 1);
        i |= (i >> 2);
        i |= (i >> 4);
        i |= (i >> 8);
        i |= (i >> 16);
        return i + 1;
    }
    _Tp* allocate(size_type size){
        return (_Tp*) new uint8_t[size* sizeof(_Tp)];
    }

    void deallocate() const { operator delete[](__begin_); }

    void clean() {
        if (__begin_ != nullptr) {
            clear();
            deallocate();
            __begin_ = __end_ = __end_cap() = nullptr;
        }
    }
    static inline void arrayMove(_Tp *from, _Tp *to, size_type count) {
        while (count > 0) {
            --count;
            new (&to[count])_Tp(std::move(from[count]));
        }
    }

    void copyForm(const vector_base& other){
        size_type sz = other.size();
        __begin_=allocate(sz);
        __end_=__end_cap_=__begin_+sz;
        size_type i=0;
        for(auto&& v:other){
            new(__begin_+(i++))_Tp(v);
        }
    }

};

#endif //LUADROID_VECTOR_BASE_H
