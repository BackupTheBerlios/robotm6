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
    ROBOT_POS->setOdometerType(ODOMETER_MOTOR);
    //ROBOT_POS->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    MOVE->enableAccelerationController(false);
    MVTMGR->enableAutomaticReset(false);

    //ROBOT_POS->set(0,0,0);

    MOVE->go2Target(Point(1000, 1800)); // le robot tourne sur lui meme et va au point (1000, 1800)
    Events->wait(evtEndMove);
    
    MOVE->rotate(d2r(-90)); // tourne de 90degre a droite
    Events->wait(evtEndMove);

    MOVE->forward(200); // avance de 20cm (pas de regulation)
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

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);
  robotMain->run(strategy1, argc, argv);

  while(1) {sleep(1);}
  delete strategy1;
  delete robotMain;
  delete config;

  return 0;
}
