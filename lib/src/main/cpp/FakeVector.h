

#ifndef LUADROID_FAKEVECTOR_H
#define LUADROID_FAKEVECTOR_H

#include "Vector.h"
template<typename _Tp,int cacheCount=4>
class FakeVector {
    typedef Vector<_Tp,cacheCount> TrueType;
    typedef typename  TrueType::size_type size_type;
    _Tp *array;
    size_type _size = 0;
    size_type _capacity = 0;
public:
    FakeVector(_Tp* array,size_type capacity):array(array),_capacity(capacity){}

    inline operator  TrueType &(){
        return asVector();
    }

    inline TrueType & asVector() {
        return *reinterpret_cast< TrueType *>(this);
    }

    void release(){
        for (int i = 0; i < _size; ++i) {
            forceRelease(array[i]);
        }
    }
};
#endif //LUADROID_FAKEVECTOR_H
