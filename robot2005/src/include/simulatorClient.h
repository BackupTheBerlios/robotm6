/**
 * @file simulatorClient.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */

#ifndef __SIMULATOR_CLIENT_H__
#define __SIMULATOR_CLIENT_H__

#include "robotBase.h"
#include "simulatorBase.h"

#define Simulator SimulatorClient::instance()

class Socket;

/**
 * @class SimulatorClient
 * Simule les déplacements du robot, les balles, les autres robot sur la 
 * table et leur interaction
 */
class SimulatorClient : public RobotBase {
 public:
    static SimulatorClient* instance();
    virtual ~SimulatorClient();

    // ------------------------------------------------------------------------
    // initialisation et configuration
    // ------------------------------------------------------------------------
    
    /** 
     * @brief connection au server de simulation. Par defaut le client essaye 
     * de se connecter sur la liste de host definie dans simulatorBase.h 
     */
    bool connectToServer(const char* hostName=NULL);

    /**
     * @brief Nom du robot (util pour savoir quel programme tourne dans quel 
     * robot) 
     */
    void setRobotName(const char* name);

    /**
     * @brief Defini le poids du robot pour savoir s'il peut pousser les autres 
     * robots. Le robot le plus lourd pousse les autres, les robots de meme 
     * poids ne bougent pas
     */
    void setRobotWeight(SimuWeight weight);

    /** 
     * @brief definition du model du robot utiliser pour calculer sa forme
     * et les collisions avec les autres objets du terrain 
     * Un robot de type SIMU_MODEL_UNKNOWN est immateriel et ne genere pas 
     * de collision
     */
    void setRobotModel(RobotModel model);

    /** 
     * @brief Valeurs de coefficient concernant les codeurs, la taille des roues, 
     * l'entre 2 roues ...
     */
    void setRobotMotorCoef(Millimeter D, Millimeter K, double speed);

    /** @brief le robot est considere comme une brique: il peut se deplacer 
        partout, sa position n'est pas inversee quand il change d'equipe */
    void setModeBrick(bool brick);
    
    // ------------------------------------------------------------------------
    // Ordres envoyes au robot
    // ------------------------------------------------------------------------
    /** @brief Met a jour la position du robot */
    void setRobotPosition(Position const& pos);
    /** @brief Renvoie l'etat du match */
    SimuMatchStatus getMatchStatus();
    /** @brief renvoie vrai si la jack de depart est inseree */
    bool isJackin();
    /** @brief renvoie vrai si l'arret d'urgence est enfonce */
    bool isEmergencyStop();
    /** @brief renvoie vrai si l'arret d'urgence est enfonce */
    void getLcdButtonsState(bool& btnYes, bool& btnNo);
    /** @brief Renvoie la valeur simulee des PWM pour le moteur */
    void getPwm(MotorPWM& left, 
		MotorPWM& right);
    /** @brief Met a jour la vitesse envoyee aux moteurs */
    void setSpeed(MotorSpeed left, 
		  MotorSpeed right);
    /** @brief Renvoie la valeur des codeurs des moteurs des roues */
    void getMotorPosition(MotorPosition& left, 
			  MotorPosition& right);
    /** @brief Renvoie la position estimee du robot (simulation de codeurs) */
    void getRobotEstimatedPosition(Point& pt, Radian& dir);
    /** @brief Renvoie la position simulee du robot */
    void getRobotRealPosition(Point& pt, Radian& dir);
    /** @brief envoie un message sur l'afficheur LCD */
    void setLCDMessage(const char* message);

    // ------------------------------------------------------------------------
    // Simulation des capteurs
    // ------------------------------------------------------------------------
    /** 
     * @brief Simulation d'un capteur de distance place horizontalement
     * Retoune la distance au plus proche objet sur le terrain pour capteur 
     * place aux coordonnees cylindriques rPosCaptor, dirPosCaptor, zPosCaptor 
     * centrees sur le robot et regardant dans la direction dirCaptor
     */
    Millimeter getObstacleDistance(Millimeter rPosCaptor, 
                                   Radian dirPosCaptor,
                                   Millimeter zPosCaptor,
                                   Radian dirCaptor);
    /**
     * @brief Simulation d'un bumber definit par un segment horizontal entre
     * 2 points dans des coordonnees cylindriques centrees sur le robot
     */
    bool isCollision(Millimeter rPosCaptorPt1, 
                     Radian dirPosCaptorPt1,
                     Millimeter rPosCaptorPt2, 
                     Radian dirPosCaptorPt2,
                     Millimeter zPosCaptor);
    /**
     * @brief Simulation d'un capteur de distance regardant le sol
     * @return Renvoie la distance entre le capteur et le sol (valeurs negatives
     * si le capteur et sous le sol == le capteur vient de rentrer dans un
     * obstacle)
     */
    Millimeter getGroundDistance(Millimeter rPosCaptor, 
                                 Radian dirPosCaptor,
                                 Millimeter zPosCaptor);
 protected:
    SimulatorClient();
    
    bool connectTryAllPorts(const char* hostName);
    unsigned char* setBufferHeader(SimuRequestType type, 
                                   int dataLength);
    void SimulatorClient::sendBuffer();
    unsigned char* SimulatorClient::recvBuffer(SimuRequestType type);

 private:
    static SimulatorClient* simulatorClient_;
    Socket* socket_;
    unsigned char buffer_[SIMU_BUFFER_LENGTH];
};

#endif // __SIMULATOR_CLIENT_H__
