#ifndef __ENV_MAPPING_H__
#define __ENV_MAPPING_H__

#include "events.h"
#include "env.h"

enum EnvDetectorId {
    ENV_DETECTOR_0=0,
    ENV_DETECTOR_1,
    ENV_DETECTOR_2,
    ENV_DETECTOR_3,
    
    ENV_DETECTOR_NBR
};

struct EnvDetectorMappingStruct {
    EnvDetectorId   id;
    char*           name;
    EventsEnum      evt;
    unsigned char   byte;
    unsigned char   bit;
    bool            reversed;
    // si reversed = false, declenche l'evenement si captorValue >= eventDistLevel et l'unraise sinon
    // si reversed = true, declenche l'evenement si captorValue <= eventDistLevel et l'unraise sinon
    EnvDetectorDist eventDistLevel; 
    // if set to false, no events will be raised by this captor
    bool            enableAtReset;
 
};

EnvDetectorMappingStruct EnvDetectorMapping[ENV_DETECTOR_NBR]= {
    {ENV_DETECTOR_0, "EnvDetector 0",   EVENTS_ENV_TOP_FRONT,   0, 0, false,  ENV_NEAR,
    true },
    {ENV_DETECTOR_1, "EnvDetector 1",   EVENTS_ENV_TOP_FRONT,   0, 2, false,  ENV_NEAR,
     true},
    {ENV_DETECTOR_2, "EnvDetector 2",   EVENTS_ENV_TOP_FRONT,   0, 4, false,  ENV_NEAR,
    true},
    {ENV_DETECTOR_3, "EnvDetector 3",   EVENTS_ENV_TOP_FRONT,   0, 6, false,  ENV_NEAR,
    true}
    // TODO: set the correct events (need to know the captor position
};

#endif
