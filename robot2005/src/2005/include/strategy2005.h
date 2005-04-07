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

    /** @brief Return the name of the strategy */
    const char* menuName() const;
    /** @brief Configure the robot and wait for the startJack */
    bool waitStart(InitMode mode);
    /** @brief Fonction qui effectue la derniere action du robot puis arrete le robot */
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
    /**
     * @brief Gere les collisions 
     */
    virtual bool checkCollision(bool &collisionEvt);

 protected:
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
