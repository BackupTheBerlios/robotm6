#ifndef __SHORTEST_WAY_H__
#define __SHORTEST_WAY_H__

#include "robotBase.h"
#include "grid.h"
#include <list>

typedef enum ShortestMode {
    SHORTEST_ZONE_MODE,
    SHORTEST_TARGET_MODE
} ShortestMode;

typedef enum ShortestWayPointMode {
  SHORTEST_INVALID_POINT,
  SHORTEST_VALID_POINT,
  SHORTEST_SOLUTION
} ShortestWayPointMode;

typedef struct ShortestWayStruct {
  int rank;
  ShortestWayPointMode mode;
  int handicap;

  ShortestWayStruct() : rank(INT_MAX), 
			mode(SHORTEST_VALID_POINT), 
			handicap(0) {}
} ShortestWayStruct;

typedef struct ShortestWayProp {
  int rank;
  unsigned int i;
  unsigned int j;
} ShortestWayProp;
typedef std::list<ShortestWayProp> ShortestWayPropList;

inline bool operator == (ShortestWayProp const& prop1, 
			 ShortestWayProp const& prop2)
{
  return prop1.i == prop2.i && prop1.j == prop2.j;
}

inline bool operator < (ShortestWayProp const& prop1, 
			ShortestWayProp const& prop2)
{
  return prop1.rank < prop2.rank;
}

typedef enum ExploreZone {
  EXPLORE_X0_Y0=0,
  EXPLORE_X1_Y0,
  EXPLORE_X2_Y0,
  EXPLORE_X0_Y1,
  EXPLORE_X1_Y1,
  EXPLORE_X2_Y1,

  EXPLORE_ZONE_NBR
} ExploreZone;

typedef struct ExploreZoneStruct {
  ExploreZone zone;
  const char* name;
  Millimeter  left;
  Millimeter  lower;
  Millimeter  right;
  Millimeter  upper;
} ExploreZoneStruct;

// taille des grilles utilisees pour le calcul de trajectoires
static const Millimeter SHORTEST_WAY_STEP_X = 50;
static const Millimeter SHORTEST_WAY_STEP_Y = 50;
static const unsigned int SHORTEST_WAY_X = 60;
static const unsigned int SHORTEST_WAY_Y = 42;
static const unsigned int SHORTEST_EXPLORE_NBR_PT=200;
static const unsigned int SHORTEST_EXPLORE_MAX_CASE=100;
/**
 * @class ShortestWay
 * Calcul le chemin le plus court evitant les obstacles pour aller d'un point
 * a un autre du terrain
 */
class ShortestWay: public RobotBase
{
 public:
  ShortestWay(RobotConfig const* config, 
	      Millimeter margin=ROBOT_RAYON_PLUS_MARGE);
  virtual ~ShortestWay();

  /**
   * @brief Set the list of obstacles to consider
   */
  void setObstacleList(ListOfObstacles* list);
#ifdef ROBOT_EXPLORE
  /**
   * @brief Add a coconut obstacle and its symetrical point
   */
  void setCoconutPosition(Point* coconut1);
#endif
  /**
   * @brief Set the list of dangerous zones
   */
  void setDangerGrid(Grid<DangerLevel>* dangerGrid);
  /**
   * @brief Set utilise la direction du robot adverse vue par le gonio 
   * pour definir un cone de zone ou le cout de passage est plus eleve
   */
  void setEnemyDirection();
  /**
   * @brief Set the costly zone near the border=> the robot will avoid this zone
   */
  void setDangerousBorder(Millimeter borderZone);
#ifdef ROBOT_EXPLORE
  /**
   * @brief Set the list of visited positions
   */
  void setVisitedGrid(Grid<Millisecond>* visitedGrid);
  /**
   * @brief Set the list of ball positions
   */
  void setBallGrid(Grid<bool>* ballGrid);
#endif

  /**
   * @brief Set the starting point of the trajectory
   */
  void setStartingPoint(Point const& pt); 

  /**
   * @brief Return the shortest trajectory to go from current robot position to
   * 'target' and by avoiding obstacles that have been inserted using
   * base_registerCollision in the last 'oldestObstacleDelay' milliseconds
   * @param target destination point, must be in a valid area (not on an 
   *        obstacle)
   * @param trajectory list of point that will be updated. Must != NULL
   * @param oldestObstacleDelay Maximum delay between collision detection and 
   *        current time
   * @return TRUE  if a trajectory has been found\n
   *         FALSE if the starting or ending point were invalid (on an 
   *         obstacle) or if there is not trajectory that avoid all obstacles
   */
  bool getShortestTrajectoryToTarget(Point       target,
				     Trajectory& trajectory,
				     Millisecond oldestObstacleDelay=INFINITE_TIME);
  bool getShortestTrajectoryToDrop(Trajectory& trajectory,
				   Millisecond oldestObstacleDelay=INFINITE_TIME);
  bool getShortestTrajectoryToTouchDown(Trajectory& trajectory,
					Millisecond oldestObstacleDelay=INFINITE_TIME,
					bool continueExploration=false);
#ifdef ROBOT_EXPLORE
  /** 
   * @brief Calcul une trajectoire d'exploration de la carte à partir de la 
   * position courante 
   */
  bool getExploringTrajectory(Trajectory& trajectory,
			      Millisecond oldestObstacleDelay=INFINITE_TIME);

  /**
   * @brief Renvoie le meilleur point du voisinage à visiter
   */
  bool getExploringPoint(Point       &nextPt,
			 Millisecond oldestObstacleDelay=INFINITE_TIME);
#endif
  /**
   * @brief display the grid in the ascii art format
   */
  void displayASCII();

 private:
  /**
   * @brief Clear grid values and reset obstacles position
   */
  void resetGridValues(bool attractiveBorder,
		       Millisecond oldestObstacleDelay);
  /**
   * @brief Enter an invalid position on the field (the robot must not run 
   * over there)
   */
  void addImpossibleCircle(Point center,
			   Millimeter radius);

  /**
   * @brief Remplace le point de depart par le point valid le plus proche
   * Retourne false si pas de point trouve
   */
  bool useNearestValidPoint();
 protected:
  /**
   * @brief Return TRUE if the point is valid (in the grid and not on an 
   * obstacle)
   */
  bool isPointValid(Point pt);
  ShortestWayStruct** getGrid() { return grid_;}
  int getI(Millimeter x);
  int getJ(Millimeter y);
  friend class TrajecGen;

 private:
  // ==================== Chemin le plus court ===========================

  /**
   * @brief Update the grid value for one point and the list of points to 
   * visit. You should not run it on a grid point which have not a higher 
   * value than counter.
   * @return TRUE on success, FALSE if there is a problem (should never happen)
   */
  bool addPropagatePoint(unsigned int i, unsigned int j, int counter);
  /**
   * @brief update the grid data so that each case contains a score 
   * corresponding to the minimal distance to the ending point
   * Note that to speed up the function, some grid points may not be scored 
   * if they are not on the shortest path between (startX_,startY_) and 
   * (endX_,endY_).\n
   * Then to get the minimal path bettwen (startX_,startY_) and (endX_,endY_),
   * just start from (startX_,startY_) and move point, by point to get the 
   * minimal score of all points neighboors
   *
   * @return TRUE if there is a path between start and end
   */
  bool propagateCounter();
  /**
   * @brief Returns the shortest way to go from startingPoint to
   *        endingPoint and avoid obstacles
   * @param trajectory: list of points to go one. Must not be a NULL pointer!
   * @return TRUE on success (trajectory filled), or FALSE if it is not 
   *         possible to go from one point to another
   */
  bool getSolution(Trajectory& trajectory);

  // ======================  Exploration  ========================
#ifdef ROBOT_EXPLORE
  Radian getBestStartingDir(Point  currentPt,
			    Radian currentDir);
  /** @brief Retourne le prochain point a teste dans la direction donnee */
  Point getDirNextPoint(Point        pt, 
			Radian       direction,
			bool         extended=false);
  /** @brief Score d'une ligne jusqu'a l'obstacle le plus proche 
      score bas = lige a explorer */
  int  scoreLine(Point        pt, 
		 Radian       direction,
		 unsigned int maxNumCase);
  /** @brief Score d'un point 
      score bas = lige a explorer */
  int  scorePoint(Point pt);
  /** @brief Trouve le meilleur point dans le voisinage immediat du point 
      donne. Retourne false si aucun point n'est trouve */
  bool getBestNeighboor(Point  currentPt,
			Radian currentDir,
			Point  &nextPt,
			unsigned int maxNumCase=SHORTEST_EXPLORE_MAX_CASE);
  /** @brief Retourne TRUE si le point a deja ete visite ou si le point est 
      sur la trajectoire qui est en train d'etre generee */
  bool isPointVisited(Point pt);
  /** @brief Retourne true si pt2 a visite plus de VISITED_TIME/2s avant pt1 */
  bool isMajorVisitedDiff(Point pt1, Point pt2);
  /** @brief retourne le score d'une zone du terrain
      score bas = a explorer, score tres haut (>) zone a quitter */
  double scoreZone(ExploreZone zone);
  /** @brief Retourne la zone correspondant au point donne */
  ExploreZone getZone(Point pt);
  /** @brief Retourne la trajectoire permettant d'aller dans la zone donnee */
  bool getTrajectoryToBestZone(ExploreZone zone,
			       Trajectory& trajectory);
  /** @brief Retourne le score d'une zone sans obstalces et non visitee (sert 
      de reference pour normaliser le resultat de scoreZone */
  void getDefaultZoneScores();
  /** @brief Trouve le point le plus proche de pt qui soit valide */
  Point findNearestValidPoint(Point pt);
  ExploreZone getOldestVisitedZone();
#endif

  // ================= grille du chemin plus court =====================
  bool isIndexValid(int i, int j);
  Point getPoint(int i, int j);

  // ---------------- Members ------------------------------------------
 private:
  ShortestWayStruct   *grid_[SHORTEST_WAY_X];
  Point               start_;
  Point               target_;
  int                 startI_;
  int                 startJ_;
  ShortestWayPropList propagateList_; // List of points to visit during 
                                      // computation
  int                 shortestWayBorder_; // border zone where the trip is dangerous
  ListOfObstacles*    obstacles_;
  Grid<DangerLevel>*  dangerGrid_;  // list of zones to avoid 

#ifdef ROBOT_EXPLORE
  Grid<Millisecond>*  visitedGrid_; // list of visited zones
  Grid<bool>*         ballGrid_;    // list of zones where a ball has been detected
  Point*              coconutPoint_;
  bool                demiTour_; 
  Trajectory*         exploreTrajec_;
#endif
  Millimeter          margin_;

  ShortestMode        mode_;
  int                 endI_;
  int                 endJ_;
};

// ============================================================================
// INLINE Functions
// ============================================================================
inline void ShortestWay::setObstacleList(ListOfObstacles* obstacleList)
{
    obstacles_ = obstacleList;
}
#ifdef ROBOT_EXPLORE
inline void ShortestWay::setCoconutPosition(Point* coconut1)
{
    coconutPoint_=coconut1;
}

inline void ShortestWay::setVisitedGrid(Grid<Millisecond>* visitedGrid)
{
    visitedGrid_ = visitedGrid;
}
 
inline void ShortestWay::setBallGrid(Grid<bool>* ballGrid)
{
    ballGrid_ = ballGrid;
}
#endif

inline void ShortestWay::setDangerGrid(Grid<DangerLevel>* dangerGrid)
{
    dangerGrid_ = dangerGrid;
}

inline int ShortestWay::getI(Millimeter x)
{
  return (int)(x/TERRAIN_X*SHORTEST_WAY_X);
}

inline int ShortestWay::getJ(Millimeter y)
{
  return (int)(y/TERRAIN_Y*SHORTEST_WAY_Y);
}

inline Point ShortestWay::getPoint(int i, int j)
{
  if (!isIndexValid(i,j)) return Point();
  return Point((i+0.5)*TERRAIN_X/SHORTEST_WAY_X,
	       (j+0.5)*TERRAIN_Y/SHORTEST_WAY_Y);
}

inline void ShortestWay::setDangerousBorder(Millimeter borderZone)
{
    shortestWayBorder_ = getI(borderZone);
}

#endif // __SHORTEST_WAY_H__
