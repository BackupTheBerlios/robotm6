/**
 * @file fileDescriptorPoller.h
 *
 * @author Florian Loitsch
 *
 * "listens" on registered fileDescriptors and calls back whenever new data
 * is ready. This replaces some parts of the 2004-IoManager.
 */

#pragma once

#include "mthread.h"
#include "robotBase.h"

#include <unistd.h>
#include <map>

#define FileDescriptorPoller FileDescriptorPollerCL::instance()

typedef void (*FileDescriptorPollerCallBack)(void*, int); //void* userdata, int fileDescriptor

struct FileDescriptorPollerCBData {
    int fd;
    FileDescriptorPollerCallBack callback;
    void* userData;
    char callbackName[100];

    FileDescriptorPollerCBData(): fd(-1), callback(NULL), userData(NULL) {
        callbackName[0]=0;
    }
};

typedef std::map<int, FileDescriptorPollerCBData> FileDescriptorPollerCBDataList;

/** 
 * @class FileDescriptorPollerCL
 * all registered filedescriptors are periodically polled, to see if data is
 * available. If yes, the callback is triggered.
 */
class FileDescriptorPollerCL : public RobotBase
{
public:
    /**
     * @brief Returns the unique instance of this classe.
     * instance creates lazily a new FileDescriptorPollerCL.
     */
    static FileDescriptorPollerCL* instance();
private: // constructor/destructor
    /**
     * @brief Constructor.
     * This class is a singleton -> can only be called by instance();
     */
    FileDescriptorPollerCL();
    /**
     * @brief Destructor
     */
    ~FileDescriptorPollerCL();

public:
    /** 
     * @brief Register a new filedescriptor and its callback-function.
     */
    void registerFileDescriptor(int fd, 
				FileDescriptorPollerCallBack cb, 
				const char* cbName, 
				void* userData);
    /**
     * @brief Unregisters the filedescriptor (and its callback-function).
     * @see registerFileDescriptor
     */
    void unregisterFileDescriptor(int fd);

public: // RobotComponent
    bool reset();

private:
    /** 
     * @brief periodic task, that's executed by the thread.
     * This function checks for new data (ie it polls the fds).
     */
  void periodicTask();
  friend void* FileDescriptorPollerThreadBody(void*);

private:
    int init_;
    int maxFd_;
    fd_set listenedFds_;
    FileDescriptorPollerCBDataList cbList_;
    static FileDescriptorPollerCL* instance_;
    pthread_t thread_;
};

// ============================================================================
// Inline functions
// ============================================================================

inline FileDescriptorPollerCL* FileDescriptorPollerCL::instance()
{
    if (!instance_)
	instance_ = new FileDescriptorPollerCL();
    return instance_;
}
