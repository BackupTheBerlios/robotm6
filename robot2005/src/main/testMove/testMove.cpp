#include "strategy.h"
#include "robotMain.h"
#include "log.h"
#include "motor.h"

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
    ROBOT_POS->set(300,1000,0);
    waitStart(INIT_NONE);
    MOVE->enableAccelerationController(false);
    MVTMGR->enableAutomaticReset(false);

    MOVE->go2Target(Point(1000, 1000));
    Events->wait(evtEndMove);
    ROBOT_POS->print();
    MOVE->stop();
    sleep(5);
    return;
}



int main(int argc, char* argv[])
{
  RobotConfigCL*  config;
  RobotMainCL*    robotMain=NULL;
  StrategyCL*     strategy1=NULL;


#ifdef SIMULATED
  config = new RobotConfigSimuCL();
#else
  config = new RobotConfigCL();
#endif

  robotMain = new RobotMainFullCL();
  strategy1 = new TestMoveStrategy1CL(robotMain);

  robotMain->run(strategy1, argc, argv);

  while(1) {sleep(1);}
  delete strategy1;
  delete robotMain;
  delete config;

  return 0;
}
