#include "odometer.h"


// =========================================================================
// Odometer
// =========================================================================
// Interface avec la carte detecteur d'environement 2003
// =========================================================================

// -------------------------------------------------------------------------
// OdometerSimu::OdometerSimu
// -------------------------------------------------------------------------
OdometerSimu::OdometerSimu() : 
  Odometer()
{
  init_=true;
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
  return false;
}

// -------------------------------------------------------------------------
// OdometerSimu::setMode
// -------------------------------------------------------------------------
//  Definit le mode de communication avec la carte
//  En mode automatique la carte envoie toute seule des donnees et 
//    getCoderPosition retourne true si la carte a mise a jour la 
//    position des codeurs depuis la derniere requete getCoderPosition
//  En mode manuel getCoderPosition va directement lire la valeur 
//    des codeurs et retourne true si la communication s'est bien passee.
//  By default it is manual
// Retourne FALSE en cas d'erreur de communication avec la carte
// -------------------------------------------------------------------------
bool OdometerSimu::setMode(bool automatic)
{
  return false;
}

// -------------------------------------------------------------------------
// OdometerSimu::getMode
// -------------------------------------------------------------------------
// retourne le mode dans lequel se trouve la carte
// -------------------------------------------------------------------------
bool OdometerSimu::getMode(bool& automatic)
{
  return false;
}
