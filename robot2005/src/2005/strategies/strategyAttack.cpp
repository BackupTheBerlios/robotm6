#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "servo.h"

StrategyAttackCL::StrategyAttackCL(RobotMainCL* main): 
    Strategy2005CL("StrategyAttack", "Robot attack", CLASS_STRATEGY, main),
    bridgeAvailibility_(0xFF), bridge_(BRIDGE_POS_UNKNOWN), 
    useLeftBridge_(true), bridgeDetectionByCenter_(true)
{

}

void StrategyAttackCL::run(int argc, char* argv[])
{
    Lcd->print("SophiaTeam");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);
    
    // tire les balles!
    fireCatapults();

    // va vers le pont, le cherche et le traverse
    gotoBridgeDetection();
    if (checkEndEvents()) return; // fin du match
    if (!getBridgePosBySharp()) {
        // les sharps ne marchent pas, on va tout droit pour voir...
        getNearestBridgeEntry();

    }
    findAndCrossBridge();
    if (checkEndEvents()) return; // fin du match

    return;
}

// ================ Tire des catapultes ==================
/** @brief Tire des balles avec les catapultes */
void StrategyAttackCL::fireCatapults()
{
    LOG_FUNCTION();
    Servo->setServoPosition(1, 0xFF);
    usleep(CATAPULT_AWAIT_DELAY*1000);
    Servo->disableAll();
}

// ================ Exploration de la zone ennemie ==================

/** @brief explore juste les zone ou il peut y avoir des quilles 
    d'apres le reglement */
bool StrategyAttackCL::basicSkittleExploration()
{
  return false;
}
/** @brief se promene sur le terrain adverse en allant voir partout: sur
    les bords et chez l'adversaire */
bool StrategyAttackCL::extendedSkittleExploration()
{
  return false;
}
/** @brief va faire tomber les quilles du milieu du terrain 
    (la pile de 3) */
bool StrategyAttackCL::killCenterSkittles()
{
  return false;
}
