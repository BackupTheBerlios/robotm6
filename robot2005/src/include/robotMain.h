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

class Strategy;
class RobotMain;
class RobotMainFull;
class Log;

typedef struct MainMenuItem {
    char      text[35];
    Strategy* strategy;
} MainMenuItem;
typedef std::deque<MainMenuItem> MainMenuList;



// ============================================================================
// ==============================  class RobotMain   ==========================
// ============================================================================

#include "robotTimer.h"
/**
 * @class RobotMain
 * Classe de depart permettant de configurer le robot (qui reset
 * automatiquement les composants) et de lancer des strategies en utilisant 
 * un menu
 */
class RobotMain : public RobotBase
{
 public:
    /** Constructeur */
    RobotMain();
    /** Destructeur */
    virtual ~RobotMain();
    static RobotMain* instance() { return main_; }

    /** Ajoute une strategie au menu */
    void addStrategyToMenu(Strategy* strate);
    /** Enleve toutes les strategies du menu */
    void clearMenu();
    /** Menu utilisant le clavier. Il lance la strategy correspondante */
    virtual void menu(int argc, char* argv[]);
    /** Lancer une seule strategy, c'est une alternative a "menu" */
    virtual void run(Strategy* strategy, int argc, char* argv[]);
    /** Verifie que tous les composants sont initialisez correctement */
    bool checkInitDone(); 
    /** Met à zero le chronometre */
    virtual void startMatch();
 protected:
    /** Reset tous les composants de RobotMain */
    virtual void resetAll();

 protected:
    RobotComponentList* robotComponents_;
    Log*                log_;
    RobotTimer*         timer_;
    MainMenuList        menu_;

    friend class Strategy;
    static RobotMain* main_;
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
class RobotMainFull: public RobotMain
{
 public:
    RobotMainFull();
    ~RobotMainFull();

    /** Met à zero le chronometre */
    virtual void startMatch();
    /** Menu de strategies utilisant l'afficheur LCD */
    virtual void menu(int argc, char* argv[]);
    /** Lancer une seule strategy, c'est une alternative a "menu" */
    virtual void run(Strategy* strategy, int argc, char* argv[]);


 protected:
    /** Reset tous les composants de RobotMain */
    virtual void resetAll();

 protected:
    IoManager*       ioMgr_;
    MovementManager* mvtMgr_;
    EventsManager*   evtMgr_;
};

#endif // __ROBOT_MAIN_H__
