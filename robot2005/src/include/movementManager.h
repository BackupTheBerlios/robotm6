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
#include "robotPosition.h"
#include "robotBase.h"
#include "mthread.h"

#define MVTMGR MovementManager::instance()
// ============================================================================
// ==========================  class MovementManager   ========================
// ============================================================================

typedef struct MotorCom {
    bool reset;
    bool resetPwmAlert;
    MotorAcceleration setAcc;
    MotorSpeed    speedLeft, speedRight;
    MotorPosition posLeft, posRight;
    MotorPWM      pwmLeft, pwmRight;
    MotorCom():reset(false), resetPwmAlert(false),
               setAcc(-1),
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

/**
 * @class MovementManager
 * Gere les composants necessaires pour faire bouger le robot. S'occupe de 
 * creer le thread pour les taches periodic de motor, move et position
 */
class MovementManager : public RobotComponent
{
 public:
    /** Le constructeur ne doit etre appele qu'une fois par main. Ensuite, 
        utiliser instance */
    MovementManager();
    virtual ~MovementManager();
    /** Function réservée au thread!: calcul la nouvelle position et la 
        nouvelle consigne */
    void periodicTask();

    /** @brief Retourne le singleton correspondant a la classe 
        MovementManager */
    static MovementManager* instance();

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

    RobotPosition* position(){return position_;}
    Move*          move()    {return move_;}

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

 protected:
    Motor* motor() { return motor_; }
    void startThread();
    void setSpeed(MotorSpeed  speedLeft,
		  MotorSpeed  speedRight);
    void getCoderPosition(CoderPosition& left, 
			  CoderPosition& right);
    void getPWM(MotorPWM& left, 
		MotorPWM& right);
    friend class Move;
    friend class RobotPosition;
    friend class LogCL;

 private:
    static MovementManager* mvtMgr_;
    Motor*         motor_;
    RobotPosition* position_;
    Move*          move_;
    FunctionPtr    periodicCallback_;
    volatile bool  needMotorReset_;
    bool           threadStarted_;
    MotorCom       motorCom_;
    MThreadId      thread_;
    MoveDirection  direction_;
};

// ----------------------------------------------------------------------------
// MovementManager::registerPeriodicCallback
// ----------------------------------------------------------------------------
inline void MovementManager::registerPeriodicCallback(FunctionPtr fn)
{
    periodicCallback_ = fn;
}

// ----------------------------------------------------------------------------
// MovementManager::instance
// ----------------------------------------------------------------------------
inline MovementManager* MovementManager::instance()
{
    assert(mvtMgr_);
    return mvtMgr_;
}

// ----------------------------------------------------------------------------
// MovementManager::getDirection
// ----------------------------------------------------------------------------
inline MoveDirection MovementManager::getDirection() 
{
    return direction_; 
}

// ----------------------------------------------------------------------------
// MovementManager::getMotorSpeed
// ----------------------------------------------------------------------------
inline void MovementManager::getMotorSpeed(MotorSpeed  &speedLeft,
					   MotorSpeed  &speedRight)
{
    speedLeft  = motorCom_.speedLeft;
    speedRight = motorCom_.speedRight;
}

#endif // __MOVEMENT_MANAGER_H__
