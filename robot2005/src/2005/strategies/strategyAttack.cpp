#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"

StrategyAttackCL::StrategyAttackCL(RobotMainCL* main): 
    Strategy2005CL("StrategyAttack", "Robot attack", CLASS_STRATEGY, main)
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
