/**
 * @file robotMain.h
 *
 * @author Laurent Saint-Marcel
 *
 * Point d'entree qui alloue tous les composants necessaires pour faire un
 * programme du robot: initialise, reset le robot et permet de gerer un menu
 * de strategies
 */

#ifndef __ROBOT_MAIN_H__
#define __ROBOT_MAIN_H__

#include <deque>
#include "robotConfig.h"
#include "robotBase.h"

// ============================================================================
// ==================================  typedef   ==============================
// ============================================================================

class StrategyCL;
class RobotMainCL;
class RobotMainFullCL;
class LogCL;
class RobotTimerCL;

typedef struct MainMenuItem {
    char      text[35];
    StrategyCL* strategy;
} MainMenuItem;
typedef std::deque<MainMenuItem> MainMenuList;



// ============================================================================
// ==============================  class RobotMain   ==========================
// ============================================================================

/**
 * @class RobotMainCL
 * Classe de depart permettant de configurer le robot (qui reset
 * automatiquement les composants) et de lancer des strategies en utilisant 
 * un menu
 */
class RobotMainCL : public RobotBase
{
 public:
    /** Constructeur */
    RobotMainCL();
    /** Destructeur */
    virtual ~RobotMainCL();
    static RobotMainCL* instance() { return main_; }

    /** Ajoute une strategie au menu */
    void addStrategyToMenu(StrategyCL* strate);
    /** Enleve toutes les strategies du menu */
    void clearMenu();
    /** Menu utilisant le clavier. Il lance la strategy correspondante */
    virtual void menu(int argc, char* argv[]);
    /** Lancer une seule strategy, c'est une alternative a "menu" */
    virtual void run(StrategyCL* strategy, int argc, char* argv[]);
    /** Verifie que tous les composants sont initialisez correctement */
    bool checkInitDone(); 
    /** Met à zero le chronometre */
    virtual void startMatch();
 protected:
    /** Reset tous les composants de RobotMain */
    virtual void resetAll();

 protected:
//    RobotComponentList* robotComponents_;
    LogCL*              log_;
    RobotTimerCL*       timer_;
    MainMenuList        menu_;

    friend class StrategyCL;
    static RobotMainCL* main_;
};

// ============================================================================
// ===========================  class RobotMainFull   =========================
// ============================================================================

#include "events.h"
#include "movementManager.h"
#include "ioManager.h"

/**
 * @class RobotMainFull
 * Classe de depart permettant de configurer le robot et de lancer des 
 * strategies pour les matchs
 */
class RobotMainFullCL: public RobotMainCL
{
 public:
    RobotMainFullCL();
    ~RobotMainFullCL();

    /** Met à zero le chronometre */
    virtual void startMatch();
    /** Menu de strategies utilisant l'afficheur LCD */
    virtual void menu(int argc, char* argv[]);
    /** Lancer une seule strategy, c'est une alternative a "menu" */
    virtual void run(StrategyCL* strategy, int argc, char* argv[]);


 protected:
    /** Reset tous les composants de RobotMain */
    virtual void resetAll();

 protected:
    IoManagerCL*       ioMgr_;
    MovementManagerCL* mvtMgr_;
    EventsManagerCL*   evtMgr_;
};

#endif // __ROBOT_MAIN_H__
