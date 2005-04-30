/**
 * @file odometer05.h
 * 
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __ODOMETER_05_H__
#define __ODOMETER_05_H__

#include "odometer.h"

// forward-declarations
class MotorOdom05;

// ===========================================================================
// class Odometer05
// ===========================================================================

/** 
 * @class Odometer05
 */
class Odometer05 : public OdometerCL
{
 public:
    Odometer05(MotorOdom05* motorOdomImpl);
  
  bool exists() const { return true; }

  bool reset();

  /** @brief If useCache=true (default), the function getCoderPosition return
      the position that has been returned while doing a 
      motor05::setSpeedAndCachePosition. if set to false, it does a real
      request to the motorOdom device (it is slower) */
  void useCache(bool useCache);

  /**
   * @brief Renvoie la position des 2 codeurs
   */
  bool getCoderPosition(CoderPosition &left,
                        CoderPosition &right);

 private:
  MotorOdom05* motorOdomImpl_;
  bool useCache_;
  CoderPosition oldLeft_;
  CoderPosition oldRight_;
};

#endif
