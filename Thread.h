#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "Mutex.h"
#include "Condition.h"

inline long getThreadId() {
#ifdef WIN32
    return GetCurrentThreadId();
#elif __APPLE__
    return (long) pthread_self();
#else
    return pthread_self();
#endif
}

class Thread {
public:
    // Create a Thread object, but doesn't create or start the associated
    // thread. See the run() method.
    Thread() :
            mThreadId(-1),
            mLock("Thread::mLock"),
            mExitPending(false),
            mRunning(false) {
#ifdef WIN32
		mThread.p = 0;
#else
		mThread = 0;
#endif
    }

    virtual ~Thread() {
#ifdef WIN32
		pthread_join(mThread, NULL);
#else
        if (mThread) {
            pthread_join(mThread, NULL);
        }
#endif
    }

    // Start the thread in threadLoop() which needs to be implemented.
    virtual int run(const char *name = 0) {
        Mutex::Autolock _l(mLock);
        mName = name;

        if (mRunning) {
            // thread already started
            return -1;
        }

        // reset status and exitPending to their default value, so we can
        // try again after an error happened (either below, or in readyToRun())
        mExitPending = false;

        mRunning = true;

        bool res;

        int ret = 0;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        ret = pthread_create(&mThread, &attr, _threadLoop, this);
        pthread_attr_destroy(&attr);

        if (ret != 0) {
            mRunning = false;
            return -1;
        }

        // Do not refer to mStatus here: The thread is already running (may, in fact
        // already have exited with a valid mStatus result). The NO_ERROR indication
        // here merely indicates successfully starting the thread and does not
        // imply successful termination/execution.
        return 0;

        // Exiting scope of mLock is a memory barrier and allows new thread to run
    }

    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void requestExit() {
        Mutex::Autolock _l(mLock);
        mExitPending = true;
    }

    // Good place to do one-time initializations
    virtual int readyToRun() {
        return 0;
    }

    // Call requestExit() and wait until this object's thread exits.
    // BE VERY CAREFUL of deadlocks. In particular, it would be silly to call
    // this function from this object's thread. Will return WOULD_BLOCK in
    // that case.
    int requestExitAndWait() {
        Mutex::Autolock _l(mLock);
        if (mThreadId == getThreadId()) {
            return -1;
        }

        mExitPending = true;

        while (mRunning == true) {
            mThreadExitedCondition.wait(mLock);
        }
        // This next line is probably not needed any more, but is being left for
        // historical reference. Note that each interested party will clear flag.
        mExitPending = false;
        return 0;
    }

    // Wait until this object's thread exits. Returns immediately if not yet running.
    // Do not call from this object's thread; will return WOULD_BLOCK in that case.
    void join() {
        Mutex::Autolock _l(mLock);
        if (mThreadId == getThreadId()) {
            return;
        }

        while (mRunning == true) {
            mThreadExitedCondition.wait(mLock);
        }
    }

    // Indicates whether this thread is running or not.
    bool isRunning() const {
        Mutex::Autolock _l(mLock);
        return mRunning;
    }

protected:
    // exitPending() returns true if requestExit() has been called.
    bool exitPending() const {
        Mutex::Autolock _l(mLock);
        return mExitPending;
    }

protected:
    const char *mName;

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool threadLoop() = 0;

private:

    static void *_threadLoop(void *user) {
        Thread *const self = static_cast<Thread *>(user);
        bool first = true;

        do {
            bool loop;
            if (first) {
                self->mThreadId = getThreadId();
                first = false;
                if (!self->exitPending()) {
                    loop = self->threadLoop();
                }
            } else {
                loop = self->threadLoop();
            }

            // establish a scope for mLock
            {
                Mutex::Autolock _l(self->mLock);
                if (loop == false || self->mExitPending) {
                    self->mExitPending = true;
                    self->mRunning = false;
                    // clear thread ID so that requestExitAndWait() does not exit if
                    // called by a new thread using the same thread ID as this one.
                    self->mThreadId = -1;
                    // note that interested observers blocked in requestExitAndWait are
                    // awoken by broadcast, but blocked on mLock until break exits scope
                    self->mThreadExitedCondition.broadcast();
                    break;
                }
            }

        } while (1);

        return 0;
    }

    // always hold mLock when reading or writing
    pthread_t mThread;
    long mThreadId;
    mutable Mutex mLock;
    Condition mThreadExitedCondition;
    // note that all accesses of mExitPending and mRunning need to hold mLock
    volatile bool mExitPending;
    volatile bool mRunning;
};
