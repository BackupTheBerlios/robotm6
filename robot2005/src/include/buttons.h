/**
 * @file buttons.h
 *
 * @author Laurent Saint-Marcel lstmarcel@yahoo.fr
 *
 * Class qui gere les boutons sur la carte E/S.
 * Les boutons enregistrent des callbacks sur leur changement
 * d'etat au niveau du gestionniare de la carte E/S (dridaqManager) 
 * et ils declenchent des evenements quand l'etat est modifie
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "robotBase.h"
#include "events.h"

// ============================================================================
// class ButtonCL
// ============================================================================

/**
 * @class ButtonCL
 * Definition d'un bumper qui met automatiquement a jour l'event correspondant
 */
class ButtonCL: public RobotIODevice
{
 public:
    ButtonCL(const char*   name, 
             unsigned char pinId,
             bool          pinReversed,
             EventsEnum    evt);

    /** @brief Reset the bumper and the events value */
    bool reset();
    /** @brief test if the bumper is responding */
    bool ping();

    bool isSimu() const { return false; }
    /** @brief Cette fonction retourne TRUE si le bouton est relache */
    bool autoTest(bool *ioStatus=NULL);
    /** @brief Return the name of the io device */
    const char* getIoName(int ioID=0) const;
    bool validate();

 protected:
    /** 
     * @brief functions run by dridaq when the value of the pin changes 
     * It updates the button event
     */
    void pinChangeCallBack(unsigned char pin, 
                           bool          pinValue);
    friend void buttonPinChangeCallBack(void*          userData,
					unsigned char  pinId, 
					bool           pinValue);
 private:
    EventsEnum    evt_;
    bool          reversed_;
    unsigned char pinId_;
    char          buttonName_[100];
};

// ============================================================================
// inline functions
// ============================================================================

// ----------------------------------------------------------------------------
// Button::ping
// ----------------------------------------------------------------------------
inline bool ButtonCL::ping()
{
    return true;
}

// ----------------------------------------------------------------------------
// Button::validate
// ----------------------------------------------------------------------------
inline bool ButtonCL::validate()
{
    return false;
}

// ----------------------------------------------------------------------------
// Button::getIoName
// ----------------------------------------------------------------------------
inline const char* ButtonCL::getIoName(int ioID) const
{
    return buttonName_;
}


#endif // __BUTTON_H__
