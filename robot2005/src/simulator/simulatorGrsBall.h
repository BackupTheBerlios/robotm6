/**
 * @file simulatorGrsBall.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */

#ifndef __SIMULATOR_GRS_BALL_H__
#define __SIMULATOR_GRS_BALL_H__

#include <string>

#include "robotBase.h"
#include "simulatorBase.h"

class SimulatorGrsBall;
class SimulatorSkittle;
class SimulatorRobot;
class Segment;

/**
 * @class SimulatorSkittle
 * Simule les déplacements du robot, les balles, les autres robot sur la 
 * table et leur interaction
 */
class SimulatorGrsBall : public RobotBase {
 public:
    SimulatorGrsBall();
    virtual ~SimulatorGrsBall(); 

    // -------------------------------------------------------------------
    // set
    // -------------------------------------------------------------------
    void set(GRSBall* ball)   { ball_=ball;}
  
    // -------------------------------------------------------------------
    // update the simulated robot position
    // -------------------------------------------------------------------
    /** @brief calcul la nouvell position du robot en mettant a jour la 
        consigne des moteurs */
    void updatePosition();
    /** @brief cete fonction permet de restaurer les donnes du robot en cas de
        nouvelle position invalide. */
    void setNewPositionValid();
    void checkPosAndWall();
    void checkPosAndBridge(BridgePosition const& bridge);
    //void checkPosAndOtherRobot(SimulatorRobot* other);
    void checkPosAndGRSBall(SimulatorGrsBall* ball);
    void checkPosAndSkittle(SimulatorSkittle* skittle);
    bool getIntersection(Point const&   captor, 
                         Segment const& captorVision, 
                         Millimeter     zPosCaptor, 
                         Point&         intersectionPt);
    // -------------------------------------------------------------------
    // private members
    // -------------------------------------------------------------------
 private:
    GRSBall* ball_;
    Millimeter speedX_;
    Millimeter speedY_;
    bool isValid_;
};

#endif // __SIMULATOR_GRS_BALL_H__
