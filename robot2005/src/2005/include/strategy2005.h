#ifndef __STRATEGY_2005_H__
#define __STRATEGY_2005_H__

#include <deque>
#include "strategy.h"
#include "sound.h"

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
 * @class Strategy2005CL
 * Classe dont herites toutes les strategies du robot. Une strategie 
 * correspond en fait au programme du robot
 */
class Strategy2005CL : public StrategyCL
{
 public:
    Strategy2005CL(const char* name, 
                   const char* menuName,
                   ClassId classId, 
                   RobotMainCL* main);
    virtual ~Strategy2005CL();

    bool resetMotorForPrepareRobot();
    /** @brief Return the name of the strategy */
    const char* menuName() const;
    /** @brief Configure the robot and wait for the startJack */
    bool waitStart(InitMode mode);
    /** @brief Fonction qui effectue la derniere action du robot puis 
        arrete le robot */
    virtual bool timerAlert();
    /** @brief Function lancee au bout d'1min30 */
    virtual void gameOver();
    /** @brief Fonction lancee si l'arret d'urgence est enfonce pendant
     * le match */
    virtual void emergencyStop();
    /** 
     * @brief Execute les commandes en cas d'arret d'urgence, de fin de match
     * Retourne false si la strategy est terminee et qu'il faut retourner 
     * au menu  \n
     * endEvt=true si l'evenement a ete catche
     */
    virtual bool checkEndEvents();

 protected:
    // Variables qui activent les tests pendant l'autoCheck
    bool testBumper_;
    bool testMove_;

 protected:
    /** @brief Verifie que les cartes electroniques repondent bien :
     Overload cette fnction pour rajouter d'autres tests. C'est la
     fonction qui est appelee quand on fait un waitStart(INIT_COMPLETE) */
    virtual bool autoCheck();
    /** @brief Attend la jack de depart. C'est la
     fonction qui est appelee quand on fait un waitStart(INIT_FAST) */
    bool waitJackout();
    /** @brief verifie que l'arret d'urgence n'est pas enfonce */
    void unlockEmergencyStop();
    /** @brief verifie que toutes les cartes sont connectees */
    void testDevicesConnection();
    /** @brief verifie que le bouton reboot a ete enclenche */
    bool checkRebootSwitch();
    /** @brief verifie l'asservissement des roues et les odometres */
    bool testMove();
    // === test de la communication avec les cartes UART ===
    bool checkAlim();
    bool checkTesla();
    bool checkSkittleDetector();
    bool checkCrane();
    bool checkEnv();
    bool checkServo();
    bool checkMove();
    bool checkBumper();
    bool checkLcd();

    /** @brief change le titre utilise pour le menu lcd pour decrire la 
        strategie */
    void setMenuName(const char* name);
    /** @brief Met a jour la position de depart du robot en fonction de 
        robotConfig.isRobotAttack*/
    void setStartingPosition();
    /** @brief Le robot va taper dans les mains des equipiers */
    void happyEnd();
    /** @brief affiche un texte sur le lcd et attends la pression d'une
        touche. Le bouton  yes renvoie true, le bouton NO renvoie false */
    bool menu(const char*fmt, ...);

    /** @brief defini les joues a la fin du match */
    void setEndingSounds(SoundId victory, SoundId defeat);

 private:
    SoundId soundDefeat_;
    SoundId soundVictory_;
    bool    matchIsFinished_;
};


// ---------------------------------------------------------------------
// Fonction d'events->wait
// ---------------------------------------------------------------------
/** @brief Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
    mouvement sans detecter les collisions ou le patinage du robot.
    Attends: find du mouvment, arret d'urgence, fin du match, timer_alert */
bool evtEndMoveCollision(bool evt[]);
/** @brief Cette fonction est un EventsFn qui permet d'attendre la 
    fin d'un mouvement en testant en plus les collisions basees sur les 
    bumper */
bool evtEndMoveNoCollision(bool evt[]);
/** @brief Cette fonction est un EventsFn qui permet d'attendre la 
    fin d'un mouvement en testant en plus les collisions basees sur les 
    bumper */
bool evtEndMove(bool evt[]);
/** @brief Cette fonction est un EventsFn qui permet d'attendre la pression
    d'une touche du lcd */
bool evtDialogButtonFilter(bool evt[]);
/** @brief  Cette fonction est un EventsFn qui permet d'attendre la presion de
    la touche yes/no ou du changement de l'interrupteur de reboot */
bool evtRebootSwitch(bool evt[]);
// ---------------------------------------------------------------------
// INLINE FUNCTIONS
// ---------------------------------------------------------------------
inline void Strategy2005CL::setMenuName(const char* name)
{
    strncpy(menuName_, name, LCD_CHAR_PER_LINE);
    menuName_[LCD_CHAR_PER_LINE]=0;
}

#endif // __STRATEGY_2005_H__
