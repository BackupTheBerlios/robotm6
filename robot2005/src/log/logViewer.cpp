/**
 * @file logViewer.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Outil d'affichage d'un log utilisant le viewer3D
 */
#if 0

#define SOUND_INFO
#include "log.h"
#include "viewer3D.h"
#include "robotFile.h"
#include "robotTimer.h"

#include <map>

static const ViewerScreenEN LOGVIEWER_RECORD_SCREEN = VIEWER_SCREEN_MAP;

using namespace std;

extern void reshapeRobot3D(int w, int h);

typedef long LogTime; // millisecond
typedef multimap<LogTime, LogPacketMessage>    LogVMessage;
typedef multimap<LogTime, LogPacketPosition>   LogVPosition;
typedef multimap<LogTime, LogPacketMotorInfo>  LogVMotor;
typedef multimap<LogTime, LogPacketEnumValue>      LogVSound;
typedef multimap<LogTime, LogPacketCamera>     LogVCamera;
typedef multimap<LogTime, LogPacketTrajectory> LogVTrajectory;
typedef multimap<LogTime, LogPacketObstacle>   LogVObstacle;
typedef multimap<LogTime, LogPacketGonio>      LogVGonio;
typedef multimap<LogTime, LogType>             LogVMisc; // autres infos

typedef struct LogVData {
    LogVMessage  msg;
    LogVPosition pos;
    LogVMotor    motor;
    LogVSound    snd;
    LogVMisc     misc;
    LogVCamera   camera;
    LogVTrajectory traj;
    LogVObstacle obs;
    LogVGonio    gonio;
    bool         simulated;
} LogVData;

static LogVData data_;
static char info_[255];
static char baseName[255];
LogTime endTime_=0;
static Viewer3DCL* viewer_=NULL;
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
bool restorePacket(File* file,
                   LogPacketHeader& packetHeader, 
                   Byte* data)
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

// ---------------------------------------------------------------------------
// loadLog
// ---------------------------------------------------------------------------
// Lit le fichier est sauvegarde les donnees dans la structure data_
// ---------------------------------------------------------------------------
void loadLog(const char* filename)
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
	endTime_ = currentTime;
	switch(packetHeader.type) {
	case LOG_TYPE_SIMU:
	    data_.simulated=true;
	    break; 
	case LOG_TYPE_REAL:
	    data_.simulated=false;
	    break; 
	case LOG_LEVEL_ERROR:
	case LOG_LEVEL_WARNING:
	case LOG_LEVEL_INFO:
	case LOG_LEVEL_OK:
	case LOG_LEVEL_FUNCTION:
	    data_.msg.insert(std::pair<LogTime, LogPacketMessage>
			     (currentTime, *((LogPacketMessage*)data)));
	    break;

	case LOG_TYPE_POSITION:
	    data_.pos.insert(std::pair<LogTime, LogPacketPosition>
			     (currentTime, *((LogPacketPosition*)data)));
	    break;
	case LOG_TYPE_MOTOR:
	    data_.motor.insert(std::pair<LogTime, LogPacketMotorInfo>
			     (currentTime, *((LogPacketMotorInfo*)data)));
	    break;
	case LOG_TYPE_SOUND:
	    data_.snd.insert(std::pair<LogTime, LogPacketEnumValue>
			     (currentTime, *((LogPacketEnumValue*)data)));
	    break;
	case LOG_TYPE_CAMERA_SIMU:
	    data_.camera.insert(std::pair<LogTime, LogPacketCamera>
			     (currentTime, *((LogPacketCamera*)data)));
	    break;
	case LOG_TYPE_TRAJECTORY:
	    data_.traj.insert(std::pair<LogTime, LogPacketTrajectory>
			     (currentTime, *((LogPacketTrajectory*)data)));
	    break;
	case LOG_TYPE_OBSTACLE:
	    data_.obs.insert(std::pair<LogTime,LogPacketObstacle>
			     (currentTime,  *((LogPacketObstacle*)data)));
	    break;
	case LOG_TYPE_GONIO:
	    data_.gonio.insert(std::pair<LogTime,LogPacketGonio>
			       (currentTime,  *((LogPacketGonio*)data)));
	    break;
	case LOG_TYPE_PING:
	case LOG_TYPE_START_MATCH: 
	case LOG_TYPE_END_MATCH:
	case LOG_TYPE_START_STRATEGY: 
	default: 
	    while (data_.misc.find(currentTime) != data_.misc.end()) currentTime++;
	    data_.misc.insert(std::pair<LogTime, LogType>
			      (currentTime, packetHeader.type));
	    break; 
	}
    }
    printf("  %s loaded\n", filename);
}

// ==========================================================================
// Recupper les informations necessaire a l'instant t
// ==========================================================================

// ---------------------------------------------------------------------------
// getNearestBefore
// ---------------------------------------------------------------------------
template<typename T>
bool getNearestBefore(LogTime t, multimap<LogTime, T> data, T& result)
{
    typename multimap<LogTime, T>::iterator it;
    for(it=data.begin(); it!=data.end(); it++) {
	if ((*it).first > t) {
	    if ( it != data.begin()) {
		return true;
	    } else {
		return false;
	    }
	}
	result = (*it).second;
    }
    return false;
}

// ---------------------------------------------------------------------------
// getSound
// ---------------------------------------------------------------------------
bool getSound(LogTime t1, LogTime t2, int& snd)
{
    LogVSound::iterator it;
    for(it=data_.snd.begin(); it!=data_.snd.end(); it++) {
	if ((*it).first > t2 ) return false;
	if ((*it).first >= t1) {
	    snd = (*it).second.value;
	    return true;
	}
    }
    return false;
}

// ---------------------------------------------------------------------------
// getPosition
// ---------------------------------------------------------------------------
Position getPosition(LogTime t)
{
    LogVPosition::iterator it;
    LogTime t1=0, t2=0;
    Point p1, p2;
    Radian dir1=0, dir2=0;
    for(it=data_.pos.begin(); it != data_.pos.end(); it++) {
	if ((*it).first > t) {
	    t2 = (*it).first;
	    p2 = Point((*it).second.x, (*it).second.y);
	    dir2=(*it).second.theta;
	    if (t2 == t1 || it == data_.pos.begin()) {
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
bool getNextMatchBegin(LogTime fromTime,
		       LogTime &nextMatchTime)
{
    LogVMisc::iterator it;
    for(it=data_.misc.begin(); it != data_.misc.end(); it++) {
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
bool getPreviousMatchBegin(LogTime fromTime,
			   LogTime &previousMatchTime)
{
    LogVMisc::iterator it;
    previousMatchTime=0;
    for(it=data_.misc.begin(); it != data_.misc.end(); it++) {
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
// getCameraPosition
// ---------------------------------------------------------------------------
void getCameraPosition(LogTime fromTime,
		       Millimeter& x, Millimeter& y, Millimeter& z)
{
    LogVCamera::iterator it;
    x=0;
    y=0;
    z=700;
    for(it=data_.camera.begin(); it != data_.camera.end(); it++) {
	if ((*it).first >= fromTime) {
	    return ;
	}
	x = (*it).second.eyesX;
	y = (*it).second.eyesY;
	z = (*it).second.eyesZ;
    }
}

// ---------------------------------------------------------------------------
// getCoconutPosition
// ---------------------------------------------------------------------------
/*
void getCoconutPosition(LogTime fromTime, 
			Point& pt)
{
    LogVCoconut::iterator it;
    pt=Point();
    for(it=data_.coconut.begin(); it != data_.coconut.end(); it++) {
	if ((*it).first >= fromTime) {
	    return ;
	}
	pt = (*it).second;
    }
}
*/

// ---------------------------------------------------------------------------
// getTrajectory
// ---------------------------------------------------------------------------
void getTrajectory(LogTime fromTime,
		   Trajectory &t)
{
    LogVTrajectory::iterator it;
    t.clear();
    for(it=data_.traj.begin(); it != data_.traj.end(); it++) {
	if ((*it).first > fromTime) {
	    return ;
	}
	t.clear();
	for(int i=0; i<(*it).second.nbrPt; i++) {
	    t.push_back(Point((*it).second.ptx[i], (*it).second.pty[i]));
	}
    }
}

// ---------------------------------------------------------------------------
// getObstacles
// ---------------------------------------------------------------------------
void getObstacles(LogTime fromTime,
		  ListOfObstacles &obstacles,
                  int maxObstacles)
{
    LogVObstacle::iterator it;
    for(it=data_.obs.begin(); it != data_.obs.end(); it++) {
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
// getBalls
// ---------------------------------------------------------------------------
/*
void getBalls(LogTime fromTime,
	      ListOfBalls &balls,
              int maxBalls)
{
    LogVBall::iterator it;
    for(it=data_.balls.begin(); it != data_.balls.end(); it++) {
	if ((*it).first > fromTime) {
	    return ;
	}
	Ball ball;
	ball.center.x = (*it).second.ball.x;
	ball.center.y = (*it).second.ball.y;
	balls.push_back(ball);
        if ((int)balls.size() > maxBalls) balls.pop_front();
    }
}
*/

// ---------------------------------------------------------------------------
// getGonio
// ---------------------------------------------------------------------------
bool getGonio(LogTime fromTime,
	      Point& gonioCenter,
	      Radian& baliseDir,
	      int& baliseId)
{
    bool found=false;
    LogVGonio::iterator it;
    LogTime minTime=10000; // 10s
    for(it=data_.gonio.begin(); it != data_.gonio.end(); it++) {
	if ((*it).first < fromTime-minTime) continue;
	if ((*it).first > fromTime) return found;
	baliseId = (int)(*it).second.baliseId;
	gonioCenter.x = (*it).second.pos.x;
	gonioCenter.y = (*it).second.pos.y;
	baliseDir = d2r((*it).second.pos.t);
	found=true;
    }
    return found;
}


// ---------------------------------------------------------------------------
// updateDisplay
// ---------------------------------------------------------------------------
// Actualise l'affichage avec les valeurs a l'instant t
// ---------------------------------------------------------------------------
void updateDisplay(Viewer3DCL &viewer,
		   LogTime time)
{
    /*    viewer.setRobotPosition(getPosition(time));

    Position robotRS; 
    Position ennemyRP; 
    Position ennemyRS;
    static Ball balls[BALLON_NBR];
    if (data_.simulated) {
	getSimuPosition(time, robotRS, ennemyRP, ennemyRS, balls);
	if (robotRS.center.x>10)
	    viewer.setRobotRSPosition(robotRS);
	if (ennemyRP.center.x>10)
	    viewer.setEnnemyPosition(ennemyRP);
	if (ennemyRS.center.x>10)
	    viewer.setEnnemyRSPosition(ennemyRS);
	viewer.setBallsPosition(balls);
    }
  
    LogTime matchTime=time;
    bool armLeft=false;
    bool armRight=false;
    bool entryOpened=true;
    bool lowerArmOut=true;
    bool backDoorClosed=true;
    getActuatorStatus(time, matchTime, armLeft, armRight, entryOpened, 
		      lowerArmOut, backDoorClosed);
    viewer.setRobotActuators(armLeft, armRight, entryOpened, 
			     lowerArmOut, backDoorClosed);

    Millimeter cx; 
    Millimeter cy; 
    Millimeter cz;
    if (data_.simulated) {
	getCameraPosition(time, cx, cy, cz);
	viewer.setCameraPosition(cx, cy, cz); 
    }
    
    LogGrid* grid=NULL;
    getGrid(time, &grid);
    viewer.setGrid(grid);


    Trajectory t;
    getTrajectory(time, t);
    viewer.setTargetTrajectory(t);

    ListOfObstacles obstacles;
    getObstacles(time, obstacles, 10);
    viewer.setObstacleList(obstacles, ALL_OBSTACLES);

    ListOfBalls ballsDetected;
    getBalls(time, ballsDetected, 10);
    viewer.setBallListDetected(ballsDetected);

    Point coconut;
    getCoconutPosition(time, coconut);
    viewer.setCoconutPosition(coconut);

    Point gonioCenter;
    Radian baliseDir;
    GonioBalise baliseId;
    if (getGonio(time, gonioCenter, baliseDir, baliseId)) {
	viewer.setGonioBalise(baliseId, gonioCenter, baliseDir);
    } 
    
    playerControlSetTimes(time, matchTime, info_);
    viewer.updateDisplay((Millisecond)matchTime);
    */
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
bool   play=false;
bool   record=false;
double playSpeed=1;
LogTime realTime=0;

void btnPlay()
{
    play = !play;
    if (play) {
	realTimeReset();
	playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PAUSE_0, TEX_BTN_PAUSE_1);
    } else {
	playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
    }
}

void btnBack()
{
    play=false;
    if (viewer_) viewer_->reset();
    getPreviousMatchBegin(realTime, realTime);
    if (realTime==0) {
	playerControlSetBtnEnable(CTRL_BTN_PREVIOUS, false);
	playerControlSetBtnEnable(CTRL_BTN_STEP_BACKWARD, false); 
    }
    playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
}

void btnForward()
{
    play=false;
    getNextMatchBegin(realTime, realTime);
    if (realTime>0) {
	playerControlSetBtnEnable(CTRL_BTN_PREVIOUS, true);
	playerControlSetBtnEnable(CTRL_BTN_STEP_BACKWARD, true); 
    }
    playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
}

void btnStepForward()
{
    play=false;
    realTime+=1000;
    if (realTime>0) {
	playerControlSetBtnEnable(CTRL_BTN_PREVIOUS, true);
	playerControlSetBtnEnable(CTRL_BTN_STEP_BACKWARD, true); 
    }
    playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
}

void btnStepBackward()
{
    play=false;
    realTime-=1000;
    if (realTime<0) realTime=0;

    if (realTime<=0) {
	playerControlSetBtnEnable(CTRL_BTN_PREVIOUS, false);
	playerControlSetBtnEnable(CTRL_BTN_STEP_BACKWARD, false); 
    }
    playerControlSetBtnTexture(CTRL_BTN_PLAY, TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
}

void btnFaster()
{
    if (playSpeed<10) playSpeed*=1.5;
    if (playSpeed>=10) {
	playSpeed=10;
	playerControlSetBtnEnable(CTRL_BTN_FASTER, false);
    }
    playerControlSetBtnEnable(CTRL_BTN_SLOWER, true);
}

void btnSlower()
{
    if (playSpeed>0.1) playSpeed/=1.5;
    if (playSpeed<=0.1) {
	playSpeed=0.1;
	playerControlSetBtnEnable(CTRL_BTN_SLOWER, false);
    }
    playerControlSetBtnEnable(CTRL_BTN_FASTER, true);
}


void btnRecordMovie()
{
    record = !record;
    if (record) {
	//reshapeRobot3D(200, 150);
	printf("Start recording %s\n", baseName);
	Viewer3D::movieStart(baseName, LOGVIEWER_RECORD_SCREEN);
	playerControlSetBtnTexture(CTRL_BTN_RECORD, TEX_BTN_REC_STOP_0, TEX_BTN_REC_STOP_1);
    } else {
	Viewer3D::movieStop(LOGVIEWER_RECORD_SCREEN);
	playerControlSetBtnTexture(CTRL_BTN_RECORD, TEX_BTN_REC_START_0, TEX_BTN_REC_STOP_1);
	static char filename[256];
	Viewer3D::getMovieBaseName(LOGVIEWER_RECORD_SCREEN, 
				   filename, false);
	filename[strlen(filename)-5]=0;
	endSSMMFile(filename);
    }
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
    if (record && play) {
	realTime += 80; // un multiple de 40 (unite de temps de base de SSMM)
	SoundId snd;
	if (getSound(realTime-80, realTime, snd)) {
	    soundAdded=copySoundFile(snd);
	}
    } 
    if (record) {
	static char filename[256];
	int counter = Viewer3D::getMovieBaseName(LOGVIEWER_RECORD_SCREEN, 
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
    if (argc<2) {
        printf("Usage: %s logFileName \n", argv[0]);
        return -1;
    }
    viewer3DDisplayCB = displayCB;

    char* tmp=strrchr(argv[1], '/');
    if (tmp) {
        strcpy(baseName, tmp+1);
    } else {
	strcpy(baseName, argv[1]);
    }
    tmp=strchr(baseName, '.');
    if (tmp) tmp[0]=0;

    loadLog(argv[1]);
    RobotConfigSimu config;
    config.viewerLogPlayer=true;
    Log log(&config);
    Viewer3D viewer(&config);
    viewer_=&viewer;
    viewer.enableRobotPoint(true);
    realTimeReset();

    playerControlRecordBtnCB(CTRL_BTN_PREVIOUS, btnBack);
    playerControlRecordBtnCB(CTRL_BTN_STEP_BACKWARD, btnStepBackward);
    playerControlRecordBtnCB(CTRL_BTN_PLAY, btnPlay);
    playerControlRecordBtnCB(CTRL_BTN_STEP_FORWARD, btnStepForward);
    playerControlRecordBtnCB(CTRL_BTN_NEXT, btnForward);
    playerControlRecordBtnCB(CTRL_BTN_SLOWER, btnSlower);
    playerControlRecordBtnCB(CTRL_BTN_FASTER, btnFaster);
    playerControlRecordBtnCB(CTRL_BTN_RECORD, btnRecordMovie);
    
    realTime=0;
    play=false;
    getNextMatchBegin(realTime, realTime);
    while(1) {
	sprintf(info_, "Speed x%.1f ", (float)playSpeed);
	if (record) strcat(info_, "Recording... ");
	else strcat(info_, baseName);
      
	updateDisplay(viewer, realTime);
	//   if (!record) {
	// quand on fait une video c'est deja assez lent, pas 
	// besoin de s'arreter un peu plus...
	usleep(50000);

	if (play) {
	    if (!record) {
		realTime+=(LogTime)(playSpeed*realTimeDeltaT());
	    } else {
		// ca c'est fait par le displayCallBack
		// realTime+=100;
	    }
	}
	if (realTime >= endTime_) {
	    play = false;
	    if (record) {
		Viewer3D::movieStop(LOGVIEWER_RECORD_SCREEN);
		record = false;
		playerControlSetBtnTexture(CTRL_BTN_RECORD, 
					   TEX_BTN_REC_START_0, TEX_BTN_REC_STOP_1);
	    }
	    playerControlSetBtnTexture(CTRL_BTN_PLAY, 
				       TEX_BTN_PLAY_0, TEX_BTN_PLAY_1);
	}
    }
    viewer_=NULL;
    return(EXIT_SUCCESS);
}

// function pour eviter des warnign du compilo car music* n'est pas utilise
void dummyFunction()
{
    printf("%s %s %s\n", 
	   musicList[0],
	   musicYukulele[0],
	   musicHaka);
}

#else 
#include <stdio.h>
int main() {
    printf("Not implemented\n");
    return -1;
}
#endif
