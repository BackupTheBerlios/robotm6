//#define LOG_DEBUG_ON
#include "simulatorClient.h"
#include "network.h"
#include "log.h"
#include "mthread.h"

namespace {
    pthread_mutex_t repositoryLock = PTHREAD_MUTEX_INITIALIZER;
    void no_op(...){};
}

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
    LOG_DEBUG("Write type=%d length=%d\n", buffer_[0], buffer_[1]);
    if (socket_) {
        if (socket_->write(buffer_, buffer_[1] + 2)==-1) {
            LOG_ERROR("Connection to simulator closed\n");
            delete socket_;
            socket_ = NULL;
        }
    }
    LOG_DEBUG("Writen length=%d\n", buffer_[1]);
}

// ----------------------------------------------------------------------------
// SimulatorClient::recvBuffer
// ----------------------------------------------------------------------------
unsigned char* SimulatorClient::recvBuffer(SimuRequestType type)
{
  //usleep(50000);
     LOG_DEBUG("recvBuffer type=%d\n", (int)type);
     if (socket_) {
       if (socket_->read(buffer_, 2) != 2) {
	 LOG_ERROR("readHeader error\n");
	 return NULL; 
       }
       if (buffer_[0] != type) {
	 LOG_ERROR("bad data type: expected:%d read=%d, length=%d\n", type, buffer_[0], buffer_[1]);
	 return NULL;
       }
       int r=0;
       if (buffer_[1] > 0 &&
	   (r=socket_->read(buffer_+2, buffer_[1])) != buffer_[1]) {
	 LOG_ERROR("type=%d expected:%d read=%d\n", buffer_[0], buffer_[1], r);
	 return NULL; 
       } 
       if (buffer_[0] == type) return buffer_+2;
    } 
     LOG_DEBUG("recvedBuffer type=%d length=%d\n", buffer_[0], buffer_[1]);
    return NULL;
}

// ----------------------------------------------------------------------------
// SimulatorClient::SimulatorClient
// ----------------------------------------------------------------------------
SimulatorClient::SimulatorClient() : 
    RobotBase("Simulator Client", CLASS_SIMULATOR), socket_(NULL)
{
   pthread_mutex_init(&lock_, NULL);
}
   
 
// ----------------------------------------------------------------------------
// SimulatorClient::~SimulatorClient
// ----------------------------------------------------------------------------
SimulatorClient::~SimulatorClient()
{
    if (socket_) {
        socket_->close();
        delete socket_;
	socket_=NULL;
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
   
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_NAME,  strlen(name)+1);
    strcpy(buf, name);
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotPosition
// ----------------------------------------------------------------------------
// Defini la position reelle du robot
// ----------------------------------------------------------------------------
void SimulatorClient::setRobotPosition(Position const& pos) 
{
    if (!socket_) return;
    Lock localLock(&lock_);
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_POS,  sizeof(Position));
    memcpy(buf, &pos, sizeof(Position));
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setEstimatedRobotPosition
// ----------------------------------------------------------------------------
// Defini la position estimee du robot
// ----------------------------------------------------------------------------
void SimulatorClient::setEstimatedRobotPosition(Position const& pos) 
{
    if (!socket_) return;
    Lock localLock(&lock_);
    char* buf = (char*)setBufferHeader(SIMU_REQ_SET_ESTIMATED_POS,  sizeof(Position));
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
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
void SimulatorClient::setRobotMotorCoef(Millimeter D, Millimeter K, 
                                        double speedL, double speedR)
{
    if (!socket_) return;
    Lock localLock(&lock_);
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_MOTOR_COEF,  
                                         2*sizeof(Millimeter)
                                         +2*sizeof(double));
    memcpy(buf, &D, sizeof(Millimeter)); buf += sizeof(Millimeter);
    memcpy(buf, &K, sizeof(Millimeter)); buf += sizeof(Millimeter);
    memcpy(buf, &speedL, sizeof(double)); buf += sizeof(double);
    memcpy(buf, &speedR, sizeof(double));
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setRobotMotorCoef
// ----------------------------------------------------------------------------
// Valeurs de coefficient concernant les odometres position en coordonnees
// cylindriques + coefficient 1pas=>distance en mm
  // ----------------------------------------------------------------------------
   
void SimulatorClient::setRobotOdomCoef(Millimeter D, Radian R, Millimeter K, 
                                       double speedL, double speedR)
{
    if (!socket_) return;
    Lock localLock(&lock_);
    unsigned char* buf = setBufferHeader(SIMU_REQ_SET_ODOM_COEF,  
                                         2*sizeof(Millimeter)
                                         +sizeof(Radian)
                                         +2*sizeof(double));
    memcpy(buf, &D, sizeof(Millimeter)); buf += sizeof(Millimeter);
    memcpy(buf, &R, sizeof(Millimeter)); buf += sizeof(Radian);
    memcpy(buf, &K, sizeof(Millimeter)); buf += sizeof(Radian);
    memcpy(buf, &speedL, sizeof(double)); buf += sizeof(double);
    memcpy(buf, &speedR, sizeof(double));
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::setModeBrick
// ----------------------------------------------------------------------------
void SimulatorClient::setModeBrick(bool brick)
{
    if (!socket_) return;
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_STATUS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_STATUS);
    if (!buf) return SIMU_STATUS_NEED_RESET;
    //printf("match=0x%2.2x\n", buf[0]);
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
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_JACKIN, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_JACKIN);
    if (!buf) return false;
    //printf("jack=0x%2.2x\n", buf[0]);
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
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_EMERGENCY, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_EMERGENCY);
    if (!buf) return false;
    //printf("au=0x%2.2x\n", buf[0]);
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
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_LCD_BUTTONS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_LCD_BUTTONS);
    if (!buf) {
      //printf("Err\n");
      return;
    }
    btnYes = buf[0] & 0x01;
    btnNo  = buf[0] & 0x02;
    //printf("lcd=0x%2.2x\n", buf[0]);
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_MOTOR, 0);
    sendBuffer(); 

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_MOTOR);
    if (!buf) { return;}
    memcpy(&left, buf, sizeof(MotorPosition)); buf += sizeof(MotorPosition);
    memcpy(&right, buf, sizeof(MotorPosition));
}

// ----------------------------------------------------------------------------
// SimulatorClient::getOdomPosition
// ----------------------------------------------------------------------------
// Renvoie la valeur des codeurs des odometres
// ----------------------------------------------------------------------------
void SimulatorClient::getOdomPosition(CoderPosition& left, 
                                      CoderPosition& right)
{
    if (!socket_) return;
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_ODOM, 0);
    sendBuffer(); 

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_ODOM);
    if (!buf) return;
    memcpy(&left, buf, sizeof(CoderPosition)); buf += sizeof(CoderPosition);
    memcpy(&right, buf, sizeof(CoderPosition));
}

// ----------------------------------------------------------------------------
// SimulatorClient::getRobotEstimatedPosition
// ----------------------------------------------------------------------------
// Renvoie la position estimee du robot (simulation de la carte odometrie)
// ----------------------------------------------------------------------------
void SimulatorClient::getRobotEstimatedPosition(Point& pt, Radian& dir)
{
    if (!socket_) return;
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_ESTIMATE_POS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_ESTIMATE_POS);
    if (buf) {
      memcpy(&pt, buf, sizeof(Point)); buf += sizeof(Point);
      memcpy(&dir, buf, sizeof(Radian)); 
    }
}

// ----------------------------------------------------------------------------
// Copie la vraie position dans la position estimee
// ----------------------------------------------------------------------------
void SimulatorClient::resetRobotEstimatedPosition()
{
    if (!socket_) return;
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_RESET_ESTIMATED_POS, 0);
    sendBuffer();
}

// ----------------------------------------------------------------------------
// SimulatorClient::getRobotRealPosition
// ----------------------------------------------------------------------------
// Renvoie la position simulee du robot
// ----------------------------------------------------------------------------
void SimulatorClient::getRobotRealPosition(Point& pt, Radian& dir)
{
    if (!socket_) return;
    Lock localLock(&lock_);
    setBufferHeader(SIMU_REQ_GET_REAL_POS, 0);
    sendBuffer();

    unsigned char* buf=recvBuffer(SIMU_REQ_SET_REAL_POS);
    if (buf) {
      memcpy(&pt, buf, sizeof(Point)); buf += sizeof(Point);
      memcpy(&dir, buf, sizeof(Radian));
    }
    
}

// ----------------------------------------------------------------------------
// SimulatorClient::setLCDMessage
// ----------------------------------------------------------------------------
// envoie un message sur l'afficheur LCD
// ----------------------------------------------------------------------------
void SimulatorClient::setLCDMessage(const char* message)
{
    if (!socket_) return;
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
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
    Lock localLock(&lock_);
    unsigned char* buf=setBufferHeader(SIMU_REQ_GET_GROUND_DIST, 2*sizeof(Millimeter)+sizeof(Radian));
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
    
 
