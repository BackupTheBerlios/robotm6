#ifndef __STRATEGY_H__
#define __STRATEGY_H__

#include <deque>
#include "robotBase.h"
#include "lcd.h"

// ============================================================================
// ==================================  typedef   ==============================
// ============================================================================

class StrategyCL;
class RobotMainCL;

// ============================================================================
// ===============================  class Strategy   ==========================
// ============================================================================

typedef enum InitMode {
  /** Aucune vérification et pas d'attente de la jack de départ */
  INIT_NONE,
  /** attente de la jack de départ mais pas d'auto-vérification */
  INIT_FAST,
  /** auto-vérification complete et attente de la jack de depart */
  INIT_COMPLETE
} InitMode;


/**
 * @class Strategy
 * Classe dont herites toutes les strategies du robot. Une strategie 
 * correspond en fait au programme du robot
 */
class StrategyCL : public RobotBase
{
 public:
    StrategyCL(const char* name, 
               const char* menuName,
               ClassId classId, 
               RobotMainCL* main);
    virtual ~StrategyCL();

    /** @brief main function to implent in a strategy. It shoud first run waitStart */
    virtual void run(int argc, char* argv[])=0;
    /** @brief Return the name of the strategy */
    const char* name() const;
    /** @brief Return the name of the strategy */
    const char* menuName() const;
    /** @brief Configure the robot and wait for the startJack */
    bool waitStart(InitMode mode);
    /** @brief Fonction qui libere les dernieres balles contenues dans 
     * le robot puis attend la fin du match */
    virtual bool timerAlert();
    /** @brief Function lancee au bout d'1min30 */
    virtual void gameOver();
    /** @brief Fonction lancee si l'arret d'urgence est enfonce pendant 
     * le match */
    virtual void emergencyStop();
    /** @brief Fonctionqui renvoie l'heure a laquelle l'evenement TIMER_ALERT
     * est decenche */
    Millisecond getTimerAlertTime() { return timeAlert_;}

    /** 
     * @brief Execute les commandes en cas d'arret d'urgence, de fin de match
     * Retourne false si la strategy est terminee et qu'il faut retourner 
     * au menu  \n
     * endEvt=true si l'evenement a ete catche
     */
    virtual bool checkEvents(bool &endEvt);
    /** 
     * @brief Gere les collisions 
     */
    virtual bool checkCollision(bool &collisionEvt);

 protected:
    RobotMainCL* main_;
    Millisecond timeAlert_;

    // Variables qui activent les tests pendant l'autoCheck
    bool testBumper_;
    bool testMove_;

 protected:
    /** @brief Verifie que les cartes electroniques repondent bien */
    bool autoCheck();
    /** @brief Attend la jack de depart */
    bool waitJackout();

    // === test de la communication avec les cartes UART ===
    /** @brief Verifie que la carte odometre ping */
    bool checkOdometer();
    /** @brief Verifie que la carte lcd ping */
    bool checkLcd();
    /** @brief Verifie que les bumpers ne sont pas enfonces */
    bool checkBumper();
    /** @brief Verifie que l'inter de reboot est mis */
    bool checkRebootSwitch();
    /** @brief  Verifie que les cartes UART pinguent */
    void testUARTCom();
    /** @brief Fonction lancee si l'arret d'urgence est enfonce pendant 
     * le match */
    void unlockEmergencyStop();
    
    // === Teste et calibre les capteurs ===
    /** @brief Verifie que les bumpers sont dans le bon etat */
    void testEnvDetector();
    /** @brief Test les deplacements du robot */
    void testMove();

    /** @brief change le titre utilise pour le menu lcd */
    void setMenuName(const char* name);
    /** @brief Met a jour la position de depart du robot en fonction de robotConfig.isRobotAttack*/
    void setStartingPosition();
    /** @brief Le robot va taper dans les mains des equipiers */
    void happyEnd();

 private:
    char menuName_[LCD_CHAR_PER_LINE];
	
};


// ---------------------------------------------------------------------
// Fonction d'events->wait
// ---------------------------------------------------------------------
bool evtEndMoveCollision(bool evt[]);
/** @brief Cette fonction est un EventsFn qui permet d'attendre la 
    fin d'un mouvement */
bool evtEndMoveNoCollision(bool evt[]);
/** @brief Cette fonction est un EventsFn qui permet d'attendre la 
    fin d'un mouvement */
bool evtEndMove(bool evt[]);

// ---------------------------------------------------------------------
// INLINE FUNCTIONS
// ---------------------------------------------------------------------
inline void StrategyCL::setMenuName(const char* name)
{
    strncpy(menuName_, name, LCD_CHAR_PER_LINE);
    menuName_[LCD_CHAR_PER_LINE]=0;
}

#endif // __STRATEGY_H__
