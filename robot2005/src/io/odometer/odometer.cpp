#include "odometer.h"

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
  RobotIODevice("Odometer", CLASS_ODOMETER, 2)
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
 
// -------------------------------------------------------------------------
// Odometer::getIoName
// -------------------------------------------------------------------------
const char* OdometerCL::getIoName(int coderId) const
{
  static char txt[32];
  sprintf(txt, "Odometer %d", coderId);
  return txt;
}
