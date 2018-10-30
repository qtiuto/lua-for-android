

#ifndef HOOKMANAGER_FAKE_DLFCN_H
#define HOOKMANAGER_FAKE_DLFCN_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
void *fake_dlopen(const char *libpath, int flags);
int fake_dlclose(void* handle);
void *fake_dlsym(void *handle, const char *name);
#ifdef __cplusplus
};
#endif
#endif //HOOKMANAGER_FAKE_DLFCN_H
