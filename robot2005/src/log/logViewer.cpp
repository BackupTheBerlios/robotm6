/**
 * @file logViewer.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Outil d'affichage d'un log utilisant le viewer3D
 */

#define SOUND_INFO
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"
#include "viewer3D.h"
#include "robotFile.h"
#include "robotTimer.h"
#include "sound.h"
#include "soundPlayer.h"

#include <map>
#include <string>

static const ViewerScreenEN LOGVIEWER_RECORD_SCREEN = VIEWER_SCREEN_MAP;

using namespace std;

extern void reshapeRobot3D(int w, int h);

typedef long LogTime; // millisecond
typedef multimap<LogTime, LogPacketMessage>    LogVMessage;
typedef multimap<LogTime, LogPosition>         LogVPosition;
typedef multimap<LogTime, LogPacketMotorInfo>  LogVMotor;
typedef multimap<LogTime, LogPacketEnumValue>  LogVSound;
typedef multimap<LogTime, LogPacketTrajectory> LogVTrajectory;
typedef multimap<LogTime, LogPacketEnumValue>  LogVBridge;
typedef multimap<LogTime, LogPacketLcd>        LogVLcd;
typedef multimap<LogTime, LogPacketBoolean>    LogVJack;
typedef multimap<LogTime, LogPacketBoolean>    LogVEmergency;
typedef multimap<LogTime, LogPacketObstacle>   LogVObstacle;
typedef multimap<LogTime, LogType>             LogVMisc; // autres infos

typedef struct LogVData {
    LogVMessage    msg;
    LogVPosition   pos;
    LogVMotor      motor;
    LogVSound      snd;
    LogVMisc       misc;
    LogVTrajectory traj;
    LogVObstacle   obs;
    LogVLcd        lcd;
    LogVJack       jack;
    LogVEmergency  emergency;
    LogVBridge     bridge;
    bool           simulated;
    RobotModel     model;
} LogVData;

static LogVData data_[VIEWER_MAX_ROBOT_NBR]; // donnees des logs
static char     info_[255];
static char     baseName_[255]; // nom du fichier pour les screenshot
LogTime         endTime_=0;     // date de fin du log
bool            play_=false;    // animation
bool            record_=false;  // enregistrement de screenshot pour faire une video apres
double          playSpeed_=1;   // vitesse d'animation
LogTime         realTime_[VIEWER_MAX_ROBOT_NBR];    // temps actuel utilise pour l'animation   
int             nbrLogs_=0;

// ---------------------------------------------------------------------------
// timeval2LogTime
// ---------------------------------------------------------------------------
LogTime timeval2LogTime(const timeval &t1)
{
    return t1.tv_sec*1000+t1.tv_usec/1000;
}

// ---------------------------------------------------------------------------
// operator-(const timeval &t1 , const timeval &t2)
// ---------------------------------------------------------------------------
timeval operator-(const timeval &t1 , const timeval &t2)
{
    timeval t3;
    t3.tv_sec = t1.tv_sec  - t2.tv_sec;
    if (t1.tv_usec < t2.tv_usec) {
	t3.tv_usec= 1000000-t2.tv_usec+t1.tv_usec;
	t3.tv_sec--;
    } else {
	t3.tv_usec= t1.tv_usec - t2.tv_usec;
    }
    return t3;
}

// ---------------------------------------------------------------------------
// Le logiciel SSMM permet d'assembler des images et des sons et d'en faire 
// un film
// ---------------------------------------------------------------------------
static FILE* ssmmFile_=NULL;
static const char* ssmmDir_="C:\\SSMM_tmp\\";

// ---------------------------------------------------------------------------
// updateSSMMFile
// ---------------------------------------------------------------------------
void updateSSMMFile(const char* filename, 
		    int imgCounter,
		    bool sound)
{
    char filename2[256];
    sprintf(filename2, "%s.ini", filename);
    if (imgCounter == 1) {
	ssmmFile_ = fopen(filename2, "w");	
    } else {
	ssmmFile_ = fopen(filename2, "a");
    }
    char* filename3 = strrchr(filename, '/');
    if (filename3 == NULL) filename3 = const_cast<char*>(filename);
    else filename3 = filename3+1;
    
    fprintf(ssmmFile_, "[PICTURE_%d]\n", imgCounter-1);
    fprintf(ssmmFile_, "AudioFadingTime=0\n");
    if (false && sound) {
	fprintf(ssmmFile_, "AudioFile=%s%s_%.4d.wav\n",
		ssmmDir_, filename3, imgCounter);
    } else {
	fprintf(ssmmFile_, "AudioFile=\n");
    }
    fprintf(ssmmFile_, "AudioStartTime=0\n");
    fprintf(ssmmFile_, "ColorFade_ColorRef=0\n");
    fprintf(ssmmFile_, "Fade2NextPicture=0\n");
    fprintf(ssmmFile_, "FadeOption=-1\n");
    fprintf(ssmmFile_, "Fading2NextPictureName=Blur-Fade\n");
    fprintf(ssmmFile_, "FileName=%s%s_%.4d.bmp\n", 
	    ssmmDir_, filename3, imgCounter-1);
    fprintf(ssmmFile_, "FlightPoints=\n");
    fprintf(ssmmFile_, "FlightPointsColor=255\n");
    fprintf(ssmmFile_, "FlightPointsRadius=10\n");
    fprintf(ssmmFile_, "Font_CharSet=0\n");
    fprintf(ssmmFile_, "Font_ClipPrecision=0\n");
    fprintf(ssmmFile_, "Font_Escapement=0\n");
    fprintf(ssmmFile_, "Font_Height=30\n");
    fprintf(ssmmFile_, "Font_Italic=0\n");
    fprintf(ssmmFile_, "Font_Name=Arial\n");
    fprintf(ssmmFile_, "Font_Orientation=0\n");
    fprintf(ssmmFile_, "Font_OutPrecision=0\n");
    fprintf(ssmmFile_, "Font_PitchAndFamily=34\n");
    fprintf(ssmmFile_, "Font_Quality=0\n");
    fprintf(ssmmFile_, "Font_StrikeOut=0\n");
    fprintf(ssmmFile_, "Font_TextBackColor=5242880\n");
    fprintf(ssmmFile_, "Font_TextColor=16777215\n");
    fprintf(ssmmFile_, "Font_Underline=0\n");
    fprintf(ssmmFile_, "Font_Weight=200\n");
    fprintf(ssmmFile_, "Font_Width=14\n");
    fprintf(ssmmFile_, "IsColorFade=0\n");
    fprintf(ssmmFile_, "LoopAudio=0\n");
    fprintf(ssmmFile_, "NormalizeAudio=0\n");
    fprintf(ssmmFile_, "PanoramaScrolling=0\n");
    fprintf(ssmmFile_, "PanoramaScrollingType=-1\n");
    fprintf(ssmmFile_, "PictureMove=0\n");
    fprintf(ssmmFile_, "PictureMoveSettings=\n");
    fprintf(ssmmFile_, "PictureText=\n");
    fprintf(ssmmFile_, "Rotation=0\n");
    fprintf(ssmmFile_, "ShowPicture=2\n");
    fprintf(ssmmFile_, "SynchronizeFF2NP=0\n");
    fprintf(ssmmFile_, "Text-FadeIn=0\n");
    fprintf(ssmmFile_, "Text-FadeOut=0\n");
    fprintf(ssmmFile_, "TextBackWidth=2\n");
    fprintf(ssmmFile_, "TextFadeInEffectName=Cross Dissolve\n");
    fprintf(ssmmFile_, "TextFadeOutEffectName=Cross Dissolve\n");
    fprintf(ssmmFile_, "TextPosition=1\n");
    fprintf(ssmmFile_, "\n");
    fclose(ssmmFile_);
}

// ---------------------------------------------------------------------------
// endSSMMFile
// ---------------------------------------------------------------------------
void endSSMMFile(const char* filename)
{
    char filename2[256];
    if (ssmmFile_ == NULL) return;
    sprintf(filename2, "%s.ini", filename);
    ssmmFile_ = fopen(filename2, "a");

    char* filename3 = strrchr(filename, '/');
    if (filename3 == NULL) filename3 = const_cast<char*>(filename);
    else filename3 = filename3+1;

    fprintf(ssmmFile_, "[PROJECT_SETTINGS]\n");
    fprintf(ssmmFile_, "CropBottom=0\n");
    fprintf(ssmmFile_, "CropLeft=0\n");
    fprintf(ssmmFile_, "CropRight=0\n");
    fprintf(ssmmFile_, "CropTop=0\n");
    fprintf(ssmmFile_, "Frames-Fading2NextPicture=0\n");
    fprintf(ssmmFile_, "Frames-Showing-Picture=2\n");
    fprintf(ssmmFile_, "Frames-Text-Fade-In=0\n");
    fprintf(ssmmFile_, "Frames-Text-Fade-Out=0\n");
    fprintf(ssmmFile_, "FramesPerSeconds=25\n");
    fprintf(ssmmFile_, "GlobalBackgroundColorB=0\n");
    fprintf(ssmmFile_, "GlobalBackgroundColorG=0\n");
    fprintf(ssmmFile_, "GlobalBackgroundColorR=0\n");
    fprintf(ssmmFile_, "MaxFileSizeLimit=640\n");
    fprintf(ssmmFile_, "Movie-Size-X=320\n");
    fprintf(ssmmFile_, "Movie-Size-Y=240\n");
    fprintf(ssmmFile_, "Output-AVI=%s%s.avi\n", 
	    ssmmDir_, filename3);
    fprintf(ssmmFile_, "PictureMove_Quality=2\n");
    fprintf(ssmmFile_, "PictureMove_ScaleLimit=2\n");
    fprintf(ssmmFile_, "RescalePictures=1\n");
    fprintf(ssmmFile_, "TempDir=%s\n", ssmmDir_);
    fprintf(ssmmFile_, "UseFadingBetweenPictures=1\n");
    fprintf(ssmmFile_, "UseTextFading=1\n");
    fclose(ssmmFile_);
    ssmmFile_ = NULL;
}

// ---------------------------------------------------------------------------
// restorePacket
// ---------------------------------------------------------------------------
bool restorePacket(File*            file,
                   LogPacketHeader& packetHeader, 
                   Byte*            data)
{
    if (file) {
        if (file->read((Byte*)(&packetHeader), 
		       sizeof(LogPacketHeader)) != sizeof(LogPacketHeader)) {
            return false;
	}
	assert((unsigned int)packetHeader.length<LOG_DATA_MAX_LENGTH);
	if (file->read(data, packetHeader.length) != packetHeader.length) {
            return false;
	}
    }
 
    return true;
}

// -------------------------------------------------------------------------
// Config
// -------------------------------------------------------------------------

char logFilenames_[1250];
static char *const logFilename_[VIEWER_MAX_ROBOT_NBR]= {
    logFilenames_,
    logFilenames_+250,
    logFilenames_+500,
    logFilenames_+750
};
static char *const logConfigFilename_=logFilenames_+1000;

// ---------------------------------------------------------------------------
// parseArgs
// ---------------------------------------------------------------------------
bool parseArgs(int argc, char*argv[])
{
    if (argc<2) {
        printf("Usage: %s logFileName1 (logFileName2)  (logFileName1) (logFileName2) "
               "(-c=configFileName)\n", argv[0]);
        return false;
    }
    logConfigFilename_[0]=0;
    int i=0;
    for(i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
        logFilename_[i][0] = 0;
    }
    for(i=0; i<VIEWER_MAX_ROBOT_NBR+1 && nbrLogs_<VIEWER_MAX_ROBOT_NBR ; i++) {
        if (argc<i+2) break;
        if (strncmp(argv[i+1], "-c=", 3) == 0) {
            strcpy(logConfigFilename_, (argv[i+1]+3)); 
        } else {
            strcpy(logFilename_[nbrLogs_++], (argv[i+1])); 
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// createSkittlePacket
// ---------------------------------------------------------------------------
Skittle logSkittles_[QUILLE_NBR];
void createSkittlePacket(Skittle* skittles,
                         Point center, 
                         Millimeter altitude, 
                         bool four,
                         bool red)
{
    if (four) {
	skittles[3].center = center;
	skittles[3].altitude = altitude+QUILLE_HAUTEUR;
	skittles[3].status=SKITTLE_UP;
        skittles[3].color = red?COLOR_RED:COLOR_GREEN;
    }
    Radian deltaTheta = robotRand(0,10)*M_PI/5.;
    for(int i=0;i<3;i++) {
	skittles[i].center.x = center.x+40*cos(i*2.*M_PI/3.+deltaTheta);
	skittles[i].center.y = center.y+40*sin(i*2.*M_PI/3.+deltaTheta);
	skittles[i].altitude = altitude;
	skittles[i].color = red?COLOR_RED:COLOR_GREEN;
	skittles[i].status=SKITTLE_UP;
    }
}

// ---------------------------------------------------------------------------
// loadConfigFile
// ---------------------------------------------------------------------------
void loadConfigFile(const char* filename)
{
    if (!filename) return;
    FILE* fptr=fopen(filename, "r");
    if (!fptr) return;
    char c;
    while((c=fgetc(fptr)) != EOF) {
        switch(c) {
        case 'b':
        case 'B':
        case 'p':
        case 'P':
            while((c=fgetc(fptr)) != EOF && c !='=' && c !='\n');
            if (c == EOF || c =='\n') {
                LOG_ERROR("ConfigFile: invalid bridge (\"pont=1\")\n");
                continue;
            }
            c=fgetc(fptr);
            if (c>='0' && c < '5') {
                Viewer3D->setBridgePosition((BridgePosition)(c-'0'));
                LOG_INFO("ConfigFile: bridge=%d\n", (c-'0'));
            } else {
                LOG_ERROR("ConfigFile: invalid bridge id=%d (\"pont:1\")\n", (c-'0'));
            }
            break;
        case 's':
            {
                int x1=0, y1=0, x2=0, y2=0;
                if (fscanf(fptr,"upport=(%d,%d) (%d,%d)\n", &x1, &y1, &x2, &y2) != 4) {
                    LOG_ERROR("ConfigFile: invalid support definition \"support=(0,0)(1,2)\"\n");
                    continue;
                }
                Point pt1(150+x1*300, 150+y1*300);
                Point pt2(150+x2*300, 150+y2*300);
                Viewer3D->setSupportPosition(pt1, pt2);
                createSkittlePacket(logSkittles_, pt1, 
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
                createSkittlePacket(logSkittles_+4, pt2, 
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
                createSkittlePacket(logSkittles_+8, Point(TERRAIN_X, TERRAIN_Y) - pt1, 
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
                createSkittlePacket(logSkittles_+12, Point(TERRAIN_X, TERRAIN_Y) - pt2,
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
                LOG_INFO("ConfigFile: support (%d %d) (%d %d)\n",x1, y1, x2, y2); 
            }
            break;
        case 'q':
            {
                int x1=0, y1=0;
                if (fscanf(fptr,"uille=(%d,%d)", &x1, &y1) != 2) {
                    LOG_ERROR("ConfigFile: invalid quille definition \"quille=(0,0)\"\n");
                    continue;
                }
                Point pt1(150+x1*300, 150+y1*300);
                createSkittlePacket(logSkittles_+16, pt1, 
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
                createSkittlePacket(logSkittles_+20, Point(TERRAIN_X, TERRAIN_Y) - pt1, 
                                    TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
                LOG_INFO("ConfigFile: quille (%d %d)\n",x1, y1);
            }
            break;
        case '\n':
            continue;
        case '/':   
        default: 
            while((c=fgetc(fptr)) != EOF && c !='\n');
            continue;
        }
            
    }
    fclose(fptr);
    Point pt1(1522, 1050-75);
    createSkittlePacket(logSkittles_+24, pt1, 
                        TERRAIN_SUPPORT_QUILLE_HAUTEUR, false, true);
    createSkittlePacket(logSkittles_+27, Point(TERRAIN_X, TERRAIN_Y) - pt1, 
                        TERRAIN_SUPPORT_QUILLE_HAUTEUR, false, false);
    Viewer3D->setSkittlePosition(logSkittles_);
    LOG_OK("ConfigFile loaded\n");
}

// ---------------------------------------------------------------------------
// loadLog
// ---------------------------------------------------------------------------
// Lit le fichier est sauvegarde les donnees dans la structure data_
// ---------------------------------------------------------------------------
void loadLog(int id, const char* filename)
{
    ZFile file;
    if (!file.open(filename, FILE_MODE_READ)) {
	printf("Cannot load log file: %s\n", filename);
	exit(1);
    }
    LogPacketHeader packetHeader((LogType)0); 
    Byte data[LOG_DATA_MAX_LENGTH];
    struct timeval packetTime;
    bool first=true;
    while (restorePacket(&file, packetHeader, data)) {

	if (first) {
	    packetTime=packetHeader.timeStamp;
	    first=false;
	}
	LogTime currentTime = timeval2LogTime(packetHeader.timeStamp-packetTime);
	if (endTime_<currentTime) endTime_ = currentTime;
	switch(packetHeader.type) {
	case LOG_TYPE_SIMU:
	    data_[id].simulated=true;
	    break; 
	case LOG_TYPE_REAL:
	    data_[id].simulated=false;
	    break; 
        case LOG_TYPE_ROBOT_MODEL:
	    data_[id].model=(RobotModel)((LogPacketEnumValue*)data)->value;
	    break; 
	case LOG_LEVEL_ERROR:
	case LOG_LEVEL_WARNING:
	case LOG_LEVEL_INFO:
	case LOG_LEVEL_OK:
	case LOG_LEVEL_FUNCTION:
	    data_[id].msg.insert(std::pair<LogTime, LogPacketMessage>
                                 (currentTime, *((LogPacketMessage*)data)));
	    break;

	case LOG_TYPE_POSITION:
	    data_[id].pos.insert(std::pair<LogTime, LogPosition>
                                 (currentTime, *((LogPosition*)data)));
	    break;
	case LOG_TYPE_MOTOR:
	    data_[id].motor.insert(std::pair<LogTime, LogPacketMotorInfo>
                                   (currentTime, *((LogPacketMotorInfo*)data)));
	    break;
	case LOG_TYPE_SOUND:
	    data_[id].snd.insert(std::pair<LogTime, LogPacketEnumValue>
                                 (currentTime, *((LogPacketEnumValue*)data)));
	    break;
        case LOG_TYPE_TRAJECTORY:
	    data_[id].traj.insert(std::pair<LogTime, LogPacketTrajectory>
                                  (currentTime, *((LogPacketTrajectory*)data)));
	    break;
	case LOG_TYPE_OBSTACLE:
	    data_[id].obs.insert(std::pair<LogTime,LogPacketObstacle>
                                 (currentTime,  *((LogPacketObstacle*)data)));
	    break;
        case LOG_TYPE_EMERGENCY_STOP:
	    data_[id].emergency.insert(std::pair<LogTime, LogPacketBoolean>
                                       (currentTime, *((LogPacketBoolean*)data)));
	    break;
        case LOG_TYPE_JACKIN:
	    data_[id].jack.insert(std::pair<LogTime, LogPacketBoolean>
                                  (currentTime, *((LogPacketBoolean*)data)));
	    break;
	case LOG_TYPE_LCD:
	    data_[id].lcd.insert(std::pair<LogTime,LogPacketLcd>
                                 (currentTime,  *((LogPacketLcd*)data)));
	    break;
        case LOG_TYPE_BRIDGE:
	    data_[id].bridge.insert(std::pair<LogTime,LogPacketEnumValue>
                                 (currentTime,  *((LogPacketEnumValue*)data)));
	    break;
        case LOG_TYPE_PING:
	case LOG_TYPE_START_MATCH: 
	case LOG_TYPE_END_MATCH:
	case LOG_TYPE_START_STRATEGY: 
	default: 
	    while (data_[id].misc.find(currentTime) != data_[id].misc.end()) 
                currentTime++;
	    data_[id].misc.insert(std::pair<LogTime, LogType>
                                  (currentTime, packetHeader.type));
	    break; 
	}
    }
    LOG_OK("Robot[%d] : %s loaded\n", id, filename);
}

// ==========================================================================
// Recupper les informations necessaire a l'instant t
// ==========================================================================

// ---------------------------------------------------------------------------
// getNearestBefore
// ---------------------------------------------------------------------------
template<typename T>
bool getNearestBefore(LogTime t, multimap<LogTime, T> const& data, T& result)
{
    bool status = false;
    typename multimap<LogTime, T>::const_iterator it;
    for(it=data.begin(); it!=data.end(); it++) {
	if ((*it).first > t) {
            break;
	} else {
            result = (*it).second;
            status = true;
        }
    }
    return status; 
}

// ---------------------------------------------------------------------------
// getSound
// ---------------------------------------------------------------------------
bool getSound(int robotId, LogTime t1, LogTime t2, SoundId& snd)
{
    LogVSound::iterator it;
    for(it=data_[robotId].snd.begin(); it!=data_[robotId].snd.end(); it++) {
	if ((*it).first > t2 ) return false;
	if ((*it).first >= t1) {
	    snd = (SoundId)(*it).second.value;
	    return true;
	}
    }
    return false;
}

// ---------------------------------------------------------------------------
// getPosition
// ---------------------------------------------------------------------------
Position getPosition(int robotId, LogTime t)
{
    LogVPosition::iterator it;
    LogTime t1=0, t2=0;
    Point p1, p2;
    Radian dir1=0, dir2=0;
    for(it=data_[robotId].pos.begin(); it != data_[robotId].pos.end(); it++) {
	if ((*it).first > t) {
	    t2 = (*it).first;
	    p2 = Point((*it).second.x, (*it).second.y);
	    dir2=(*it).second.t*M_PI/180.;
	    if (t2 == t1 || it == data_[robotId].pos.begin()) {
		return Position(p2, dir2);
	    } else {
		return Position(p1.x+(p2.x-p1.x)*(t-t1)/(t2-t1),
				p1.y+(p2.y-p1.y)*(t-t1)/(t2-t1),
				dir1+(dir2-dir1)*(t-t1)/(t2-t1));
	    }
	}
	t1 = (*it).first;
	p1 = Point((*it).second.x, (*it).second.y);
	dir1=(*it).second.theta;
    }
    return Position(p1, dir1);
}

// ---------------------------------------------------------------------------
// getNextMatchBegin
// ---------------------------------------------------------------------------
bool getNextMatchBegin(int robotId,
                       LogTime fromTime,
		       LogTime &nextMatchTime)
{
    LogVMisc::iterator it;
    for(it=data_[robotId].misc.begin(); it != data_[robotId].misc.end(); it++) {
	if ((*it).first < fromTime) {
	    continue;
	}
	if ((*it).second == LOG_TYPE_START_MATCH) {
	    nextMatchTime = (*it).first; 
	    return true;
	}
    }
    return false;
}

// ---------------------------------------------------------------------------
// getPreviousMatchBegin
// ---------------------------------------------------------------------------
bool getPreviousMatchBegin(int robotId, 
                           LogTime fromTime,
			   LogTime &previousMatchTime)
{
    LogVMisc::iterator it;
    previousMatchTime=0;
    for(it=data_[robotId].misc.begin(); it != data_[robotId].misc.end(); it++) {
	if ((*it).first >= fromTime) {
	    return true;
	}
	if ((*it).second == LOG_TYPE_START_MATCH) {
	    previousMatchTime = (*it).first; 
	}
    }
    return false;
}

// ---------------------------------------------------------------------------
// getTrajectory
// ---------------------------------------------------------------------------
void getTrajectory(int robotId, 
                   LogTime fromTime,
		   Trajectory &t)
{
    LogVTrajectory::iterator it;
    t.clear();
    for(it=data_[robotId].traj.begin(); it != data_[robotId].traj.end(); it++) {
	if ((*it).first > fromTime) {
	    return ;
	}
	t.clear();
	for(int i=0; i<(*it).second.nbrPt; i++) {
	    t.push_back(Point((*it).second.ptx[i], (*it).second.pty[i]));
	}
    }
}

void getBridgePosition(int robotId, 
                       LogTime fromTime,
                       BridgePosition &bridge)
{
    LogPacketEnumValue bv;
    if (!getNearestBefore(fromTime, data_[robotId].bridge, bv))
        bridge = BRIDGE_POS_UNKNOWN;
    else
        bridge = (BridgePosition)bv.value;
}

bool getJackin(int robotId, 
               LogTime fromTime)
{
    LogPacketBoolean bv;
    if (!getNearestBefore(fromTime, data_[robotId].jack, bv))
        return false;
    else 
        return bv.value;
}

bool getEmergencyStop(int robotId, 
                      LogTime fromTime)
{
    LogPacketBoolean bv;
    if (!getNearestBefore(fromTime, data_[robotId].emergency, bv))
        return false;
    else 
        return bv.value;
}

void getLcd(int robotId, 
            LogTime fromTime,
            char* txt)
{
    LogPacketLcd bv("");
    if (!getNearestBefore(fromTime, data_[robotId].lcd, bv)) {
        txt[0]=0;
    } else {
        if (strlen(bv.txt)>0) strcpy(txt, bv.txt);
        else txt[0]=0;
    }
}

// ---------------------------------------------------------------------------
// getObstacles
// ---------------------------------------------------------------------------
void getObstacles(int robotId, 
                  LogTime fromTime,
		  ListOfObstacles &obstacles,
                  int maxObstacles)
{
    LogVObstacle::iterator it;
    for(it=data_[robotId].obs.begin(); it != data_[robotId].obs.end(); it++) {
	if ((*it).first > fromTime) {
	    return ;
	}
	Obstacle obs(Point((*it).second.x, (*it).second.y), 0);
	obs.setType((ObstacleType)(*it).second.type);
	obstacles.push_back(obs);
        if ((int)obstacles.size() > maxObstacles) obstacles.pop_front();
    }
}
// ---------------------------------------------------------------------------
// getMatchTime
// ---------------------------------------------------------------------------
void getMatchTime(int id,
                  LogTime t,
                  LogTime &matchTime)
{
    LogVMisc::iterator it;
    for(it=data_[id].misc.begin(); it != data_[id].misc.end(); it++) {
	if ((*it).first > t) {
	    return;
	}
	switch((*it).second) {
	case LOG_TYPE_START_MATCH:
	    matchTime=t-(*it).first;
	    break;
	case LOG_TYPE_END_MATCH:
	    matchTime=t;
	    break;
	default:
	    break;
	}
    }
}
// ---------------------------------------------------------------------------
// updateDisplay
// ---------------------------------------------------------------------------
// Actualise l'affichage avec les valeurs a l'instant t
// ---------------------------------------------------------------------------
void updateDisplay()
{
    for(int id=0;id < nbrLogs_; id++) {
        Viewer3D->setRobotPosition(id, getPosition(id, realTime_[id]));

        Trajectory t;
        getTrajectory(id, realTime_[id], t);
        Viewer3D->setRobotTrajectory(id, t);
        
        ListOfObstacles obstacles;
        getObstacles(id, realTime_[id], obstacles, 10);
        Viewer3D->setObstacleList(id, obstacles, ALL_OBSTACLES);

        BridgePosition estimatedBridge;
        getBridgePosition(id, realTime_[id], estimatedBridge);
        Viewer3D->setEstimatedBridgePosition(id, estimatedBridge);

        Viewer3D->setBtnClick((ViewerControlButtonId)(CTRL_BTN_R0_JACK+id*CTRL_BTN_NBR_PER_ROBOT),
                              getJackin(id, realTime_[id]));

        Viewer3D->setBtnClick((ViewerControlButtonId)(CTRL_BTN_R0_AU+id*CTRL_BTN_NBR_PER_ROBOT),
                              getEmergencyStop(id, realTime_[id]));

        static char lcdMsg[35];
        getLcd(id, realTime_[id], lcdMsg);
        Viewer3D->setRobotLcd(id, lcdMsg);
       
    }
    LogTime matchTime=realTime_[0];
    getMatchTime(0, realTime_[0], matchTime);
    Viewer3D->setTime(realTime_[0]);
    Viewer3D->setMatchTime(matchTime);
}

struct timeval chronometerTic;
// -------------------------------------------------------------------------
// realTimeReset
// -------------------------------------------------------------------------
void realTimeReset()
{
    gettimeofday(&chronometerTic, NULL);
}

// -------------------------------------------------------------------------
// realTimeDeltaT
// -------------------------------------------------------------------------
LogTime realTimeDeltaT()
{
    struct timeval tv_after;
    LogTime result;
    gettimeofday(&tv_after, NULL);
  
    result = (tv_after.tv_sec-chronometerTic.tv_sec)*1000000 +
	(tv_after.tv_usec - chronometerTic.tv_usec);

    chronometerTic = tv_after;
  
    return result/1000;
}

// -------------------------------------------------------------------------
// boutons
// -------------------------------------------------------------------------
void btnPlay(ViewerControlButtonId btnId)
{
    play_ = !play_;
    if (play_) {
	realTimeReset();
	Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PAUSE_0, TEX_BTN_PAUSE_1);
        Viewer3D->setBtnEnable(CTRL_BTN_PREVIOUS, true);
        Viewer3D->setBtnEnable(CTRL_BTN_STEP_BACKWARD, true); 
    } else {
	Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    }
    updateDisplay();
}

void btnBack(ViewerControlButtonId btnId)
{
    play_=false;
    Viewer3D->reset();
    if (strlen(logConfigFilename_)>0) loadConfigFile(logConfigFilename_);
    for(int id=0; id<nbrLogs_; id++)
        getPreviousMatchBegin(id, realTime_[id], realTime_[id]);
    if (realTime_[0] == 0) {
        Viewer3D->setBtnEnable(CTRL_BTN_PREVIOUS, false);
        Viewer3D->setBtnEnable(CTRL_BTN_STEP_BACKWARD, false); 
    }
    Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    updateDisplay();
}

void btnForward(ViewerControlButtonId btnId)
{
    play_=false;
    for(int id=0; id<nbrLogs_; id++)
        getNextMatchBegin(id, realTime_[id], realTime_[id]);
    if (realTime_[0] > 0) {
        Viewer3D->setBtnEnable(CTRL_BTN_PREVIOUS, true);
        Viewer3D->setBtnEnable(CTRL_BTN_STEP_BACKWARD, true); 
    }
    Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    updateDisplay();
}

void btnStepForward(ViewerControlButtonId btnId)
{
    play_=false;
    for(int id=0; id<nbrLogs_; id++)
        realTime_[id]+=1000;
    if (realTime_[0] > 0) {
        Viewer3D->setBtnEnable(CTRL_BTN_PREVIOUS, true);
        Viewer3D->setBtnEnable(CTRL_BTN_STEP_BACKWARD, true); 
    }
    Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    updateDisplay();
}

void btnStepBackward(ViewerControlButtonId btnId)
{
    play_=false;
    for(int id=0; id<nbrLogs_; id++) {
        realTime_[id] -= 1000;
        if (realTime_[id] < 0) realTime_[id] = 0;
    }
    if (realTime_[0] <= 0) {
        Viewer3D->setBtnEnable(CTRL_BTN_PREVIOUS, false);
        Viewer3D->setBtnEnable(CTRL_BTN_STEP_BACKWARD, false); 
    }
    Viewer3D->setBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    updateDisplay();
}

void btnFaster(ViewerControlButtonId btnId)
{
    if (playSpeed_ < 10) playSpeed_ *= 1.5;
    if (playSpeed_ >= 10) {
	playSpeed_ = 10;
        Viewer3D->setBtnEnable(CTRL_BTN_FASTER, false);
    }
    Viewer3D->setBtnEnable(CTRL_BTN_SLOWER, true);
    updateDisplay();
}

void btnSlower(ViewerControlButtonId btnId)
{
    if (playSpeed_ > 0.1) playSpeed_ /= 1.5;
    if (playSpeed_ <= 0.1) {
	playSpeed_ = 0.1;
        Viewer3D->setBtnEnable(CTRL_BTN_SLOWER, false);
    }
    Viewer3D->setBtnEnable(CTRL_BTN_FASTER, true);
    updateDisplay();
}


void btnRecordMovie(ViewerControlButtonId btnId)
{
    record_ = !record_;
    if (record_) {
	//reshapeRobot3D(200, 150);
	printf("Start recording %s\n", baseName_);
	Viewer3D->movieStart(baseName_, LOGVIEWER_RECORD_SCREEN);
	Viewer3D->setBtnTexture(CTRL_BTN_RECORD, 
                                TEX_BTN_REC_STOP_0, TEX_BTN_REC_STOP_1);
    } else {
	Viewer3D->movieStop(LOGVIEWER_RECORD_SCREEN);
	Viewer3D->setBtnTexture(CTRL_BTN_RECORD, 
                                TEX_BTN_REC_START_0, TEX_BTN_REC_STOP_1);
	static char filename[256];
	Viewer3D->getMovieBaseName(LOGVIEWER_RECORD_SCREEN, 
				   filename, false);
	filename[strlen(filename)-5]=0;
	endSSMMFile(filename);
    }
    updateDisplay();
}

extern void (*viewer3DDisplayCB)();

// ---------------------------------------------------------------------------
// displayCB
// ---------------------------------------------------------------------------
// Callback appele apres checkscreen shot pour la video
// ---------------------------------------------------------------------------
void displayCB()
{
    bool soundAdded=false;
    if (record_ && play_) {
	for(int id=0; id<nbrLogs_; id++) {
            realTime_[id] += 80; // un multiple de 40 (unite de temps de base de SSMM)
            SoundId snd;
            if (getSound(id, realTime_[id]-80, realTime_[id], snd)) {
                //   soundAdded = copySoundFile(snd);
            }
        }
    } 
    if (record_) {
	static char filename[256];
	int counter = Viewer3D->getMovieBaseName(LOGVIEWER_RECORD_SCREEN, 
						 filename, false);
	filename[strlen(filename)-5]=0;
	updateSSMMFile(filename, counter, soundAdded);
    }
}


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char*argv[]) 
{   
    nbrLogs_ = 0;
    if (!parseArgs(argc, argv)) {
        return -1;
    }

    // load log
    char* tmp=strrchr(argv[1], '/');
    if (tmp) {
        strcpy(baseName_, tmp+1);
    } else {
	strcpy(baseName_, argv[1]);
    }
    tmp=strchr(baseName_, '.');
    if (tmp) tmp[0]=0;

    // start viewer
//    RobotConfigCL config("Viewer config", true);
    LogCL log;
    Viewer3D->createWindows(true, true);
    realTimeReset();

    // load logs
    for(int i=0; i < nbrLogs_; i++) {
        realTime_[i] = 0;
        loadLog(i, logFilename_[i]); 
        Viewer3D->setRobotModel(i,
                                logFilename_[i],
                                data_[i].model,
                                false);
    }
    if (strlen(logConfigFilename_)>0) loadConfigFile(logConfigFilename_);
    
    Viewer3D->registerBtnCallback(CTRL_BTN_PREVIOUS, btnBack);
    Viewer3D->registerBtnCallback(CTRL_BTN_STEP_BACKWARD, btnStepBackward);
    Viewer3D->registerBtnCallback(CTRL_BTN_PLAY, btnPlay);
    Viewer3D->registerBtnCallback(CTRL_BTN_STEP_FORWARD, btnStepForward);
    Viewer3D->registerBtnCallback(CTRL_BTN_NEXT, btnForward);
    Viewer3D->registerBtnCallback(CTRL_BTN_SLOWER, btnSlower);
    Viewer3D->registerBtnCallback(CTRL_BTN_FASTER, btnFaster);
    Viewer3D->registerBtnCallback(CTRL_BTN_RECORD, btnRecordMovie);
    
    play_=false;
    for(int i=0; i < nbrLogs_; i++) {
        getNextMatchBegin(i, realTime_[i], realTime_[i]);
    }
    
    updateDisplay();
    
    while(1) {
	sprintf(info_, "Speed x%.1f ", (float)playSpeed_);
	if (record_) strcat(info_, "Recording... ");
	else strcat(info_, baseName_);
      
	//   if (!record) {
	// quand on fait une video c'est deja assez lent, pas 
	// besoin de s'arreter un peu plus...
	usleep(50000);

	if (play_) {
            updateDisplay();
	    if (!record_) {
                LogTime dt=(LogTime)(playSpeed_*realTimeDeltaT());
		for(int i=0; i < nbrLogs_; i++) {
                    realTime_[i] += dt;
                }
	    } else {
		// ca c'est fait par le displayCallBack
		// realTime+=100;
	    }
	}	
        bool end=false;
        for(int i=0; i < nbrLogs_; i++) {
            if (realTime_[i] >= endTime_) end = true;
        }
        if (end) {
	    play_ = false;
	    if (record_) {
		Viewer3D->movieStop(LOGVIEWER_RECORD_SCREEN);
		record_ = false;
		Viewer3D->setBtnTexture(CTRL_BTN_RECORD, 
                                        TEX_BTN_REC_START_0, TEX_BTN_REC_STOP_1);
	    }
	    Viewer3D->setBtnTexture(CTRL_BTN_PLAY, 
                                    TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
	}
    }
    return(EXIT_SUCCESS);
}
