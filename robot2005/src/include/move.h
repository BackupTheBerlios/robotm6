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

#define Move MoveCL::instance()

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
static const Millimeter MOVE_LINEAR_REDUCE_SPEED_SQUARE_DIST = 250; 

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
static const Millimeter MOVE_NEAR_TARGET_SQUARE_DIST        = MOVE_ATTRACTIVE_DIST*MOVE_ATTRACTIVE_DIST*25;
/** Distance en dessous de laquelle on considere etre arrive a 
    destination en mode Go2 ou trajectory*/
static const Millimeter MOVE_XY_SQUARE_EPSILON       = 100; 

static const Millimeter MOVE_REALIGN_SQUARE_DIST_DEFAULT = 900;

static const MoveGain   MOVE_USE_DEFAULT_GAIN  =-1;
static const MotorSpeed MOVE_USE_DEFAULT_SPEED =-1;
static const Radian     ANGLE_LIMIT_VIRAGE     =M_PI/4;
static const MotorSpeed VITESSE_MAX_VIRAGE     =20;
static const Millimeter MOVE_USE_DEFAULT_DIST  =-1;

// ============================================================================
// =================================  class Move   ============================
// ============================================================================

/** 
 * @class MoveCL
 * Gestion des deplacements du robot. Elle definie des fonctions haut niveau 
 * de deplacements regules
 */
class MoveCL : public RobotComponent
{
 public: 
    bool reset();
    bool validate();
    void calibrate();
    static MoveCL* instance();
 public: 
    /** Stoppe le robot de maniere douce (deceleration progressive) */
    void stop();

    /** Stoppe le robot de manière brutale = setMotorSpeed(0,0) */
    void emergencyStop();

    /** Active la limation 'soft' de l'accélération des moteurs des 
	roues. Ne devrait pas être utilisé si le gain des HCTL est bien réglé*/
    void enableAccelerationController(bool enable);

    /** Fait avancer le robot vers l'avant. Le movement n'est pas
	réguler(le robot n'ira pas droit). A n'utiliser que sur des courtes
	distances (moins de 30cm) */
    void forward(Millimeter  dist,
                 MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Fait reculer le robot vers l'arriere. Le movement n'est pas
	régule (le robot n'ira pas droit). A n'utiliser que sur des courtes
	distances (moins de 30cm) */
    void backward(Millimeter  dist,
                  MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED); 

    /** Envoie la meme consigne aux moteurs jusqu'a ce que le robot ce soit 
        deplace de la distance voulue par rapport a son point de depart
        idleBlockedWheel : desasservi la roue dont la vitesse est nulle */
    void setSpeedOnDist(Millimeter  dist, 
                        MotorSpeed  speedLeft,
                        MotorSpeed  speedRight,
                        bool        idleBlockedWheel=false);
    
    /** Fait tourner le robot sur lui même vers un angle donné, en allant 
        en marche arriere et en reculant a pein d'une des deux roues 
	cf repère du terrain */
    void realign(Radian      finalDir,
                 Millimeter  backwardDist=MOVE_USE_DEFAULT_DIST,
                 MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                 MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Fait tourner le robot sur lui même vers un angle donné. 
	cf repère du terrain */
    void rotate(Radian      finalDir,
                MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /** Fait tourner le robot sur lui même d'un angle donné. L'angle 
	est donné dans le repère du robot. Direction finale = direction 
	actuelle + deltaTheta */
    void rotateFromAngle(Radian      deltaTheta,
                         MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                         MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED);

    /**  Fait tourner le robot sur lui même vers un angle donné en bloquand une roue.
         si stopLeftWheel = true on fait tourner que le moteur droit
         idleBlockedWheel : deasservi la roue autour de laquelle on tourne
    */
    void rotateOnWheel(Radian      finalDir,
                       bool        stopLeftWheel,
                       MoveGain    gain=MOVE_USE_DEFAULT_GAIN,
                       MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED,
                       bool        idleBlockedWheel=false);

    /** Le robot va jusqu'à un point cible. Tout d'abord il tourne sur
	lui même dans la bonne direction, puis il s'asservit sur le point 
	cible et suit à peu près une ligne droite. */
    void go2Target(Millimeter  x, 
                   Millimeter  y,
                   MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                   MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED,
                   bool        noRotation=false);
    /** Le robot va jusqu'à un point cible. Tout d'abord il tourne sur
	lui même dans la bonne direction, puis il s'asservit sur le point 
	cible et suit à peu près une ligne droite. */
    void go2Target(Point       pt,
                   MoveGain    gain=MOVE_USE_DEFAULT_GAIN ,
                   MotorSpeed  maxSpeed=MOVE_USE_DEFAULT_SPEED,
                   bool        noRotation=false);
    
    /** Le robot suit une trajectoire: il passe par une liste de points
	donnés à l'avance. \n
	En mode BASIC, le robot tourne sur lui meme, puis avance en ligne 
	droite jusqu'au point suivant, tourne sur lui meme... C'est une suite 
	de rotate et de go2Target\n
	En mode RECTILINEAR, le robot suit a peu près des droites entre les 
	points mais il coupe les angles. L'avant du robot passe exactement sur
	la suite de droites reliant chaque point.\n
	En mode SPLINE, le robot suit une courbe qui passe exactment par les
	points de la trajectoire mais on ne sait pas trop où il est censé 
	passé entre les points... */
    void followTrajectory(Trajectory const&   trajectory, 
                          MoveTrajectoryMode  mode=TRAJECTORY_RECTILINEAR,
                          MoveGain            gain=MOVE_USE_DEFAULT_GAIN ,
                          MotorSpeed          maxSpeed=MOVE_USE_DEFAULT_SPEED,
			  bool                noRotation=false);
    
    /** Renvoie le numero du point de la trajectoire vers lequel va le robot 
        (premier point=0) */
    unsigned int getTrajectoryCurrentIndex();
    
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
    MoveCL(MoveCL const& move); 
 public:
    /** @brief Met a jour la vitesse des moteurs */
    void setSpeed(MotorSpeed  speedLeft,
		  MotorSpeed  speedRight);
    /** @brief Desasservi le moteur gauche jusqu'a ce qu'on lui envoie une
        vitesse != 0 */
    void idleMotorLeft();
    /** @brief Desasservi le moteur droit jusqu'a ce qu'on lui envoie une
        vitesse != 0 */
    void idleMotorRight();

 private:
    // members
    Movement*         currentMovement_;
    bool              enableAccelerationController_;
    bool              userAbort_;
    static MoveCL*    move_;
    static MotorSpeed lastSpeedLeft_;
    static MotorSpeed lastSpeedRight_;
    static MotorSpeed lastSpeedReqLeft_;
    static MotorSpeed lastSpeedReqRight_; 

    MoveGain   defaultRotationGain_;
    MoveGain   defaultLinearGain_;
    MotorSpeed defaultMaxRotationSpeed_;
    MotorSpeed defaultMaxLinearSpeed_;
    MotorSpeed defaultBasicSpeed_;
    Millimeter defaultRealignDist_;
    unsigned int trajectoryIndexOffset_;
    unsigned int trajectoryCurrentIndex_;

 protected:
    // functions
    void setCurrentMovement(Movement* mvt, bool needLock=true);
    void periodicTask(Millisecond time);
    void setTrajectoryCurrentIndex(unsigned int currentIndex);

    // constructeur
    MoveCL();
    ~MoveCL();
    
    friend class Movement;
    friend class MovementTrajectory;
    friend class MovementManagerCL;
};

// =======================================================================
// INLINE FUNCTIONS
// =======================================================================

// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Gain par default en mode rotation 
// -----------------------------------------------------------------------
inline void MoveCL::setDefaultRotationGain(MoveGain gain)
{
    defaultRotationGain_ = gain;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Max speed en mode rotation 
// -----------------------------------------------------------------------
inline void MoveCL::setDefaultMaxRotationSpeed(MotorSpeed speed)
{
    defaultMaxRotationSpeed_ = speed;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Gain par default en mode go2target 
// -----------------------------------------------------------------------
inline void MoveCL::setDefaulLinearGain(MoveGain gain)
{
    defaultLinearGain_ = gain;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Max speed en mode go2target 
// -----------------------------------------------------------------------
inline void MoveCL::setDefaultMaxLinearSpeed(MotorSpeed speed)
{
    defaultMaxLinearSpeed_ = speed;
}
// -----------------------------------------------------------------------
// setDefaultRotationGain
// -----------------------------------------------------------------------
// Speed en mode forward/backward 
// -----------------------------------------------------------------------
inline void MoveCL::setDefaultBasicSpeed(MotorSpeed speed)
{
    defaultBasicSpeed_ = speed;
}

#endif /* __MOVE_H__ */ 
