#include "odometer.h"

// =========================================================================
// Odometer
// =========================================================================
// Interface avec la carte de detection de couleur
// =========================================================================

Odometer* Odometer::odometer_=NULL;

// -------------------------------------------------------------------------
// Odometer::Odometer
// -------------------------------------------------------------------------
Odometer::Odometer() : 
  RobotIODevice("Odometer", CLASS_ODOMETER, 2)
{
  assert(!odometer_);
  odometer_=this;
}

// -------------------------------------------------------------------------
// Odometer::~Odometer
// -------------------------------------------------------------------------
Odometer::~Odometer()
{
  odometer_=NULL;
}
 
// -------------------------------------------------------------------------
// Odometer::getIoName
// -------------------------------------------------------------------------
const char* Odometer::getIoName(int coderId) const
{
  static char txt[32];
  sprintf(txt, "Odometer %d", coderId);
  return txt;
}
