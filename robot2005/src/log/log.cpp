/**
 * @file log.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Class qui permet d'envoyer des information de la telemetrie du robot sur
 * une socket UNIX vers un programme qui va enregistrer les données sur le
 * disque
 */

#define LOG_SOCKET_INFO
#include "log.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>     /* For AFUNIX sockets */
#include <stdarg.h>

// ============================================================================
// ================================   class Log   =============================
// ============================================================================
LogCL* LogCL::log_=NULL;

bool LogCL::FLAG_SENDING=true;
bool LogCL::FLAG_PRINTF =true;

static const int LOG_MAX_RETRY =2;

// ---------------------------------------------------------------------------
// sigpipe_handler
// ---------------------------------------------------------------------------
extern "C" void sigpipe_handler(int sig) 
{
    printf("Client has been disconnected !\n");
}

// ---------------------------------------------------------------------------
// Log::Log
// ---------------------------------------------------------------------------
LogCL::LogCL(): 
  RobotComponent("Log", CLASS_LOG), sock_(-1)
{  
    setVerboseLevel(VERBOSE_DEBUG);
    if (log_) delete log_;
    log_=this;
    initSocket();
    reset();
}

// ---------------------------------------------------------------------------
// LogCL::~LogCL
// ---------------------------------------------------------------------------
LogCL::~LogCL()
{
    LOG_DEBUG("delete log\n");
    close();
    log_=NULL;
}

// ---------------------------------------------------------------------------
// LogCL::reset
// ---------------------------------------------------------------------------
bool LogCL::reset()
{
    LOG_DEBUG("reset\n");
    init_= FLAG_SENDING;
    return init_;
}

// ---------------------------------------------------------------------------
// LogCL::close
// ---------------------------------------------------------------------------
void LogCL::close() {
    if (sock_ > 0) {
        LOG_DEBUG("socket close\n");
        shutdown(sock_, SHUT_RDWR);
        ::close(sock_);
        sock_ = -1;
    }
    FLAG_SENDING = false;
}

// ---------------------------------------------------------------------------
// LogCL::initSocket
// ---------------------------------------------------------------------------
// Cree la socket UNIX qui connecte le programme du robot au systeme de 
// sauvegarde de logs
// ---------------------------------------------------------------------------
void LogCL::initSocket() {
    /* Create socket on which to send. */
#ifdef LOG_DATAGRAM
    sock_ = socket(AF_UNIX, SOCK_DGRAM, 0);
#else
    sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
#endif			
    if (sock_ < 0) {		
        LOG_DEBUG("Cannot open the socket\n");
        FLAG_SENDING = false;
        return;
    }
    
    /* Construct name of socket to send to. */
    name_.sun_family = AF_UNIX;
    strcpy(name_.sun_path, LOG_SOCKET_PATH);
    
    /* Send message. */
    LogPacketHeader packetHeader(LOG_TYPE_PING);
    sendPacket(packetHeader, NULL);
    if (!FLAG_SENDING) {
      LOG_DEBUG("Cannot connect to the socket %s\n", LOG_SOCKET_PATH);
    }
    signal(SIGPIPE, sigpipe_handler);
}

// ---------------------------------------------------------------------------
// LogCL::sendPacket
// ---------------------------------------------------------------------------
// Envoyer un packet composé d'un header et de donnée sur la socket
// ---------------------------------------------------------------------------
void LogCL::sendPacket(LogPacketHeader& header, 
                     Byte* data) {
    if (FLAG_SENDING && sock_ >= 0) {
        int n = 0;
        int length=0;
        int retry=0;
        // send Header
        do {
            n = sendto(sock_, (&header)+length, 
                     sizeof(LogPacketHeader)-length, 0,
		     (struct sockaddr *)&name_, sizeof(struct sockaddr_un));
            if (n < 0) {
                LOG_DEBUG("Cannot send the packet\n");
                FLAG_SENDING = false;
                return;
            }
            length += n;
        } while (length < (int)sizeof(LogPacketHeader) 
		 && retry++<LOG_MAX_RETRY);
        if (retry >= LOG_MAX_RETRY) {
            LOG_DEBUG("Cannot send the packet\n");
            FLAG_SENDING = false;
            return;
        }

        // send Data
        if (header.length == 0 || data==NULL) return;
        length = 0;
        retry=0;
        do {
            n = sendto(sock_, data+length, header.length-length, 0,
		     (struct sockaddr *)&name_, sizeof(struct sockaddr_un));
            if (n < 0) {
                LOG_DEBUG("Cannot send the packet\n");
                FLAG_SENDING = false;
                return;
            }
            length+=n;
        } while(length < header.length && retry++<LOG_MAX_RETRY);
        if (retry >= LOG_MAX_RETRY) {
            LOG_DEBUG("Cannot send the packet\n");
            FLAG_SENDING = false;
            return;
        }
    }
}

// ---------------------------------------------------------------------------
// LogCL::message
// ---------------------------------------------------------------------------
// Enregistre un message de type texte
// ---------------------------------------------------------------------------
void LogCL::message(ClassId     classId, 
                  LogType     msgType,  
                  int         line, 
                  const char* file,
                  const char* msg, ...)
{	
    LogPacketHeader header(msgType);
    LogPacketMessage data;
    data.cid  = classId;
    data.line = line;

    strncpy(data.file, file, LOG_FILE_LENGTH);
    data.file[LOG_FILE_LENGTH-1]=0;

    va_list argp;
    va_start(argp, msg);
    vsnprintf(data.mesg, LOG_MESG_LENGTH, msg, argp);
    va_end(argp);
    data.mesg[LOG_MESG_LENGTH-1]=0;
    header.length=sizeof(LogPacketMessage)-LOG_MESG_LENGTH+strlen(data.mesg)+1;
    sendPacket(header, (Byte*)(&data));	
}

// ---------------------------------------------------------------------------
// LogCL::position
// ---------------------------------------------------------------------------
// Enregistre la position du robot
// ---------------------------------------------------------------------------
void LogCL::position(Millimeter x,
		     Millimeter y,
		     Radian     t)
{	
    LogPacketHeader header(LOG_TYPE_POSITION);
    LogPosition data(x, y, t);
    header.length=sizeof(LogPosition);
    sendPacket(header, (Byte*)(&data));	
}

// ---------------------------------------------------------------------------
// LogCL::motor
// ---------------------------------------------------------------------------
// Enregistre des donnees sur la consigne envoyee aux moteurs et la valeur
// des codeurs
// ---------------------------------------------------------------------------
void LogCL::motor(MotorPWM pwmLeft, 
		  MotorPWM pwmRight,
		  MotorPosition posLeft, 
		  MotorPosition posRight)
{
    LogPacketHeader header(LOG_TYPE_MOTOR);
    LogPacketMotorInfo data;
    data.pwmLeft  = pwmLeft;
    data.pwmRight = pwmRight;
    data.posLeft  = posLeft;
    data.posRight = posRight;
    header.length=sizeof(LogPacketMotorInfo);
    sendPacket(header, (Byte*)(&data));
}

// ---------------------------------------------------------------------------
// LogCL::startMatch
// ---------------------------------------------------------------------------
// Enregistre le signal de debut de match
// ---------------------------------------------------------------------------
void LogCL::startMatch()
{
    LogPacketHeader header(LOG_TYPE_START_MATCH);
    sendPacket(header, NULL);
}

// ---------------------------------------------------------------------------
// LogCL::stopMatch
// ---------------------------------------------------------------------------
// Enregistre le signal de fin de match
// ---------------------------------------------------------------------------
void LogCL::stopMatch()
{
    LogPacketHeader header(LOG_TYPE_END_MATCH);
    sendPacket(header, NULL);
}

// ---------------------------------------------------------------------------
// LogCL::newStrategy
// ---------------------------------------------------------------------------
// Enregistre le signal de l'execution d'une nouvelle strategie
// ---------------------------------------------------------------------------
void LogCL::newStrategy()
{
    LogPacketHeader header(LOG_TYPE_START_STRATEGY);
    sendPacket(header, NULL);
}

// ---------------------------------------------------------------------------
// LogCL::fireBalls
// ---------------------------------------------------------------------------
// Enregistre le signal de fin de match
// ---------------------------------------------------------------------------
void LogCL::fireBalls()
{
    LogPacketHeader header(LOG_TYPE_FIRE_BALLS);
    sendPacket(header, NULL);
}

// ---------------------------------------------------------------------------
// LogCL::sound
// ---------------------------------------------------------------------------
// Enregistre un son joue par le robot
// ---------------------------------------------------------------------------
void LogCL::sound(int snd)
{
    LogPacketHeader header(LOG_TYPE_SOUND);
    LogPacketEnumValue data(snd);
    header.length = sizeof(LogPacketEnumValue);
    sendPacket(header, (Byte*)&data);
}
// ---------------------------------------------------------------------------
// LogCL::robotModel
// ---------------------------------------------------------------------------
// Enregistre le type de robot
// ---------------------------------------------------------------------------
void LogCL::robotModel(RobotModel model)
{
    LogPacketHeader header(LOG_TYPE_ROBOT_MODEL);
    LogPacketEnumValue data((int)model);
    header.length = sizeof(LogPacketEnumValue);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::lcdMessage
// ---------------------------------------------------------------------------
// message affiche sur le lcd 
// ---------------------------------------------------------------------------
void LogCL::lcdMessage(const char* message)
{
    if (LogCL::getPrintf()) { 
        ::printf("%sLCD:%s \n%s%s%s\n", 
                 KB_INFO, KB_RESTORE, KB_BLUE, message, KB_RESTORE); 
    }   
    LogPacketHeader header(LOG_TYPE_LCD);
    LogPacketLcd data(message);
    header.length = sizeof(LogPacketLcd);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::jackIn
// ---------------------------------------------------------------------------
// jack de depart enfoncee ou non 
// ---------------------------------------------------------------------------
void LogCL::jackIn(bool jackin)
{
    if (LogCL::getPrintf()) { 
        if (jackin) ::printf("%sJACK-IN%s\n", KB_INFO, KB_RESTORE); 
        else ::printf("%sJACK-OUT%s\n", KB_INFO, KB_RESTORE); 
    }   
    LogPacketHeader header(LOG_TYPE_JACKIN);
    LogPacketBoolean data(jackin);
    header.length = sizeof(LogPacketBoolean);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::emergencyStopPressed
// ---------------------------------------------------------------------------
// arret d'urgence enfonce ou non 
// ---------------------------------------------------------------------------
void LogCL::emergencyStopPressed(bool esp)
{
    if (LogCL::getPrintf()) { 
        if (esp) ::printf("%sEMERGENCY STOP PRESSED%s\n", KB_INFO, KB_RESTORE); 
        else ::printf("%sEMERGENCY STOP RELEASED%s\n", KB_INFO, KB_RESTORE); 
    }   
    LogPacketHeader header(LOG_TYPE_EMERGENCY_STOP);
    LogPacketBoolean data(esp);
    header.length = sizeof(LogPacketBoolean);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::trajectory
// ---------------------------------------------------------------------------
// Enregistre les premiers points de la trajectoire du robot
// ---------------------------------------------------------------------------
void LogCL::trajectory(Trajectory const& t)
{
    LogPacketHeader header(LOG_TYPE_TRAJECTORY);
    LogPacketTrajectory data(t);
    header.length = sizeof(LogPacketTrajectory);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::obstacle
// ---------------------------------------------------------------------------
// Enregistre un obstacle
// ---------------------------------------------------------------------------
void LogCL::obstacle(Obstacle const& o,
		     int envDetectorId)
{
    LogPacketHeader header(LOG_TYPE_OBSTACLE);
    LogPacketObstacle data(o, envDetectorId);
    header.length = sizeof(LogPacketObstacle);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::skittle
// ---------------------------------------------------------------------------
// Enregistre la position d'une quille
// ---------------------------------------------------------------------------
void LogCL::skittle(Point const& skittleCenter)
{
    LogPacketHeader header(LOG_TYPE_SKITTLE);
    LogPt3D data(skittleCenter.x, skittleCenter.y, 0);
    header.length = sizeof(LogPt3D);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::support
// ---------------------------------------------------------------------------
// Enregistre la position des supports de quilles
// ---------------------------------------------------------------------------
void LogCL::support(Point const& supportCenter1,
		    Point const& supportCenter2)
{
    LogPacketHeader header(LOG_TYPE_SUPPORT);
    LogSupport data(supportCenter1, supportCenter2);
    header.length = sizeof(LogSupport);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::bridgePosition
// ---------------------------------------------------------------------------
// Enregistre la position des ponts
// ---------------------------------------------------------------------------
void LogCL::bridge(BridgePosition pos)
{
    LogPacketHeader header(LOG_TYPE_BRIDGE);
    LogPacketEnumValue data(pos);
    header.length = sizeof(LogPacketEnumValue);
    sendPacket(header, (Byte*)&data);
}

// ---------------------------------------------------------------------------
// LogCL::closeLogFile
// ---------------------------------------------------------------------------
// Ferme le log ce qui le squve sur le disque
// ---------------------------------------------------------------------------
void LogCL::closeLogFile()
{
    LogPacketHeader header(LOG_TYPE_CLOSE);
    sendPacket(header, NULL);
}

