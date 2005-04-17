/**
 * @file alim.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __ALIM_05_H__
#define __ALIM_05_H__

#include "alim.h"

class IoDevice;
/**
 * @class Alim05
 * Classe gerant la carte alim
 */

class Alim05 : public AlimCL {
 public:
    /** @brief Constructeur */
    Alim05();
    ~Alim05();
    bool exists() const { return true; }

    bool getAllTension(Millivolt tension[4]); 
    /** @brief passe en mode 12V */
    bool mode12V();
    /** @brief passe en mode 28V */
    bool mode28V();
    /** @brief passe en mode 42V */
    bool mode42V();
    /** @brief active le 12V 1*/
    bool enable12V_1();
    /** @brief active le 12V 1*/
    bool disable12V_1();
    /** @brief active le 12V 2*/
    bool enable12V_2();
    /** @brief active le 12V 2*/
    bool disable12V_2(); 
 private:
    IoDevice* device_;
};



#endif 
