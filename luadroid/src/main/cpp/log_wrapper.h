//
// Created by Karven on 2018/8/2.
//

#ifndef LUADROID_LOGGERWRAPPER_H
#define LUADROID_LOGGERWRAPPER_H

#include <functional>

extern void requireLogger(std::function<void(const char *, bool)> logger);

extern void dropLogger();

#endif //LUADROID_LOGGERWRAPPER_H
