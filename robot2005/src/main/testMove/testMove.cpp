#include "strategy.h"
#include "robotMain.h"
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"
#include "motor.h"
#include "odometer.h"
#include "robot2005.h"
// ----------------------------------------------------------------------------
// evtEndMove
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement
// ----------------------------------------------------------------------------
inline bool evtEndMove(bool evt[])
{
  return evt[EVENTS_MOVE_END];
}


/** @class TestMoveStrategy
 * Teste les déplacements du robot (MovementManager et Move)
 */
class TestMoveStrategy1CL : public StrategyCL
{
 public :
  TestMoveStrategy1CL(RobotMainCL* main): 
    StrategyCL("TestMove", "testMove1", CLASS_STRATEGY, main){}
  virtual void run(int argc, char*argv[]);
};

void TestMoveStrategy1CL::run(int argc, char* argv[])
{
    LCD->print("SophiaTeam");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    sleep(5);

    // ====================    
    // HERE ICI HERE ICI HERE ICI
    // ====================    

    RobotPos->set(0,0,0);
    Move->rotateFromAngle(d2r(720));
    //Move->go2Target(Point(1500, 0));
    Events->wait(evtEndMove);
    
    /*
    Move->rotate(d2r(-90)); // tourne de 90degre a droite
    Events->wait(evtEndMove);

    Move->forward(400); // avance de 20cm (pas de regulation)
    Events->wait(evtEndMove);
    */
    RobotPos->print();
    Move->stop();

    sleep(5);
    RobotPos->print();
    return;
}

/** @class TestMoveStrategy2CL
 * Teste les déplacements du robot (MovementManager et Move)
 */
class TestMoveStrategy2CL : public StrategyCL
{
 public :
  TestMoveStrategy2CL(RobotMainCL* main): 
    StrategyCL("TestMove", "testMove2", CLASS_STRATEGY, main){}
  virtual void run(int argc, char*argv[]);
};

void TestMoveStrategy2CL::run(int argc, char* argv[])
{
    LCD->print("SophiaTeam");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    RobotPos->set(490, 1675, 0);
    Trajectory t;
    t.push_back(Point(RobotPos->x(), RobotPos->y()));
    t.push_back(Point(900,1675));
    t.push_back(Point(1200,1580));
    t.push_back(Point(1500,1580));
    t.push_back(Point(2140,1580));
    t.push_back(Point(2290,1580));
    t.push_back(Point(2590,1650));
    Move->followTrajectory(t/*, TRAJECTORY_SPLINE*/);
    Events->wait(evtEndMove);

    Move->go2Target(Point(3190,1650));
    Events->wait(evtEndMove);
    
    Move->go2Target(Point(3190,750));
    Events->wait(evtEndMove);
    
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1350));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2590, 1350));
    Events->wait(evtEndMove);

    Move->go2Target(Point(2250, 1270));
    Events->wait(evtEndMove);

    Move->stop();
    RobotPos->print();
    sleep(2);
    // detection du pont

    Log->emergencyStopPressed(true);
    sleep(5);
    return;
}

int main(int argc, char* argv[])
{
  RobotConfigCL*  config;
  RobotMainCL*    robotMain=NULL;
  StrategyCL*     strategy1=NULL;
  StrategyCL*     strategy2=NULL;

#ifndef SIMULATED
#define SIMULATED false
LOG_INFO("MODE REAL\n");
#else
LOG_INFO("SIMULATED\n");
#endif

#ifdef ROBOT_DEFENCE
  LOG_INFO("ROBOT_DEFENCE\n");
  config = new RobotConfigDefence2005CL(SIMULATED);
#else
  LOG_INFO("ROBOT_ATTACK\n");
  config = new RobotConfigAttack2005CL(SIMULATED);
#endif

  robotMain = new RobotMainFullCL();
  strategy1 = new TestMoveStrategy1CL(robotMain);
  strategy2 = new TestMoveStrategy2CL(robotMain);

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);
  //// ICI ICI ICI => strategy2 = traverse le pont
  robotMain->run(strategy2, argc, argv); // traverse le pont
  //robotMain->run(strategy1, argc, argv); // test des deplacements

  while(1) {sleep(1);}
  delete strategy2;
  delete strategy1;
  delete robotMain;
  delete config;

  return 0;
}
