#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"

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

    gotoBridgeDetection();
    getBridgePosBySharp();
    gotoBridgeEntry();

    Move->go2Target(Point(2590,1350));
    Events->wait(evtEndMove);

    Move->go2Target(Point(3190,1350));
    Events->wait(evtEndMove);

    Move->go2Target(Point(3190,750));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1050));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2890,1050));
    Events->wait(evtEndMove);

    Move->go2Target(Point(2890, 1650));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1650));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2590, 1650));
    Events->wait(evtEndMove);

    Move->go2Target(Point(2590, 750));
    Events->wait(evtEndMove);

    Move->stop();
    RobotPos->print();
    sleep(2);
    // detection du pont

    Log->emergencyStopPressed(true);
    sleep(5);
    return;
}

// ================ Tire des catapultes ==================
/** @brief Tire des balles avec les catapultes */
void StrategyAttackCL::fireCatapults()
{
}

static const Millimeter xToDetectBridgeBySharp = 1300;
// ================ Detection et traversee du pont ==================

/** @brief va a l'endroit ou on detecte les pont par capteurs sharps */
bool StrategyAttackCL::gotoBridgeDetection()
{
  Trajectory t;
  t.push_back(Point(RobotPos->x(), RobotPos->y()));
  t.push_back(Point(900, RobotPos->y()));
  if (bridgeDetectionByCenter_) {
    // va vers le pont du milieu
    t.push_back(Point(xToDetectBridgeBySharp-250,1275));
    t.push_back(Point(xToDetectBridgeBySharp,1275));
  } else {
    // va vers la gauche du terrain pour detecter la position du pont d'un coup
    t.push_back(Point(xToDetectBridgeBySharp-250,1725));
    t.push_back(Point(xToDetectBridgeBySharp,1725));
  }
  Move->followTrajectory(t/*, TRAJECTORY_SPLINE*/);
  Events->wait(evtEndMove);
  // todo manage events
  Move->stop();
  return false;
}
/** @brief met a jour la variable bridge_ en fonction de ce que disent les 
    sharps */
bool StrategyAttackCL::getBridgePosBySharp()
{
/*
  BRIDGE_POS_BORDURE=1,        
  BRIDGE_POS_MIDDLE_BORDURE=2,
  BRIDGE_POS_MIDDLE_CENTER=3,
  BRIDGE_POS_CENTER=4
*/
  if (bridgeDetectionByCenter_) {
    
  } else {

  }
  // TODO
  bridge_=BRIDGE_POS_CENTER; // test
  return false;
}
/** @brief se place en face du pont ou on peut utiliser les bumpers pour 
    savoir si le pont est bien la. La variable bridge doit deja etre mise 
    a jour */
bool StrategyAttackCL::gotoBridgeEntry(Millimeter y, 
				       bool rotateLeft, 
				       bool rotateRight)
{
  if (fabs(RobotPos->y() - y)<50) {
    LOG_INFO("gotoBridgeEntry Easy:%d\n", (int)y);
    Move->go2Target(Point(1470, y));
    Events->wait(evtEndMove);
  } else {
    LOG_INFO("gotoBridgeEntry Hard:%d\n", (int)y);
    // verifier la direction:
    Move->backward(200);
    Events->wait(evtEndMove);
    // TODO
    Move->go2Target(Point(1470, y));
    Events->wait(evtEndMove);
  }
  
  return false;
}
/** @brief se place en face du pont ou on peut utiliser les bumpers pour 
    savoir si le pont est bien la. La variable bridge doit deja etre mise 
    a jour */
bool StrategyAttackCL::gotoBridgeEntry()
{
  if (bridgeDetectionByCenter_) {
    if (bridge_ == BRIDGE_POS_CENTER || 
	bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
      return gotoBridgeEntry(1275);
    } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
      return gotoBridgeEntry(1725);
    } else if (bridge_ == BRIDGE_POS_BORDURE) {
      return gotoBridgeEntry(1875, true);
    }
  } else {
    if (bridge_ == BRIDGE_POS_CENTER) {
      return gotoBridgeEntry(1425);
    } else if (bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
      return gotoBridgeEntry(1575);
    } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
      return gotoBridgeEntry(1725);
    } else if (bridge_ == BRIDGE_POS_BORDURE) {
      return gotoBridgeEntry(1875, true);
    }
  }
  return false;
}
/** @brief verifie que le pont est bien la ! */
bool StrategyAttackCL::getBridgePosByBumper()
{
  return false;
}
/** @brief traverse le pont : detection de collisions...*/
bool StrategyAttackCL::crossBridge()
{
  Point tgt(2200, 1275);
  if (bridgeDetectionByCenter_) {
    if (bridge_ == BRIDGE_POS_CENTER || 
	bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
      tgt.y=1275;
    } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
      tgt.y=1725;
    } else if (bridge_ == BRIDGE_POS_BORDURE) {
      tgt.y=1875;
    }
  } else {
    if (bridge_ == BRIDGE_POS_CENTER) {
      tgt.y=1425;
    } else if (bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
      tgt.y=1575;
    } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
      tgt.y=1725;
    } else if (bridge_ == BRIDGE_POS_BORDURE) {
      tgt.y=1875;
    }
  }
  LOG_INFO("crossBridge: %d\n", (int)tgt.y);
  Move->go2Target(tgt);
  Events->wait(evtEndMove);
  return true;
}
/** @brief va jusq'a l'aute pont et change l'etat de la variable
    useLeftBridge_ */
bool StrategyAttackCL::gotoOtherBridge()
{
  return false;
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
