

#ifndef LUADROID_ARRAY_H
#define LUADROID_ARRAY_H

#include <cstring>
#include "common.h"
#include "Vector.h"

template<typename _Tp, typename size_type=uint32_t>
class Array {
    typedef _Tp *iterator;
    typedef const _Tp *const_iterator;
    _Tp *array = nullptr;
    size_type _size;

    _Tp* allocate(int size){
        return  new _Tp[size];
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

public:
    Array() : _size(0) {}

    Array(size_type size) : _size(size) {
        array = allocate(size);
    }

    Array(Vector<_Tp> &&list):Array(list.size()) {
        arrayMove(list.begin(),array,list.size());

    }

    Array(const Vector<_Tp> &list):Array(list.size()) {
        arrayCopy(list.begin(),array,list.size());
    }

    Array(Array<_Tp> &&other) : array(other.array), _size(other._size){
        other.array = nullptr;
        other._size = 0;
    }

    Array(const Array<_Tp> &other) : _size(other._size) {
        _size = other._size;
        array = allocate(_size);
        arrayCopy(other.begin(),array,other.size());
    }

    iterator begin() const {
        return array;
    }

    iterator end() const {
        return array + _size;
    }

    _Tp &at(size_type index) const {
#ifndef NDEBUG
        if (unlikely(index < 0 || index >= _size)) {
            LOGE("index=%u out of bound=%u", (uint32_t)index,(uint32_t)_size);
            abort();
        }
#endif
        return array[index];
    }

    _Tp &operator[](size_type index) const {
        return at(index);
    }

    Array &operator=(Array<_Tp> &&other) {
        this->~Array();
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

public:
    ~Array() {
        delete [] array;
    }
};

#endif //LUADROID_ARRAY_H
