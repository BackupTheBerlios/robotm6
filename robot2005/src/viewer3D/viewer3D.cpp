#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
//#define VIEWER_MODE_VIDEO

#ifndef linux
#ifndef VIEWER_DISACTIVATED
#define VIEWER_DISACTIVATED
#endif
#endif

#ifndef VIEWER_DISACTIVATED 
#include <GL/glut.h>
#include "mthread.h"
#include "texture.h"
#include "text.h"
#include "button.h"
#endif // VIEWER_DISACTIVATED

#ifndef M_PI
#define M_PI (3.14)
#define M_PI_2 (M_PI/2.)
#define M_3_PI_2 (M_PI_2*3.)
#endif

#include "viewerMap2D.h"
#include "viewerMap3D.h"
#include "viewerPlayer.h"

#include "viewer3D.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------
Viewer3DCL* Viewer3DCL::viewer_=NULL;
ViewerMovieDataST Viewer3DCL::movieData_[VIEWER_SCREEN_NBR];


// ---------------------------------------------------------------------------
// Viewer3DCL::setTitle
// ---------------------------------------------------------------------------
void Viewer3DCL::setTitle(const char* title)
{
#ifndef VIEWER_DISACTIVATED 
    title_ = std::string(title);
#endif
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setTime
// ---------------------------------------------------------------------------
void Viewer3DCL::setTime(Millisecond currentTime)
{
#ifndef VIEWER_DISACTIVATED 
    time_ = currentTime;
#endif
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setMatchTime
// ---------------------------------------------------------------------------
void Viewer3DCL::setMatchTime(Millisecond currentTime)
{
#ifndef VIEWER_DISACTIVATED 
    matchTime_ = currentTime;
#endif
}

// ---------------------------------------------------------------------------
// Viewer3DCL::registerBtnCallback
// ---------------------------------------------------------------------------
void Viewer3DCL::registerBtnCallback(ViewerControlButtonId btnId,
				     clickBtnCB            clickCB,
				     clickBtnCB            unclickCB)
{
    if (btn_[btnId]
	&& btnId != CTRL_BTN_R0_COULEUR 
	&& btnId != CTRL_BTN_R1_COULEUR
	&& btnId != CTRL_BTN_R2_COULEUR 
	&& btnId != CTRL_BTN_R3_COULEUR) {
	btn_[btnId]->registerCB(clickCB, unclickCB);
    }
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setBtnClick
// ---------------------------------------------------------------------------
void Viewer3DCL::setBtnClick(ViewerControlButtonId btnId, bool click)
{
    if (btn_[btnId]) btn_[btnId]->setClicked(click);
}
// ---------------------------------------------------------------------------
// Viewer3DCL::setBtnTexture
// ---------------------------------------------------------------------------
void Viewer3DCL::setBtnTexture(ViewerControlButtonId btnId, 
                               TextureId tunclick, 
                               TextureId tclick)
{
    if (btn_[btnId]) btn_[btnId]->setTexture(tunclick, tclick);
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setBtnEnable
// ---------------------------------------------------------------------------
void Viewer3DCL::setBtnEnable(ViewerControlButtonId btnId, 
                              bool enable)
{
    if (btn_[btnId]) btn_[btnId]->setEnable(enable);
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotPosition
// ---------------------------------------------------------------------------
// Autorise ou non l'affiche du point correspondant a la position du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::enableDisplayRobots(bool enable)
{
#ifndef VIEWER_DISACTIVATED 
    enableObjectDisplay_=enable;
#endif
}

// ---------------------------------------------------------------------------
// Viewer3DCL::enableDisplayEstimatedPos
// ---------------------------------------------------------------------------
// Autorise ou non l'affiche du point correspondant a la position du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::enableDisplayEstimatedPos(bool enable)
{
#ifndef VIEWER_DISACTIVATED 
    enableEstimatedDisplay_=enable;
#endif
}
// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotModel
// ---------------------------------------------------------------------------
// Definit la forme du robot, et son nom
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotModel(int                  robotId,
                               std::string const&   name,
                               RobotModel           model,
                               bool                 brick,
                               int                  isDead)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].set(robotId, name, model, brick, isDead);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::getRobotTeam
// ---------------------------------------------------------------------------
TeamColor Viewer3DCL::getRobotTeam(int robotId)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        return robotData_[robotId].getTeam();
    }
#endif // VIEWER_DISACTIVATED 
    return TEAM_RED;
}
void Viewer3DCL::setRobotTeam(int robotId, TeamColor team)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
	setBtnClick((ViewerControlButtonId)(CTRL_BTN_R0_COULEUR+robotId*CTRL_BTN_NBR_PER_ROBOT),
		    ((team==TEAM_RED)?false:true));
        return robotData_[robotId].setTeam(team);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotColor
// ---------------------------------------------------------------------------
// Definit le message affiche sur le LCD
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotLcd(int         robotId,
			     const char* message)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setLcd(robotId, message);
    }
#endif // VIEWER_DISACTIVATED 
}
       
// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotColor
// ---------------------------------------------------------------------------
// Definit la couleur du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotColor(int                  robotId,
			       ViewerColorST const& color)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setColor(color);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::removeRobot
// ---------------------------------------------------------------------------
// N'affiche plus le robot robotId
// ---------------------------------------------------------------------------
void Viewer3DCL::removeRobot(int robotId)
{
    robotData_[robotId].remove();
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setBridgePosition
// ---------------------------------------------------------------------------
void Viewer3DCL::setBridgePosition(BridgePosition pos)
{
#ifndef VIEWER_DISACTIVATED 
    bridge_ = pos;
#endif // VIEWER_DISACTIVATED 
}
       
// ---------------------------------------------------------------------------
// Viewer3DCL::setSupportPosition
// ---------------------------------------------------------------------------
// Defini la position des supports de quilles sur le terrain
// ---------------------------------------------------------------------------
void Viewer3DCL::setSupportPosition(Point const& pt1, 
                                    Point const& pt2)
{
#ifndef VIEWER_DISACTIVATED 
    support1_ = pt1;
    support2_ = pt2;
#endif // VIEWER_DISACTIVATED 
}

// ===========================================================================
// Mettre a jour la position des objets sur le terrains
// ===========================================================================

// ---------------------------------------------------------------------------
// Viewer3DCL::setCameraPosition
// ---------------------------------------------------------------------------
// Definit la position de la camera autour de la table pour la vue 3D  
// ---------------------------------------------------------------------------
void Viewer3DCL::setCameraPosition(Millimeter cx, 
                                   Millimeter cy, 
                                   Millimeter cz)
{
#ifndef VIEWER_DISACTIVATED 
    setCameraPosition3D(cx, cy, cz);
#endif // VIEWER_DISACTIVATED 
}
// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotPosition
// ---------------------------------------------------------------------------
// Met a jour la position du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotPosition(int robotId,
                                  Millimeter x, 
                                  Millimeter y, 
                                  Radian theta)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setPos(x, y, theta);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotPosition
// ---------------------------------------------------------------------------
// Met a jour la position du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotPosition(int robotId,
                                  Position const& pos)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setPos(pos);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setEstimatedBridgePosition
// ---------------------------------------------------------------------------
void Viewer3DCL::setEstimatedBridgePosition(int robotId, 
                                            BridgePosition pos)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setEstimatedBridge(pos);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setEstimatedSupportPosition
// ---------------------------------------------------------------------------
void Viewer3DCL::setEstimatedSupportPosition(int robotId, 
                                             Point const& pt1, 
                                             Point const& pt2)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setEstimatedSupport(pt1, pt2);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotEstimatedPosition
// ---------------------------------------------------------------------------
// Met a jour la position du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotEstimatedPosition(int robotId,
                                           Position const& pos)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setEstimatedPos(pos);
    }
#endif // VIEWER_DISACTIVATED 
}

// --------------------------------------------------------------------------
// Viewer3DCL::setSkittlePosition
// ---------------------------------------------------------------------------
// Defini la position de toutes les quilles sur le jeu 
// ---------------------------------------------------------------------------
void Viewer3DCL::setSkittlePosition(Skittle* skittles)
{
#ifndef VIEWER_DISACTIVATED 
    skittles_ = skittles;
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotPosition
// ---------------------------------------------------------------------------
        /** @brief Defini la position des balles de GRS */
// ---------------------------------------------------------------------------
void Viewer3DCL::setGRSBallsPosition(GRSBall* balls)
{
#ifndef VIEWER_DISACTIVATED 
   grsBalls_ = balls;
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotPosition
// ---------------------------------------------------------------------------
// Defini la position des balles de Squatch
// ---------------------------------------------------------------------------
void Viewer3DCL::setSquatchBallsPosition(SquatchBall* balls)
{
#ifndef VIEWER_DISACTIVATED 
    squatchBalls_ = balls;
#endif // VIEWER_DISACTIVATED 
}
        
// ===========================================================================
// Donnees de debug concernant chaque robot
// ===========================================================================
 
// ---------------------------------------------------------------------------
// Viewer3DCL::setRobotTrajectory
// ---------------------------------------------------------------------------
// Trajectoire cible du robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setRobotTrajectory(int robotId,
                                    Trajectory const& t)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setTrajectory(t);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::setObstacleList
// ---------------------------------------------------------------------------
// Met a jour la liste d'obstacles detectes par un robot 
// ---------------------------------------------------------------------------
void Viewer3DCL::setObstacleList(int                    robotId,
                                 ListOfObstacles const& obstacles,
                                 ObstacleType           type)
{
#ifndef VIEWER_DISACTIVATED 
    if (robotId >= 0 && robotId < VIEWER_MAX_ROBOT_NBR) {
        robotData_[robotId].setObstacles(obstacles, type);
    }
#endif // VIEWER_DISACTIVATED 
}

// ===========================================================================
// OpenGL functions
// ===========================================================================

#ifndef VIEWER_DISACTIVATED 

// ---------------------------------------------------------------------------
// private namespace
// ---------------------------------------------------------------------------
namespace {
    bool displayMap2D_  =true;
    bool displayMap3D_  =true;
    bool displayPlayer_ =false;
  
    int windowIdMap2D_  =0;
    int windowIdMap3D_  =0;
    int windowIdPlayer_ =0;
    
    void emptyFunction() {};

#ifdef VIEWER_MODE_VIDEO
    inline void refreshDisplay() {
        displayViewerMap3D();
        displayViewerMap2D();
        displayPlayerControl();
    }
#endif

    void glTimerFunction(int value) {
#ifdef VIEWER_MODE_VIDEO
	refreshDisplay();
#else
        if (windowIdMap2D_>0) {
	    glutSetWindow(windowIdMap2D_);
	    glutPostRedisplay();
	}
        if (windowIdMap3D_>0) {
	    glutSetWindow(windowIdMap3D_);
	    glutPostRedisplay();
	}
	if (windowIdPlayer_>0) {
	    glutSetWindow(windowIdPlayer_);
	    glutPostRedisplay();
	}
#endif
        glutTimerFunc(100, glTimerFunction, value);
    }

    extern "C" {

	void glInitWindows() {
	    int argc_=1;
	    char* argv_[1];
            static  char* name="Viewer 2005";
	    argv_[0]=name;
	    
	    glutInit(&argc_, argv_); 
	    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ); 
	    
	    // creation des fenetres
	    if (displayMap2D_) {
                // Carte du terrain
		glutInitWindowPosition(0,320); 
		glutInitWindowSize(310,220); 
		windowIdMap2D_=glutCreateWindow ("2D Map"); 
		glutReshapeFunc(reshapeViewerMap2D);
		glutDisplayFunc(displayViewerMap2D);
		glutIdleFunc (idleViewerMap2D);
		glutKeyboardFunc(keyboardViewerMap2D);
		initViewerMap2D(windowIdMap2D_);
		Texture->init();	     
	    }

	    if (displayMap3D_ ) {
		// vue cool en 3D
		glutInitWindowPosition(00,00); 
		glutInitWindowSize(500, 290); 
		windowIdMap3D_=glutCreateWindow ("3D View"); 
		glutReshapeFunc(reshapeViewerMap3D);
		glutDisplayFunc(displayViewerMap3D); 
		glutIdleFunc (idleViewerMap3D);
		glutKeyboardFunc(keyboardViewerMap3D);
		initViewerMap3D(windowIdMap3D_);
		Texture->init();
	    }
	    
	    if (displayPlayer_) {
		// boutons du player
		glutInitWindowPosition(500,0); 
		glutInitWindowSize(276, 330); 
		windowIdPlayer_=glutCreateWindow ("Player"); 
		glutReshapeFunc(reshapePlayerControl);
		glutDisplayFunc(displayPlayerControl); 
		glutIdleFunc(emptyFunction);
		glutKeyboardFunc(keyboardPlayerControl);
		glutMouseFunc(mousePlayerControl);
		initPlayerControl(windowIdPlayer_);
		Texture->init();
	    }
	}

        void* glutThreadBody1(void* data) 
        {
#ifndef VIEWER_MODE_VIDEO
	    glInitWindows();
	    glutTimerFunc(100, glTimerFunction, (int)0);
#endif
	    glutMainLoop();
	    return NULL;
        }
	
    }
}
#endif

// ===========================================================================
// Callback boutons du player
// ===========================================================================

// se debrouille pour avoir toujours 2 verts et 2 rouges
void checkRobotColor(int from) {
    TeamColor fromColor = Viewer3D->getRobotTeam(from);
    TeamColor nonFromColor = (fromColor==TEAM_RED)?TEAM_GREEN:TEAM_RED;
    int sameColorCount=0;
    for(int i=1;i<VIEWER_MAX_ROBOT_NBR; i++) {
	if (Viewer3D->getRobotTeam((from+i)%VIEWER_MAX_ROBOT_NBR) == fromColor) {
	    if (sameColorCount>0) {
		Viewer3D->setRobotTeam(((from+i)%VIEWER_MAX_ROBOT_NBR), nonFromColor);
	    } else {
		sameColorCount=1;
	    }
	}
    }
    if (sameColorCount == 0) {
	Viewer3D->setRobotTeam(((from+1)%VIEWER_MAX_ROBOT_NBR), fromColor);	
    }
};

// la couleur de l'equipe passe au vert
void clickColorBtn(ViewerControlButtonId btnId) {
    switch(btnId) {
    case CTRL_BTN_R0_COULEUR:
	Viewer3D->setRobotTeam(0, TEAM_GREEN);
	checkRobotColor(0);
	break;
    case CTRL_BTN_R1_COULEUR:
	Viewer3D->setRobotTeam(1, TEAM_GREEN);
	checkRobotColor(1);
	break;
    case CTRL_BTN_R2_COULEUR:
	Viewer3D->setRobotTeam(2, TEAM_GREEN);
	checkRobotColor(2);
	break;
    case CTRL_BTN_R3_COULEUR:
	Viewer3D->setRobotTeam(3, TEAM_GREEN);
	checkRobotColor(3);
	break;
    default:
        break;
    }
}
// la couleur de l'equipe passe au rouge
void unclickColorBtn(ViewerControlButtonId btnId) {
    switch(btnId) {
    case CTRL_BTN_R0_COULEUR:
	Viewer3D->setRobotTeam(0, TEAM_RED);
	checkRobotColor(0);
	break;
    case CTRL_BTN_R1_COULEUR:
	Viewer3D->setRobotTeam(1, TEAM_RED);
	checkRobotColor(1);
	break;
    case CTRL_BTN_R2_COULEUR:
	Viewer3D->setRobotTeam(2, TEAM_RED);
	checkRobotColor(2);
	break;
    case CTRL_BTN_R3_COULEUR:
	Viewer3D->setRobotTeam(3, TEAM_RED);
	checkRobotColor(3);
	break;
    default:
        break;
    }
}

// ===========================================================================
// INIT
// ===========================================================================

// ---------------------------------------------------------------------------
// Viewer3DCL::Viewer3DCL
// ---------------------------------------------------------------------------
Viewer3DCL::Viewer3DCL() : 
    RobotBase("Viewer3D", CLASS_VIEWER_3D),
    title_("Viewer3D"), time_(0), matchTime_(0), 
    skittles_(NULL), grsBalls_(NULL), squatchBalls_(NULL), 
    bridge_(BRIDGE_POS_UNKNOWN), support1_(-10,0), support2_(-10,0),
    enableEstimatedDisplay_(false), enableObjectDisplay_(false), useTexture_(true), 
    createThread_(true),
    map3DKeyBoardCB_(NULL), map2DKeyBoardCB_(NULL), playerKeyBoardCB_(NULL),
    cameraModeAuto3D_(false), useTexture3D_(true), targetRobotId3D_(0)
{
    assert(!viewer_);
    viewer_=this;
#ifndef VIEWER_DISACTIVATED 
    enableObjectDisplay_=false;
    for(int i=0; i<CTRL_BTN_NBR; i++) {
	btn_[i]=NULL;
    }
    btn_[CTRL_BTN_PREVIOUS]
	= new ControlButton(CTRL_BTN_PREVIOUS,
			    TEX_BTN_PREV_0, TEX_BTN_PREV_1, "",
			    0, 0);
    btn_[CTRL_BTN_STEP_BACKWARD]
	= new ControlButton(CTRL_BTN_STEP_BACKWARD,
			    TEX_BTN_STEP_BACKWARD_0, TEX_BTN_STEP_BACKWARD_1, "",
			    32, 0);
    btn_[CTRL_BTN_PLAY]
	= new ControlButton(CTRL_BTN_PLAY,
			    TEX_BTN_PLAY_0, TEX_BTN_PLAY_1, "",
			    64, 0);
    btn_[CTRL_BTN_STEP_FORWARD]
	= new ControlButton(CTRL_BTN_STEP_FORWARD,
			    TEX_BTN_STEP_FORWARD_0, TEX_BTN_STEP_FORWARD_1, "",
			    96, 0);
    btn_[CTRL_BTN_NEXT]
	= new ControlButton(CTRL_BTN_NEXT,
			    TEX_BTN_NEXT_0, TEX_BTN_NEXT_1, "",
			    128, 0);
    btn_[CTRL_BTN_SLOWER]
	= new ControlButton(CTRL_BTN_SLOWER,
			    TEX_BTN_SLOWER_0, TEX_BTN_SLOWER_1, "",
			    170, 0);
    btn_[CTRL_BTN_FASTER]
	= new ControlButton(CTRL_BTN_FASTER,
			    TEX_BTN_FASTER_0, TEX_BTN_FASTER_1, "",
			    202, 0);
    btn_[CTRL_BTN_RECORD]
	= new ControlButton(CTRL_BTN_RECORD,
			    TEX_BTN_REC_START_0, TEX_BTN_REC_START_1, "",
			    244, 0);

    btn_[CTRL_BTN_BRIDGE]
	= new ControlButton(CTRL_BTN_BRIDGE,
			    TEX_BRIDGE0, TEX_BRIDGE1, "",
			    0, 92);
    btn_[CTRL_BTN_SUPPORT]
	= new ControlButton(CTRL_BTN_SUPPORT,
			    TEX_SUPPORT0, TEX_SUPPORT1, "",
			    32, 92);

    btn_[CTRL_BTN_ROBOTS_JACK]
	= new ControlButton(CTRL_BTN_ROBOTS_JACK,
			    TXT_JACKOUT, TXT_JACKIN, "",
			    80, 100, true);
    btn_[CTRL_BTN_ROBOTS_AU]
	= new ControlButton(CTRL_BTN_ROBOTS_AU,
			    TEX_AU_OUT, TEX_AU_IN, "",
			    100, 100, true);

    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR;i++) {
	int y=145+i*50;
	btn_[CTRL_BTN_R0_JACK+i*CTRL_BTN_NBR_PER_ROBOT]
	    = new ControlButton((ViewerControlButtonId)(CTRL_BTN_R0_JACK+i*CTRL_BTN_NBR_PER_ROBOT),
				TXT_JACKOUT, TXT_JACKIN, "",
				80, y, true, false);
	btn_[CTRL_BTN_R0_AU+i*CTRL_BTN_NBR_PER_ROBOT]
	    = new ControlButton((ViewerControlButtonId)(CTRL_BTN_R0_AU+i*CTRL_BTN_NBR_PER_ROBOT),
				TEX_AU_OUT, TEX_AU_IN, "",
				100, y, true, false);
	btn_[CTRL_BTN_R0_COULEUR+i*CTRL_BTN_NBR_PER_ROBOT]
	    = new ControlButton((ViewerControlButtonId)(CTRL_BTN_R0_COULEUR+i*CTRL_BTN_NBR_PER_ROBOT),
				TEX_TEAM_RED, TEX_TEAM_GREEN, "",
				10, y, true, false, (i>=2)); // equipe verte pour robots >=2
	btn_[CTRL_BTN_R0_YES+i*CTRL_BTN_NBR_PER_ROBOT]
	    = new ControlButton((ViewerControlButtonId)(CTRL_BTN_R0_YES+i*CTRL_BTN_NBR_PER_ROBOT),
				TEX_BTN_0, TXT_BTN_1, "",
				200, y+15, true, false);
	btn_[CTRL_BTN_R0_NO+i*CTRL_BTN_NBR_PER_ROBOT]
	    = new ControlButton((ViewerControlButtonId)(CTRL_BTN_R0_NO+i*CTRL_BTN_NBR_PER_ROBOT),
				TEX_BTN_0, TXT_BTN_1, "",
				224, y+15, true, false);
	btn_[CTRL_BTN_R0_COULEUR+i*CTRL_BTN_NBR_PER_ROBOT]->registerCB(clickColorBtn,
						  unclickColorBtn);
    }
    setRobotTeam(0, TEAM_RED);
    setRobotTeam(1, TEAM_RED);
    setRobotTeam(2, TEAM_GREEN);
    setRobotTeam(3, TEAM_GREEN);
    LOG_OK("Initialisation Done\n");
#endif
}

// ---------------------------------------------------------------------------
// Viewer3DCL::~Viewer3DCL
// ---------------------------------------------------------------------------
Viewer3DCL::~Viewer3DCL()
{
#ifndef VIEWER_DISACTIVATED 
    if (createThread_) MTHREAD_CANCEL(thread_);
#endif
    viewer_=NULL;
}

// ---------------------------------------------------------------------------
// Viewer3DCL::instance
// ---------------------------------------------------------------------------
Viewer3DCL* Viewer3DCL::instance()
{
    if (!viewer_) viewer_ = new Viewer3DCL();
    return viewer_;
}

// ---------------------------------------------------------------------------
// Viewer3DCL::createWindows
// ---------------------------------------------------------------------------
void Viewer3DCL::createWindows(bool robot3D, 
                               bool robotPlayer) 
{
#ifndef VIEWER_DISACTIVATED 
    
    LOG_FUNCTION();
    
    displayMap2D_  =true;
    displayMap3D_  =robot3D;
    displayPlayer_ =robotPlayer;

#ifdef VIEWER_MODE_VIDEO
    glInitWindows();
#endif
    // create a timer that will update the display
    // create a thread for glutMainLoop which is a blocking function
    if (createThread_) {
        startThread();
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::startThread
// ---------------------------------------------------------------------------
void Viewer3DCL::startThread()
{
#ifndef VIEWER_DISACTIVATED 
#ifdef VIEWER_MODE_VIDEO
    glutTimerFunc(100, glTimerFunction, (int)this);
#endif
    MTHREAD_CREATE("Viewer", &thread_, NULL, glutThreadBody1, NULL);
    //MTHREAD_CREATE("Viewer", &thread_, NULL, glutThreadBody2, NULL);
    createThread_=true;
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::reset
// ---------------------------------------------------------------------------
bool Viewer3DCL::reset()
{
#ifndef VIEWER_DISACTIVATED 
    LOG_FUNCTION();
    time_=0;
    enableObjectDisplay_ = false;
    for (unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR ;i++) {
        robotData_[i].reset();
    }
    bridge_ = BRIDGE_POS_UNKNOWN;
    support1_=Point(-10,0);
    support2_=Point(-10,0);
    skittles_=NULL;
    grsBalls_=NULL;
    squatchBalls_=NULL;

    resetViewerMap3D();
    resetViewerMap2D();
    resetPlayerControl();
    
    return true;
#else
    return true;
#endif
}

// ===========================================================================
// Video et screenshot
// ===========================================================================

// ---------------------------------------------------------------------------
// checkIfFileExist
// ---------------------------------------------------------------------------
bool checkIfFileExist(const char* filename)
{
    FILE* fptr=fopen(filename, "rb");
    if (fptr) {
        fclose(fptr);
        return true;
    } else {
        return false;
    } 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::screenShot
// ---------------------------------------------------------------------------
void Viewer3DCL::screenShot(const char* filename)
{
#ifndef VIEWER_DISACTIVATED 
    Image img;
    unsigned int width  = glutGet(GLUT_WINDOW_WIDTH);
    unsigned int height = glutGet(GLUT_WINDOW_HEIGHT);
    img.alloc(width, height);
    glReadPixels(0, 0, width-1, height-1,
                 GL_RGB, // format
                 GL_UNSIGNED_BYTE, // type
                 img.data);
    static char filename2[255];
    static int count=0;
    do {
        sprintf(filename2, 
                "../screenshot/%s_%.4d.bmp", filename, count++);
    } while(checkIfFileExist(filename2));
    if (BmpBitmap::save(img, filename2, false, true)) {
        printf("Take screenshot %s\n", filename2);
    }
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::movieStart
// ---------------------------------------------------------------------------
void Viewer3DCL::movieStart(const char*  filename,
                            ViewerScreenEN screen)
{
#ifndef VIEWER_DISACTIVATED 
    int id=(int)screen;
    movieData_[id].run=true;
    static int count=0;
    do {
        sprintf(movieData_[id].filename, 
                "../movie/%s_%.4d_0000.bmp", filename, count++);
    } while(checkIfFileExist(movieData_[id].filename));
    printf("startMovie %s\n", movieData_[id].filename);
    movieData_[id].filename[strlen(movieData_[id].filename)-8]=0;
    movieData_[id].count=0;
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::movieStop
// ---------------------------------------------------------------------------
void Viewer3DCL::movieStop(ViewerScreenEN screen)
{
#ifndef VIEWER_DISACTIVATED 
    movieData_[(int)screen].run=false;
    printf("stopMovie %s\n", movieData_[(int)screen].filename);
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::getMovieBaseName
// ---------------------------------------------------------------------------
int Viewer3DCL::getMovieBaseName(ViewerScreenEN screen, 
                                 char*filename, 
                                 bool inc)
{
#ifndef VIEWER_DISACTIVATED 
    int id=(int)screen;
    sprintf(filename, "%s%.4d", 
            movieData_[id].filename, 
            movieData_[id].count);
    if (inc) {
        movieData_[id].count++;
        return movieData_[id].count-1;
    } else {
        return movieData_[id].count;
    }
#else 
    return 0;
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------------------
// Viewer3DCL::movieScreenShot
// ---------------------------------------------------------------------------
void Viewer3DCL::movieScreenShot(ViewerScreenEN screen)
{
#ifndef VIEWER_DISACTIVATED 
    int id=(int)screen;
    if (!movieData_[(int)screen].run) return;
    unsigned int width  = glutGet(GLUT_WINDOW_WIDTH);
    unsigned int height = glutGet(GLUT_WINDOW_HEIGHT);
    printf("%d %d\n", width, height);
    movieData_[id].img.alloc(width, height);
    glReadPixels(0, 0, width-1, height-1,
		 GL_RGB, // format
		 GL_UNSIGNED_BYTE, // type
		 movieData_[id].img.data);
    printf("savepicture\n");
    static char filename[255];
    getMovieBaseName(screen, filename);
    strcat(filename, ".bmp");
    BmpBitmap::save(movieData_[id].img, filename, true, true);
#endif // VIEWER_DISACTIVATED 
}
