#include <string>

#include "simulator.h"
#include "simulatorBase.h"
#include "simulatorServer.h"
#include "simulatorRobot.h"

#define LOG_CLASSID CLASS_SIMULATOR
#include "log.h"
#ifdef USE_FTHREAD
#include "robotFThread.h"
#else
#include "mthread.h"
#endif

SimulatorConnection* SimulatorServer::connections_[SIMU_PORT_MAX_CONNECTIONS];

// ============================================================================
// Class SimulatorServer
// ============================================================================
#ifdef USE_FTHREAD
static ft_thread_t threadSimuServer_=NULL;
#else
static MThreadId threadSimuServer_;
#endif

// ----------------------------------------------------------------------------
// SimulatorServer::SimulatorServer
// ----------------------------------------------------------------------------
SimulatorServer::SimulatorServer() 
{
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
        connections_[i] = NULL;
    }
}

// ----------------------------------------------------------------------------
// SimulatorServer::~SimulatorServer
// ----------------------------------------------------------------------------
SimulatorServer::~SimulatorServer() 
{
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
        if (connections_[i]) delete connections_[i];
    }
#ifdef USE_FTHREAD
    ft_scheduler_stop(threadSimuServer_);
#else
    // TODO: kill thread
#endif
}

// ----------------------------------------------------------------------------
// simulatorReceiveAnalyzer
// ----------------------------------------------------------------------------
void simulatorReceiveAnalyzer(Socket& socket) 
{
    SimulatorServer::registerConnection(socket);
}


// ----------------------------------------------------------------------------
// simulatorServerThread
// ----------------------------------------------------------------------------
#ifdef USE_FTHREAD
void simulatorServerThread(void*)
#else
void* simulatorServerThread(void*)
#endif
{
    Socket socket;
    while(1) {
#ifdef USE_FTHREAD
        ft_thread_unlink();
#endif
	socket.openReceiver(SIMU_PORT, SIMU_PORT_MAX_CONNECTIONS);
        socket.receiveData(simulatorReceiveAnalyzer); // function that never return
#ifdef USE_FTHREAD
	ft_thread_link(ftThread::getScheduler());
        ft_thread_cooperate();
#endif
    }
    socket.close();
#ifndef USE_FTHREAD
    return NULL;
#endif
}

// ----------------------------------------------------------------------------
// SimulatorServer::startReceiver
// ----------------------------------------------------------------------------
void SimulatorServer::startReceiver()
{
#ifdef USE_FTHREAD
    threadSimuServer_ = ft_thread_create(ftThread::getScheduler(),
					 simulatorServerThread,
					 NULL, // cleanup
					 NULL);// parameters
#else
    MTHREAD_CREATE("Simulator server thread",
		   &threadSimuServer_,
		   NULL,
		   simulatorServerThread, 
		   NULL);
#endif
}

// ----------------------------------------------------------------------------
// SimulatorServer::registerConnection
// ----------------------------------------------------------------------------
void SimulatorServer::registerConnection(Socket& socket)
{
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
        if (!connections_[i]) {
            LOG_OK("Accepting connection on slot %d!\n", i);
            connections_[i] = new SimulatorConnection(socket);
            startConnectionThread(connections_[i]);
            return;
        }
    }
    LOG_ERROR("All available connections are used!");
}

// ----------------------------------------------------------------------------
// connectionThread
// ----------------------------------------------------------------------------
#ifdef USE_FTHREAD
void connectionThread(void* conn) 
#else
void* connectionThread(void* conn) 
#endif
{
    SimulatorConnection* connection = (SimulatorConnection*)conn;
    LOG_FUNCTION();
    while(1) {
        SimuRequestType type;
        unsigned char* buf=NULL;
        if ((buf = connection->recvBuffer(type)) != NULL) {
            connection->parseReceivedData(type, buf);
        } else {
            break;
        }
#ifdef USE_FTHREAD
        ft_thread_cooperate();
#endif
    }
    SimulatorServer::unregisterConnection(connection);
    delete connection;
    LOG_ERROR("Connection closed\n");
#ifndef USE_FTHREAD
    return NULL;
#endif
}

// ----------------------------------------------------------------------------
// SimulatorServer::startConnectionThread
// ----------------------------------------------------------------------------
void SimulatorServer::startConnectionThread(SimulatorConnection* connection)
{
#ifdef USE_FTHREAD
    ft_thread_create(ftThread::getScheduler(),
		     connectionThread,
		     NULL, // cleanup
		     connection);// parameters
#else
    MThreadId threadId;
    MTHREAD_CREATE("Simulator connection thread",
		   &threadId,
		   NULL,
		   connectionThread, 
		   connection);
#endif
}

// ----------------------------------------------------------------------------
// SimulatorServer::unregisterConnection
// ----------------------------------------------------------------------------
void SimulatorServer::unregisterConnection(SimulatorConnection* connection)
{
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
        if (connections_[i] == connection) {
            connections_[i] = NULL;
            LOG_OK("Unregister connection on slot %d!\n", i);
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// SimulatorServer::getRobot
// ----------------------------------------------------------------------------
SimulatorRobot* SimulatorServer::getRobot(int connectionId)
{
    if (connectionId >= 0 &&  
	connectionId < (int)SIMU_PORT_MAX_CONNECTIONS && 
	connections_[connectionId]) {
        return connections_[connectionId]->getRobot();
    }
    return NULL;
}


// ============================================================================
// Class SimulatorConnection
// ============================================================================


// ----------------------------------------------------------------------------
// SimulatorConnection::SimulatorConnection
// ----------------------------------------------------------------------------
SimulatorConnection::SimulatorConnection(Socket& socket) 
    : robot_(NULL), socket_(socket)
{
    robot_ = new SimulatorRobot();
}

// ----------------------------------------------------------------------------
// SimulatorConnection::~SimulatorConnection
// ----------------------------------------------------------------------------
SimulatorConnection::~SimulatorConnection() 
{
    socket_.close();
    if (robot_) delete robot_;
}
    
// ----------------------------------------------------------------------------
// SimulatorConnection::disconnect
// ----------------------------------------------------------------------------
void SimulatorConnection::disconnect() 
{ 
    if (robot_) delete robot_;
    robot_ = NULL;
    socket_.close();
}

// ----------------------------------------------------------------------------
// SimulatorConnection::setBufferHeader
// ----------------------------------------------------------------------------
unsigned char* SimulatorConnection::setBufferHeader(SimuRequestType type, 
                                                    int dataLength)
{
    buffer_[0] = type;
    buffer_[1] = dataLength;
    return &(buffer_[2]);
}

// ----------------------------------------------------------------------------
// SimulatorConnection::sendBuffer
// ----------------------------------------------------------------------------
void SimulatorConnection::sendBuffer()
{
#ifdef USE_FTHREAD
    ft_thread_unlink();
#endif
    socket_.write(buffer_, buffer_[1] + 2);
#ifdef USE_FTHREAD
    ft_thread_link(ftThread::getScheduler());
#endif
}

// ----------------------------------------------------------------------------
// SimulatorConnection::recvBuffer
// ----------------------------------------------------------------------------
unsigned char* SimulatorConnection::recvBuffer(SimuRequestType& type)
{
    unsigned char* result=NULL;
#ifdef USE_FTHREAD
    ft_thread_unlink();
#endif
    if ((socket_.read(buffer_, 2) == 2)
        && (socket_.read(buffer_+2, buffer_[1]) == buffer_[1])) {
        type = (SimuRequestType)buffer_[0];
        result = &(buffer_[2]);
    }
#ifdef USE_FTHREAD
    ft_thread_link(ftThread::getScheduler());
#endif
    return result;
}

// ----------------------------------------------------------------------------
// SimulatorConnection::parseReceivedData
// ----------------------------------------------------------------------------
void SimulatorConnection::parseReceivedData(SimuRequestType type, 
                                            unsigned char* buffer)
{
    unsigned char* buf=NULL;
    switch(type) {
    case SIMU_REQ_SET_NAME:
        {
	    robot_->setRobotName((char*)buffer);
	    LOG_INFO("SetRobot name %s, %s\n", robot_->getName(), buffer);
        }
        break;
    case SIMU_REQ_SET_WEIGHT:
        {
            robot_->setRobotWeight((SimuWeight)buffer[0]);
        }
        break;
    case SIMU_REQ_SET_MODEL:
        {
            robot_->setRobotModel((RobotModel)buffer[0]);
        }
        break;
    case SIMU_REQ_SET_MOTOR_COEF: 
        {
            int D=0, K=0;
	    double speed=0;
            memcpy(&D, buffer, sizeof(int)); buffer+= sizeof(int);
            memcpy(&K, buffer, sizeof(int)); buffer+= sizeof(int);
            memcpy(&speed, buffer, sizeof(int)); 
	    robot_->setRobotMotorCoef(D, K, speed);
        }
        break;
    case SIMU_REQ_GET_STATUS:
        {
            buf = setBufferHeader(SIMU_REQ_SET_STATUS, 1);
            buf[0] = (unsigned char)robot_->getMatchStatus();
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_JACKIN: 
        {
            buf = setBufferHeader(SIMU_REQ_SET_JACKIN, 1);
            buf[0] = (unsigned char)robot_->isJackin();
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_EMERGENCY:
        {
            buf = setBufferHeader(SIMU_REQ_SET_EMERGENCY, 1);
            buf[0] = (unsigned char)robot_->isEmergencyStop();
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_LCD_BUTTONS:
        {
            buf = setBufferHeader(SIMU_REQ_SET_LCD_BUTTONS, 1);
            bool yes=false, no=false;
            robot_->getLcdButtonsState(yes, no);
            buf[0] = (yes?1:0)+(no?2:0);
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_PWM:
        {
            buf = setBufferHeader(SIMU_REQ_SET_PWM, 2*sizeof(MotorPWM));
            MotorPWM left=0, right=0;
            robot_->getPwm(left, right);
            memcpy(buf, &left, sizeof(MotorPWM)); buf+= sizeof(MotorPWM);
            memcpy(buf, &right, sizeof(MotorPWM)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_SET_MOTOR_SPEED:
        {
            MotorSpeed left=0, right=0;
            memcpy(&left, buffer, sizeof(MotorSpeed)); buffer+= sizeof(MotorSpeed);
            memcpy(&right, buffer, sizeof(MotorSpeed)); 
            robot_->setSpeed(left, right);
        }
        break;
    case SIMU_REQ_GET_MOTOR:
        {
            buf = setBufferHeader(SIMU_REQ_SET_MOTOR, 2*sizeof(MotorPosition));
            MotorPosition left=0, right=0;
            robot_->getMotorPosition(left, right);
            memcpy(buf, &left, sizeof(MotorPosition)); buf+= sizeof(MotorPosition);
            memcpy(buf, &right, sizeof(MotorPosition)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_ESTIMATE_POS:
        {
            buf = setBufferHeader(SIMU_REQ_SET_ESTIMATE_POS, sizeof(Point)+sizeof(Radian));
            Point pt;
            Radian dir=0;
            robot_->getRobotEstimatedPosition(pt, dir);
            memcpy(buf, &pt, sizeof(Point)); buf+= sizeof(Point);
            memcpy(buf, &dir, sizeof(Radian)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_REAL_POS:
        {
            buf = setBufferHeader(SIMU_REQ_SET_REAL_POS, sizeof(Point)+sizeof(Radian));
            Point pt;
            Radian dir=0;
            robot_->getRobotRealPosition(pt, dir);
            memcpy(buf, &pt, sizeof(Point)); buf+= sizeof(Point);
            memcpy(buf, &dir, sizeof(Radian)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_SET_LCD:
        {
            robot_->setLcdMessage((char*)buffer);
        }
        break;
    case SIMU_REQ_GET_OBSTACLE_DIST:
        {
            Millimeter r=0, z=0;
            Radian t=0, dir=0;
            memcpy(&r,   buffer, sizeof(Millimeter)); buffer+=sizeof(Millimeter);
            memcpy(&t,   buffer, sizeof(Radian));     buffer+=sizeof(Radian);
            memcpy(&z,   buffer, sizeof(Millimeter)); buffer+=sizeof(Millimeter);
            memcpy(&dir, buffer, sizeof(Radian)); 
            
            Millimeter obsDist = 
                SimulatorCL::instance()->getObstacleDistance(getRobot(), r, t, z, dir);
            buf = setBufferHeader(SIMU_REQ_SET_OBSTACLE_DIST, sizeof(Millimeter));
            memcpy(buf, &obsDist, sizeof(Millimeter)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_COLLISION:
        {
            Millimeter r1=0, r2=0, z=0;
            Radian t1=0, t2=0;
            memcpy(&r1,  buffer, sizeof(Millimeter)); buffer+=sizeof(Millimeter);
            memcpy(&t1,  buffer, sizeof(Radian));     buffer+=sizeof(Radian);
            memcpy(&r2,  buffer, sizeof(Millimeter)); buffer+=sizeof(Millimeter);
            memcpy(&t2,  buffer, sizeof(Radian));     buffer+=sizeof(Radian);
            memcpy(&z,   buffer, sizeof(Millimeter)); 
            
            bool collision = 
                SimulatorCL::instance()->isCollision(getRobot(), r1, t1, r2, t2, z);
            buf = setBufferHeader(SIMU_REQ_SET_COLLISION, 1);
            buf[0] = (collision?1:0);
            sendBuffer();
        }
        break;
    case SIMU_REQ_GET_GROUND_DIST:
        {
            Millimeter r=0, z=0;
            Radian t=0;
            memcpy(&r,   buffer, sizeof(Millimeter)); buffer+=sizeof(Millimeter);
            memcpy(&t,   buffer, sizeof(Radian));     buffer+=sizeof(Radian);
            memcpy(&z,   buffer, sizeof(Millimeter)); 
            
            Millimeter groundDist = 
                SimulatorCL::instance()->getGroundDistance(getRobot(), r, t, z);
            buf = setBufferHeader(SIMU_REQ_SET_GROUND_DIST, sizeof(Millimeter));
            memcpy(buf, &groundDist, sizeof(Millimeter)); 
            sendBuffer();
        }
        break;
    case SIMU_REQ_SET_POS:
        {
	    Position pos;
	    memcpy(&pos, buffer, sizeof(Position));
	    robot_->setRobotPos(pos);
        }
        break;
    case SIMU_REQ_SET_BRICK:
        {
	    robot_->setModeBrick(true);
        }
        break;
    case SIMU_REQ_SET_NORMAL:
        {
	    robot_->setModeBrick(false);
        }
        break;
    
    default:
        LOG_DEBUG("Unknown request type : %d\n", (int)type);
        break;
    }
}
