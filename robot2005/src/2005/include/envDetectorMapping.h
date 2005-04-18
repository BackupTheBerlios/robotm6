#ifndef __ENV_MAPPING_H__
#define __ENV_MAPPING_H__

#include "events.h"
#include "env.h"

enum EnvDetectorId {
    ENV_DETECTOR_TOP_RIGHT=0,
    ENV_DETECTOR_TOP_LEFT,
    ENV_DETECTOR_SIDE_RIGHT,
    ENV_DETECTOR_SIDE_LEFT,
    
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

static EnvDetectorMappingStruct EnvDetectorMapping[ENV_DETECTOR_NBR]= {
    {ENV_DETECTOR_TOP_RIGHT, "EnvDetector TOP RIGHT",   EVENTS_ENV_TOP_RIGHT, 
     0, 0, false,  ENV_NEAR,
    true },
    {ENV_DETECTOR_TOP_LEFT, "EnvDetector TOP LEFT",   EVENTS_ENV_TOP_LEFT,  
     0, 2, false,  ENV_NEAR,
     true},
    {ENV_DETECTOR_SIDE_RIGHT, "EnvDetector SIDE RIGHT", EVENTS_ENV_SIDE_RIGHT,
     0, 4, false,  ENV_NEAR,
    true},
    {ENV_DETECTOR_SIDE_LEFT, "EnvDetector SIDE LEFT",   EVENTS_ENV_SIDE_LEFT,
     0, 6, false,  ENV_NEAR,
    true}
};

#endif
