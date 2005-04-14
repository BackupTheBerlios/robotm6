#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "servo.h"
#include "gridAttack.h"
#include "robotTimer.h"

void gridAttackPeriodicTask(void *userData, Millisecond time)
{
    if (!userData) return;
    ((StrategyAttackCL*)userData)->periodicTask(time);
}

// --------------------------------------------------------------------------
// Initialisation 
// --------------------------------------------------------------------------
StrategyAttackCL::StrategyAttackCL(RobotMainCL* main): 
    Strategy2005CL("StrategyAttack", "Robot attack", CLASS_STRATEGY, main),
    bridgeAvailibility_(0xFF), bridge_(BRIDGE_POS_UNKNOWN), 
    useLeftBridge_(true), bridgeDetectionByCenter_(true), 
    grid_(NULL), lastExplorationDir_(ATTACK_EXPLORE_COL),
    attackPhase_(ATTACK_WAIT_START), skittleMiddleProcessed_(false)
{
    grid_ = new GridAttack();
    Timer->registerPeriodicFunction(gridAttackPeriodicTask,
                                    "gridAttackPeriodicTask",
                                    this);
}
// --------------------------------------------------------------------------
// Destruction 
// --------------------------------------------------------------------------
StrategyAttackCL::~StrategyAttackCL()
{
    if (grid_) delete grid_;
}

// --------------------------------------------------------------------------
// Point d'entree du programme du robot: commande haut niveau du robot
// --------------------------------------------------------------------------
void StrategyAttackCL::run(int argc, char* argv[])
{
    if (!grid_) {
        grid_ = new GridAttack();
    }
    grid_->reset();
    Lcd->print("SophiaTeam");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);

    setStartingPosition();
    waitStart(INIT_FAST);

    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);
    
    // tire les balles!
    fireCatapults();

    // va vers le pont
    setAttackPhase(ATTACK_CROSS_BRIDGE);
    Lcd->print("Goto bridge");
    gotoBridgeDetection();
    if (checkEndEvents()) return; // fin du match

    // utilise les capteurs pour trouver le pont et traverse le pont
    if (!getBridgePosBySharp()) {
        // les sharps ne marchent pas, on va tout droit pour voir...
        getNearestBridgeEntry();

    }
    Lcd->print("Find and cross\nBridge");
    findAndCrossBridge();
    if (checkEndEvents()) return; // fin du match

    // utilise une trajectoire d'exploration predefinie
    setAttackPhase(ATTACK_PREDEFINED_TRAJEC);
    Lcd->print("Pre-defined expl");
    preDefinedSkittleExploration();
    if (checkEndEvents()) return; // fin du match

    // utilise un super algo d'exploration du terrain
    setAttackPhase(ATTACK_ONLY_SKITTLES);
    Lcd->print("Explore a donf");
    while(true) {
        basicSkittleExploration();
        if (checkEndEvents()) return; // fin du match

        // TODO: penser a aller faire tomber les quilles du milieu un jour
    }
    return;
}

// ========================= Tire des catapultes ========================

// ------------------------------------------------------------------------
// Tire des balles avec les catapultes 
// ------------------------------------------------------------------------
void StrategyAttackCL::fireCatapults()
{
    // pour tirer plus vite que notre ombre on commande le servo en premier,
    // les logs viendront ensuite
    Servo->setServoPosition(1, 0xFF);
    LOG_FUNCTION();
    setAttackPhase(ATTACK_FIRE_CATAPULT);
    Lcd->print("Fire");

    // on attend un peut pour etre sur qu'on a tirer avant de couper les
    // moteurs et de bouger
    usleep(CATAPULT_AWAIT_DELAY*1000);

    Servo->disableAll();
}

// ================ Exploration de la zone ennemie ==================

// ------------------------------------------------------------------------
// va faire tomber les quilles du milieu du terrain (la pile de 3) 
// ------------------------------------------------------------------------
bool StrategyAttackCL::killCenterSkittles()
{
  return false;
}

// ------------------------------------------------------------------------
// tache periodique qui met a jour la grille pour dire ou on est deja passe
// ------------------------------------------------------------------------
void StrategyAttackCL::periodicTask(Millisecond time)
{
    if (grid_ && 
        attackPhase_ >= ATTACK_PREDEFINED_TRAJEC) {
        grid_->setVisitTime(time, RobotPos->pos());
    }
}

void StrategyAttackCL::setAttackPhase(AttackPhase phase)
{
    attackPhase_ = phase;

    switch (attackPhase_) { 
    case ATTACK_WAIT_START:
        { LOG_WARNING("Change Attack Phase to ATTACK_WAIT_START\n"); }
    break;
    case ATTACK_FIRE_CATAPULT:
        { LOG_WARNING("Change Attack Phase to ATTACK_FIRE_CATAPULT\n"); }
    break;
    case ATTACK_CROSS_BRIDGE:
        { LOG_WARNING("Change Attack Phase to ATTACK_CROSS_BRIDGE\n"); }
    break;
    case ATTACK_PREDEFINED_TRAJEC:
        { LOG_WARNING("Change Attack Phase to ATTACK_PREDEFINED_TRAJEC\n"); }
    break;
    case ATTACK_ONLY_SKITTLES:
        { LOG_WARNING("Change Attack Phase to ATTACK_ONLY_SKITTLES\n"); }
    break;
    case ATTACK_GRAND_MENAGE:       
        { LOG_WARNING("Change Attack Phase to ATTACK_GRAND_MENAGE\n"); }
    break;
    }
}
