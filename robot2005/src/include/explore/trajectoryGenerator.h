/**
 * @file trajectoryGenerator.h
 *
 * Fonctions de calcul de trajectoires
 *
 * @date   2003/11/23
 * @author Laurent Saint-Marcel
 */

#ifndef __TRAJECTORY_GENERATOR_H__
#define __TRAJECTORY_GENERATOR_H__

#include "robotBase.h"
//#include "grid.h" TODO: 2004-code? [flo]
#include "events.h"
#include "robotPosition.h"
//#include "explore/shortestWay.h" TODO: 2004-code? [flo]

#define TRAJGEN TrajecGen::instance()

// taille des grilles utilisees pour le calcul de trajectoires
static const unsigned int TRAJGEN_GRID_X = 20;
static const unsigned int TRAJGEN_GRID_Y = 14;

/**
 * @class TrajecGen
 * Generateur de trajectoires pour le robot
 */
class TrajecGen : public RobotComponent {
 public:
    TrajecGen();
    virtual ~TrajecGen();
    static TrajecGen* instance();

    bool reset();
    bool validate();

    /** @brief Renvoie TRUE si le point peut etre utilise comme depart 
        de trajectoire */
    bool isPointValid(Point const& pt);
    
    /** @brief Ajoute un obstacle */
    void addObstacle(Obstacle const& obstacle);
    /** @brief Ajoute une lsite d'obstacles */
    void addObstacles(ListOfObstacles const& obstacles);
    /** @brief Retourne le ombre d'obstacles dans la liste */
    int obstaclesSize();
    /** @brief Enleve un obstacle (le dernier de la liste: pop_back) */
    void removeOldestObstacle();
    /** @brief Ajoute un obstacle de la taille d'un cocotier*/
    void addCoconut(Point coconutCenter);
    /** @brief Met a jour la direction du robot principal ennemi */
    void setEnnemyRPDir(Radian dir, Point conioPoint);
    /** @brief Met a jour la direction du robot secondaire ennemi */
    void setEnnemyRSDir(Radian dir, Point conioPoint);

    /**
     * @brief Calcule une trajectoire qui permet d'aller a une position 
     *  ou on peut utiliser la catapulte 
     */
    bool getTrajectoryToDrop(Trajectory& trajectory);
    /**
     * @brief Calcule une trajectoire qui permet d'aller a une position 
     *  ou on peut marquer un essai
     */
    bool getTrajectoryToTouchDown(Trajectory& trajectory,
				  bool useVisitedTrajectory); 
    /**
     * @brief Calcule une trajectoire qui permet d'aller a une position 
     *  ou on peut sortir un bras du robot qui tangente un cocoiter
     */
    bool getTrajectoryToCoconut(Point coconutCenter,
                                Trajectory& trajectory);
    /**
     * @brief Calcule une trajectoire d'exploration de l'aire de jeu
     */
#ifdef ROBOT_EXPLORE
    bool getTrajectoryExploring(Trajectory& trajectory);
#else
    bool getTrajectoryExploring(Point const& fromPt,
                                Point const& toPt,
                                Point & endPt,
                                Trajectory& trajectory);

    bool getTrajectoryToZone(Point const& zoneCenter,
			     Trajectory& trajectory) {
	return getTrajectoryToCoconut(zoneCenter, trajectory);
    }

#endif
    /**
     * @brief Vide la liste d'obstacle, la position des cocotiers, la 
     * direction des ennemis.
     */
    void clearObstacleList();

    /**
     * @brief Vide la liste des zones deja explorees
     */
    void clearExplorationZone();
  
    /** @brief Remove useless points of the trajectory */
    void reniceTrajectory(Trajectory& trajectory);

    /** @brief Remove useless points of the trajectory */
    void reniceTrajectoryStep1(Trajectory& trajectory);

    /** @brief Remove useless points of the trajectory for logs */
    static void reniceTrajectory2(Trajectory const& trajectory,
				  Trajectory& newTrajectory);


    /** Pour le debug dans trajectoryTest */
    /* TODO: 2004-code to be refactored for 2005 [flo]
    Grid<Millisecond>const * getVisitedGrid() const;
    ShortestWayStruct** getGrid() { return shortestWay_.getGrid();}
    void getGridIndex(Point const& pt, int &i, int &j)
	{ i=shortestWay_.getI(pt.x); j=shortestWay_.getJ(pt.y); }
    */
#ifdef ROBOT_EXPLORE
 protected:
    void ballEatenCB();
    void periodicTaskCB(Millisecond time);

    friend void TrajGenBallEvtCB(void*, Events);
    friend void TrajGenPeriodicCB(void*, Millisecond);
#endif

 private: 
    typedef struct EnnemyDirStruct {
        Radian dir;
        Point  center;
        EnnemyDirStruct():dir(0), center(-1,0){}
    } EnnemyDirStruct;
 private:
    static TrajecGen * trajecGen_;

    ListOfObstacles*   obstacleList_;
    EnnemyDirStruct    dirEnnemyRP_;
    EnnemyDirStruct    dirEnnemyRS_;
    Point              coconutCenter1_;
    Point              coconutCenter2_;
    /* TODO: 2004-code. to be refactored for 2005 [flo]
    Grid<Millisecond>  visitedGrid_; // list of visited zones
    Grid<DangerLevel>  dangerGrid_;  // list of zones to avoid
    Grid<bool>         ballGrid_;    // list of zones where a ball has been detected
    */
//    ShortestWay  shortestWay_;
};

// ---------------------------------------------------------------------------
// TrajecGen::instance
// ---------------------------------------------------------------------------
inline TrajecGen * TrajecGen::instance()
{
    assert(trajecGen_ != NULL);
    return trajecGen_;
}

/* TODO: 2004-code. to be refactored for 2005 [flo]
// ---------------------------------------------------------------------------
// TrajecGen::getVisitedGrid
// ---------------------------------------------------------------------------
inline Grid<Millisecond>const * TrajecGen::getVisitedGrid() const 
{ 
    return &visitedGrid_;
}
*/

#endif // __TRAJECTORY_GENERATOR_H__
