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

#include "robotBase.h"


#define Odometer OdometerCL::instance()

static const bool ODOMETER_MANUAL    = false;
static const bool ODOMETER_AUTOMATIC = true;

class UartBuffer;
class Uart;

// ===========================================================================
// class Odometer
// ===========================================================================

/**
 * @class Odometer
 * Interface avec la carte qui controle les servo moteurs
 */

class OdometerCL : public RobotIODevice
{
 public:
  static OdometerCL* instance();
  virtual ~OdometerCL();

  // fonctions de RobotIODevice
  const char* getIoName(int coderId) const;

  /**
   * @brief Met a jour la position des 2 codeurs (sans conversion) 
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   */
  virtual bool getCoderPosition(CoderPosition &left,
                                CoderPosition &right)=0;

  /**
   * @brief Definit le mode de communication avec la carte
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   *   La positionOdom du robot dans move/robotPosition.cpp est mise a jour 
   *   automaitquement
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   * By default it is manual
   * Retourne FALSE en cas d'erreur de communication avec la carte
   */
  virtual bool setMode(bool automatic)=0;

  /**
   * @brief retourne le mode dans lequel se trouve la carte
   */
  virtual bool getMode(bool& automatic)=0;

 private:
  OdometerCL(OdometerCL const& color); // disabled

 protected:
  OdometerCL();
  static OdometerCL* odometer_; // singleton
};

// ===========================================================================
// class OdometerSimu
// ===========================================================================

/** 
 * @class OdometerSimu
 * Implementation des fonctions de class Odometer dans le cas simule
 */
class OdometerSimu : public OdometerCL
{
 public:
  virtual ~OdometerSimu();

  bool isSimu() const           { return true;  }
  bool ping()                   { return false; }
  bool reset()                  { init_=false; return false; }
  bool validate()               { return false; }
  bool autoTest(bool* ioStatus) { return true; }
 
  /**
   * @brief Met a jour la position des 2 codeurs (sans conversion) 
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   */
  bool getCoderPosition(CoderPosition &left,
			CoderPosition &right);

  /**
   * @brief Definit le mode de communication avec la carte
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   * By default it is manual
   * Retourne FALSE en cas d'erreur de communication avec la carte
   */
  bool setMode(bool automatic);

  /**
   * @brief retourne le mode dans lequel se trouve la carte
   */
  bool getMode(bool& automatic);

 protected:
  OdometerSimu();
  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);
};
  
// ===========================================================================
// class Odometer_04
// ===========================================================================

/** 
 * @class Odometer_04
 * Implementation des fonctions de class Odometer dans le cas d'une carte sur 
 * l'UART. Protocole de communication avec la carte 2004
 */
class Odometer_04 : public OdometerCL
{
 public:
  ~Odometer_04();

  bool isSimu() const           { return false; }
  bool validate()               { return false; }
  bool autoTest(bool *ioStatus) { return true; }

  /** @brief Return true if the lcd board is responding*/
  bool ping();
  /** @brief Reset the Odometer uart */
  bool reset();

  /**
   * @brief Met a jour la position des 2 codeurs (sans conversion) 
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   */
  bool getCoderPosition(CoderPosition &left,
                        CoderPosition &right);

  /**
   * @brief Definit le mode de communication avec la carte
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   * By default it is manual
   * Retourne FALSE en cas d'erreur de communication avec la carte
   */
  bool setMode(bool automatic);

  /**
   * @brief retourne le mode dans lequel se trouve la carte
   */
  bool getMode(bool& automatic);

 protected:
  Odometer_04();
  UartBuffer* uart_;

  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);

  bool filter(Byte byte);
  friend bool Odometer04Filter(Byte byte);

 private:
  CoderPosition left_;
  CoderPosition right_;
  int mode_; // 1 = automatic, 0=manual
  int bufIndex_;
  unsigned char bufferPosition_[10];
  bool dataAvailable_;
};

// ===========================================================================
// inline functions
// ===========================================================================

// ---------------------------------------------------------------------------
// Odometer::instance
// ---------------------------------------------------------------------------
inline OdometerCL* OdometerCL::instance()
{
  assert(odometer_!=NULL); // Odometer must have been initialized by IoManager
  return odometer_;
}


#endif // __ODOMETER_H__
