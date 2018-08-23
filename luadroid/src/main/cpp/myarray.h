

#ifndef LUADROID_ARRAY_H
#define LUADROID_ARRAY_H

#include <cstring>
#include <vector>
#include "common.h"

template<typename _Tp, typename size_type=uint32_t>
class Array {
    typedef _Tp *iterator;
    typedef const _Tp *const_iterator;
    _Tp *array = nullptr;
    size_type _size;
public:
    Array() : _size(0) {}

    Array(size_type size) : _size(size) {
        array = new _Tp[size];
        //array=new _Tp[size];
    }

    Array(Vector<_Tp> &&list):Array(list.size()) {
        auto iter = array;
        for (auto iterator = list.begin(); iterator != list.end(); ++iterator) {
            *iter = std::move(*iterator);
            ++iter;
        }
    }

    Array(const Vector<_Tp> &list):Array(list.size()) {
        auto iter = array;
        for (_Tp v:list) {
            *iter = v;
            ++iter;
        }
    }

    Array(Array<_Tp> &&other) : array(other.array), _size(other._size) {
        other.array = nullptr;
        other._size = 0;
    }

    Array(const Array<_Tp> &other) : _size(other._size) {
        _size = other._size;
        array = new _Tp[_size];
        for (size_type i = _size - 1; i != -1; --i) {
            array[i] = other.array[i];
        }
    }

    const_iterator cbegin() const {
        return array;
    }

    const_iterator cend() const {
        return array + _size;
    }

    iterator begin() const {
        return array;
    }

    iterator end() const {
        return array + _size;
    }

    _Tp &at(size_type index) const {
        if (index < 0 || index >= _size) {
            LOGE("index=%u out of bound=%u", (uint32_t)index,(uint32_t)_size);
            abort();
        }
        return array[index];
    }

    _Tp &operator[](size_type index) const {
        return at(index);
    }

    Array &operator=(Array<_Tp> &&other) {
        release();
        _size = other._size;
        array = other.array;
        other.array = nullptr;
        other._size = 0;
        return *this;
    }

    /*Array& operator=(const Array<_Tp>& other){
        release();
        _size=other._size;
        array=al.allocate(_size);
        for (size_type i=_size-1;i!=-1;--i) {
            array[i]=other.array[i];
        }
        return *this;
    }*/

    const size_type size() const noexcept {
        return _size;
    }

private:
    inline void release() {
        delete[] array;
    }

public:
    ~Array() {
        release();
    }
};

#endif //LUADROID_ARRAY_H
