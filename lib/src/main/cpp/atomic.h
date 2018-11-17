

#ifndef LUADROID_ATOMIC_H
#define LUADROID_ATOMIC_H

#include "stdint.h"
#include "locale.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef _Atomic(bool) at_flag;

void at_flag_clear(volatile at_flag* flag);

bool at_flag_test_and_set(volatile at_flag* flag);

#ifdef __cplusplus
}
#endif

#endif //LUADROID_ATOMIC_H
