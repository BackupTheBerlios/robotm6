#include "strategy.h"
#include "robotMain.h"
#include "log.h"
#include "motor.h"
#include "odometer.h"
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

    //RobotPos->set(0,0,0);

    Move->go2Target(Point(1000, 1500)); // le robot tourne sur lui meme et va au point (1000, 1800)
    Events->wait(evtEndMove);
    
    Move->rotate(d2r(-90)); // tourne de 90degre a droite
    Events->wait(evtEndMove);

    Move->forward(400); // avance de 20cm (pas de regulation)
    Events->wait(evtEndMove);

    RobotPos->print();
    Move->stop();

    sleep(5);
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

    //ROBOT_POS->set(0,0,0);
    Trajectory t;
    t.push_back(Point(RobotPos->x(), RobotPos->y()));
    t.push_back(Point(RobotPos->x()+100, RobotPos->y()));
    t.push_back(Point(RobotPos->x()+300, RobotPos->y()-50));
    t.push_back(Point(1350-300, 1725+50));
    t.push_back(Point(1350-100, 1725));
    t.push_back(Point(1350, 1725));
    Move->followTrajectory(t/*, TRAJECTORY_SPLINE*/);
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


#ifdef SIMULATED
  config = new RobotConfigSimuCL();
#else
  config = new RobotConfigCL();
#endif

  robotMain = new RobotMainFullCL();
  strategy1 = new TestMoveStrategy1CL(robotMain);
  strategy2 = new TestMoveStrategy2CL(robotMain);

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);
  robotMain->run(strategy2, argc, argv);

  while(1) {sleep(1);}
  delete strategy2;
  delete strategy1;
  delete robotMain;
  delete config;

  return 0;
}
