#include "simulatorClient.h"
#include "network.h"
#include "log.h"

static const unsigned int SIMU_HOST_NBR = 3;
static const char* SIMU_HOSTS[SIMU_HOST_NBR] = { 
    SIMU_HOSTNAME_LOCALHOST,
    SIMU_HOSTNAME_PROJET5,
    SIMU_HOSTNAME_PORTABLE
};

SimulatorClient* SimulatorClient::simulatorClient_=NULL;

// ----------------------------------------------------------------------------
// SimulatorClient::setBufferHeader
// ----------------------------------------------------------------------------
unsigned char* SimulatorClient::setBufferHeader(SimuRequestType type, 
                                                int dataLength)
{
    buffer_[0] = type;
    buffer_[1] = dataLength;
    return &(buffer_[2]);
}

// ----------------------------------------------------------------------------
// SimulatorClient::sendBuffer
// ----------------------------------------------------------------------------
void SimulatorClient::sendBuffer()
{
    if (socket_) socket_->write(buffer_, buffer_[1] + 2);
}

// ----------------------------------------------------------------------------
// SimulatorClient::recvBuffer
// ----------------------------------------------------------------------------
unsigned char* SimulatorClient::recvBuffer(SimuRequestType type)
{
    if (socket_) {
        if (socket_->read(buffer_, 2) != 2) return NULL; 
        if (socket_->read(buffer_+2, buffer_[1])) return NULL; 
        if (buffer_[0] == type) return buffer_;
    } 
    return NULL;
}

// ----------------------------------------------------------------------------
// SimulatorClient::SimulatorClient
// ----------------------------------------------------------------------------
SimulatorClient::SimulatorClient() : 
    RobotBase("Simulator Client", CLASS_SIMULATOR), socket_(NULL)
{
}
   
 
// ----------------------------------------------------------------------------
// SimulatorClient::~SimulatorClient
// ----------------------------------------------------------------------------
SimulatorClient::~SimulatorClient()
{
    if (socket_) {
        socket_->close();
        delete socket_;
    } 
    simulatorClient_ = NULL;
}
  
// ----------------------------------------------------------------------------
// SimulatorClient::instance
// ----------------------------------------------------------------------------
SimulatorClient* SimulatorClient::instance()
{
    if (!simulatorClient_) simulatorClient_ = new SimulatorClient();
    return simulatorClient_;
}

// ----------------------------------------------------------------------------
// SimulatorClient::connectTryAllPorts
// ----------------------------------------------------------------------------
bool SimulatorClient::connectTryAllPorts(const char* hostName)
{
    if (hostName) {
        if (socket_ && socket_->openSender(SIMU_PORT, hostName)) {
            return true;
	}
    } 
    return false;
}

// ----------------------------------------------------------------------------
// SimulatorClient::connectToServer
// ----------------------------------------------------------------------------
// connection au server de simulation 
// ----------------------------------------------------------------------------
bool SimulatorClient::connectToServer(const char* hostName)
{
    if (!socket_) socket_ = new Socket();
    if (connectTryAllPorts(hostName)) {
        return true;
    }
    for(unsigned int i=0; i<SIMU_HOST_NBR; i++) {
	if (connectTryAllPorts(SIMU_HOSTS[i])) {
	    return true;
	}
    }
    if (socket_) {
        delete socket_; 
        socket_ = NULL;
    }
    return false;
}


// ----------------------------------------------------------------------------
// SimulatorClient::setRobotName
// ----------------------------------------------------------------------------
// Nom du robot (util pour savoir quel programme tourne dans quel robot) 
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotName(const char* name)
{
    if (!socket_) return;
    if (!name) return;
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_NAME,  strlen(name)+1);
    strcpy(buf, name);
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotWeight
// ----------------------------------------------------------------------------
// Defini la position du robot
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotPosition(Position const& pos) 
{
    if (!socket_) return;
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_POS,  sizeof(Position));
    memcpy(buf, &pos, sizeof(Position));
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotWeight
// ----------------------------------------------------------------------------
// Defini le poids du robot pour savoir s'il peut pousser les autres 
// robots. Le robot le plus lourd pousse les autres, les robots de meme 
// poids ne bougent pas
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotWeight(SimuWeight weight)
{
    if (!socket_) return;
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_WEIGHT,  1);
    buf[0] = (int)weight;
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotModel
// ----------------------------------------------------------------------------
// definition du model du robot utiliser pour calculer sa forme
// et les collisions avec les autres objets du terrain 
// Un robot de type SIMU_MODEL_UNKNOWN est immateriel et ne genere pas 
// de collision
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotModel(RobotModel model)
{
    if (!socket_) return;
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_MODEL,  1);
    buf[0] = (int)model;
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotMotorCoef
// ----------------------------------------------------------------------------
// Valeurs de coefficient concernant les codeurs, la taille des roues, 
// l'entre 2 roues ...
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotMotorCoef(Millimeter D, Millimeter K, double speed)
{
    if (!socket_) return;
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_MOTOR_COEF,  
                                         2*sizeof(Millimeter)
                                         +sizeof(double));
    memcpy(buf, &D, sizeof(Millimeter)); buf += sizeof(Millimeter);
    memcpy(buf, &K, sizeof(Millimeter)); buf += sizeof(Millimeter);
    memcpy(buf, &speed, sizeof(double));
    sendBuffer();
}
 

// ----------------------------------------------------------------------------
// SimulatorClient::setModeBrick
// ----------------------------------------------------------------------------
void SimulatorClient::setModeBrick(bool brick)
{
    if (!socket_) return;
    setBufferHeader(brick?SIMU_REQ_SET_BRICK:SIMU_REQ_SET_NORMAL, 0);
    sendBuffer();
}
   
// ============================================================================
// Ordres envoyes au robot
// ============================================================================
  
// ----------------------------------------------------------------------------
// SimulatorClient::getMatchStatus
// ----------------------------------------------------------------------------
// Renvoie l'etat du match
// ----------------------------------------------------------------------------
SimuMatchStatus SimulatorClient::getMatchStatus()
{
    if (!socket_) return SIMU_STATUS_NEED_RESET;
    setBufferHeader(SIMU_REQ_GET_STATUS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_STATUS);
    if (!buf) return SIMU_STATUS_NEED_RESET;
    return (SimuMatchStatus)buf[0];
}

// ----------------------------------------------------------------------------
// SimulatorClient::isJackin
// ----------------------------------------------------------------------------
// renvoie vrai si la jack de depart est inseree
// ----------------------------------------------------------------------------
bool SimulatorClient::isJackin()
{
    if (!socket_) return false;
    setBufferHeader(SIMU_REQ_GET_JACKIN, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_JACKIN);
    if (!buf) return false;
    return (bool)buf[0];
}

// ----------------------------------------------------------------------------
// SimulatorClient::isEmergencyStop
// ----------------------------------------------------------------------------
// renvoie vrai si l'arret d'urgence est enfonce
// ----------------------------------------------------------------------------
bool SimulatorClient::isEmergencyStop()
{
    if (!socket_) return false;
    setBufferHeader(SIMU_REQ_GET_EMERGENCY, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_EMERGENCY);
    if (!buf) return false;
    return (bool)buf[0];
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
// renvoie vrai si l'arret d'urgence est enfonce
// ----------------------------------------------------------------------------
void SimulatorClient::getLcdButtonsState(bool& btnYes, bool& btnNo)
{
    if (!socket_) return;
    setBufferHeader(SIMU_REQ_GET_LCD_BUTTONS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_LCD_BUTTONS);
    if (!buf) return;
    btnYes = buf[0] & 0x01;
    btnNo  = buf[0] & 0x02;
}

// ----------------------------------------------------------------------------
// SimulatorClient::getPwm
// ----------------------------------------------------------------------------
// Renvoie la valeur simulee des PWM pour le moteur
// ----------------------------------------------------------------------------
void SimulatorClient::getPwm(MotorPWM& left, 
                             MotorPWM& right)
{
    if (!socket_) return;
    setBufferHeader(SIMU_REQ_GET_PWM, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_PWM);
    if (!buf) return;
    memcpy(&left, buf, sizeof(MotorPWM)); buf += sizeof(MotorPWM);
    memcpy(&right, buf, sizeof(MotorPWM));
}

// ----------------------------------------------------------------------------
// SimulatorClient::setSpeed
// ----------------------------------------------------------------------------
// Met a jour la vitesse envoyee aux moteurs
// ----------------------------------------------------------------------------
void SimulatorClient::setSpeed(MotorSpeed left, 
                               MotorSpeed right)
{
    if (!socket_) return;
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_MOTOR_SPEED,  
                                         2*sizeof(MotorSpeed));
    memcpy(buf, &left, sizeof(MotorSpeed)); buf += sizeof(MotorSpeed);
    memcpy(buf, &right, sizeof(MotorSpeed));
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::getMotorPosition
// ----------------------------------------------------------------------------
// Renvoie la valeur des codeurs des moteurs des roues
// ----------------------------------------------------------------------------
void SimulatorClient::getMotorPosition(MotorPosition& left, 
                                       MotorPosition& right)
{
    if (!socket_) return;
    setBufferHeader(SIMU_REQ_GET_MOTOR, 0);
    sendBuffer(); 

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_MOTOR);
    if (!buf) return;
    memcpy(&left, buf, sizeof(MotorPosition)); buf += sizeof(MotorPosition);
    memcpy(&right, buf, sizeof(MotorPosition));
}

// ----------------------------------------------------------------------------
// SimulatorClient::getRobotEstimatedPosition
// ----------------------------------------------------------------------------
// Renvoie la position estimee du robot (simulation de la carte odometrie)
// ----------------------------------------------------------------------------
void SimulatorClient::getRobotEstimatedPosition(Point& pt, Radian& dir)
{
    if (!socket_) return;
    setBufferHeader(SIMU_REQ_GET_ESTIMATE_POS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_ESTIMATE_POS);
    if (!buf) return;
    memcpy(&pt, buf, sizeof(Point)); buf += sizeof(Point);
    memcpy(&dir, buf, sizeof(Radian));
}

// ----------------------------------------------------------------------------
// SimulatorClient::getRobotRealPosition
// ----------------------------------------------------------------------------
// Renvoie la position simulee du robot
// ----------------------------------------------------------------------------
void SimulatorClient::getRobotRealPosition(Point& pt, Radian& dir)
{
    if (!socket_) return;
    setBufferHeader(SIMU_REQ_GET_REAL_POS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_REAL_POS);
    if (!buf) return;
    memcpy(&pt, buf, sizeof(Point)); buf += sizeof(Point);
    memcpy(&dir, buf, sizeof(Radian));
}

// ----------------------------------------------------------------------------
// SimulatorClient::setLCDMessage
// ----------------------------------------------------------------------------
// envoie un message sur l'afficheur LCD
// ----------------------------------------------------------------------------
void SimulatorClient::setLCDMessage(const char* message)
{
    if (!socket_) return;
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_LCD,  
				       strlen(message)+1);
    strcpy(buf, message);
    sendBuffer();
}

// ============================================================================
// Simulation des capteurs
// ============================================================================

// ----------------------------------------------------------------------------
// SimulatorClient::getObstacleDistance
// ----------------------------------------------------------------------------
// Simulation d'un capteur de distance place horizontalement
// ----------------------------------------------------------------------------
Millimeter SimulatorClient::getObstacleDistance(Millimeter rPosCaptor, 
                                                Radian dirPosCaptor,
                                                Millimeter zPosCaptor,
                                                Radian dirCaptor)
{
    if (!socket_) return INFINITE_DIST;
    unsigned char* buf=setBufferHeader(SIMU_REQ_GET_OBSTACLE_DIST, 
                                       2*sizeof(Millimeter)+2*sizeof(Radian));
    memcpy(buf, &rPosCaptor, sizeof(Millimeter)); buf+=sizeof(Millimeter);
    memcpy(buf, &dirPosCaptor, sizeof(Radian));   buf+=sizeof(Radian);
    memcpy(buf, &zPosCaptor, sizeof(Millimeter)); buf+=sizeof(Millimeter);
    memcpy(buf, &dirCaptor, sizeof(Radian)); 
    sendBuffer();

    buf=recvBuffer(SIMU_REQ_SET_OBSTACLE_DIST);
    if (!buf) return INFINITE_DIST;
    Millimeter result;
    memcpy(&result, buf, sizeof(Millimeter)); 
    return result;
}

// ----------------------------------------------------------------------------
// SimulatorClient::isCollision
// ----------------------------------------------------------------------------
// Simulation d'un bumber definit par un segment horizontal entre
// 2 points dans des coordonnees cylindriques centrees sur le robot
// ----------------------------------------------------------------------------
bool SimulatorClient::isCollision(Millimeter rPosCaptorPt1, 
                                  Radian dirPosCaptorPt1,
                                  Millimeter rPosCaptorPt2, 
                                  Radian dirPosCaptorPt2,
                                  Millimeter zPosCaptor)
{
    if (!socket_) return false;
    unsigned char* buf=setBufferHeader(SIMU_REQ_GET_COLLISION, 
                                       3*sizeof(Millimeter)+2*sizeof(Radian));
    memcpy(buf, &rPosCaptorPt1, sizeof(Millimeter)); buf+=sizeof(Millimeter);
    memcpy(buf, &dirPosCaptorPt1, sizeof(Radian));   buf+=sizeof(Radian);
    memcpy(buf, &rPosCaptorPt2, sizeof(Millimeter)); buf+=sizeof(Millimeter);
    memcpy(buf, &dirPosCaptorPt2, sizeof(Radian));   buf+=sizeof(Radian);
    memcpy(buf, &zPosCaptor, sizeof(Millimeter));
    sendBuffer();

    buf=recvBuffer(SIMU_REQ_SET_COLLISION);
    if (!buf) return false;
    return (bool)buf[0];
}

// ----------------------------------------------------------------------------
// SimulatorClient::getGroundDistance
// ----------------------------------------------------------------------------
// Simulation d'un capteur de distance regardant le sol
// ----------------------------------------------------------------------------
Millimeter SimulatorClient::getGroundDistance(Millimeter rPosCaptor, 
                                              Radian dirPosCaptor,
                                              Millimeter zPosCaptor)
{
    if (!socket_) return INFINITE_DIST;
    unsigned char* buf=setBufferHeader(SIMU_REQ_GET_GROUND_DIST, 
                                       2*sizeof(Millimeter)+sizeof(Radian));
    memcpy(buf, &rPosCaptor, sizeof(Millimeter)); buf+=sizeof(Millimeter);
    memcpy(buf, &dirPosCaptor, sizeof(Radian));   buf+=sizeof(Radian);
    memcpy(buf, &zPosCaptor, sizeof(Millimeter)); 
    sendBuffer();

    buf=recvBuffer(SIMU_REQ_SET_GROUND_DIST);
    if (!buf) return INFINITE_DIST;
    Millimeter result;
    memcpy(&result, buf, sizeof(Millimeter)); 
    return result;
}
    
 
