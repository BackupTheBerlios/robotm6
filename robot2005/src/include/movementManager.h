/**
 * @file movementManager.h
 *
 * Class qui initialise est gere tout ce qui est relatif au deplacement
 * du robot: moteurs(motor.h), position du robot par odometrie 
 * (robotPosition.h) et commande de deplacement (move.h)
 * Cette class cree le thread unique qui envoit des consignes aux moteurs,
 * il met a jour periodiquement la position du robot et met a jour 
 * periodiquement la consigne envoyee aux moteurs en fonction de la 
 * trajectoire definie par l'utilisateur avec "move".
 *
 * @author Laurent Saint-Marcel
 * @date   2003/06/22
 */

#ifndef __MOVEMENT_MANAGER_H__
#define __MOVEMENT_MANAGER_H__

#include "robotBase.h"
#include "motor.h"
#include "move.h"
#include "odometer.h"
#include "robotPosition.h"
#include "robotBase.h"
#include "mthread.h"

#define MvtMgr MovementManagerCL::instance()
// ============================================================================
// ==========================  class MovementManager   ========================
// ============================================================================

typedef struct MotorCom {
    bool reset;
    bool idle;
    int  unidle;
    bool idleLeft;
    bool idleRight;
    bool resetPwmAlert;
    MotorAcceleration setAcc;
    MotorSpeed    speedLeft, speedRight;
    MotorPosition posLeft, posRight;
    MotorPWM      pwmLeft, pwmRight;
  MotorCom():reset(false), idle(false), unidle(0), idleLeft(false), idleRight(false), 
	     resetPwmAlert(false), setAcc(-1),
	     speedLeft(0), speedRight(0), 
	     posLeft(0), posRight(0),
	     pwmLeft(0), pwmRight(0){}
} MotorCom;

typedef enum MoveDirection {
    MOVE_DIRECTION_FORWARD  =0,
    MOVE_DIRECTION_BACKWARD =1
} MoveDirection;

typedef enum MotorDirection {
    MOTOR_DIRECTION_STOP=0,
    MOTOR_DIRECTION_FORWARD,
    MOTOR_DIRECTION_BACKWARD,
    MOTOR_DIRECTION_LEFT,
    MOTOR_DIRECTION_RIGHT
} MotorDirection;

static const unsigned int PATINAGE_BUFFER_SIZE=20;

/**
 * @class MovementManager
 * Gere les composants necessaires pour faire bouger le robot. S'occupe de 
 * creer le thread pour les taches periodic de motor, move et position
 */
class MovementManagerCL : public RobotComponent
{
 public:
    /** Le constructeur ne doit etre appele qu'une fois par main. Ensuite, 
        utiliser instance */
    MovementManagerCL(MotorCL* motor, OdometerCL* odom);
    virtual ~MovementManagerCL();
    /** Function réservée au thread!: calcul la nouvelle position et la 
        nouvelle consigne */
    void periodicTask();

    /** @brief Retourne le singleton correspondant a la classe 
        MovementManager */
    static MovementManagerCL* instance();

    /** @brief Redemarre le composant dans son etat initial et vide les 
               buffers... */
    bool reset();

    /** @brief just reset the motor */
    void motorReset();

    /** @brief update the acceleration */
    void setAcceleration(MotorAcceleration acc);
     
    void resetPwmAlert();

    /** @brief Cette fonction lance une serie de tests sur le composants pour 
               savoir si le code n'a pas ete boggue */
    bool validate();

    RobotPositionCL* position() { return position_; }
    MoveCL*          move()     { return move_; }
    MotorCL*         motor()    { return motor_; }
    OdometerCL*      odometer() { return odometer_; }
    /** @brief Reset les moteurs quand on detecte qu'ils sont bloques */
    void enableAutomaticReset(bool enable);
   
    /** @brief Enregistre un callback qui sera appele apres chaque iteration 
        de mise a jour de la position et de la consigne envoee aux roues */
    void registerPeriodicCallback(FunctionPtr);

    /** @brief Renvoie la direction du robot (mode avant ou arriere) en mode 
        arriere le repere du robot est tourne de 180d */
    MoveDirection getDirection();

     /** @brief Definit la direction du robot (mode avant ou arriere) en mode 
         arriere le repere du robot est tourne de 180d */
    void setRobotDirection(MoveDirection);

    /** @brief Renvoie la vitesse des moteurs */
    void getMotorSpeed(MotorSpeed  &speedLeft,
		       MotorSpeed  &speedRight);

    /** @brief Renvoie le type de consigne envoyee aux moteurs: avancer, 
	reculer, tourner gauche, tourner droite */
    MotorDirection getMotorDirection();
    void setSpeed(MotorSpeed  speedLeft,
		  MotorSpeed  speedRight);
    /* asservissement des roues */
    void motorIdle();
    void motorIdleLeft();
    void motorIdleRight();
    void motorUnidle();
    void resetPatinageDetection();

 protected:
    void startThread();
    void getCoderPosition(CoderPosition& left, 
			  CoderPosition& right);
    void getPWM(MotorPWM& left, 
		MotorPWM& right);
    void updatePatinageBuffers();
    void checkPatinage();

    friend class MoveCL;
    friend class RobotPositionCL;
    friend class LogCL;

 private:
    static MovementManagerCL* mvtMgr_;
    MotorCL*         motor_;
    RobotPositionCL* position_;
    MoveCL*          move_;
    OdometerCL*      odometer_;
    FunctionPtr      periodicCallback_;
    volatile bool    needMotorReset_;
    bool             threadStarted_;
    MotorCom         motorCom_;
    MThreadId        thread_;
    MoveDirection    direction_;

   
    CoderPosition hctlLeftBuffer_[PATINAGE_BUFFER_SIZE];
    CoderPosition hctlRightBuffer_[PATINAGE_BUFFER_SIZE];
    CoderPosition odomLeftBuffer_[PATINAGE_BUFFER_SIZE];
    CoderPosition odomRightBuffer_[PATINAGE_BUFFER_SIZE];
    MotorPWM      pwmLeftBuffer_[PATINAGE_BUFFER_SIZE];
    MotorPWM      pwmRightBuffer_[PATINAGE_BUFFER_SIZE];
    int    hctlDeltaMoveLeft_;
    int    hctlDeltaMoveRight_;
    int    odomDeltaMoveLeft_;
    int    odomDeltaMoveRight_;
    int           pwmDeltaLeft_;
    int           pwmDeltaRight_;
    int           oldHctlLeftDelta_;
    int           oldHctlRightDelta_;
    int           oldOdomLeftDelta_;
    int           oldOdomRightDelta_;
    unsigned int  pattinageIndex_;
    unsigned int  pattinageBufferSize_;
};

// ----------------------------------------------------------------------------
// MovementManager::registerPeriodicCallback
// ----------------------------------------------------------------------------
inline void MovementManagerCL::registerPeriodicCallback(FunctionPtr fn)
{
    periodicCallback_ = fn;
}

// ----------------------------------------------------------------------------
// MovementManager::instance
// ----------------------------------------------------------------------------
inline MovementManagerCL* MovementManagerCL::instance()
{
    assert(mvtMgr_);
    return mvtMgr_;
}

// ----------------------------------------------------------------------------
// MovementManager::getDirection
// ----------------------------------------------------------------------------
inline MoveDirection MovementManagerCL::getDirection() 
{
    return direction_; 
}

// ----------------------------------------------------------------------------
// MovementManager::getMotorSpeed
// ----------------------------------------------------------------------------
inline void MovementManagerCL::getMotorSpeed(MotorSpeed  &speedLeft,
					   MotorSpeed  &speedRight)
{
    speedLeft  = motorCom_.speedLeft;
    speedRight = motorCom_.speedRight;
}

#endif // __MOVEMENT_MANAGER_H__
