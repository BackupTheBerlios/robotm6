/**
 * @file odometer.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Interface avec la carte odometre qui est connectee sur un port serie et
 * qui donne la position du robot. En mode automatique, des qu'on recoit la 
 * donnee des deux codeurs, on fait appel aux methodes de robotPosition pour
 * mettre a jour la position du robot. Si la carte ne repond plus, 
 * robotPosition s'en rend compte et utilise les codeurs des roues motrices 
 * pour calculer la position du robot
 */

#ifndef __ODOMETER_H__
#define __ODOMETER_H__

#include "robotDevice.h"


#define Odometer OdometerCL::instance()

// ===========================================================================
// class Odometer
// ===========================================================================

/**
 * @class Odometer
 * Interface avec la carte qui controle les servo moteurs
 */

class OdometerCL : public RobotDeviceCL
{
 public:
    OdometerCL();
    virtual ~OdometerCL();
    virtual bool exists() const { return false; }
    /**
     * @brief Met a jour la position des 2 codeurs (sans conversion) 
     * En mode automatique la carte envoie toute seule des donnees et 
     *   getCoderPosition retourne true si la carte a mise a jour la 
     *   position des codeurs depuis la derniere requete getCoderPosition
     * En mode manuel getCoderPosition va directement lire la valeur 
     *   des codeurs et retourne true si la communication s'est bien passee.
     */
    virtual bool getCoderPosition(CoderPosition &left,
                                  CoderPosition &right){ return false;}

    static OdometerCL* instance() { return odometer_;}
    
 private:
    OdometerCL(OdometerCL const& odom); // disabled
    static OdometerCL* odometer_;
};

#endif // __ODOMETER_H__
