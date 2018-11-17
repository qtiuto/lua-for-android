//
// Created by Karven on 2018/8/23.
//

#ifndef LUADROID_VECTOR_H
#define LUADROID_VECTOR_H

#include <cstdlib>
#include <new>
#include "macros.h"

template<typename _Tp,uint cacheCount=4>
class Vector {
public:
    typedef uint size_type;// int type is enough since the most usage is for arg passage
private:
    typedef _Tp *iterator;
    _Tp *array = nullptr;
    size_type _size = 0;
    size_type _capacity = 0;
    uint8_t cache[cacheCount* sizeof(_Tp)];//use cache to improve performance and avoid memory fragments

    inline bool isUsingCache(){
        return intptr_t (array)==intptr_t (cache);
    }

    inline void free(_Tp* v){
        v->~_Tp();
    }

    void deallocate() const { operator delete[]((void*)array); }

    inline void release() {
        if (!isUsingCache()){
            _Tp* soon_to_be_end = end();
            while (array != soon_to_be_end)
                free(--soon_to_be_end);
            deallocate();
        }
    }

    inline void checkIndex(size_type index) const {
#ifndef NDEBUG
        if (unlikely(index < 0 || index >= _size)) {
            LOGE("index=%d out of bound=%d", index, _size);
            abort();
        }
#endif
    }

    static inline void arrayMove(_Tp *from, _Tp *to, size_type count) {
        while (count > 0) {
            --count;
            new (&to[count])_Tp(std::move(from[count]));
        }
    }

    static inline void arrayCopy(const _Tp *from, _Tp *to, size_type count) {
        while (count > 0) {
            --count;
            new (&to[count])_Tp(from[count]);
        }
    }

    static inline size_type binaryCeil(size_type i) {
        i |= (i >> 1);
        i |= (i >> 2);
        i |= (i >> 4);
        i |= (i >> 8);
        i |= (i >> 16);
        return i + 1;
    }


public:
    Vector() {}

    Vector(size_type capacity) : _size(0), _capacity(capacity) {
        array = (_Tp*)new uint8_t[_capacity*sizeof(_Tp)];
    }

    Vector(std::initializer_list<_Tp>&& list) {
        reserve((size_type)list.size());
        _size =(size_type) list.size();
        arrayMove(const_cast<_Tp*>(list.begin()),array,_size);
    }

    Vector(Vector<_Tp> &&other) : array(other.array), _size(other._size), _capacity(other._capacity) {
        if(other.isUsingCache()){
            array=cache;
            arrayMove(other.cache,cache,_size);
        }
        other.array = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    Vector(const Vector<_Tp> &other) {
        reserve(other._size);
        _size = other._size;
        arrayCopy(other.array,array , _size);
    }

    iterator begin() const {
        return array;
    }

    iterator end() const {
        return array + _size;
    }

    _Tp &at(size_type index) const {
        checkIndex(index);
        return array[index];
    }

    _Tp &operator[](size_type index) const {
        return at(index);
    }


    Vector &operator=(Vector<_Tp> &&other) {
        release();
        _size = other._size;
        array = other.array;
        _capacity = other._capacity;
        if(other.isUsingCache()){
            array=cache;
            arrayMove(other.cache,cache,_size);
        }
        other.array = nullptr;
        other._size = 0;
        other._capacity = 0;
        return *this;
    }

    Vector &operator=(const Vector<_Tp> &other)= delete;

    void push_back(_Tp &&val) {
        reserve(_size + 1);
        new (&array[_size++])_Tp(std::move(val));
    }

    void push_back(const _Tp &val) {
        reserve(_size + 1);
        new (&array[_size++])_Tp(val);
    }

    template<typename... Args>
    void emplace_back(Args &&... args) {
        reserve(_size + 1);
        new((void *) &array[_size++])_Tp(std::forward<Args>(args)...);
    }

    void eraseAt(size_type index) {
        checkIndex(index);
        arrayMove(array + index + 1, array + index, _size - index - 1);
        --_size;//move dosen't seem to need free any more.
    }

    void erase(iterator iterator) {
        eraseAt(static_cast<size_type>(iterator - begin()));
    }
    size_type indexOf(_Tp& value){
        for(size_type i=0;i<=_size;++i){
            if(std::equal_to<_Tp>()(value,array[i])){
                return i;
            }
        }
        return size_type (-1);
    }
    void erase(_Tp& value){
        size_type index = indexOf(value);
        if(index!=-1)eraseAt(index);
    }

    void reserve(size_type len) {
        if (len <= _capacity) return;
        _capacity = binaryCeil(len);
        if (_capacity <= cacheCount) {
            array = (_Tp*)cache;
        } else {
            _Tp *newArr =(_Tp*) new uint8_t[_capacity* sizeof(_Tp)];
            arrayMove(array, newArr, _size);
            if(!isUsingCache())deallocate();//should be save to free moved memory
            array = newArr;
        }
    }

    void clear() {
        _size = 0;
    }

    const size_type size() const noexcept {
        return _size;
    }

    ~Vector() {
        release();
    }
};

#endif //LUADROID_VECTOR_H
