

#include "log_wrapper.h"
#include "SpinLock.h"
#include "common.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <poll.h>
#include <time.h>
#include <thread>

static volatile int refCount = 0;
static SpinLock mutex;
static volatile bool loggerRunning = false;//avoid bug in loop
static struct pollfd fds[2]{
        {0, POLLIN, 0},
        {0, POLLIN, 0}
};

struct Info{
    LoggerCallback callback;
    void* arg;
    Destroyer destroyer;
};

void* thread_run(void* threadInfo){
    Info* info= static_cast<Info *>(threadInfo);
    LoggerCallback  logger=info->callback;
    void* arg=info->arg;
    JNIEnv *env;
    vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
    vm->AttachCurrentThread(&env, NULL);
    char *buffer = new char[1024];
    ssize_t nBytes;
    jclass c=env->FindClass("android/os/Process");
    static jmethodID mid=env->GetStaticMethodID(c,"setThreadPriority","(I)V");
    env->CallStaticVoidMethod(c,mid,-2);
    env->DeleteLocalRef(c);
    while (refCount > 0) {
        int nfds = poll(fds, 2, 100);
        if (nfds > 0) {
            if (fds[0].revents & (POLLIN)) {
                nBytes = read(fds[0].fd, buffer, 1023);
                if (nBytes < 0) {
                    break;
                }
                buffer[nBytes] = 0;
                logger(env,buffer, false,arg);
            }
            if (fds[1].revents & (POLLIN)) {
                nBytes = read(fds[1].fd, buffer, 1023);
                if (nBytes < 0)
                    break;
                buffer[nBytes] = 0;
                logger(env,buffer, true,arg);
            }
        } else if ((nfds < 0 && errno != EAGAIN)) break;
    }
    close(fds[0].fd);
    close(fds[1].fd);
    fds[0].fd = 0;
    fds[1].fd = 0;
    loggerRunning = false;
    vm->DetachCurrentThread();
    delete[] buffer;
    if(info->destroyer)
        info->destroyer(arg);
    delete info;
    return nullptr;
}

void requireLogger(LoggerCallback callback, void* arg,Destroyer destroyer){

    ScopeLock sentry(mutex);
    if (refCount++ == 0) {
        setvbuf(stdout, 0, _IOLBF, 0);
        setvbuf(stderr, 0, _IONBF, 0);
        int stdOutFd[2];
        int stdErrFd[2];
        pipe(stdOutFd);

        pipe(stdErrFd);
        dup2(stdOutFd[1], STDOUT_FILENO);
        dup2(stdErrFd[1], STDERR_FILENO);

        close(stdOutFd[1]);
        close(stdErrFd[1]);

        fds[0].fd = stdOutFd[0];
        fds[1].fd = stdErrFd[0];
        pthread_t th;
        int err=pthread_create(&th,NULL,thread_run,new Info{callback,arg,destroyer});
        if(!err){
            pthread_detach(th);
            loggerRunning = true;
        } else LOGE("Failed to create logger");
    }
}

void dropLogger() {
    ScopeLock sentry(mutex);
    if (!loggerRunning)
        return;
    --refCount;
    if (refCount == 0) {
        while (loggerRunning);
    }
}