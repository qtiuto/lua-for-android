//
// Created by Karven on 2018/8/23.
//

#ifndef LUADROID_BASE_FUNC_H
#define LUADROID_BASE_FUNC_H
struct BaseFunction {
    int javaRefCount = 0;

    virtual bool isLocal()=0;

    virtual ~BaseFunction() {};
};
#endif //LUADROID_BASE_FUNC_H
