#ifndef __BUMPER_MAPPING_H__
#define __BUMPER_MAPPING_H__

#include "events.h"

enum BumperId {
  BUTTON_YES=0,
  BUTTON_NO,
  SWITCH_MATCH,
  SWTICH_REBOOT,
  EMERGENCY_STOP,
  START_JACK,

  BRIDG_BUMP_LEFT,
  BRIDG_BUMP_RIGHT,
  BRIDG_SHARP_LEFT,
  BRIDG_SHARP_CENTER,
  BRIDG_SHARP_RIGHT,

  BUMPERS_NBR
};

struct BumperMappingStruct {
    enum BumperId id;
    char*         name;
    EventsEnum    evt;
    bool          reversed;
    unsigned char byte;
    unsigned char bit;
};

BumperMappingStruct BumpersMapping[BUMPERS_NBR]= {
    {BUTTON_YES,    "Button Yes",      EVENTS_BUTTON_YES,      false, 0, 0 },
    {BUTTON_NO,     "Button No",       EVENTS_BUTTON_NO,       false, 0, 0 },
    {SWITCH_MATCH,  "Switch Match",    EVENTS_GROUP_NONE,      false, 0, 0 },
    {SWTICH_REBOOT, "Switch Reboot",   EVENTS_SWITCH_REBOOT,   false, 0, 0 },
    {EMERGENCY_STOP,"Emergency Stop",  EVENTS_EMERGENCY_STOP,  false, 0, 0 },
    {START_JACK,    "Start Jack",      EVENTS_JACKIN,          false, 0, 0 },
 
    {BRIDG_BUMP_LEFT,"BumpBridgeLeft",        EVENTS_BRIDGE_BUMP_LEFT, 
     false, 0, 0 },
    {BRIDG_BUMP_RIGHT,"BumpBridgeRight",      EVENTS_BRIDGE_BUMP_RIGHT, 
     false, 0, 0 },
    {BRIDG_SHARP_LEFT,"SharpBridgeLeft",      EVENTS_GROUP_NONE,        
     false, 0, 0 },
    {BRIDG_SHARP_CENTER, "SharpBridgeCenter", EVENTS_GROUP_NONE,        
     false, 0, 0 },
    {BRIDG_SHARP_RIGHT,"SharpBridgeRight",    EVENTS_GROUP_NONE,        
     false, 0, 0 }
    // TODO: add bumpers
};

#endif
