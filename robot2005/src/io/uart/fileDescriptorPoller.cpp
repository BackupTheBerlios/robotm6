#include "io/fileDescriptorPoller.h"
#include "log.h"
#include <poll.h>

namespace {
    pthread_mutex_t repositoryLock4 = PTHREAD_MUTEX_INITIALIZER;
    void no_op(...){};
}

FileDescriptorPollerCL* FileDescriptorPollerCL::instance_ = NULL;

FileDescriptorPollerCL::FileDescriptorPollerCL()
    // TODO: add CLASS_FILE_DESCRIPTOR_POLLER to classconfig.h
    //: RobotComponent("FileDescriptorPoller", CLASS_FILE_DESCRIPTOR_POLLER),
    : RobotBase("FileDescriptorPoller", CLASS_DEFAULT)
{
    LOG_FUNCTION();
    no_op(repositoryLock4);
    thread_ = 0;
    FD_ZERO(&listenedFds_);
    init_ = (MTHREAD_CREATE("FileDescriptorPoller Thread",
			    &thread_,
			    NULL,
			    FileDescriptorPollerThreadBody,
			    NULL)
	     == 0);

    if (!init_) {
	LOG_ERROR("Can't run thread Filedescriptor poller\n");
    } else {
	LOG_OK("Initialisation Done\n");
    }
}

FileDescriptorPollerCL::~FileDescriptorPollerCL()
{
    LOG_WARNING("Destructing FileDescriptorPoller\n");
    MTHREAD_CANCEL(thread_);
    instance_ = NULL;
}

void FileDescriptorPollerCL::registerFileDescriptor(int fd,
						    FileDescriptorPollerCallBack cb,
						    const char* cbName,
						    void* userData)
{
    if (thread_ != 0)
    {
	// TODO: should not be a problem. am i wrong? [flo]
	LOG_WARNING("Serious warning! registerFiledDescriptor after having "
		    "created the thread \n");
	return;
    }
    Lock localLock(&repositoryLock4);
    cbList_[fd].fd = fd;
    cbList_[fd].callback = cb;
    cbList_[fd].userData = userData;
    // TODO: don't use magic const here! [flo]
    strncpy(cbList_[fd].callbackName, cbName, 99);
    cbList_[fd].callbackName[99] = 0;
    LOG_INFO("Registered %s fd=%d maxFd_=%d\n", cbName, fd, maxFd_);
    maxFd_ = max(maxFd_, fd);
    FD_SET(fd, &listenedFds_);
}

void FileDescriptorPollerCL::unregisterFileDescriptor(int fd)
{
    // TODO: Apparently this methods crashes the program [flo]
    Lock localLock(&repositoryLock4);
    LOG_ERROR("Unregistering \"%s\"\n", cbList_[fd].callbackName);
    cbList_.erase(fd);
    maxFd_ = -1;
    FD_ZERO(&listenedFds_);
    FileDescriptorPollerCBDataList::const_iterator it;
    for (it = cbList_.begin();
	 it != cbList_.end();
	 ++it)
    {
	maxFd_ = max(maxFd_, (*it).first);
	FD_SET(fd, &listenedFds_);
    }
}

bool FileDescriptorPollerCL::reset()
{
    // TODO: what could be reset in here? [flo]
    return true;
}

void FileDescriptorPollerCL::periodicTask()
{
    static int first = 0;
    if (maxFd_ < 0)
    {
	if (first != 1)
	{
	    LOG_ERROR("FileDescriptorPoller::periodicTask, "
		      "maxFd_ < 0 (%d)\n", maxFd_);
	    first = 1;
	}
	return;
    }
    if (first != 2)
    {
	LOG_WARNING("Listening to=%d file descriptors (maxFd_=%d)\n",
		    cbList_.size(), maxFd_);
	first = 2;
    }

    Lock localLock(&repositoryLock4);
    // TODO: remove magic number [flo]
    pollfd fds[8];
    unsigned int i = 0;
    FileDescriptorPollerCBDataList::iterator it;
    for (it = cbList_.begin();
	 it != cbList_.end();
	 ++it, ++i)
    {
	fds[i].fd = it->first;
	fds[i].events = POLLIN | POLLPRI;
	fds[i].revents = 0;
    }
    // TODO: remove hardcoded magic number.
    int pollr = poll(fds, cbList_.size(), 20);
    if (pollr > 0)
    {
	for (i = 0; i < cbList_.size(); ++i)
	{
	    if ((fds[i].revents == POLLIN || fds[i].revents == POLLPRI))
	    {
		FileDescriptorPollerCBData data = cbList_[fds[i].fd];
		if (data.callback != NULL)
		{
		    data.callback(data.userData, fds[i].fd);
		}
	    }
	}
    }
}

/*
    int maxFd_;
    fd_set listenedFds_;
    FileDescriptorPollerCBDataList cbList_;
    static FileDescriptorPollerCL* instance_;
    pthread_t thread_;
*/
void* FileDescriptorPollerThreadBody(void*)
{
    while(1)
    {
	usleep(10000);
	FileDescriptorPoller->periodicTask();
    }
    return NULL;
}
