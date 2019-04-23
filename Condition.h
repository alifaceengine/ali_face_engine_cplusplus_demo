#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#include "Mutex.h"

typedef int64_t nsecs_t;

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define CLOCK_REALTIME 0
//struct timespec { long tv_sec; long tv_nsec; };    //header part
static int clock_gettime(int, struct timespec *spec)      //C-file part
{
    __int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
    wintime -= 116444736000000000i64;  //1jan1601 to 1jan1970
    spec->tv_sec = wintime / 10000000i64;           //seconds
    spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
    return 0;
}
#endif

class Condition {
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };

    enum WakeUpType {
        WAKE_UP_ONE = 0,
        WAKE_UP_ALL = 1
    };

    Condition();

    Condition(int type);

    ~Condition();

    // Wait on the condition variable.  Lock the mutex before calling.
    int wait(Mutex &mutex);

    // same with relative timeout
    int waitRelative(Mutex &mutex, int waitmillsseconds);

    // Signal the condition variable, allowing one thread to continue.
    void signal();

    // Signal the condition variable, allowing one or all threads to continue.
    void signal(WakeUpType type) {
        if (type == WAKE_UP_ONE) {
            signal();
        } else {
            broadcast();
        }
    }

    // Signal the condition variable, allowing all threads to continue.
    void broadcast();

private:
    pthread_cond_t mCond;
};

inline Condition::Condition() {
    pthread_cond_init(&mCond, NULL);
}

inline Condition::Condition(int type) {
    if (type == SHARED) {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&mCond, &attr);
        pthread_condattr_destroy(&attr);
    } else {
        pthread_cond_init(&mCond, NULL);
    }
}

inline Condition::~Condition() {
    pthread_cond_destroy(&mCond);
}

inline int Condition::wait(Mutex &mutex) {
    return -pthread_cond_wait(&mCond, &mutex.mMutex);
}

inline int Condition::waitRelative(Mutex &mutex, int waitmillsseconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (int) (waitmillsseconds / 1000);
    ts.tv_nsec += (waitmillsseconds % 1000) * 1000000;
    if (ts.tv_nsec > 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }
    return -pthread_cond_timedwait(&mCond, &mutex.mMutex, &ts);
}

inline void Condition::signal() {
    pthread_cond_signal(&mCond);
}

inline void Condition::broadcast() {
    pthread_cond_broadcast(&mCond);
}
