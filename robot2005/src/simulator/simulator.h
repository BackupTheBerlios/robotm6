/**
 * @file simulator.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */

#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "robotBase.h"
#include "simulatorBase.h"

class SimulatorServer;
class SimulatorConnection;
class SimulatorRobot;
class SimulatorSkittle;
class SimulatorGrsBall;

/**
 * @class Simulator
 * Simule les déplacements du robot, les balles, les autres robot sur la 
 * table et leur interaction
 */
class SimulatorCL : public RobotBase {
 public:
    SimuMatchStatus getStatus();
    static SimulatorCL* instance();

    SimulatorCL();
    virtual ~SimulatorCL();
    // -------------------------------------------------------------------
    // Simulation des capteurs
    // -------------------------------------------------------------------
    /** 
     * @brief Simulation d'un capteur de distance place horizontalement
     * Retoune la distance au plus proche objet sur le terrain pour capteur 
     * place aux coordonnees cylindriques rPosCaptor, dirPosCaptor, zPosCaptor 
     * centrees sur le robot et regardant dans la direction dirCaptor
     */
    Millimeter getObstacleDistance(SimulatorRobot* robot,
                                   Millimeter rPosCaptor, 
                                   Radian dirPosCaptor,
                                   Millimeter zPosCaptor,
                                   Radian dirCaptor);
    /**
     * @brief Simulation d'un bumber definit par un segment horizontal entre
     * 2 points dans des coordonnees cylindriques centrees sur le robot
     */
    bool isCollision(SimulatorRobot* robot,
		     Millimeter rPosCaptorPt1, 
		     Radian dirPosCaptorPt1,
		     Millimeter rPosCaptorPt2, 
		     Radian dirPosCaptorPt2,
		     Millimeter zPosCaptor);
    /**
     * @brief Simulation d'un capteur de distance regardant le sol
     * @return Renvoie la distance entre le capteur et le sol (valeurs 
     * negatives si le capteur et sous le sol == le capteur vient de rentrer
     * dans un obstacle)
     */
    Millimeter getGroundDistance(SimulatorRobot* robot,
                                 Millimeter rPosCaptor, 
                                 Radian dirPosCaptor,
                                 Millimeter zPosCaptor);
    
    /** @brief Met a jour la position des objets sur le terrain */
    void update();
    /** @brief affichage des objets qui ont bouge */
    void draw();

    /** @brief remet tous les objets a leur position de depart */
    void reset();

    /** @brief renvoie les infos sur un robot, robotId=[0..3] */
    SimulatorRobot* getRobot(int robotId);
    /** @brief deplace le pont */
    void changeBridge();
    /** @brief change la position des supports de quilles */
    void changeSupport();
#define SIMU_WALL_BORDER_PTS_NBR   4
#define SIMU_BRIDGE_BORDER_PTS_NBR 14
#define SIMU_RIVER_BORDER_PTS_NBR  18
    Point* getWallPts()   { return wallBorderPts_; }
    Point* getBridgePts() { return bridgeXPts_; }
    Point* getRiverPts()  { return riverYPts_; }
    bool isInRiver(Point const& pt); 
    double getSimulationSpeed() { return simulationSpeed_; }
 protected:
    void registerViewerBtnCB();
    void createSkittlePacket(Skittle* skittles,
                             Point center, 
                             Millimeter altitude, 
                             bool four,
                             bool red);
    void setBridgeBorder();
    void setWallBorder();
    void computeSimulationSpeed();
 private:
    static SimulatorCL* simulator_;
    SimulatorServer* server_;

    SquatchBall    sBalls_[BALLE_SQUATCH_NBR];
    GRSBall        gBalls_[BALLE_GRS_NBR];
    Skittle        skittles_[QUILLE_NBR];
    SimulatorGrsBall* sgBalls_[BALLE_GRS_NBR];
    SimulatorSkittle* sskittles_[QUILLE_NBR];
    BridgePosition bridge_;
    Point          supportPosition1_;
    Point          supportPosition2_;
    Point          supportPosition3_;
    int            supportConfigId_;

    Point          wallBorderPts_[SIMU_WALL_BORDER_PTS_NBR];
    Point          bridgeXPts_[SIMU_BRIDGE_BORDER_PTS_NBR];
    Point          riverYPts_[SIMU_RIVER_BORDER_PTS_NBR];
    double         simulationSpeed_;
};

#endif // __SIMULATOR_H__
