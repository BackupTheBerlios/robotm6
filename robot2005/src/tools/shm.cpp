#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "robotShm.h"
#define LOG_CLASSID CLASS_SHM
#include "log.h"

// ----------------------------------------------------------------------------
// RobotShm::create
// ----------------------------------------------------------------------------
int RobotShm::create(const char* shmKeyFile,
		     int mode,
		     int shmSize,
		     void** dataPtr, 
		     bool &existingShared,
		     bool createIfNotExisting)
{
    key_t key;
    int shm_id;

    existingShared = true;

    /* Create the key */
    if ((key = ftok(shmKeyFile, 'S')) == -1) {
        LOG_ERROR("Cannot create shared memory (step 1-3) %s\n", 
                  shmKeyFile);
        return 1;
    }
    
    /* Connect to (and possibly create) the segment */
    if ((shm_id = shmget(key, shmSize, mode)) == -1) {
        existingShared = false;
    } else {
        LOG_INFO("Found an existing shared memory with key=%s\n", 
		 shmKeyFile);
        existingShared = true;
    }
    if (!existingShared && createIfNotExisting) {
        LOG_INFO("Creating a shared memory with key=%s\n", 
		 shmKeyFile);
        if ((shm_id = shmget(key, shmSize, mode | IPC_CREAT)) == -1) {
	    LOG_ERROR("cannot create shared memory (step 2-3) %s\n", 
		      shmKeyFile);
	    return 2;
      } 
    }
    if (!existingShared && !createIfNotExisting) {
        return 4;
    }

    /* Attach to the segment to get a pointer to it */
    *dataPtr = (void *)shmat(shm_id, (void *)0, 0);
    if ((char *)(*dataPtr) == (char *)(-1)) {
        LOG_ERROR("cannot create shared memory (step 3-3) %s\n", 
                  shmKeyFile);
        return 3;
    }
    LOG_OK("SHM created %s\n", shmKeyFile);
    return 0;
}
