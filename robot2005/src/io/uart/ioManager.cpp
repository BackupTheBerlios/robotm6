#include "ioManager.h"
#include "robotConfig.h"
#include "log.h"
#include "uart.h"
#include <poll.h>



IoManagerCL* IoManagerCL::ioManager_=NULL;

void* IoManagerThreadBody(void*);

namespace {
  pthread_mutex_t repositoryLock4 = PTHREAD_MUTEX_INITIALIZER;
  void no_op(...){};
}

// -------------------------------------------------------------------------
// IoManager::IoManager
// -------------------------------------------------------------------------
IoManagerCL::IoManagerCL() : 
  RobotComponent("IoManager", CLASS_IO_MANAGER), 
  uartMgr_(NULL)
{
    LOG_FUNCTION();
    assert(!ioManager_);
    no_op(repositoryLock4);
    ioManager_ = this;
    maxFd_ = -1; 
    thread_=0;
    FD_ZERO(&listenedFds_);
    if (RobotConfig->ioManagerAlloc) {
        // start the uart driver (ports serie) and scan all ports to find devices
	// TODO: don't use singleton? [flo]
        uartMgr_ = new UartManagerCL();
    } else {
    }
    // start the thread which checks for new messages on registered file
    // descriptors
    init_= (MTHREAD_CREATE("IoManager Thread",
                           &thread_, 
                           NULL, 
                           IoManagerThreadBody,
                           NULL)==0);
    if (!init_) {
        LOG_ERROR("Can not run thread RobotTimer\n");
    } else {
        LOG_OK("Initialisation Done\n");
    }
}

// -------------------------------------------------------------------------
// IoManager::~IoManager
// -------------------------------------------------------------------------
IoManagerCL::~IoManagerCL()
{
    LOG_WARNING("Destructing IoManager\n");
    MTHREAD_CANCEL(thread_);
    if (uartMgr_) { delete uartMgr_; uartMgr_ = NULL; }
    ioManager_ = NULL;
}
 
// -------------------------------------------------------------------------
// IoManager::reset
// -------------------------------------------------------------------------
bool IoManagerCL::reset()
{
    init_=true;
    if (uartMgr_) { uartMgr_->reset();} 
    return init_;
}

// -------------------------------------------------------------------------
// IoManager::emergencyStop
// -------------------------------------------------------------------------
void IoManagerCL::emergencyStop()
{
    if (uartMgr_) { uartMgr_->emergencyStop();}
}

// -------------------------------------------------------------------------
// IoManager::validate
// -------------------------------------------------------------------------
bool IoManagerCL::validate()
{
    bool result = true;
    if (uartMgr_) { result &= uartMgr_->validate();}
    return result;
}

// -------------------------------------------------------------------------
// IoManager::periodicTask
// -------------------------------------------------------------------------
void IoManagerCL::periodicTask()
{
    //printf("IoManager::periodicTask(%d)\n", maxFd_);
    static int first=0;
    if (maxFd_<0) {
        if (first != 1) {
            LOG_ERROR("IoManager::periodicTask, maxFd_<0 (%d)\n", maxFd_);
            first = 1;
        }
        return;
    }
    if (first != 2) {
        LOG_WARNING("Listening to=%d file descriptors (maxFd_=%d)\n", cbList_.size(), maxFd_);   
        first = 2;
    }

    Lock localLock(&repositoryLock4);
#if 1
    pollfd fds[8];
    int i=0;
    IoManagerCBDataList::iterator it;
    for(it=cbList_.begin(); it!=cbList_.end(); it++, i++) { 
        fds[i].fd=(*it).first;
        fds[i].events  = POLLIN | POLLPRI;
        fds[i].revents = 0;
	//printf(" fd=%d\n", fds[i].fd);
    }
    //printf("before poll (%d)\n", cbList_.size());
    int pollr=poll(fds, cbList_.size(), 20);
    //printf("pollr=%d\n", pollr);
    if (pollr > 0) {
      for(i=0;i<(int)cbList_.size();i++) { 
        if ((fds[i].revents == POLLIN || fds[i].revents == POLLPRI)) {
	  IoManagerCBData data=cbList_[fds[i].fd];
	  if (data.callback!=NULL) {
	    //printf("exec\n");
	    data.callback(data.userData, fds[i].fd);
	  }
	}
      }
    }
    //printf("exec\n");
#else
    //printf("IoManager::wait(%d)\n", maxFd_);
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;  
    fd_set copyFds = listenedFds_;
    // TODO: check what happens when closing one selected fd ! => ca crashe
    select(maxFd_+1, &copyFds, 0, 0, &timeout);
    //printf("after select(%d)\n", cbList_.size());
    if (timeout.tv_usec != 0 || timeout.tv_sec != 0) {
         // If there is no timeout , we check the file descriptors
        IoManagerCBDataList::iterator it;
        for(it=cbList_.begin(); it!=cbList_.end();it++) { 
	  //printf("isset(%d)\n", (*it).first);
            if (FD_ISSET((*it).first, &copyFds) ) {
	      //printf("exec\n");
                (*it).second.callback((*it).second.userData, (*it).first);
            }
        }
    }
#endif
}

// -------------------------------------------------------------------------
// IoManagerThreadBody
// -------------------------------------------------------------------------
void* IoManagerThreadBody(void*)
{
    while(1) {
        usleep(10000);
        IoManagerCL::instance()->periodicTask();
    }
    return NULL;
}

// -------------------------------------------------------------------------
// IoManager::registerFileDescriptor
// -------------------------------------------------------------------------
void IoManagerCL::registerFileDescriptor(int fd, 
				       IoManagerCallBack cb, 
				       const char* cbName,
				       void* userData)
{
    if (thread_ != 0) {
        LOG_WARNING("Serious warning! registerFileDescriptor after having "
                  "created the thread\n");
    }
    if (fd<0 || cb == NULL || cbName == NULL) {
        LOG_ERROR("Invalid file descriptor(%d) or callback(0x%x) or "
		  "cbName(0x%x)\n", fd, (int)cb, (int)cbName);
        return;
    }
    Lock localLock(&repositoryLock4);
    cbList_[fd].fd=fd;
    cbList_[fd].callback=cb;
    cbList_[fd].userData=userData;
    strncpy(cbList_[fd].callbackName, cbName, 99);
    cbList_[fd].callbackName[99]=0;
    LOG_INFO("Registering %s fd=%d maxFd_=%d\n", 
	     cbName, fd, maxFd_);
    
    maxFd_ = max(maxFd_, fd);
    FD_SET(fd, &listenedFds_);
}

// -------------------------------------------------------------------------
// IoManager::unregisterFileDescriptor
// -------------------------------------------------------------------------
void IoManagerCL::unregisterFileDescriptor(int fd)
{
    // cette fonction fait crasher le programme!
    Lock localLock(&repositoryLock4);
    LOG_ERROR("Unregistering \"%s\"\n", cbList_[fd].callbackName);
    cbList_.erase(fd);
    maxFd_ = -1;
    FD_ZERO(&listenedFds_);
    for(IoManagerCBDataList::iterator it=cbList_.begin(); 
	it!=cbList_.end();
	it++) {
        maxFd_ = max(maxFd_, (*it).first);
        FD_SET((*it).first, &listenedFds_);
    }
}

