#include "robotMain2005.h"
#include "simulatorClient.h"
#include "sound.h"
#include "log.h"
#include "sound.h"
#include "robotConfig2005.h"

// -------------------------------------------------------------------------
// RobotMain2005CL::RobotMain2005CL
// -------------------------------------------------------------------------
RobotMain2005CL::RobotMain2005CL() : 
    RobotMainCL()
{ 
    if (RobotConfig->needSimulator()) {
        LOG_INFO("needSimulator == true\n");
        if (!Simulator->connectToServer()) {
            LOG_ERROR("Cannot connect to SIMULATOR server\n");
        } else {
            Simulator->setRobotName(RobotConfig->name);
            if (RobotConfig2005->isRobotAttack) {
                Simulator->setRobotModel(ROBOT_MODEL_ATTACK);
            } else {
                Simulator->setRobotModel(ROBOT_MODEL_DEFENCE);  
            } 
            Simulator->setRobotMotorCoef(RobotConfig->getMotorD(),
                                         RobotConfig->motorK, 
                                         RobotConfig->motorSignLeft,
                                         RobotConfig->motorSignRight);
            Simulator->setRobotOdomCoef(RobotConfig->odometerD,
                                        M_PI/2, 
                                        RobotConfig->odometerK, 
                                        RobotConfig->odometerSignLeft,
                                        RobotConfig->odometerSignRight);
        }
    }
    if (RobotConfig2005->isRobotAttack) {
        Log->robotModel(ROBOT_MODEL_ATTACK);
    } else {
        Log->robotModel(ROBOT_MODEL_DEFENCE);
    }

    Sound->play(SOUND_BONJOUR);
}
