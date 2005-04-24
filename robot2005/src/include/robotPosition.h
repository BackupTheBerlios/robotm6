/**
 * @file robotPosition.h
 *
 * @brief Calcul de la position du robot.
 * Calcul de la position du robot en utilisant 2 systemes de mesures: les
 * codeurs sur les roues motrices (hctl) et les codeurs sur des roues non 
 * motrices (odometer). Gere aussi la detection de collisions quand les roues 
 * motrices tournent et pas les roues independantes (le robot est souleve)
 *
 * @author Laurent Saint-Marcel
 * @date   2003/06/22
 */

#ifndef __POSISTION_H__
#define __POSISTION_H__

#include "robotBase.h"

#define RobotPos RobotPositionCL::instance()

/** @brief Nombre d'erreurs maximum de communication avec la carte odometre 
    avant de passer en mode hctl */
static const unsigned short ODOMETER_ERROR_MAX = 50;

/** @brief taille du Buffer utilise pour memoriser les anciennes positions 
    des odometres et des hctl */
static const int ODOM_COLLI_DETECT_BUFFER_SIZE=30;

/** @brief Seuils de detection des blocage par comparaison des HCTL et
    des odometres sur un deplacement de BUFFER_SIZEx100ms max */
static const Radian      POSITION_ODOM_ALERT_LEVEL_DR=M_PI/4.;
static const Millimeter  POSITION_ODOM_ALERT_LEVEL_DP=300;
static const Millisecond PERIOD_CHECK_PATINAGE=100;

static const int    POSITION_TEXT_LENGTH = 256;
/** @brief Conversion pas de codeur HCTl/distance parcourue par une roue
    motrice*/
static const double POSITION_ROBOT_HCTL_K     = 0.00430513851;//0.00435228546; // .0043286478;  // 1700/3014
/** @brief Distance entre les 2 roues motrices */
static const double POSITION_ROBOT_HCTL_D     = 340;
/** 
 * @brief Facteurs de correction 
 *  Voir: Measurement and Correction of Systematic Odometry Errors in 
 *  Mobile Robots
 */
static const double POSITION_ROBOT_HCTL_Eb     = 1.;
static const double POSITION_ROBOT_HCTL_Ed     = 1.;
static const double POSITION_ROBOT_HCTL_Cr     = 2./((1./POSITION_ROBOT_HCTL_Ed)+1.);
static const double POSITION_ROBOT_HCTL_Cl     = 2./(POSITION_ROBOT_HCTL_Ed+1.);
/** @brief Signe du codeur hctl gauche en allant vers l'avant */
static const double POSITION_CODER_HCTL_SIGN_LEFT  = +1.;
/** @brief Signe du codeur hctl droit en allant vers l'avant */
static const double POSITION_CODER_HCTL_SIGN_RIGHT = -1.;


/** @brief Conversion pas de codeur ODOMETRE/distance parcourue par une roue
    odometre */
static const double POSITION_ROBOT_ODOM_K     = .057521639;
/** @brief Distance entre les 2 roues motrices */
static const double POSITION_ROBOT_ODOM_D     = 313;
/** @brief Facteurs de correction */
static const double POSITION_ROBOT_ODOM_Eb     = 1.; //1.004; //0.99241848;//1.00463051;  //1.
static const double POSITION_ROBOT_ODOM_Ed     = 0.9972640;//0.99589715; //1.;  
static const double POSITION_ROBOT_ODOM_Cr     = 2./((1./POSITION_ROBOT_ODOM_Ed)+1.);
static const double POSITION_ROBOT_ODOM_Cl     = 2./(POSITION_ROBOT_ODOM_Ed+1.);
/** @brief Signe du codeur hctl gauche en allant vers l'avant */
static const double POSITION_CODER_ODOM_SIGN_LEFT  = 1.;
/** @brief Signe du codeur hctl droit en allant vers l'avant */
static const double POSITION_CODER_ODOM_SIGN_RIGHT = -1.;

static const int POS_BUFFER_SIZE = 20;
static const Millisecond POS_BUFFER_STEP_TIME = 50;

typedef enum OdometerType {
    ODOMETER_UART_MANUAL,
    ODOMETER_UART_AUTOMATIC,
    ODOMETER_MOTOR
} OdometerType;

void robotPositionMotorHasBeenReset();

/**
 * @class RobotPosition
 * Calcul de la position du robot en temps réel.
 * Calcul de la position du robot en utilisant 2 systemes de mesures: les
 * codeurs sur les roues motrices (hctl) et les codeurs sur des roues non 
 * motrices (odometer). Gere aussi la detection de collisions quand les roues 
 * motrices tournent et pas les roues independantes (le robot est souleve)
 *
 */
class RobotPositionCL : public RobotComponent 
{
  public: 
    /** @brief Constructeur (Doit etre appele une seule fois, voir get */
    RobotPositionCL();
    ~RobotPositionCL();
    /** @brief Retourne un pointer vers la classe unique position */
    static RobotPositionCL* instance();
    /** @brief Ne fait rien de particulier */
    bool reset();
    /** @brief Retourne TRUE si le calcul de position est correct */
    bool validate();

  public :
    /** 
     * @brief Met a jour la position du robot 
     */
    void set(Millimeter x, Millimeter y, Radian t);

    /**
     * @brief Affiche la position du robot a l'ecran et dans les logs
     */
    void print();

    /**
     * @brief Retourne un char* qui contient la position du robot
     */
    char* txt(); 

    /**
     * @brief Definit le type d'odometre a utiliser: codeurs roues motrices 
     *        (HCTL) ou codeur roues libres (UART)\n
     *        Par defaut RobotPosition utilise les hctl
     */
    void setOdometerType(OdometerType odometer);
    
    /**
     * @brief Retourne le type d'odometre utilise
     */
    OdometerType getOdometerType() const;

    /**
     * @brief Quand les codeurs ont ete resetes, il ne faut pas tenir compte de la valeur precedente
     */
    void resetHctlCoders();
    /**
     * @brief Quand les codeurs ont ete resetes, il ne faut pas tenir compte de la valeur precedente
     */
    void resetOdomCoders();

  public :
    /** @brief Abscisse du robot */
    Millimeter x() const;

    /** @brief Ordonnee du robot */
    Millimeter y() const;

    /** @brief Direction du robot. En mode 
        MOVE_DIRECTION_BACKWARD, la direction est decalee de PI */
    Radian     theta() const; 

    /** @brief Direction du robot. La valeur n'est pas modifiee 
        par MOVE_DIRECTION_BACKWARD */
    Radian     thetaAbsolute() const;

    /** @brief Position du robot (x,y) */
    Point      pt() const;

    /** @brief Position du robot (center, direction). En mode 
        MOVE_DIRECTION_BACKWARD, la direction est decalee de PI */
    Position   pos() const;

    /** @brief Position du robot (center, direction). La valeur n'est 
        pas modifiee par MOVE_DIRECTION_BACKWARD */
    Position   posAbsolute() const;

    /** @brief Renvoie true si le point est devant le robot (qu'il 
        faut passer en mode
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD); */
    bool isTargetForward(Point const& target);

    /** @brief active la detection des collision par comparaison des hctl
	       et des odometres */
    void enableColliDetectionWithOdometer(bool enable);

    Position getPosition(Millisecond deltaT);
    
 private:
    void addPositionToBuffer();
    void clearBufferPosition();
    Position posBuf_[POS_BUFFER_SIZE];
    int posBufIndex_;
    static RobotPositionCL*  position_;

    CoderPosition rightHctlOld_, leftHctlOld_;
    bool firstHctl_;
    Position posHctl_;
    CoderPosition rightOdomOld_, leftOdomOld_;
    bool firstOdom_;
    Position posOdom_;

    Position     pos_;
    bool         txtChanged_;
    char         txt_[POSITION_TEXT_LENGTH];

    OdometerType odometerType_;
    bool         odometerIsAutomatic_;
    short        odometerErrorCount_;

    bool enableOdoHtclColliDetec_;
    Radian     drHctl_[ODOM_COLLI_DETECT_BUFFER_SIZE];
    Radian     drOdom_[ODOM_COLLI_DETECT_BUFFER_SIZE];
    Millimeter dpHctl_[ODOM_COLLI_DETECT_BUFFER_SIZE];
    Millimeter dpOdom_[ODOM_COLLI_DETECT_BUFFER_SIZE];
    Position   oldPosHctl_;
    Position   oldPosOdom_;
    Radian     sumDrOdom_;
    Radian     sumDrHctl_;
    Millimeter sumDpOdom_;
    Millimeter sumDpHctl_;
    Millisecond timeToCheckPatinage_;
    bool       firstSum_;
    
 protected:
    void getPosition(Position&      posi,
                     CoderPosition  leftPos, 
                     CoderPosition  rightPos,
                     CoderPosition& leftPosOld, 
                     CoderPosition& rightPosOld,
                     Millimeter     KLeft,
                     Millimeter     KRight,
                     Millimeter     D,
                     bool           first);
  public:
    Position getHctlPosition() const;
    Position getOdometerPosition() const; 
  protected:
    void updateHctlPosition();
    void updateOdometerPosition();
    
    bool isOdometerAlive();
    void setOdometerAliveStatus(bool alive);
    /** 
     * @brief Cette fonction est appelee periodiquement avec l'heure courante 
     * et permet de mettre a jour la position du robot en fonciton des 
     * informations des codeurs 
     */
    void periodicTask(Millisecond time);
    friend class MovementManagerCL;
    
    void clearOdoColliDetectBuffer();
    void resetOdoColliDetection();
    void detectCollision();
};

// ----------------------------------------------------------------------------
// RobotPosition::instance
// ----------------------------------------------------------------------------
inline RobotPositionCL* RobotPositionCL::instance()
{
    assert(position_ != NULL);
    return position_;
}

// ----------------------------------------------------------------------------
// RobotPosition::x
// ----------------------------------------------------------------------------
inline Millimeter RobotPositionCL::x() const
{
    return pos_.center.x;
}

// ----------------------------------------------------------------------------
// RobotPosition::y
// ----------------------------------------------------------------------------
inline Millimeter RobotPositionCL::y() const
{
    return pos_.center.y;
}

// ----------------------------------------------------------------------------
// RobotPosition::thetaAbsolute
// ----------------------------------------------------------------------------
inline Radian RobotPositionCL::thetaAbsolute() const
{
    return pos_.direction;
}

// ----------------------------------------------------------------------------
// RobotPosition::pt
// ----------------------------------------------------------------------------
inline Point RobotPositionCL::pt() const
{
    return pos_.center;
}

// ----------------------------------------------------------------------------
// RobotPosition::pos
// ----------------------------------------------------------------------------
inline Position RobotPositionCL::pos() const
{
    return Position(pt(), theta());
}

// ----------------------------------------------------------------------------
// RobotPosition::posAbsolute
// ----------------------------------------------------------------------------
inline Position RobotPositionCL::posAbsolute() const
{
    return pos_;
}

// ----------------------------------------------------------------------------
// RobotPosition::getHctlPosition
// ----------------------------------------------------------------------------
inline Position RobotPositionCL::getHctlPosition() const
{
    return posHctl_;
}

// ----------------------------------------------------------------------------
// RobotPosition::getOdometerPosition
// ----------------------------------------------------------------------------
inline Position RobotPositionCL::getOdometerPosition() const
{
    return posOdom_;
}

// ----------------------------------------------------------------------------
// RobotPosition::resetHctlCoders
// ----------------------------------------------------------------------------
inline void RobotPositionCL::resetHctlCoders()
{
    firstHctl_ = true;
    resetOdoColliDetection();
}

// ----------------------------------------------------------------------------
// RobotPosition::resetOdomCoders
// ----------------------------------------------------------------------------
inline void RobotPositionCL::resetOdomCoders()
{
    firstOdom_ = true;
    resetOdoColliDetection();
}

#endif // __POSISTION_H__
