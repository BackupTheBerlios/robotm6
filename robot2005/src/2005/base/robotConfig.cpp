#include "robotConfig2005.h"

// ----------------------------------------------------------------------------
// class RobotConfigAttack2005CL 
// ----------------------------------------------------------------------------
/**
 * @class RobotConfigAttack2005CL
 * Configuration du robot d'attaque 2005
 */
RobotConfigAttack2005CL::RobotConfigAttack2005CL(bool simulated)
    : RobotConfig2005CL("Robot attack", simulated)
{
    isRobotAttack=true;
#ifndef GUMSTIX
    isMotorISA=true;
#else
    isMotorISA=false;
#endif
    startingPos=Position(72, 1725, 0);
    timeAlertBeforeEnd =(80000);

    odometerK=(.23197);
    odometerD=(310); //311.90); // augmenter pour tourner plus
    odometerEd=(0.994464); //(0.997264);
    odometerCr=2./((1./odometerEd)+1);
    odometerCl=2./(odometerEd+1);
    odometerSignLeft=(-1);
    odometerSignRight=(1);

    automaticMotorReset=(true);
    motorK=(0.00430613851);
    motorD=(338);
    motorEd=(1);
    motorCr=(1);
    motorCl=(1);
    motorSignLeft=(1);
    motorSignRight=(-1); 

    hasEnv=true;
    hasServo=true;
    hasCrane=false;
    hasSkittleDetector=false;
    hasTesla=false;
    hasAlim=true;
}

// ----------------------------------------------------------------------------
// class RobotConfigDefence2005CL
// ----------------------------------------------------------------------------
/**
 * @class 
 * Configuration du robot de defence 2005
 */
RobotConfigDefence2005CL::RobotConfigDefence2005CL(bool simulated)
    : RobotConfig2005CL("Robot defence", simulated)
{
    isRobotAttack=(false);
#ifndef GUMSTIX
    isMotorISA=true;
#else
    isMotorISA=false;
#endif
    startingPos=Position(50, 1725, 0);
    timeAlertBeforeEnd =(75000);

    odometerK=(.23197);
    odometerD=(254.);
    odometerEd=(0.997264);
    odometerCr=2./((1./odometerEd)+1);
    odometerCl=2./(odometerEd+1);
    odometerSignLeft=(-1);
    odometerSignRight=(1);

    automaticMotorReset=(true);
    motorK=(0.00430613851);
    motorD=(280);
    motorEd=(1);
    motorCr=(1);
    motorCl=(1);
    motorSignLeft=(1);
    motorSignRight=(-1); 

    hasEnv=false;
    hasServo=false;
    hasCrane=true;
    hasSkittleDetector=true;
    hasTesla=true;
    hasAlim=true;
}

