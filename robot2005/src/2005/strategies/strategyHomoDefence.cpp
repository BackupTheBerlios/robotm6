#include "robotMain.h"
#include "strategyHomoDefence.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "tesla.h"
#include "skittleDetector.h"
#include "crane.h"
#include "robotTimer.h"

StrategyHomoDefenceCL::StrategyHomoDefenceCL(RobotMainCL* main): 
    Strategy2005CL("StrategyDefence de Homologation", "Robot defence", CLASS_STRATEGY, main)
{

}

// --------------------------------------------------------------------------
// Point d'entree du programme du robot: commande haut niveau du robot
// --------------------------------------------------------------------------
void StrategyHomoDefenceCL::run(int argc, char* argv[])
{
    Lcd->print("SophiaTeam\nDefence");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);

    // give other robot time to leave.
    usleep(DEFENCE_DELAY_AT_START*1000);

    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    Move->go2Target(150, RobotPos->y());
    Crane->setPosition(8, 0); // center top.
    Millimeter y = RobotPos->y();
    int delta = -100;
    while (y > 300) {
	Move->go2Target(150, RobotPos->y() + delta);
	Events->wait(evtEndMove);
	if (checkEndEvents()) return;
	Crane->setPosition(8, 0x0F);
	usleep(1000 * 1000); // 1s
	if (checkEndEvents()) return;
	Tesla->startTesla(TESLA_MODE_12V);
	// time to descend completely and attach skittle.
	usleep(2500 * 1000); // 2.5s
	if (checkEndEvents()) return;
	Crane->setPosition(8, 0); // top.
	usleep(2500 * 1000); // 2.5s
	Tesla->stopTesla();
	if (checkEndEvents()) return;
    }
    return;
}

