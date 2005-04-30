#include "devices/odometer.h"

// =========================================================================
// Odometer
// =========================================================================
// Interface avec la carte de detection de couleur
// =========================================================================

OdometerCL* OdometerCL::odometer_=NULL;

// -------------------------------------------------------------------------
// OdometerCL::OdometerCL
// -------------------------------------------------------------------------
OdometerCL::OdometerCL() : 
  RobotDeviceCL("Odometer", CLASS_ODOMETER),
  notMovingCounterLeft_(0), notMovingCounterRight_(0), notMovingSeuil_(ODOMETER_NOT_MOVING_SEUIL)
{
  assert(!odometer_);
  odometer_=this;
}

// -------------------------------------------------------------------------
// OdometerCL::~OdometerCL
// -------------------------------------------------------------------------
OdometerCL::~OdometerCL()
{
  odometer_=NULL;
}
 
