#ifndef __STRATEGY_H__
#define __STRATEGY_H__

#include <deque>
#include "robotBase.h"
#include "lcd.h"

// ============================================================================
// ==================================  typedef   ==============================
// ============================================================================

class StrategyCL;
class RobotMainCL;

/**
 * @class Strategy
 * Classe dont herites toutes les strategies du robot. Une strategie 
 * correspond en fait au programme du robot
 */
class StrategyCL : public RobotBase
{
 public:
    StrategyCL(const char* name, 
               const char* menuName,
               ClassId classId, 
               RobotMainCL* main);
    virtual ~StrategyCL();

    /** @brief main function to implent in a strategy. It shoud first run waitStart */
    virtual void run(int argc, char* argv[])=0;

    /** @brief Return the name of the strategy */ 
    const char* name() const;
    /** @brief Return the name of the strategy used in the menu */ 
    const char* menuName() const;
    /** menu qui affiche un texte et renvoie true=YES or false=NO */
    virtual bool menu(const char*fmt, ...)=0;
     
 protected:
    RobotMainCL* main_;
    char menuName_[35];
};

#endif // __STRATEGY_H__
