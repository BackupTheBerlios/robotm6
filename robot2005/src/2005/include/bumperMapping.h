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
    // if set to false, no events will be raised by this captor
    bool enableAtReset;
};

// TODO set the correct mapping
BumperMappingStruct BumpersMapping[BUMPERS_NBR]= {
    {BUTTON_YES,    "Button Yes",      EVENTS_BUTTON_YES,      false, 0, 0,
    true},
    {BUTTON_NO,     "Button No",       EVENTS_BUTTON_NO,       false, 0, 0,
    true},
    {SWITCH_MATCH,  "Switch Match",    EVENTS_GROUP_NONE,      false, 2, 0,
    true},
    {SWTICH_REBOOT, "Switch Reboot",   EVENTS_SWITCH_REBOOT,   false, 1, 2,
    true},
    {EMERGENCY_STOP,"Emergency Stop",  EVENTS_EMERGENCY_STOP,  false, 1, 1,
    true},
    {START_JACK,    "Start Jack",      EVENTS_JACKIN,          false, 1, 0,
    true},
 
    {BRIDG_BUMP_LEFT,"BumpBridgeLeft",        EVENTS_BRIDGE_BUMP_LEFT, 
     false, 0, 4, true},
    {BRIDG_BUMP_RIGHT,"BumpBridgeRight",      EVENTS_BRIDGE_BUMP_RIGHT, 
     false, 0, 3, true},
    {BRIDG_SHARP_LEFT,"SharpBridgeLeft",      EVENTS_GROUP_NONE,        
     true, 0, 0, true},
    {BRIDG_SHARP_CENTER, "SharpBridgeCenter", EVENTS_GROUP_NONE,        
     true, 0, 1, true},
    {BRIDG_SHARP_RIGHT,"SharpBridgeRight",    EVENTS_GROUP_NONE,        
     true, 0, 2, true}
    // TODO: add bumpers 2,1 a 2,7
};

#endif
