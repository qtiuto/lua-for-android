
#include "stdatomic.h"
#include "atomic.h"

void at_flag_clear(volatile at_flag* flag){
    atomic_flag_clear((atomic_flag *) flag);
}
bool at_flag_test_and_set(volatile at_flag* flag){
    return atomic_flag_test_and_set((atomic_flag *) flag);
}


