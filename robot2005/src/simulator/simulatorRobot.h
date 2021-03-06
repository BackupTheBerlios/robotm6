/**
 * @file simulatorRobot.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */

#ifndef __SIMULATOR_ROBOT_H__
#define __SIMULATOR_ROBOT_H__

#include <string>

#include "robotBase.h"
#include "simulatorBase.h"
#include "geometry2D.h"

class SimulatorGrsBall;
class SimulatorSkittle;

/**
 * @class SimulatorRobot
 * Simule les déplacements du robot, les balles, les autres robot sur la 
 * table et leur interaction
 */
class SimulatorRobot : public RobotBase {
 public:
    SimulatorRobot();
    virtual ~SimulatorRobot(); 

    // ------------------------------------------------------------------------
    // set
    // ------------------------------------------------------------------------   
    void setRobotPos(Position const& pos)   { realPos_ = pos; 
                                              simuPosFirst_ = true;
                                              isDead_=0;
                                              needSendDisplayInfo_=true;}
    void resetRobotEstimatedPosition()      { estimatedPos_ = realPos_; }
    void setRobotEstimatedPos(Position const& pos)   { estimatedPos_ = pos; }
    void setRobotName(const char* name)     { name_ = std::string(name); 
                                              needSendDisplayInfo_=true;}
    void setRobotWeight(SimuWeight weight)  { weight_ = weight; }
    void setRobotModel(RobotModel model)    { model_ = model; 
                                              needSendDisplayInfo_=true;
                                              if (model_==ROBOT_MODEL_ATTACK)
                                                  setBorderRobotAttack();
                                              else if (model_==ROBOT_MODEL_DEFENCE)
                                                  setBorderRobotDefence();
                                              else setBorderRobotBrick();
                                            }
    void setModeBrick(bool brick)           { brick_ = brick;
                                              needSendDisplayInfo_=true;}
    void setRobotMotorCoef(Millimeter D, Millimeter K, double speedL, double speedR)   
                                            {   D_ = D; motorK_ = K; 
                                                motorSpeedL_ = speedL; 
                                                motorSpeedR_ = speedR;}
    void setRobotOdomCoef(Millimeter D, Radian R, Millimeter K, double speedL, double speedR)  
                                            {   odomLeftPt_=Point(D, R);
                                                odomRightPt_=Point(D,-R); 
                                                odomK_ = K;
                                                odomSpeedL_=speedL;
                                                odomSpeedR_=speedR; }
    void setJackin(bool jackin)             { jackin_ = jackin; }
    void setEmergencyStop(bool es)          { emergencyStop_ = es; }
    void setLcdButtonsYes(bool yes)         { lcdBtnYes_=yes; }
    void setLcdButtonsNo(bool no)           { lcdBtnNo_=no; }
    void setSpeed(MotorSpeed left, 
		  MotorSpeed right)         {speedLeft_=left;speedRight_=right;}
    void setLcdMessage(const char* message) { lcdMessage_ = std::string(message); 
                                              needSendDisplayInfo_=true;}
    void displayInfoSent()                  { needSendDisplayInfo_=false; }
    void setMatchStatusReset()         { matchStatus_=SIMU_STATUS_NEED_RESET; }
   
    
    // ------------------------------------------------------------------------
    // get
    // ------------------------------------------------------------------------
    bool needSendDisplayInfo() const { 
	return needSendDisplayInfo_; 
    }
    bool isJackin() const { 
        return jackin_;
    }
    int isDead() const { 
        return isDead_;
    }
    bool isEmergencyStop() const { 
        return emergencyStop_; 
    }
    bool isModeBrick() const { 
        return brick_; 
    }
    void getLcdButtonsState(bool& btnYes, bool& btnNo) const { 
        btnYes = lcdBtnYes_;
        btnNo  = lcdBtnNo_;
    }
    void getPwm(MotorPWM& left, MotorPWM& right) const { 
        left  = pwmLeft_;
        right = pwmRight_;
    }
    void getMotorPosition(MotorPosition& left, 
			  MotorPosition& right) const {
        left  = motorLeft_;
        right = motorRight_;
    }
    void getOdomPosition(CoderPosition& left, 
                         CoderPosition& right) const {
        left  = odomLeft_;
        right = odomRight_;
    }
    void getRobotEstimatedPosition(Point& pt, Radian& dir) const {
        pt  = estimatedPos_.center;
        dir = estimatedPos_.direction;
    }
    void getRobotRealPosition(Point& pt, Radian& dir) const {
        pt  = realPos_.center;
        dir = realPos_.direction;
    }
    const char* getLcdMessage() const { 
        return lcdMessage_.c_str();
    }
    const char* getRobotName() const { 
        return name_.c_str();
    }
    RobotModel getRobotModel() const { 
        return model_;
    }
    /** 
     * @brief renvoie le status et le passe en mode SIMU_STATUS_WAIT_START 
     * c'est pour qu'on envoie qu'une seule fois SIMU_STATUS_NEED_RESET aux
     * robots 
     */
    SimuMatchStatus getMatchStatus() {
        SimuMatchStatus result = matchStatus_;
        if (matchStatus_==SIMU_STATUS_NEED_RESET) {
            matchStatus_ = SIMU_STATUS_WAIT_START;
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // update the simulated robot position
    // ------------------------------------------------------------------------
    /** @brief calcul la nouvell position du robot en mettant a jour la 
        consigne des moteurs */
    void updatePosition();
    void updateOdometer();
    /** @brief cete fonction permet de restaurer les donnes du robot en cas de
        nouvelle position invalide. */
    void setNewPositionValid();
    void checkPosAndWall();
    void checkPosAndBridge(BridgePosition const& bridge);
    void checkPosAndOtherRobot(SimulatorRobot* other);
    void checkPosAndGRSBall(SimulatorGrsBall* ball);
    void checkPosAndSkittle(SimulatorSkittle* skittle);
    bool getIntersection(Point const&   captor, 
                         Segment const& captorVision, 
                         Millimeter     zPosCaptor, 
                         Point&         intersectionPt);
 protected:
    void setRealPos(Position const& pos);
    bool checkSegmentIntersectionWithRobot(Segment const& seg,
                                           Millimeter z,
                                           Point& intersectionPt);
    bool checkCircleIntersectionWithRobot(Circle const& circle,
                                          Millimeter z,
                                          Point& intersectionPt);
    void convertBorderToCylindric(Point const& center);
    void convertBorderToOrthogonal(Position const& pos);
    void setBorderRobotAttack();
    void setBorderRobotDefence();
    void setBorderRobotBrick();

    // ------------------------------------------------------------------------
    // private members
    // ------------------------------------------------------------------------
 private:
    std::string name_;
    SimuWeight  weight_;
    RobotModel  model_;
    bool        brick_;
    Millimeter  D_, motorK_;
    bool        jackin_;
    bool        emergencyStop_;
    bool        lcdBtnYes_;
    bool        lcdBtnNo_;
    std::string lcdMessage_;
    Position    realPos_;
    Position    estimatedPos_; // position utilise pour les odometres
    MotorSpeed  speedLeft_, speedRight_;
    MotorPWM    pwmLeft_,   pwmRight_;
    MotorPosition  motorLeft_, motorRight_;
    bool        needSendDisplayInfo_;
    SimuMatchStatus  matchStatus_;

    MotorPosition  motorLeftOld_, motorRightOld_;
    Position       realPosOld_;
    Position       PosOld_;
   
    Point odomLeftPt_;
    Point odomRightPt_;
    Millimeter odomK_;
    double odomSpeedL_;
    double odomSpeedR_;
    CoderPosition odomLeft_;
    CoderPosition odomRight_;

    double motorSpeedL_;
    double motorSpeedR_;
    bool   simuMotorNoise_;
    bool   simuPosFirst_;
    bool   isValid_;
    int    isDead_; // 0=not dead
       
#define SIMU_ROBOT_PTS_NBR     12
#define SIMU_ROBOT_POLYGON_NBR 3
#define SIMU_ROBOT_WHEEL_PTS_NBR 4
    Point    wheelPts_[SIMU_ROBOT_WHEEL_PTS_NBR];
    Point    wheelRealPts_[SIMU_ROBOT_WHEEL_PTS_NBR];
    Point    borderPts_[SIMU_ROBOT_PTS_NBR];
    Point    borderRealPts_[SIMU_ROBOT_PTS_NBR];
    Polygon  borderPol_[SIMU_ROBOT_POLYGON_NBR]; // 2 polygon au niveau bas moins de 70mm et 1 au dessus
};

#endif // __SIMULATOR_CLIENT_H__
