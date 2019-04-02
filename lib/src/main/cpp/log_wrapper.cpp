

#include "log_wrapper.h"
#include "SpinLock.h"
#include "common.h"
#include <cassert>
#include <cstdio>
#include <poll.h>
#include <time.h>
#include "errno.h"
#include "Vector.h"

struct Info{
    LoggerCallback callback;
    void* arg;
    Destroyer destroyer;
};

static SpinLock mutex;
static volatile bool loggerRunning = false;//avoid bug in loop
static struct pollfd fds[2]{
        {0, POLLIN, 0},
        {0, POLLIN, 0}
};

static Vector<Info,2> listeners;

void* thread_run(void* threadInfo){
    JNIEnv *env;
    vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
    vm->AttachCurrentThread(&env, NULL);
    char *buffer = new char[1024];
    ssize_t nBytes;
    jclass c=env->FindClass("android/os/Process");
    static jmethodID mid=env->GetStaticMethodID(c,"setThreadPriority","(I)V");
    env->CallStaticVoidMethod(c,mid,-2);
    env->DeleteLocalRef(c);
    while (listeners.size() > 0) {
        int nfds = poll(fds, 2, 100);
        if (nfds > 0) {
            if (fds[0].revents & (POLLIN)) {
                nBytes = read(fds[0].fd, buffer, 1023);
                if (nBytes < 0) {
                    break;
                }
                buffer[nBytes] = 0;
                for(auto&& info:listeners)
                    info.callback(env,buffer, false,info.arg);
            }
            if (fds[1].revents & (POLLIN)) {
                nBytes = read(fds[1].fd, buffer, 1023);
                if (nBytes < 0)
                    break;
                buffer[nBytes] = 0;
                for(auto&& info:listeners)
                    info.callback(env,buffer, true,info.arg);
            }
        } else if ((nfds < 0 && errno != EAGAIN)) break;
    }
    close(fds[0].fd);
    close(fds[1].fd);
    fds[0].fd = 0;
    fds[1].fd = 0;
    vm->DetachCurrentThread();
    delete[] buffer;
    loggerRunning = false;
    return nullptr;
}

intptr_t requireLogger(LoggerCallback callback, void *arg, Destroyer destroyer) {

    ScopeLock sentry(mutex);
    if (listeners.size() == 0) {
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
        int err=pthread_create(&th,NULL,thread_run,NULL);
        if(!err){
            pthread_detach(th);
            loggerRunning = true;
        } else LOGE("Failed to create logger");
    }
    if(loggerRunning){
        listeners.push_back(Info{callback,arg,destroyer});
        return (intptr_t)arg;
    } else return 0;
}

void dropLogger(intptr_t id) {
    ScopeLock sentry(mutex);
    if (!loggerRunning)
        return;

    if(id){
        for (int i = 0; i < listeners.size(); ++i) {
            if((intptr_t)listeners[i].arg==id){
                id=i;
                break;
            }
        }
        Info info=listeners[id];
        listeners.eraseAt(id);
        if (listeners.size()== 0) {
            while (loggerRunning);
        }
        if(info.destroyer)
            info.destroyer(info.arg);
    }

}