/**
 * @file simulatorServer.h
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
    void setRobotPos(Position const& pos)   { estimatedPos_ = pos; 
                                              realPos_ = pos; 
                                              simuPosFirst_ = true;}
    void setRobotName(const char* name)     { name_ = std::string(name); 
                                              needSendDisplayInfo_=true;}
    void setRobotWeight(SimuWeight weight)  { weight_ = weight; }
    void setRobotModel(RobotModel model)    { model_ = model; 
                                              needSendDisplayInfo_=true;}
    void setModeBrick(bool brick)           { brick_ = brick; 
                                              needSendDisplayInfo_=true;}
    void setRobotMotorCoef(int D, int K, double speed)    { D_ = D; K_ = K; simuSpeed_ = speed;}
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
    /** @brief cete fonction permet de restaurer les donnes du robot en cas de
        nouvelle position invalide. */
    void setNewPositionValid(bool isValid);

    // ------------------------------------------------------------------------
    // private members
    // ------------------------------------------------------------------------
 private:
    std::string name_;
    SimuWeight  weight_;
    RobotModel  model_;
    bool        brick_;
    int         D_, K_;
    bool        jackin_;
    bool        emergencyStop_;
    bool        lcdBtnYes_;
    bool        lcdBtnNo_;
    std::string lcdMessage_;
    Position    realPos_;
    Position    estimatedPos_;
    MotorSpeed  speedLeft_, speedRight_;
    MotorPWM    pwmLeft_,   pwmRight_;
    MotorPosition  motorLeft_, motorRight_;
    bool        needSendDisplayInfo_;
    SimuMatchStatus  matchStatus_;

    MotorPosition  motorLeftOld_, motorRightOld_;
    Position       realPosOld_;
    double simuSpeed_;
    double simuCoderSignRight_;
    double simuCoderSignLeft_;
    bool   simuMotorNoise_;
    bool   simuPosFirst_;

       
 
};

#endif // __SIMULATOR_CLIENT_H__
