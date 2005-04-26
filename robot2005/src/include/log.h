/**
 * @file log.h
 * 
 * Syteme d'enregistrement des donnees du robot pendant le match
 * Il existe differents types de logs: messages, etat d'une variable du robot.
 * les messages peuvent etre filtre en fonction du niveau de verbosite d'une 
 * class (voir RobotClass).
 * Les logs sont envoye au systeme qui enregistre les logs sur le disque a 
 * travers une socket UNIX. Chaque log envoye a l'enregistreur de log est
 * compose d'un header et de donnees. Le header de taille fixe contient le 
 * type de donnees (LogType), la date du log et la taille des donnees qui
 * vont suivre ce header. Ainsi on peut arajouter de nouveaux types de logs
 * qui ne seront pas decriptes par un ancien analyseur de log, mais on ne 
 * perturbera pas le flux de communication car les analyseur de logs doivent
 * ignorer les donnees de type inconnu.
 * 
 * @Author Laurent Saint-Marcel
 * @Author Julien Holtzer
 *
 */

#ifndef __LOG_H__
#define __LOG_H__

#include "robotBase.h"
#include "classConfig.h"

#include <sys/un.h>     /* For AFUNIX sockets */
#include <sys/time.h>
#include <unistd.h>

#define Log LogCL::instance()

// Si on inclue le fichier log.h pour afficher des logs de fonctions C et 
// non d'une class, il suffit de taper
// #define LOG_CLASSID CLASS_DEFAULT
// avant d'includer le fichier log.h
#ifdef LOG_CLASSID
static ClassId getClassId() {
    return LOG_CLASSID;
}
static VerboseLevel verbose() {
    ClassConfig* cc = ClassConfig::find(LOG_CLASSID);
    if (cc) return cc->verbose();
    else return VERBOSE_DEFAULT;
}
#endif


static const unsigned int LOG_FILE_LENGTH = 20;
static const unsigned int LOG_MESG_LENGTH = 255;
static const unsigned int LOG_DATA_MAX_LENGTH = 300;
static const unsigned int LOG_MAX_TRAJECTORY_POINTS = 30;
static const int LOG_MAGIC_NBR   = 0xa0a0a0a0;

#define LOG_DATAGRAM

/**
 * @enum LogType  
 */
typedef enum LogType {
    LOG_LEVEL_ERROR=0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_OK,
    LOG_LEVEL_INFO,
    LOG_LEVEL_FUNCTION,
    LOG_LEVEL_COMMAND,

    LOG_TYPE_START_MATCH,
    LOG_TYPE_END_MATCH,
    LOG_TYPE_CLOSE,

    LOG_TYPE_SIMU,
    LOG_TYPE_REAL,
    LOG_TYPE_POSITION,
    LOG_TYPE_MOTOR,
    LOG_TYPE_SOUND,
    LOG_TYPE_START_STRATEGY,
    LOG_TYPE_PING, 
    LOG_TYPE_BRIDGE,
    LOG_TYPE_SKITTLE,
    LOG_TYPE_SUPPORT,
    LOG_TYPE_FIRE_BALLS,
    LOG_TYPE_TRAJECTORY,
    LOG_TYPE_OBSTACLE,

    LOG_TYPE_ROBOT_MODEL,
    LOG_TYPE_EMERGENCY_STOP,
    LOG_TYPE_JACKIN,
    LOG_TYPE_LCD,

    LOG_TYPE_NBR
} LogType;

	
/**
 * @struct LogPacketHeader  
 */
typedef struct LogPacketHeader { 
    int            magic;
    LogType        type;
    struct timeval timeStamp;
    int            length;
    LogPacketHeader(LogType Type, int Length=0): 
        magic(LOG_MAGIC_NBR), type(Type), length(Length) {
        gettimeofday(&timeStamp, NULL);
    }
} LogPacketHeader;

typedef struct LogPacketMessage { 
    ClassId  cid;
    int      line;
    char     file[LOG_FILE_LENGTH];
    char     mesg[LOG_MESG_LENGTH];
} LogPacketMessage;

typedef struct LogPacketPosition { 
    Millimeter x, y;
    Radian     theta;
    LogPacketPosition(): x(0), y(0), theta(0){}
} LogPacketPosition;
  	
typedef struct LogPacketMotorInfo { 
    MotorPosition posLeft, posRight;
    MotorPWM      pwmLeft, pwmRight;
    LogPacketMotorInfo(): posLeft(0), posRight(0), pwmLeft(0), pwmRight(0){}
} LogPacketMotorInfo;

typedef struct LogPacketLcd {
    char txt[35];
    LogPacketLcd(const char* msg) {
        if (strlen(msg)>0) strncpy(txt,msg,34); 
        else txt[0] = 0;
        txt[34]=0; 
    }
    LogPacketLcd(LogPacketLcd const& l2) { 
        if (strlen(l2.txt)>0) strcpy(txt, l2.txt); 
        else txt[0] = 0;
    }
} LogPacketLcd;

typedef struct LogPacketBoolean {
    bool value;
    LogPacketBoolean(bool v=false): value(v) {}
} LogPacketBoolean;

typedef struct LogPacketEnumValue {
    short value;
    LogPacketEnumValue(int v=0): value(v) {}
} LogPacketEnumValue;

typedef struct LogPt3D {
  short x, y, z;
  LogPt3D() : x(0), y(0), z(0) {}
  LogPt3D(Millimeter X, Millimeter Y, Millimeter Z)
      : x((short)X), y((short)Y), z((short)Z) {}
} LogPt3D;

typedef struct LogSupport {
  short x1, y1, x2, y2;
  LogSupport() : x1(0), y1(0), x2(0), y2(0) {}
  LogSupport(Point const& pt1,
	     Point const& pt2)
      : x1((short)pt1.x), y1((short)pt1.y),
	x2((short)pt2.x), y2((short)pt2.y)  {}
} LogSupport;

typedef struct LogPosition {
  short x, y, t;
  LogPosition():x(0), y(0), t(0){}
  LogPosition(Point pt, Radian direction){
    x=(short)pt.x;
    y=(short)pt.y;
    t=(short)r2d(direction);
  }
  LogPosition(Millimeter X, Millimeter Y, Radian direction){
    x=(short)X;
    y=(short)Y;
    t=(short)r2d(direction);
  }
  LogPosition(Position p){
    x=(short)p.center.x;
    y=(short)p.center.y;
    t=(short)r2d(p.direction);
  }
} LogPosition;

typedef struct LogPacketObstacle {
    short x, y;
    unsigned char type;
    char envDetectorId;
    LogPacketObstacle(): x(0), y(0), type(0), envDetectorId(-1){}
    LogPacketObstacle(Obstacle const& o, int envId) {
	x=(short)o.center.x;
	y=(short)o.center.y;
	type=(unsigned char)o.type;
        envDetectorId=envId;
    }
} LogPacketObstacle;

typedef struct LogPacketGonio {
    LogPosition pos; // t de position = direction de la balise, x,y=pos du gonio
    unsigned char baliseId;
    LogPacketGonio(): pos(), baliseId(255) {}
    LogPacketGonio(int baliseId, 
		   Point const& gonioCenter,
		   Radian  direction): 
	pos(gonioCenter, direction), baliseId((unsigned char)baliseId) {}
} LogPacketGonio;

typedef struct LogPacketTrajectory {
  unsigned char nbrPt;
  short ptx[LOG_MAX_TRAJECTORY_POINTS]; 
  short pty[LOG_MAX_TRAJECTORY_POINTS]; 
  LogPacketTrajectory(): nbrPt(0){}
  LogPacketTrajectory(Trajectory const& t):nbrPt(0){
    for(unsigned int i=0; i<t.size() && i<LOG_MAX_TRAJECTORY_POINTS; i++) {
      ptx[nbrPt]  =(short)t[i].x;
      pty[nbrPt++]=(short)t[i].y;
    }
  } 
} LogPacketTrajectory;

// ============================================================================
// ===================================  MACRO   ===============================
// ============================================================================
	
#ifndef linux
#define LOG_COMMAND(...)  do{}while(0)
#define LOG_ERROR(...)    do{}while(0)
#define LOG_WARNING(...)  do{}while(0)
#define LOG_OK(...)       do{}while(0)
#define LOG_DEBUG(...)    do{}while(0)
#define LOG_INFO(...)     do{}while(0)
#define LOG_FUNCTION(...) do{}while(0)
#else

// --------------------------------------------------------------------------
// LOG_COMMAND
// --------------------------------------------------------------------------
#define LOG_COMMAND(arg...)                                     \
  if (1) {                                                      \
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sCommand:%s ", __FILE__, __LINE__, \
               KB_COMMAND, KB_RESTORE);                           \
      ::printf(arg);                                            \
    }                                                      	\
    LogCL* log=LogCL::instance();                                   \
    if (log) {	                                           	\
	 log->message(getClassId(), LOG_LEVEL_COMMAND,    \
                      __LINE__, __FILE__, arg);                 \
    }                                                           \
  }
// --------------------------------------------------------------------------
// LOG_ERROR
// --------------------------------------------------------------------------
#define LOG_ERROR(arg...)                                       \
  if (verbose() >= VERBOSE_ERROR) {                             \
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sError:%s ", __FILE__, __LINE__, \
               KB_ERROR, KB_RESTORE);                           \
      ::printf(arg);                                            \
    }                                                      	\
    LogCL* log=LogCL::instance();                                   \
    if (log) {	                                           	\
	 log->message(getClassId(), LOG_LEVEL_ERROR,      \
                      __LINE__, __FILE__, arg);                 \
    }	                                                        \
  }

// --------------------------------------------------------------------------
// LOG_WARNING
// --------------------------------------------------------------------------
#define LOG_WARNING(arg...)                                     \
  if (verbose() >= VERBOSE_ERROR) {                             \
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sWarning:%s ", __FILE__,         \
               __LINE__, KB_WARNING, KB_RESTORE);           	\
      ::printf(arg);			                    	\
    }                                                     	\
    LogCL* log=LogCL::instance();	                           	\
    if (log) {		                                  	\
       log->message(getClassId(), LOG_LEVEL_WARNING,      \
                    __LINE__, __FILE__, arg);                   \
    }	                                                   	\
  } 

// --------------------------------------------------------------------------
// LOG_OK
// --------------------------------------------------------------------------
#define LOG_OK(arg...)                                          \
  if (verbose() >= VERBOSE_ERROR) {                             \
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sOk:%s ", __FILE__, __LINE__,    \
               KB_OK, KB_RESTORE);                              \
      ::printf(arg);					        \
    }                                                           \
    LogCL* log=LogCL::instance();                                   \
    if (log) {			                       	        \
      log->message(getClassId(), LOG_LEVEL_OK,            \
                   __LINE__, __FILE__, arg);                    \
    }	                                                        \
  } 

// --------------------------------------------------------------------------
// LOG_INFO
// --------------------------------------------------------------------------
#define LOG_INFO(arg...)                                        \
  if (verbose() >= VERBOSE_INFO) {	                    	\
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sInfo:%s ", __FILE__, __LINE__,  \
               KB_INFO, KB_RESTORE);                            \
      ::printf(arg);	                                    	\
    }                                                       	\
    LogCL* log=LogCL::instance();	                           	\
    if (log) {			                            	\
      log->message(getClassId(), LOG_LEVEL_INFO,          \
                   __LINE__, __FILE__, arg);                    \
    }		                                                \
  } 
  

// --------------------------------------------------------------------------
// LOG_FUNCTION
// --------------------------------------------------------------------------
#define LOG_FUNCTION(arg...) 	                                \
  if (verbose()>=VERBOSE_INFO) {	       			\
    if (LogCL::getPrintf()) {                                     \
      ::printf("[%15.15s:%4d] %sFunction:%s %s\n", __FILE__,    \
               __LINE__, KB_FUNCTION, KB_RESTORE,__FUNCTION__); \
    }	                                                        \
    LogCL* log=LogCL::instance();                                   \
    if (log) {	                                                \
      log->message(getClassId(), LOG_LEVEL_FUNCTION,      \
                   __LINE__, __FILE__, __FUNCTION__);           \
    }	      		                                        \
  } 
  

// --------------------------------------------------------------------------
// LOG_DEBUG
// --------------------------------------------------------------------------
#ifdef LOG_DEBUG_ON
#define LOG_DEBUG(arg...) 	                                \
    ::printf("[%15.15s:%4d] %sDebug:%s ", __FILE__, __LINE__,   \
             KB_DEBUG, KB_RESTORE);                             \
    ::printf(arg);                                            
#else
#define LOG_DEBUG(arg...) 	                                \
  if (verbose() >= VERBOSE_DEBUG) {                             \
    ::printf("[%15.15s:%4d] %sDebug:%s ", __FILE__, __LINE__,   \
             KB_DEBUG, KB_RESTORE);                             \
    ::printf(arg);                                              \
  }
#endif
  
#endif // solaris

#ifdef LOG_SOCKET_INFO
#define LOG_SOCKET_PATH "/tmp/log_robot1"   
#endif // LOG_INFO

#ifdef LOG_DIRECTORY_INFO
static const char* LOG_DIRECTORY_NAME = ".";
#endif // LOG_INFO

// ============================================================================
// ================================   class Log   =============================
// ============================================================================

/**
 * @class Log
 * Classe permettant d'enregistrer des donnees pendant le match
 */
class LogCL : RobotComponent
{
 public:
    LogCL();                          
    virtual ~LogCL();
    static LogCL* instance();
    void setPrintf(bool enable);
    static bool getPrintf();
    /** @brief Ferme la connection */
    void close();
    /** @brief */
    bool reset();

    /** @brief Enregistre un message de type char* */
    void message(ClassId     classId, 
                 LogType     logLevel, 
                 int         line,
                 const char* file, 
                 const char* msg, ...);

    /** @brief Debut du match (sert a initialiser le chronometre du replay) */
    void startMatch();
    /** @brief Fin du match */
    void stopMatch();
    /** @brief Lancement d'une nouvelle strategy */
    void newStrategy();
    /** @brief Enregistre l'entree ou la sortie d'un bras */
    void closeLogFile();
   
    /** @brief Enregistre les sons pour le playback c'est plus cool! */
    void sound(int snd); 
    /** @brief Enregistre les 15 premiers pints d'une trajectoire */
    void trajectory(Trajectory const& t);
    /** @brief Enregistre la detection d'un obstacle */
    void obstacle(Obstacle const& o, int envId);
    /** @brief Enregistre la detection d'une quille */
    void skittle(Point const& skittleCenter);
    /** @brief Enregistre la position des supports de quilles */
    void support(Point const& supportCenter1,
                 Point const& supportCenter2); 
    /** @brief Enregistre le tire des balles de squatch */
    void fireBalls();
    /** @brief enregistre la position du pont */
    void bridge(BridgePosition pos);
    /** @brief enregistre la position du robot */
    void position(Millimeter x,
		  Millimeter y,
		  Radian     t);
    /** @brief enregistre les données moteur */
    void motor(MotorPWM motorPwmLeft,
	       MotorPWM motorPwmRight,
	       MotorPosition posLeft,
	       MotorPosition posRight);
    /** @brief defini le type de robot */
    void robotModel(RobotModel model);
    /** @brief message affiche sur le lcd */
    void lcdMessage(const char* message);
    /** @brief jack de depart enfoncee ou non */
    void jackIn(bool jackin);
    /** @brief arret d'urgence enfonce ou non */
    void emergencyStopPressed(bool esp);

 private:
    void initSocket();
    void sendPacket(LogPacketHeader& packetHeader, 
                    Byte* data);	

    // data members
 private:
    static LogCL* log_;
    static bool FLAG_SENDING;
    static bool FLAG_PRINTF; 
    int                sock_; 	/* Socket id */
    struct sockaddr_un name_;  	/* Server address */
};

// --------------------------------------------------------------------------
// LogCL::instance
// --------------------------------------------------------------------------
inline LogCL* LogCL::instance()
{
    return log_;
}

// --------------------------------------------------------------------------
// LogCL::getPrintf
// --------------------------------------------------------------------------
inline bool LogCL::getPrintf()
{
    return FLAG_PRINTF;
}

// --------------------------------------------------------------------------
// Log::setPrintf
// --------------------------------------------------------------------------
inline void LogCL::setPrintf(bool enable)
{
   FLAG_PRINTF = enable;
}

#endif // __LOG_H__
