#include "implementation/odometerSimu.h"
#include "simulatorClient.h"

// =========================================================================
// Odometer
// =========================================================================
// Interface avec la carte detecteur d'environement 2003
// =========================================================================

// -------------------------------------------------------------------------
// OdometerSimu::OdometerSimu
// -------------------------------------------------------------------------
OdometerSimu::OdometerSimu() : 
  OdometerCL()
{
}

// -------------------------------------------------------------------------
// OdometerSimu::~OdometerSimu
// -------------------------------------------------------------------------
OdometerSimu::~OdometerSimu()
{
 
}

// -------------------------------------------------------------------------
// OdometerSimu::getCoderPosition
// -------------------------------------------------------------------------
// Met a jour la position des 2 codeurs (sans conversion)
// En mode automatique la carte envoie toute seule des donnees et
//   getCoderPosition retourne true si la carte a mise a jour la
//   position des codeurs depuis la derniere requete getCoderPosition
// En mode manuel getCoderPosition va directement lire la valeur
//   des codeurs et retourne true si la communication s'est bien passee.
// -------------------------------------------------------------------------
bool OdometerSimu::getCoderPosition(CoderPosition &left,
				    CoderPosition &right)
{
    Simulator->getOdomPosition(left, right);
    return true;
}
