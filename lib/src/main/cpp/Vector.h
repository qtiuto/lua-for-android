//
// Created by Karven on 2018/8/23.
//

#ifndef LUADROID_VECTOR_H
#define LUADROID_VECTOR_H

#include <cstdlib>
#include <algorithm>
#include "macros.h"

template<typename _Tp,int cacheCount=4>
class Vector {
    typedef int size_type;// int type is enough since the most usage is for arg passage
    typedef _Tp *iterator;
    _Tp *array = nullptr;
    size_type _size = 0;
    size_type _capacity = 0;
    _Tp cache[cacheCount];//use cache to improve performance and avoid memory fragments

    inline bool isUsingCache(){
        return array==cache;
    }

    inline void release() {
        if (array != cache){
            delete[] array;
        }
    }

    inline void checkIndex(int index) const {
        if (unlikely(index < 0 || index >= _size)) {
            LOGE("index=%d out of bound=%d", index, _size);
            abort();
        }
    }

    static inline void arrayMove(_Tp *from, _Tp *to, size_type count) {
        while (count > 0) {
            --count;
            to[count] = std::move(from[count]);
        }
    }

    static inline void arrayCopy(const _Tp *from, _Tp *to, size_type count) {
        while (count > 0) {
            --count;
            to[count] = from[count];
        }
    }

    static inline int binaryCeil(int i) {
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
        array = new _Tp[_capacity];
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
        array[_size++] = std::move(val);
    }

    void push_back(const _Tp &val) {
        reserve(_size + 1);
        array[_size++] = val;
    }

    template<typename... Args>
    void emplace_back(Args &&... args) {
        reserve(_size + 1);
        new((void *) &array[_size++])_Tp(std::forward<Args>(args)...);
    }

    void erase(size_type index) {
        checkIndex(index);
        arrayMove(array + index + 1, array + index, _size - index - 1);
        --_size;
    }

    void erase(iterator iterator) {
        erase(iterator - begin());
    }
    int indexOf(_Tp& value){
        for(int i=0;i<=_size;++i){
            if(std::equal_to<_Tp>()(value,array[i])){
                return i;
            }
        }
        return -1;
    }
    void erase(_Tp& value){
        int index = indexOf(value);
        if(index!=-1)erase(index);
    }

    void reserve(size_type len) {
        if (len <= _capacity) return;
        _capacity = binaryCeil(len);
        if (_capacity <= cacheCount) {
            array = cache;
        } else {
            _Tp *newArr = new _Tp[_capacity];
            arrayMove(array, newArr, _size);
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
