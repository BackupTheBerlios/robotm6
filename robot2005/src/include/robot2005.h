#ifndef __ROBOT_2005_H__
#define __ROBOT_2005_H__

#include "robotConfig.h"

// ----------------------------------------------------------------------------
// class RobotConfigAttack2005CL 
// ----------------------------------------------------------------------------
/**
 * @class RobotConfigAttack2005CL
 * Configuration du robot d'attaque 2005
 */
class RobotConfigAttack2005CL : public RobotConfigCL
{
 public:
    void reset() {}
 private:
    // only the main can create the config
    RobotConfigAttack2005CL(bool simulated=false);
    friend int main(int, char*[]);
};

// ----------------------------------------------------------------------------
// class RobotConfigDefence2005CL
// ----------------------------------------------------------------------------
/**
 * @class 
 * Configuration du robot de defence 2005
 */
class RobotConfigDefence2005CL : public RobotConfigCL
{
 public:
    void reset() {}
 private:
    // only the main can create the config
    RobotConfigDefence2005CL(bool simulated=false);
    friend int main(int, char*[]);
};

#endif // __ROBOT_2005_H__
