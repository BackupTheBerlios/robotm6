/**
 * @file movePvt.h
 *
 * @brief Class permettant de gerer les deplacements du robot de maniere 
 * haut niveau: aller en avant, arriere, tourner dans la direction, suivre
 * une trajectoire
 *
 * @author Laurent Saint-Marcel
 * @date   2003/06/22
 */

#ifndef __MOVE_H__
#define __MOVE_H__

//#include "robotComponent.h"
#include "robotBase.h"
#include "motor.h"

// ============================================================================
// ================================  Macro   ==================================
// ============================================================================

#define MOVE Move::instance()

// ============================================================================
// ===============================  typedef   =================================
// ============================================================================
typedef double MoveGain;

typedef enum MoveTrajectoryMode {
  TRAJECTORY_BASIC,
  TRAJECTORY_RECTILINEAR,
  TRAJECTORY_SPLINE
} MoveTrajectoryMode;

class Movement;

// ============================================================================
// =================================  const MOVE_  ============================
// ============================================================================

static const MoveGain   MOVE_GAIN_DEFAULT   = 0.5;
static const MoveGain   MOVE_GAIN_SLOW      = 0.5;
static const MoveGain   MOVE_GAIN_FAST      = 2.;

static const MotorSpeed MOVE_SPEED_DEFAULT      = 20;
static const MotorSpeed MOVE_SPEED_SLOW         = 20;
static const MotorSpeed MOVE_SPEED_FAST         = 60;
static const MotorSpeed MOVE_MAX_ROTATION_SPEED = 20;

/** Consigne minimale pour avancer */
static const MotorSpeed MOVE_MIN_SPEED                = 8;
/** Acceleration maximale autorisee (variation de la consigne entre 
    deux setSpeed()) */
static const MotorSpeed MOVE_MAX_DELTA_SPEED          = 1;

/** En mode forward ou backward, distance a partir de laquelle on ralenti */
static const Millimeter MOVE_LINEAR_REDUCE_SPEED_DIST = 50; 

/** En rotation, gain I du PID */
static const MoveGain   MOVE_ROTATION_INTEGRAL_GAIN  = 0.001;
/** Angle a la cible en dessous duquel on considere etre arrive a 
    destination */
static const Radian     MOVE_ROTATION_EPSILON        = 0.05; 

/** Position du centre de traction par rapport au centre des roues en mode
    Go2 ou trajectory */
static const Millimeter MOVE_ATTRACTIVE_DIST         = 50;
/** Distance en dessous de laquelle on augmente le gain en mode 
    Go2 ou trajectory */
static const Millimeter MOVE_NEAR_TARGET_DIST        = MOVE_ATTRACTIVE_DIST*5;
/** Distance en dessous de laquelle on considere etre arrive a 
    destination en mode Go2 ou trajectory*/
static const Millimeter MOVE_XY_EPSILON              = 10; 

static const MoveGain   MOVE_USE_DEFAULT_GAIN  =-1;
static const MotorSpeed MOVE_USE_DEFAULT_SPEED =-1;
static const Radian ANGLE_LIMIT_VIRAGE=M_PI/4;
static const MotorSpeed VITESSE_MAX_VIRAGE=20;

// ============================================================================
// =================================  class Move   ============================
// ============================================================================

/** 
 * @class Move
 * Gestion des deplacements du robot. Elle definie des fonctions haut niveau 
 * de deplacements regules
 */
class Move : public RobotComponent
{
 public: 
    bool reset();
    bool validate();
    void calibrate();
    static Move* instance();
 public: 
    /** Stoppe le robot de maniere douce (deceleration progressive) */
    void stop();

    /** Stoppe le robot de mani�re brutale = setMotorSpeed(0,0) */
    void emergencyStop();

    /** Active la limation 'soft' de l'acc�l�ration des moteurs des 
	roues. Ne devrait pas �tre utilis� si le gain des HCTL est bien r�gl�*/
    void enableAccelerationController(bool enable);

    /** Fait avancer le robot vers l'avant. Le movement n'est pas
	r�guler(le robot n'ira pas droit). A n'utiliser que sur des courtes
	distances (moins de 30cm) */
    void forward(Millimeter  dist,
                 MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Fait reculer le robot vers l'arriere. Le movement n'est pas
	r�guler(le robot n'ira pas droit). A n'utiliser que sur des courtes
	distances (moins de 30cm) */
    void backward(Millimeter  dist,
                  MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED); 
    
    /** Fait tourner le robot sur lui m�me vers un angle donn�. 
	cf rep�re du terrain */
    void rotate(Radian      finalDir,
                MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Fait tourner le robot sur lui m�me d'un angle donn�. L'angle 
	est donn� dans le rep�re du robot. Direction finale = direction 
	actuelle + deltaTheta */
    void rotateFromAngle(Radian      deltaTheta,
                         MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                         MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Le robot va jusqu'� un point cible. Tout d'abord il tourne sur
	lui m�me dans la bonne direction, puis il s'asservit sur le point 
	cible et suit � peu pr�s une ligne droite. */
    void go2Target(Millimeter  x, 
                   Millimeter  y,
                   MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                   MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);
    /** Le robot va jusqu'� un point cible. Tout d'abord il tourne sur
	lui m�me dans la bonne direction, puis il s'asservit sur le point 
	cible et suit � peu pr�s une ligne droite. */
    void go2Target(Point       pt,
                   MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                   MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);
    
    /** Le robot suit une trajectoire: il passe par une liste de points
	donn�s � l'avance. \n
	En mode BASIC, le robot tourne sur lui meme, puis avance en ligne 
	droite jusqu'au point suivant, tourne sur lui meme... C'est une suite 
	de rotate et de go2Target\n
	En mode RECTILINEAR, le robot suit a peu pr�s des droites entre les 
	points mais il coupe les angles. L'avant du robot passe exactement sur
	la suite de droites reliant chaque point.\n
	En mode SPLINE, le robot suit une courbe qui passe exactment par les
	points de la trajectoire mais on ne sait pas trop o� il est cens� 
	pass� entre les points... */
    void followTrajectory(Trajectory const&   trajectory, 
                          MoveTrajectoryMode  mode=TRAJECTORY_RECTILINEAR,
                          MoveGain            gain=MOVE_USE_DEFAULT_GAIN ,
                          MotorSpeed          maxSpeed=MOVE_USE_DEFAULT_SPEED,
			  bool                noRotation=false);

    /** Arrete le robot quand l'utilisateur fait un CTRL+C */
    void userAbort();

    /** @brief Gain par default en mode rotation */ 
    void setDefaultRotationGain(MoveGain gain);
    /** @brief Max speed en mode rotation */ 
    void setDefaultMaxRotationSpeed(MotorSpeed speed);
    /** @brief Gain par default en mode go2target */ 
    void setDefaulLinearGain(MoveGain gain);
    /** @brief Max speed en mode go2target */
    void setDefaultMaxLinearSpeed(MotorSpeed speed);
    /** @brief Speed en mode forward/backward */
    void setDefaultBasicSpeed(MotorSpeed speed);
    
 private:
    // disable
    Move(Move const& move); 
 public:
    /** @brief Met a jour la vitesse des moteurs */
    void setSpeed(MotorSpeed  speedLeft,
		  MotorSpeed  speedRight);
 private:
    // members
    Movement* currentMovement_;
    Motor*    motor_;
    bool      enableAccelerationController_;
    bool      userAbort_;
    static Move*      move_;
    static MotorSpeed lastSpeedLeft_;
    static MotorSpeed lastSpeedRight_;
    static MotorSpeed lastSpeedReqLeft_;
    static MotorSpeed lastSpeedReqRight_; 

    MoveGain   defaultRotationGain_;
    MoveGain   defaultLinearGain_;
    MotorSpeed defaultMaxRotationSpeed_;
    MotorSpeed defaultMaxLinearSpeed_;
    MotorSpeed defaultBasicSpeed_;

 protected:
    // functions
    void setCurrentMovement(Movement* mvt, bool needLock=true);
    void periodicTask(Millisecond time);

    // constructeur
    Move();
    ~Move();
    
    friend class Movement;
    friend class MovementManager;
};

// =======================================================================
// INLINE FUNCTIONS
// =======================================================================

// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Gain par default en mode rotation 
// -----------------------------------------------------------------------
inline void Move::setDefaultRotationGain(MoveGain gain)
{
    defaultRotationGain_ = gain;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Max speed en mode rotation 
// -----------------------------------------------------------------------
inline void Move::setDefaultMaxRotationSpeed(MotorSpeed speed)
{
    defaultMaxRotationSpeed_ = speed;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Gain par default en mode go2target 
// -----------------------------------------------------------------------
inline void Move::setDefaulLinearGain(MoveGain gain)
{
    defaultLinearGain_ = gain;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Max speed en mode go2target 
// -----------------------------------------------------------------------
inline void Move::setDefaultMaxLinearSpeed(MotorSpeed speed)
{
    defaultMaxLinearSpeed_ = speed;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Speed en mode forward/backward 
// -----------------------------------------------------------------------
inline void Move::setDefaultBasicSpeed(MotorSpeed speed)
{
    defaultBasicSpeed_ = speed;
}

#endif /* __MOVE_H__ */ 