#ifndef __ENV_MAPPING_H__
#define __ENV_MAPPING_H__

#include "events.h"

struct EnvDetectorMappingStruct {
    char*         name;
    Events        evt;
    unsigned char octet;
    unsigned char bit;
};

static const unsigned int ENV_DETECTOR_NBR = 1;

EnvDetectorMappingStruct EnvDetectorMapping[ENV_DETECTOR_NBR]= {
    {"EnvLeft",        EVENTS_ENV_TOP_FRONT,        0, 0 }
    // TODO: add env
};

#endif
