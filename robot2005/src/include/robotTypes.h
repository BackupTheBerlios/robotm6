/**
 * @file robotTypes.h
 * Types et structures de bases pour le robot: Color, Millimeter, Position
 */
#ifndef __ROBOT_TYPES_H__
#define __ROBOT_TYPES_H__

#ifdef __cplusplus
#include "robotPoint2D.h"
#include <vector>
#include <deque>

// ============================================================================
// =============================   Types de base   ============================
// ============================================================================

/** @brief Unité de temps par défaut */
typedef unsigned char Byte;
/** @brief Unité de temps par défaut */
typedef int Millisecond;
/** @brief Unité de distance par défaut */
typedef double Millimeter;
/** @brief Unité d'angle par défaut */
typedef double Radian;
typedef short Millivolt;
/** @brief Unite des codeurs montes sur des axes de rotation (odometres) */
typedef int CoderPosition;
/** @brief Unite des consignes en vitesse du robot On a besoin que d'un short, mais pour faire les calculs et la detection des overflow, c'est plus simple d'avoir plus grand qu'un short=>un int */
typedef signed char MotorSpeed;
/** @brief Unite des codeurs montes sur les moteurs. On a besoin que d'un short, mais pour faire les calculs et la detection des overflow, c'est plus simple d'avoir plus grand qu'un short=>un int */
typedef int MotorPosition;
/** @brief Unite de la consigne envoyee aux moteurs.  */
typedef signed char MotorPWM;
/** @brief Unite de l'acceleration des moteurs (cf hctl1100) */
typedef short MotorAcceleration;
/** @brief Point pour le robot, on parle en nombre a virgules */
typedef RobotPoint2D<Millimeter, Radian> Point;
/** @brief Pointeur sur fonction */
typedef void (*FunctionPtr)(void);
/** @brief Une position est un point et une direction */
typedef struct Position {
  Point  center;
  Radian direction;
  // constructeur
  Position(): center(), direction(0){}
  Position(Point pt, Radian t): center(pt), direction(t){}
  Position(Millimeter x, Millimeter y, Radian t): 
    center(Point(x,y)), direction(t){}
} Position;

inline bool operator != (Position const& pos1, Position const& pos2) {
    return (pos1.center != pos2.center) || (pos1.direction != pos2.direction);
}
// ============================================================================
// ==========================   Constantes de base   ==========================
// ============================================================================
// ROBOT_A == robot attack
// ROBOT_D == robot defence
static const Millimeter INFINITE_DIST = -1;

static const Millisecond TIME_MATCH             = 87000;
static const Millisecond TIME_MARGIN            =  2000;
static const Millisecond TIME_INFINITE          = -1;
static const Millisecond TIME_NEVER             = -1;

static const Millimeter TERRAIN_LARGEUR  = 2100; // Y
static const Millimeter TERRAIN_LONGUEUR = 3644; // X
static const Millimeter TERRAIN_Y        = 2100; // Y
static const Millimeter TERRAIN_X        = 3644; // X
static const Millimeter TERRAIN_CASE_LARGEUR       = 300;
static const Millimeter TERRAIN_BORDURE_HAUTEUR    = 50;
static const Millimeter TERRAIN_BORDURE_LARGEUR    = 22;
static const Millimeter TERRAIN_RIVIERE_PROFONDEUR = 36;
static const Millimeter TERRAIN_PONT_LARGEUR = 420;
static const Millimeter TERRAIN_PONT_FIXE_LARGEUR = 120;
static const Millimeter TERRAIN_PONT_LONGUEUR = 600;
static const Millimeter TERRAIN_BORDURE_PONT_LARGEUR = 15;
static const Millimeter TERRAIN_BORDURE_PONT_HAUTEUR = 22;
static const unsigned int TERRAIN_PONT_NBR = 2;

static const Millimeter TERRAIN_SUPPORT_QUILLE_HAUTEUR  = 22;
static const Millimeter TERRAIN_SUPPORT_QUILLE_RAYON    = 100;
static const unsigned int TERRAIN_SUPPORT_QUILLE_NBR = 4;

static const Millimeter QUILLE_HAUTEUR  = 160;
static const Millimeter QUILLE_H_REFLE  = 25;
static const Millimeter QUILLE_H_BOIS   = 95;
static const Millimeter QUILLE_H_VIS    = 37;
static const Millimeter QUILLE_H_TVIS   = 3;
static const Millimeter QUILLE_VIS_R    = 4;
static const Millimeter QUILLE_TVIS_R   = 10;
static const Millimeter QUILLE_RAYON = 30;
static const unsigned int QUILLE_NBR = 30;

static const Millimeter BALLE_GRS_RAYON = 70;
static const unsigned int BALLE_GRS_NBR = 2;

static const Millimeter BALLE_SQUATCH_RAYON = 20;
static const unsigned int BALLE_SQUATCH_NBR = 16;

static const Millimeter OBSTACLE_RAYON = 100;

// palette de couleur pour les printf
#define KB_BLACK        "\033[1;30m"
#define KB_LIGHT_RED    "\033[1;31m"
#define KB_LIGHT_GREEN  "\033[1;32m"
#define KB_YELLOW       "\033[1;33m"
#define KB_LIGHT_BLUE   "\033[1;34m"
#define KB_LIGHT_PURPLE "\033[1;35m"
#define KB_LIGHT_CYAN   "\033[1;36m"
#define KB_WHITE        "\033[1;37m"
#define KB_RED          "\033[0;31m"
#define KB_GREEN        "\033[0;32m"
#define KB_BROWN        "\033[0;33m"
#define KB_BLUE         "\033[0;34m"
#define KB_PURPLE       "\033[0;35m"
#define KB_CYAN         "\033[0;36m"
#define KB_GREY         "\033[0;37m"
#define KB_RESTORE      "\33[0;0m"
#define KB_CLEAR        "\033[0;0H\033[J"

// couleurs adaptées aux messages
#define KB_COMMAND      KB_GREY
#define KB_ERROR        KB_LIGHT_RED
#define KB_WARNING      KB_BROWN
#define KB_OK           KB_LIGHT_GREEN
#define KB_INFO         KB_LIGHT_CYAN
#define KB_FUNCTION     KB_CYAN
#define KB_DEBUG        KB_YELLOW

typedef enum RobotModel {
    ROBOT_MODEL_ATTACK,
    ROBOT_MODEL_DEFENCE,

    ROBOT_MODEL_UNKNOWN,

    ROBOT_MODEL_NBR
} RobotModel;

// ============================================================================
// =========================  enum BridgePosition    ==========================
// ============================================================================

/**
 * @brief Niveau de verbosite d'un composant: quantite de message renvoyes
 *        a l'utilisateur 
 */
typedef enum BridgePosition {
    /** @brief Position inconnue */
    BRIDGE_POS_UNKNOWN=0,
    /** @brief Pret du bord du terrain */
    BRIDGE_POS_BORDURE=1,        
    /** @brief Au milieu cote bord du terrain */
    BRIDGE_POS_MIDDLE_BORDURE=2,
    /** @brief Au milieu cote centre du terrain */
    BRIDGE_POS_MIDDLE_CENTER=3,
    /** @brief Contre le pond du milieu */
    BRIDGE_POS_CENTER=4
} BridgePosition;

// ============================================================================
// ============================  enum RobotColor    ===========================
// ============================================================================

/**
 * @brief Niveau de verbosite d'un composant: quantite de message renvoyes
 *        a l'utilisateur 
 */
typedef enum RobotColor {
    COLOR_ANY,
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_LIGHT_YELLOW,
    COLOR_DARK_YELLOW,
    COLOR_BLUE,
    COLOR_GREEN
} RobotColor;

/**
 * @brief Couleur des équipes
 */
typedef enum TeamColor {
    TEAM_RED,
    TEAM_GREEN
} TeamColor;

// ============================================================================
// ============================  enum DangerLevel  ============================
// ============================================================================

/**
 * @brief Dangerosité des zones visistees par le robot
 */
typedef enum DangerLevel {
    DANGER_NONE,
    DANGER_LOW,
    DANGER_MEDIUM,
    DANGER_HIGH
} DangerLevel;

// ============================================================================
// ============================  enum VerboseLevel   ==========================
// ============================================================================


/**
 * @enum VerboseLevel
 * @brief Niveau de verbosite d'un composant: quantite de message renvoyes
 *         a l'utilisateur = niveau de débug, par défaut: VERBOSE_INFO
 */
typedef enum VerboseLevel 
{
    /** @brief N'affiche aucun message */
    VERBOSE_NO_MESSAGE=0,
    /** @brief N'envoie que les messages de type ERROR */
    VERBOSE_ERROR,
    /** @brief N'envoie que les messages de type OK + WARNING + ERROR */
    VERBOSE_OK,
    /** @brief N'envoie que les messages de type VERBOSE_ERROR+INFO+FUNCTION */
    VERBOSE_INFO,
    /** @brief Affiche toutes les donnees possibles. mode reserve au debug */
    VERBOSE_DEBUG
} VerboseLevel;

static const VerboseLevel VERBOSE_DEFAULT = VERBOSE_INFO;

// ============================================================================
// ============================  enum ObstacleType    =========================
// ============================================================================

/**
 * @enum ObstacleType
 * Type d'obstacle detecte
 */
typedef enum ObstacleType {
    /** @brief N'importe quel type */
    ALL_OBSTACLES=0,
    /** @brief Obstacle detecte par detecteur de distance sharp */
    ENV_OBSTACLE,
    /** @brief Obstacle detecte par un contacteur */
    BUMPER_OBSTACLE,
    /** @brief Obstacle defini par le programmeur (non detecte par un capteur) */
    USER_OBSTACLE,
    /** @brief Obstacle rajoute par le programme pour calculer une trajectoire (non detecte par un capteur) */
    TRAJECTORY_OBSTACLE,

    OBSTACLE_TYPE_NBR
} ObstacleType;

// ============================================================================
// ============================ @enum SkittleStatus   =========================
// ============================================================================

/** 
 * @enum SkittleStatus
 * position d'une quille
 */
typedef enum SkittleStatus {
    /** @brief debout a l'endroit */
    SKITTLE_UP,
    /** @brief debout a l'envers */
    SKITTLE_REVERSED,
    /** @brief renversee */
    SKITTLE_DOWN
} SkittleStatus;


// ============================================================================
// =============================   Types evolues   ============================
// ============================================================================


// ============================================================================
// ===================================== Balles ===============================
// ============================================================================

/** 
 * @class SquatchBall
 * Balle de squatch tiree par les catapultes
 */
class SquatchBall {
 public:
    Point       center;
    Millimeter  altitude;
    
    SquatchBall() : center(), altitude(BALLE_SQUATCH_RAYON) {}
    SquatchBall(Point pt, Millimeter alt) : center(pt), altitude(alt) {}
};

/** 
 * @class GRSBall
 * Grosse balle de GRS
 */
class GRSBall {
 public:
    Point       center;
    Millimeter  altitude;
    
    GRSBall(): center(), altitude(BALLE_GRS_RAYON) {}
    GRSBall(Point pt) : center(pt), altitude(BALLE_GRS_RAYON) {}
};


// ============================================================================
// ==================================== Quilles ===============================
// ============================================================================

/** 
 * @class Skittle
 * quilles
 */
class Skittle {
 public:
    Point         center;
    Millimeter    altitude;  // le bas de la quille
    Radian        direction; // direction (des vis vers le reflechissant) quand la quille est renversee
    SkittleStatus status; 
    RobotColor    color; // COLOR_RED / COLOR_GREEN

    Skittle()
        : center(), altitude(0), direction(0), status(SKITTLE_UP), 
        color(COLOR_RED) {}
    Skittle(Point      pt, 
	    Millimeter alt,
	    bool       red)
        : center(pt), altitude(alt), direction(0), status(SKITTLE_UP), 
        color(red?COLOR_RED:COLOR_GREEN) {}
};


// ============================================================================
// =================================== Obstacles ==============================
// ============================================================================

/**
 * @brief Un obstacle est un objet supposé rond détecté à une date donnée et 
 * dont ont estime grossièrement la position. Il a également une durée de vie 
 * limitée au dela de laquelle on ne doit plus en tenir compte 
 */
class Obstacle
{
 public:
    Obstacle(Point        Center, 
             Point        DetectionPoint, 
	     Millisecond  Date, 
	     Millimeter   Radius=OBSTACLE_RAYON, 
             Millisecond  LifeTime=TIME_INFINITE,
	     ObstacleType Type=ALL_OBSTACLES):
	center(Center), detectionPoint(DetectionPoint), date(Date), 
	radius(Radius), lifeTime(LifeTime), type(Type) {}
    
  Obstacle(Point        Center, 
           Millisecond  Date, 
	   Millimeter   Radius=OBSTACLE_RAYON, 
           Millisecond  LifeTime=TIME_INFINITE,
	   ObstacleType Type=ALL_OBSTACLES):
    center(Center), detectionPoint(center), date(Date), 
    radius(Radius), lifeTime(LifeTime), type(Type) {}

  Obstacle():
    center(Point(-100, -100)), detectionPoint(center), date(TIME_NEVER), 
    radius(OBSTACLE_RAYON), lifeTime(TIME_INFINITE), type(ALL_OBSTACLES) {}

  Obstacle(Obstacle const &o2):
    center(o2.center), detectionPoint(o2.detectionPoint), date(o2.date), 
    radius(o2.radius), lifeTime(o2.lifeTime), type(o2.type) {}

  void setType(ObstacleType t) { type = t; }

 public:
 /** @brief Estimation du centre de l'obstacle */
  Point center;
  /** @brief Point de l'obstacle qui a été détecté par une mesure physique */
  Point detectionPoint;
  /** @brief Date à laquelle l'obstacle a été détecté */
  Millisecond date;
  /** @brief Dimensuion de l'obstacle */
  Millimeter  radius;
  /** @brief Durée de vie de l'obstacle, quand date+lifeTime<TIMER->time(), 
  on ne tient plus compte de l'obstacle */
  Millisecond lifeTime;
  /** @brief Type d'obstacle: (par qui a t'il ete detecte) */
  ObstacleType type; 
};

// ============================================================================
// =================================   Listes   ===============================
// ============================================================================

/** 
 * @class ListIter
 * @brief iterateur sur une liste (n'importe quel type de liste)
 */
template<typename TList, typename TListIter, typename TElem>
class ListIter {
 public:
    ListIter(TList const& list1);
    virtual ~ListIter(){}
    /** Renvoie vraie et renvoie un element de la liste tant qu'on 
        n'est pas au bout de la liste */
    bool getNext(TElem& elem);
 private:
    TListIter iter_;
    TList const& list_;
};


/** @brief Liste de quilles */
typedef std::deque<Skittle>            
    ListOfSkittles;
/** @brief Iterateur sur les balles */
typedef ListIter<ListOfSkittles, ListOfSkittles::const_iterator, Skittle> 
    SkittleIter;


/** @brief Liste d'obstacles */
typedef std::deque<Obstacle>            
    ListOfObstacles;
/** @brief Iterateur sur les obstacles */
typedef ListIter<ListOfObstacles, ListOfObstacles::const_iterator, Obstacle> 
    ObstacleIter;

/** @brief Une trajectoire est un vecteur de points */
typedef std::vector<Point>               
    Trajectory;
/** @brief Iterateur sur les trajectoires */
typedef ListIter<Trajectory, Trajectory::const_iterator, Point> 
    TrajectoryIter;
/** @brief concatener 2 trajectoires */
void concat(Trajectory &target, 
            Trajectory const& tEnd);


/** @brief Point avec des coordonnes entieres  */
typedef RobotPoint2D<int, Radian> PointInt;
/** @brief Liste de points entiers */
typedef std::vector<PointInt> ListOfPointInts;
/** @brief Iterateur sur des points entiers */
typedef ListIter<ListOfPointInts, ListOfPointInts::const_iterator, PointInt> 
    PointIntIter;


// ============================================================================
// =================================== INLINES ================================
// ============================================================================

/**
 * @brief Convertit des radians en degre
 */
inline int r2d(Radian angle)
{
    int r=(int)(angle*180./M_PI);
    while(r<0) r+=360;
    while(r>=360) r-=360;
    return r;
}

/**
 * @brief Convertit des degrés en radian
 */
inline Radian d2r(int angleEnDegre)
{
	return (Radian)(angleEnDegre*M_PI/180.);
}

// ----------------------------------------------------------------------------
// ListIter::ListIter
// ----------------------------------------------------------------------------
template<typename TList, typename TListIter, typename TElem>
inline ListIter<TList, TListIter, TElem>::ListIter(TList const& list1) : 
    list_(list1)
{
    iter_ = list_.begin();
}

// ----------------------------------------------------------------------------
// ListIter::getNext
// ----------------------------------------------------------------------------
template<typename TList, typename TListIter, typename TElem>
bool ListIter<TList, TListIter, TElem>::getNext(TElem& elem)
{
    if (iter_ == list_.end()) return false;
    elem= *iter_;
    iter_++;
    return true;
}

// ----------------------------------------------------------------------------
// concat
// ----------------------------------------------------------------------------
inline void concat(Trajectory &target, 
		   Trajectory const& tEnd)
{
    TrajectoryIter iter(tEnd);
    Point pt;
    while(iter.getNext(pt)) {
        target.push_back(pt);
    }
}

#include <sys/time.h>
// ----------------------------------------------------------------------------
// initialise un generateur de nombre aleatoires
// ----------------------------------------------------------------------------
inline void robotRandInit() 
{
    struct timeval tv_after;
    gettimeofday(&tv_after, NULL);
    srand((int)tv_after.tv_usec);
}

// ----------------------------------------------------------------------------
// initialise un generateur de noombre aleatoires
// ----------------------------------------------------------------------------
inline int robotRand(int from, int to) 
{
    double r = ((double)rand())/RAND_MAX;
    int result = from+(int)((to-from)*r);
    if (result >= to) result--;
    return result;
}

// ============================================================================
// ===================================  MACRO  ================================
// ============================================================================

template<typename T> T max(const T a, const T b) { return a>b? a: b; }
template<typename T> T min(const T a, const T b) { return a<b? a: b; }
template<typename T> T sign(const T a) { return a>=0? (T)1 : (T)(-1); }

// b2s : boolean to string
#ifndef b2s
#define b2s(a) ((a)?"TRUE":"FALSE")
#endif

// ============================================================================
// ===================================  TEST   ================================
// ============================================================================

/** 
 * @brief Testing macro. Initialise test result to OK=true=1 
 */
#define test_begin()                 \
    int lsm_test_result=0;

/** 
 * @brief Testing macro. To be used like assert. Format must be a 
 *        true condition 
 */
#define test_condition(format)					\
     do {							\
       if(!(format)) {						\
          printf("[%s:%d] Test Failure\n", __FILE__, __LINE__);	\
          lsm_test_result++;					\
       }							\
     } while(0)

/** 
 * @brief Testing macro. Return true on success, false on error
 */
#define test_end()  							     \
    if (lsm_test_result==0) 						     \
      printf("[%s:%d] All tests passed\n", __FILE__, __LINE__);		     \
    else       								     \
      printf("[%s:%d] Warning, validation failed (%d failures)!\n", __FILE__, __LINE__, lsm_test_result);                           \
    return (lsm_test_result==0)					     

#endif // __cplusplus

#endif // __ROBOT_TYPES_H__
