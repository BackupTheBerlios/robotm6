#ifndef __BUMPER_MAPPING_H__
#define __BUMPER_MAPPING_H__

#include "events.h"

struct BumperMappingStruct {
    char*         name;
    Events        evt;
    bool          reversed;
    unsigned char octet;
    unsigned char bit;
};

static const unsigned int BUMPERS_NBR = 11;

BumperMappingStruct BumpersMapping[BUMPERS_NBR]= {
    {"Button Yes",        EVENTS_BUTTON_YES,        false, 0, 0 },
    {"Button No",         EVENTS_BUTTON_NO,         false, 0, 0 },
    {"Switch Match",      EVENTS_GROUP_NONE,        false, 0, 0 },
    {"Switch Reboot",     EVENTS_SWITCH_REBOOT,     false, 0, 0 },
    {"Emergency Stop",    EVENTS_EMERGENCY_STOP,    false, 0, 0 },
    {"Start Jack",        EVENTS_JACKIN,            false, 0, 0 },

    {"BumpBridgeLeft",    EVENTS_BRIDGE_BUMP_LEFT,  false, 0, 0 },
    {"BumpBridgeRight",   EVENTS_BRIDGE_BUMP_RIGHT, false, 0, 0 },
    {"SharpBridgeLeft",   EVENTS_GROUP_NONE,        false, 0, 0 },
    {"SharpBridgeCenter", EVENTS_GROUP_NONE,        false, 0, 0 },
    {"SharpBridgeRight",  EVENTS_GROUP_NONE,        false, 0, 0 }
    // TODO: add bumpers
};

#endif
