/**
 * @file buttons.cpp
 *
 * @author Laurent Saint-Marcel lstmarcel@yahoo.fr
 *
 * Class qui gere les boutons sur la carte E/S.
 * Les boutons enregistrent des callbacks sur leur changement
 * d'etat au niveau du gestionniare de la carte E/S (dridaqManager) 
 * et ils declenchent des evenements quand l'etat est modifie
 */

#include "devices/buttons.h"
#include "log.h"

// ----------------------------------------------------------------------------
// ButtonCL::ButtonCL
// ----------------------------------------------------------------------------
ButtonCL::ButtonCL(const char*   name, 
                   unsigned char pin,
                   bool          pinReversed,
                   EventsEnum    evt) :
    RobotIODevice(name, CLASS_BUTTON),
    evt_(evt), reversed_(pinReversed), pinId_(pin)
{
    strncpy(buttonName_, name, 100);
    reset();
}

// ----------------------------------------------------------------------------
// buttonPinChangeCallBack
// ----------------------------------------------------------------------------
void buttonPinChangeCallBack(void*          userData,
                             unsigned char  pin,
                             bool           pinValue)
{
    ((ButtonCL*)userData)->pinChangeCallBack(pin, pinValue);
}

// ----------------------------------------------------------------------------
// ButtonCL::ButtonCL
// ----------------------------------------------------------------------------
// Reset the button and the events value 
// ----------------------------------------------------------------------------
bool ButtonCL::reset()
{
    LOG_DEBUG("Reset %s\n", getButtonName());
#ifdef LSM_TODO
    pinChangeCallBack(pin_, DRIDAQ->getPinValue(pin_));
    // enregistre un callback qui est appele quand la pin change de valeur
    DRIDAQ->registerChangeCallback(pin_, this, buttonPinChangeCallBack);
#endif
    init_ = true;
    return init_;
}

// ----------------------------------------------------------------------------
// ButtonCL::pinChangeCallBack
// ----------------------------------------------------------------------------
// This is the functions run by dridaq when the value of the pin changes 
// ----------------------------------------------------------------------------
void ButtonCL::pinChangeCallBack(unsigned char pin, 
                                 bool          pinValue)
{
    if (pin != pinId_) {
        LOG_ERROR("%s: bad pin\n", getButtonName());
        return;
    }
    LOG_DEBUG("%s, %d\n", getButtonName(), pinValue);
    if (reversed_ ^ pinValue) {
        Events->raise(evt_);
    } else {
        Events->unraise(evt_);
    }
}
