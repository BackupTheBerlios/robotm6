/**
 * @file alim.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __ALIM_SIMU_H__
#define __ALIM_SIMU_H__

#include "alim.h"


/**
 * @class AlimSimu
 * Classe gerant la carte alim
 */

class AlimSimu : public AlimCL {
 public:
  /** @brief Constructeur */
  AlimSimu();

  bool exists() const { return true; }
  
  /** @brief passe en mode 12V */
  bool mode12V() { return true; }
  /** @brief passe en mode 28V */
  bool mode28V() { return true; }
  /** @brief passe en mode 42V */
  bool mode42V() { return true; }
  /** @brief active le 12V 1*/
  bool enable12V_1();
  /** @brief active le 12V 1*/
  bool disable12V_1();
  /** @brief active le 12V 2*/
  bool enable12V_2();
  /** @brief active le 12V 2*/
  bool disable12V_2();
};



#endif 
