/**
 * @file simulator.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */
#include <unistd.h>
#include <string>

#include "simulator.h"
#include "simulatorServer.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"
#include "simulatorSkittle.h"
#include "viewer3D.h"
#define LOG_CLASSID CLASS_SIMULATOR
#include "log.h"
#ifdef USE_FTHREAD
#include "robotFThread.h"
#else  // USE_FTHREAD
#include "mthread.h"
#endif // USE_FTHREAD

SimulatorCL* SimulatorCL::simulator_=NULL;
#ifdef USE_FTHREAD
ft_thread_t threadUpdatePos;
#else
MThreadId threadUpdatePos;
#endif
// ===========================================================================
// Constructeurs
// ===========================================================================

// ---------------------------------------------------------------------------
// simulatorThread
// ---------------------------------------------------------------------------
#ifdef USE_FTHREAD
void simulatorThread(void*)
#else
void* simulatorThread(void*)
#endif 
{
    LOG_FUNCTION();
    while(1) {
        SimulatorCL::instance()->update();
        SimulatorCL::instance()->draw();
#ifdef USE_FTHREAD
        ft_thread_cooperate_n(100);
#else // USE_FTHREAD
        usleep(1000);
#endif// USE_FTHREAD
    }
    LOG_WARNING("simulatorThread stopped\n");
#ifndef USE_FTHREAD
    return NULL;
#endif
}

// ---------------------------------------------------------------------------
// SimulatorCL::SimulatorCL
// ---------------------------------------------------------------------------
SimulatorCL::SimulatorCL() : 
    RobotBase("Simulator", CLASS_SIMULATOR), 
    server_(NULL), bridge_(BRIDGE_POS_BORDURE), 
    supportPosition1_(450, 450), 
    supportPosition2_(750, 1050), 
    supportPosition3_(1050, 750), 
    supportConfigId_(0)
{
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      sgBalls_[i] = new SimulatorGrsBall();
      sgBalls_[i]->set(&gBalls_[i]);
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      sskittles_[i] = new SimulatorSkittle();
      sskittles_[i]->set(&skittles_[i]);
    }
    setBridgeBorder();
    setWallBorder();
    server_ = new SimulatorServer();
    if (server_) server_->startReceiver();
    Viewer3D->createWindows(true, true);
    registerViewerBtnCB();
    reset();
    // Start thread simulatorThread() 
#ifdef USE_FTHREAD
    threadUpdatePos = ft_thread_create(ftThread::getScheduler(),
				       simulatorThread,
				       NULL, // cleanup
				       NULL);// parameters
#else
    MTHREAD_CREATE("Main simulator thread",
		   &threadUpdatePos,
		   NULL,
		   simulatorThread, 
		   NULL);
#endif
    LOG_OK("SimulatorCL started\n");
}

// ---------------------------------------------------------------------------
// SimulatorCL::~SimulatorCL
// ---------------------------------------------------------------------------
SimulatorCL::~SimulatorCL() 
{
#ifdef USE_FTHREAD
    ft_scheduler_stop(threadUpdatePos);
#endif
    if (server_) delete server_;
    delete Viewer3D;
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      delete sgBalls_[i];
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      delete sskittles_[i];
    } 
}

// ---------------------------------------------------------------------------
// SimulatorCL::instance
// ---------------------------------------------------------------------------
SimulatorCL* SimulatorCL::instance() 
{
    if (!simulator_) simulator_ = new SimulatorCL();
    return simulator_;
}

// ---------------------------------------------------------------------------
// SimulatorCL::getRobot
// ---------------------------------------------------------------------------
SimulatorRobot* SimulatorCL::getRobot(int robotId)
{
    if (server_) {
	return server_->getRobot(robotId);
    } else {
	return NULL;
    }
}

// ===========================================================================
// Utilitaires pour les clients pour savoir s'il y a des collisions
// ===========================================================================

// ---------------------------------------------------------------------------
// SimulatorCL::getObstacleDistance
// ---------------------------------------------------------------------------
Millimeter SimulatorCL::getObstacleDistance(SimulatorRobot* robot,
                                            Millimeter rPosCaptor, 
                                            Radian dirPosCaptor,
                                            Millimeter zPosCaptor,
                                            Radian dirCaptor)
{
    return INFINITE_DIST;
}

// ---------------------------------------------------------------------------
// SimulatorCL::isCollision
// ---------------------------------------------------------------------------
bool SimulatorCL::isCollision(SimulatorRobot* robot,
                              Millimeter rPosCaptorPt1, 
                              Radian dirPosCaptorPt1,
                              Millimeter rPosCaptorPt2, 
                              Radian dirPosCaptorPt2,
                              Millimeter zPosCaptor)
{
    return false;
}

// ---------------------------------------------------------------------------
// SimulatorCL::getGroundDistance
// ---------------------------------------------------------------------------
Millimeter SimulatorCL::getGroundDistance(SimulatorRobot* robot,
					  Millimeter rPosCaptor, 
					  Radian dirPosCaptor,
					  Millimeter zPosCaptor)
{
    return 0;
}
  
// ===========================================================================
// Position des objets sur la table
// ===========================================================================

// ---------------------------------------------------------------------------
// SimulatorCL::changeBridge
// ---------------------------------------------------------------------------
void SimulatorCL::changeBridge()
{
    bridge_ = (BridgePosition)((1+bridge_)%(BRIDGE_POS_CENTER+1));
    if (bridge_ == BRIDGE_POS_UNKNOWN) bridge_ = BRIDGE_POS_BORDURE;
    Viewer3D->setBridgePosition(bridge_);
    setBridgeBorder();
}

// ---------------------------------------------------------------------------
// SimulatorCL::changeSupport
// ---------------------------------------------------------------------------
void SimulatorCL::changeSupport()
{
    int x1=robotRand(0,3);
    int y1=4;
    int x2=0, y2=0, x3, y3;
    if (x1==1) y1=3;
    if (x1==2) y1=2;
    y1=robotRand(0,y1);
    supportPosition1_ = Point(450+x1*300, 450+y1*300);
    // position du 2e support de quille
    if (x1==1 && y1==1) supportPosition2_ = Point(450, 1350);
    else {
        bool ok=false;
        do {
            x2 = robotRand(0,3);
            y2 = 4;
            if (x2==1) y2=3;
            if (x2==2) y2=2;
            y2 = robotRand(0,y2);
            ok = (abs(x1-x2)>1 || abs(y1-y2)>1);
        } while(!ok);
        supportPosition2_ = Point(450+x2*300, 450+y2*300);
    }
    // position des quilles restantes
    bool ok=false;
    do {
        x3 = robotRand(0,3);
        y3 = 4;
        if (x3==1) y3=3;
        if (x3==2) y3=2;
        y3 = robotRand(0,y3);
        ok = (x1!=x3 || y1!=y3) && (x2!=x3 || y2!=y3);
    } while(!ok);
    supportPosition3_ = Point(450+x3*300, 450+y3*300);

    // support de quilles
    Viewer3D->setSupportPosition(supportPosition1_, supportPosition2_);
    
    // quilles
    createSkittlePacket(&(skittles_[0]), supportPosition1_,
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
    createSkittlePacket(&(skittles_[4]), Point(TERRAIN_X, TERRAIN_Y)-supportPosition1_, 
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
    createSkittlePacket(&(skittles_[8]), supportPosition2_,
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
    createSkittlePacket(&(skittles_[12]), Point(TERRAIN_X, TERRAIN_Y)-supportPosition2_, 
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
    createSkittlePacket(&(skittles_[16]), supportPosition3_,
			0, true, true);
    createSkittlePacket(&(skittles_[20]), Point(TERRAIN_X, TERRAIN_Y)-supportPosition3_, 
			0, true, false);
    createSkittlePacket(&(skittles_[24]), Point(1522, 1050-75),
			0, false, true);
    createSkittlePacket(&(skittles_[27]), Point(TERRAIN_X, TERRAIN_Y)-Point(1522, 1050-75), 
			0, false, false);
    Viewer3D->setSkittlePosition(skittles_);
} 

// ---------------------------------------------------------------------------
// SimulatorCL::createSkittlePacket
// ---------------------------------------------------------------------------
void SimulatorCL::createSkittlePacket(Skittle* skittles,
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
// SimulatorCL::reset
// ---------------------------------------------------------------------------
void SimulatorCL::reset()
{
    // envoie l'ordre de reset aux robots
    if (server_) {
        for(unsigned int i=0;i<SIMU_PORT_MAX_CONNECTIONS; i++) {
            SimulatorRobot* robot = getRobot(i);
            if (robot) {
                robot->setMatchStatusReset();
            }
        }
    }

    // pont
    Viewer3D->setBridgePosition(bridge_);
    
    // balles de grs
    gBalls_[0].center = Point(1522, 1050+75);
    gBalls_[1].center = Point(TERRAIN_X-1522, 1050-75);
    Viewer3D->setGRSBallsPosition(gBalls_);
    
    // balles de squatch
    Viewer3D->setSquatchBallsPosition(sBalls_);
}

// ===========================================================================
// Mise a jour de la position des objets et calcul des interactions
// ===========================================================================

// ---------------------------------------------------------------------------
// SimulatorCL::update
// ---------------------------------------------------------------------------
void SimulatorCL::update()
{
  if (server_) {
    // calcul la nouvelle position des objets
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot = server_->getRobot(i);
      if (robot) {
	robot->updatePosition();
      }
    }
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      sgBalls_[i]->updatePosition();
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      sskittles_[i]->updatePosition();
    }
    // verifie que la position des objets n'entre pas en collision avec le terrain
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot = server_->getRobot(i);
      if (robot) {
	robot->checkPosAndWall();
        robot->checkPosAndBridge(bridge_);
      }
    } 
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      sgBalls_[i]->checkPosAndWall();
      sgBalls_[i]->checkPosAndBridge(bridge_);
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      sskittles_[i]->checkPosAndWall();
      sskittles_[i]->checkPosAndBridge(bridge_);
    } 
    // verifie la position des robots
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot1 = server_->getRobot(i);
      if (robot1) {
	for(unsigned int j=i; j<SIMU_PORT_MAX_CONNECTIONS; j++) {
	  SimulatorRobot* robot2 = server_->getRobot(j);
	  if (robot2) {
	    robot1->checkPosAndOtherRobot(robot2);
	  }
	}
      }
    }
    // verifie la position des balles de grs
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot = server_->getRobot(i);
      if (robot) {
	for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
	  robot->checkPosAndGRSBall(sgBalls_[i]);
	}
      }
    }
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      for(unsigned int j=0;j!=BALLE_GRS_NBR;j++) {
	sgBalls_[i]->checkPosAndGRSBall(sgBalls_[j]);
      }
    }
    // verifie la position des quilles
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot = server_->getRobot(i);
      if (robot) {
	for(unsigned int i=0;i!=QUILLE_NBR;i++) {
	  robot->checkPosAndSkittle(sskittles_[i]);
	}
      }
    }
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      for(unsigned int j=0;j!=QUILLE_NBR;j++) {
	sgBalls_[i]->checkPosAndSkittle(sskittles_[j]);
      }
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      for(unsigned int j=0;j!=QUILLE_NBR;j++) {
	sskittles_[i]->checkPosAndSkittle(sskittles_[j]);
      }
    }
    // met a jour la position reelle
    for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
      SimulatorRobot* robot = server_->getRobot(i);
      if (robot) {
	robot->setNewPositionValid();
      }
    }
    for(unsigned int i=0;i!=BALLE_GRS_NBR;i++) {
      sgBalls_[i]->setNewPositionValid();
    }
    for(unsigned int i=0;i!=QUILLE_NBR;i++) {
      sskittles_[i]->setNewPositionValid();
    } 
  }
}

// ===========================================================================
// Affichage
// ===========================================================================

// ---------------------------------------------------------------------------
// SimulatorCL::draw
// ---------------------------------------------------------------------------
void SimulatorCL::draw()
{
    if (server_) {
	for(unsigned int i=0; i<SIMU_PORT_MAX_CONNECTIONS; i++) {
	    SimulatorRobot* robot = server_->getRobot(i);
	    if (robot) {
		Position pos;
		robot->getRobotRealPosition(pos.center, pos.direction);
		Viewer3D->setRobotPosition(i, pos);
		if (robot->needSendDisplayInfo()) {
		    Viewer3D->setRobotModel(i, 
					    robot->getRobotName(), 
					    robot->getRobotModel(),
                                            robot->isModeBrick());
		    Viewer3D->setRobotLcd(i, robot->getLcdMessage());
		    robot->displayInfoSent();
		}
	    } else {
		Viewer3D->removeRobot(i);
	    }
	}
    }
}

// ===========================================================================
// Callbacks des boutons du playerControl
// ===========================================================================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void playerBtnClickedCB(ViewerControlButtonId btnId) 
{
    switch(btnId) {
    case CTRL_BTN_BRIDGE:
        SimulatorCL::instance()->changeBridge();
        break;
    case CTRL_BTN_SUPPORT:
        SimulatorCL::instance()->changeSupport();
        break;
    case CTRL_BTN_PLAY:
        SimulatorCL::instance()->reset();
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void mainEmergencyStopClickedCB(ViewerControlButtonId btnId) 
{
    Viewer3D->setBtnClick(CTRL_BTN_R0_AU, true);
    Viewer3D->setBtnClick(CTRL_BTN_R1_AU, true);
    Viewer3D->setBtnClick(CTRL_BTN_R2_AU, true);
    Viewer3D->setBtnClick(CTRL_BTN_R3_AU, true);
}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void mainEmergencyStopUnClickedCB(ViewerControlButtonId btnId) 
{
    Viewer3D->setBtnClick(CTRL_BTN_R0_AU, false);
    Viewer3D->setBtnClick(CTRL_BTN_R1_AU, false);
    Viewer3D->setBtnClick(CTRL_BTN_R2_AU, false);
    Viewer3D->setBtnClick(CTRL_BTN_R3_AU, false);
}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void mainJackinClickedCB(ViewerControlButtonId btnId) 
{
    Viewer3D->setBtnClick(CTRL_BTN_R0_JACK, true);
    Viewer3D->setBtnClick(CTRL_BTN_R1_JACK, true);
    Viewer3D->setBtnClick(CTRL_BTN_R2_JACK, true);
    Viewer3D->setBtnClick(CTRL_BTN_R3_JACK, true);
}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void mainJackinUnClickedCB(ViewerControlButtonId btnId) 
{
    Viewer3D->setBtnClick(CTRL_BTN_R0_JACK, false);
    Viewer3D->setBtnClick(CTRL_BTN_R1_JACK, false);
    Viewer3D->setBtnClick(CTRL_BTN_R2_JACK, false);
    Viewer3D->setBtnClick(CTRL_BTN_R3_JACK, false);
}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void robotBtnClickedCB(ViewerControlButtonId btnId)
{
    SimulatorRobot* robot=NULL;
    if (btnId<CTRL_BTN_R0_JACK) return;
    robot=SimulatorCL::instance()->getRobot((btnId-CTRL_BTN_R0_JACK)/CTRL_BTN_NBR_PER_ROBOT);
    if (!robot) return;
    switch(btnId) {
    case CTRL_BTN_R0_JACK:
    case CTRL_BTN_R1_JACK:
    case CTRL_BTN_R2_JACK:
    case CTRL_BTN_R3_JACK:
	robot->setJackin(true);
	break;
    case CTRL_BTN_R0_AU:
    case CTRL_BTN_R1_AU:
    case CTRL_BTN_R2_AU:
    case CTRL_BTN_R3_AU:
	robot->setEmergencyStop(true);
	break;
    case CTRL_BTN_R0_YES:
    case CTRL_BTN_R1_YES:
    case CTRL_BTN_R2_YES:
    case CTRL_BTN_R3_YES:
	robot->setLcdButtonsYes(true);
	break;
    case CTRL_BTN_R0_NO:
    case CTRL_BTN_R1_NO:
    case CTRL_BTN_R2_NO:
    case CTRL_BTN_R3_NO:
	robot->setLcdButtonsYes(false);
	break;
    default:
        break;
    }
}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void robotBtnUnClickedCB(ViewerControlButtonId btnId)
{
    SimulatorRobot* robot=NULL;
    if (btnId<CTRL_BTN_R0_JACK) return;
    robot=SimulatorCL::instance()->getRobot((btnId-CTRL_BTN_R0_JACK)/CTRL_BTN_NBR_PER_ROBOT);
    if (!robot) return;
    switch(btnId) {
    case CTRL_BTN_R0_JACK:
    case CTRL_BTN_R1_JACK:
    case CTRL_BTN_R2_JACK:
    case CTRL_BTN_R3_JACK:
	robot->setJackin(false);
	break;
    case CTRL_BTN_R0_AU:
    case CTRL_BTN_R1_AU:
    case CTRL_BTN_R2_AU:
    case CTRL_BTN_R3_AU:
	robot->setEmergencyStop(false);
	break;
    case CTRL_BTN_R0_YES:
    case CTRL_BTN_R1_YES:
    case CTRL_BTN_R2_YES:
    case CTRL_BTN_R3_YES:
	robot->setLcdButtonsYes(false);
	break;
    case CTRL_BTN_R0_NO:
    case CTRL_BTN_R1_NO:
    case CTRL_BTN_R2_NO:
    case CTRL_BTN_R3_NO:
	robot->setLcdButtonsYes(false);
	break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// SimulatorCL::registerViewerBtnCB
// ---------------------------------------------------------------------------
void SimulatorCL::registerViewerBtnCB()
{
    Viewer3D->registerBtnCallback(CTRL_BTN_ROBOTS_JACK,
				mainJackinClickedCB,
				mainJackinUnClickedCB);
    Viewer3D->registerBtnCallback(CTRL_BTN_ROBOTS_AU,
				mainEmergencyStopClickedCB,
				mainEmergencyStopUnClickedCB);
    Viewer3D->registerBtnCallback(CTRL_BTN_BRIDGE,
                                  playerBtnClickedCB);
    Viewer3D->registerBtnCallback(CTRL_BTN_SUPPORT,
                                  playerBtnClickedCB);
    Viewer3D->registerBtnCallback(CTRL_BTN_PLAY,
                                  playerBtnClickedCB);
    for(int i = CTRL_BTN_R0_JACK; i<CTRL_BTN_R3_NO; i++) {
	Viewer3D->registerBtnCallback((ViewerControlButtonId)i,
				    robotBtnClickedCB,
				    robotBtnUnClickedCB);
    }
}

void SimulatorCL::setBridgeBorder()
{
    Millimeter x1=(TERRAIN_X-TERRAIN_PONT_LONGUEUR)/2;
    Millimeter x2=(TERRAIN_X+TERRAIN_PONT_LONGUEUR)/2;
    // pont du milieu
    bridgeXPts_[0]=Point(x1, TERRAIN_Y/2-TERRAIN_CASE_LARGEUR/2);
    bridgeXPts_[1]=Point(x2, bridgeXPts_[1].y);
    bridgeXPts_[2]=Point(x1,TERRAIN_Y/2);
    bridgeXPts_[3]=Point(x2, bridgeXPts_[3].y);
    bridgeXPts_[4]=Point(x1,TERRAIN_Y/2+TERRAIN_CASE_LARGEUR/2);
    bridgeXPts_[5]=Point(x2, bridgeXPts_[5].y);
    // pont du haut
    switch(bridge_) {
    case BRIDGE_POS_UNKNOWN:
        bridgeXPts_[6]=Point(x1,TERRAIN_Y*2); // out of terrain
        riverYPts_[0]=Point(x1, 0);
        riverYPts_[1]=Point(x1, TERRAIN_Y/2-TERRAIN_CASE_LARGEUR/2);
        riverYPts_[2]=Point(x1, TERRAIN_Y);
        riverYPts_[3]=Point(x1, TERRAIN_Y/2+TERRAIN_CASE_LARGEUR/2);
        riverYPts_[4]=Point(x1, 2*TERRAIN_Y);// out of terrain
        riverYPts_[5]=Point(x1, 3*TERRAIN_Y);
        riverYPts_[6]=Point(x1, 4*TERRAIN_Y);
        riverYPts_[7]=Point(x1, 5*TERRAIN_Y);
        break;
    case BRIDGE_POS_BORDURE: 
        bridgeXPts_[6]=Point(x1,0); // out of terrain
        riverYPts_[0]=Point(x1, TERRAIN_PONT_LARGEUR);
        riverYPts_[1]=Point(x1, TERRAIN_Y/2-TERRAIN_CASE_LARGEUR/2);
        riverYPts_[2]=Point(x1, TERRAIN_Y-riverYPts_[0].y);
        riverYPts_[3]=Point(x1, TERRAIN_Y-riverYPts_[1].y);
        riverYPts_[4]=Point(x1, 2*TERRAIN_Y);// out of terrain
        riverYPts_[5]=Point(x1, 3*TERRAIN_Y);
        riverYPts_[6]=Point(x1, 4*TERRAIN_Y);
        riverYPts_[7]=Point(x1, 5*TERRAIN_Y);
        break;  
    case BRIDGE_POS_MIDDLE_BORDURE: 
        bridgeXPts_[6]=Point(x1,TERRAIN_PONT_LARGEUR*0.5); // out of terrain
        riverYPts_[0]=Point(x1, TERRAIN_PONT_LARGEUR+bridgeXPts_[6].y);
        riverYPts_[1]=Point(x1, TERRAIN_Y/2-TERRAIN_CASE_LARGEUR/2);
        riverYPts_[2]=Point(x1, TERRAIN_Y-riverYPts_[0].y);
        riverYPts_[3]=Point(x1, TERRAIN_Y-riverYPts_[1].y);
        riverYPts_[4]=Point(x1, 0);// out of terrain
        riverYPts_[5]=Point(x1, bridgeXPts_[6].y);
        riverYPts_[6]=Point(x1, TERRAIN_Y-riverYPts_[4].y);
        riverYPts_[7]=Point(x1, TERRAIN_Y-riverYPts_[5].y);
        break;  
    case BRIDGE_POS_MIDDLE_CENTER: 
        bridgeXPts_[6]=Point(x1,TERRAIN_PONT_LARGEUR); // out of terrain
        riverYPts_[0]=Point(x1, TERRAIN_PONT_LARGEUR+bridgeXPts_[6].y);
        riverYPts_[1]=Point(x1, TERRAIN_Y/2-TERRAIN_CASE_LARGEUR/2);
        riverYPts_[2]=Point(x1, TERRAIN_Y-riverYPts_[0].y);
        riverYPts_[3]=Point(x1, TERRAIN_Y-riverYPts_[1].y);
        riverYPts_[4]=Point(x1, 0);// out of terrain
        riverYPts_[5]=Point(x1, bridgeXPts_[6].y);
        riverYPts_[6]=Point(x1, TERRAIN_Y-riverYPts_[4].y);
        riverYPts_[7]=Point(x1, TERRAIN_Y-riverYPts_[5].y);
        break;  
    case BRIDGE_POS_CENTER:
        bridgeXPts_[6]=Point(x1,TERRAIN_PONT_LARGEUR*1.5); // out of terrain
        riverYPts_[0]=Point(x1, TERRAIN_PONT_LARGEUR*1.5);
        riverYPts_[1]=Point(x1, 0);
        riverYPts_[2]=Point(x1, TERRAIN_Y-riverYPts_[0].y);
        riverYPts_[3]=Point(x1, TERRAIN_Y-riverYPts_[1].y);
        riverYPts_[4]=Point(x1, 2*TERRAIN_Y);// out of terrain
        riverYPts_[5]=Point(x1, 3*TERRAIN_Y);
        riverYPts_[6]=Point(x1, 4*TERRAIN_Y);
        riverYPts_[7]=Point(x1, 5*TERRAIN_Y);
        break;
    }
    for(unsigned int i=8;i!=SIMU_RIVER_BORDER_PTS_NBR;i++) {
        riverYPts_[i]=Point(x2, riverYPts_[i-8].y);
    }
    bridgeXPts_[7]=Point(x2, bridgeXPts_[6].y);
    bridgeXPts_[8]=Point(x1, TERRAIN_Y-bridgeXPts_[6].y);
    bridgeXPts_[9]=Point(x2, TERRAIN_Y-bridgeXPts_[6].y);
    bridgeXPts_[10]=Point(x1, bridgeXPts_[6].y+TERRAIN_PONT_LARGEUR);
    bridgeXPts_[11]=Point(x2, bridgeXPts_[10].y);
    bridgeXPts_[12]=Point(x1, TERRAIN_Y-bridgeXPts_[10].y);
    bridgeXPts_[13]=Point(x2, TERRAIN_Y-bridgeXPts_[10].y);
}   
void SimulatorCL::setWallBorder()
{
    wallBorderPts_[0]=Point(0,0);
    wallBorderPts_[1]=Point(TERRAIN_X,0);
    wallBorderPts_[2]=Point(TERRAIN_X,TERRAIN_Y);
    wallBorderPts_[3]=Point(0,TERRAIN_Y);
}
// ===========================================================================
// main
// ===========================================================================
int main(int argc, char **argv) {
    robotRandInit();
    SimulatorCL::instance();
#ifdef USE_FTHREAD
    ft_scheduler_start(ftThread::getScheduler());
    
    ftThread::cleanup();
#else
    while(1) {sleep(10);}
#endif
    LOG_INFO("MAIN Cleanup\n");
    delete SimulatorCL::instance();
    return 0;
}
