/**
 * @file movement.h
 *
 * @brief Classes definissant les differents movements
 *
 * @author Laurent Saint-Marcel
 * @date   2003/06/22
 */

#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "move.h"
#include "motor.h"
#include "robotPosition.h"

// ============================================================================
// ====================================  typedef  =============================
// ============================================================================

typedef enum MoveType {
    MOVE_STOP,
    MOVE_FORWARD_TYPE,
    MOVE_BACKWARD_TYPE,
    MOVE_ROTATE_TYPE,
    MOVE_ROTATE_FROM_ANGLE_TYPE,
    MOVE_ROTATE_ON_WHEEL_TYPE,
    MOVE_REALIGN_TYPE,
    MOVE_GOTOTARGET_TYPE,
    MOVE_TRAJECTORY_TYPE,
    MOVE_SERIES_TYPE,
    MOVE_SET_SPEED_TYPE
} MoveType;

typedef double MoveLinearSpeed;
typedef double MoveRotationSpeed;
class MoveCL;

// ============================================================================
// ===============================  const MOVEMENT_  ==========================
// ============================================================================

static const int MOVEMENT_NAME_LENGHT = 60;
static const int MOVEMENT_TXT_LENGHT  = 200;

// ============================================================================
// ================================  class Movement  ==========================
// ============================================================================

/**
 * @class Movement
 * Movement elementaire du robot et fonctions de regulation
 */
class Movement : public RobotBase
{
 public:
    // public functions
    Movement(MoveType     type, 
             const char*  name,
             MotorSpeed   maxSpeed,
             MoveGain     gain,
             MoveCL*      move);
    virtual ~Movement();

    virtual void periodicTask()=0; 
    virtual char* txt()=0;
    const char* name();
    void registerNextMovement(Movement* nextMovement);
    void unregisterNextMovement();
    void start();
    void stop();

 protected:
    // protected functions
    void setLRSpeed(MoveLinearSpeed   linearSpeed,
                    MoveRotationSpeed rotationSpeed);
    bool moveToPoint(Point      target, 
                     MoveGain   gain,
                     Point      startingPoint, 
                     Millimeter maxSquareDist);
 protected:
    // protected members
    MotorSpeed   maxSpeed_;
    MoveGain     gain_;
    Point        startingPoint_;
    static bool  endOfMovement_;
    static char  txt_[MOVEMENT_TXT_LENGHT];
    Millisecond  startTime_;
    MoveCL*      move_;
 private:
    // private members
    MoveType   type_;
    char       name_[MOVEMENT_NAME_LENGHT];
    Movement * nextMovement_;
    static int countMovementAllocated_;
};

// -----------------------------------------------------------------
// class MovementForward
// -----------------------------------------------------------------

/**
 * @class MovementForward
 * Movement vers l'avant du robot
 */
class MovementForward : public Movement
{
 public:
    MovementForward(Millimeter dist,
                    MotorSpeed maxSpeed,
                    MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Millimeter squareDistance_;
};

// -----------------------------------------------------------------
// class MovementBackward
// -----------------------------------------------------------------

/**
 * @class MovementBackward
 * Movement vers l'arriere du robot
 */
class MovementBackward : public Movement
{
 public:
    MovementBackward(Millimeter dist,
                     MotorSpeed maxSpeed,
                     MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Millimeter squareDistance_;
};

// -----------------------------------------------------------------
// class MovementSetSpeed
// -----------------------------------------------------------------

/**
 * @class MovementSetSpeed
 * Met toujours la meme vitesse sur les roues jusqu'a ce qu'on se soit
 * deplace d'une certaine distance
 */
class MovementSetSpeed : public Movement
{
 public:
    MovementSetSpeed(Millimeter dist,
                     MotorSpeed speedLeft,
		     MotorSpeed speedRight,
                     MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Millimeter squareDistance_;
    MotorSpeed speedLeft_;
    MotorSpeed speedRight_;
};

// -----------------------------------------------------------------
// class MovementRotate
// -----------------------------------------------------------------

/**
 * @class MovementRotate
 * Movement vers l'arriere du robot
 */
class MovementRotate : public Movement
{
 public:
    MovementRotate(Radian     theta,
                   MoveGain   gain,
                   MotorSpeed maxSpeed,
                   MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Radian     theta_;
};

// -----------------------------------------------------------------
// class MovementRealign
// -----------------------------------------------------------------

/**
 * @class MovementRealign
 * tourne dans la direction indiquee en bloquand une roue au bout d'une 
 * certaine distance
 */
class MovementRealign : public Movement
{
 public:
    MovementRealign(Millimeter distMaxWheel,
                    Radian     theta,
                    MoveGain   gain,
                    MotorSpeed maxSpeed,
                    MoveCL*    move);

    Point getStopWheelPoint();
    void  periodicTask();
    char* txt();

 protected:
    Radian     theta_;
    bool       leftWheel_;
    Millimeter squareDistMax_;
    Point      blockedWheelPoint_;
    bool       canMoveStopWheel_;
};

// -----------------------------------------------------------------
// class MovementRotateOnWheel
// -----------------------------------------------------------------

/**
 * @class MovementRotateOnWheel
 * Torune autour d'une roue
 */
class MovementRotateOnWheel : public Movement
{
 public:
    MovementRotateOnWheel(bool       stopLeftWheel, // true si tourne autour de roue gauche
                          Radian     theta,
                          MoveGain   gain,
                          MotorSpeed maxSpeed,
                          MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Radian     theta_;
    bool       leftWheel_;
};

// -----------------------------------------------------------------
// class MovementRotateFromAngle
// -----------------------------------------------------------------

/**
 * @class MovementRotateFromAngle
 * Movement vers l'arriere du robot
 */
class MovementRotateFromAngle : public Movement
{
 public:
    MovementRotateFromAngle(Radian     deltaTheta,
                            MoveGain   gain,
                            MotorSpeed maxSpeed,
                            MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Radian     theta_;
};

// -----------------------------------------------------------------
// class MovementGo2Target
// -----------------------------------------------------------------

/**
 * @class MovementGo2Target
 * Movement vers l'arriere du robot
 */
class MovementGo2Target : public Movement
{
 public:
    MovementGo2Target(Point      target,
                      MoveGain   gain,
                      MotorSpeed maxSpeed,
                      MoveCL*    move);
    void periodicTask();
    char* txt();

 protected:
    Point      target_;
    Millimeter totalSquareLength_;
    bool       totalLengthInit_;
};

// -----------------------------------------------------------------
// class MovementTrajectory
// -----------------------------------------------------------------

static const Millimeter MVT_TRAJECTORY_MIN_SQUARE_LENGTH =16000;  /*2000 ( regarde combien devant lui en mm (au carre)? 12 cm*/
static const double MVT_TRAJECTORY_DELTA_INDEX = 0.01;

/**
 * @class MovementTrajectory
 * Movement vers l'arriere du robot
 */
class MovementTrajectory : public Movement
{
 public:
    MovementTrajectory(Trajectory const&   trajectory, 
                       MoveTrajectoryMode  mode,
                       MoveGain            gain,
                       MotorSpeed          maxSpeed,
                       MoveCL*             move);
    void periodicTask();
    char* txt();

 protected:
    void getNextPoint(Point &newPoint, double &index);
    Millimeter distToVirage(int currentIndex);
 protected:
    Trajectory          trajectory_;
    MoveTrajectoryMode  trajectoryMode_;
    double              currentIndex_;
    double              lastIndex_;
    Millimeter          totalSquareLength_;
    Millimeter          lastSquareLength_;
    Point               beforeLastPoint_;
    MotorSpeed          realMaxSpeed_;
    bool                first_;

};

#endif // __MOVEMENT_H__
