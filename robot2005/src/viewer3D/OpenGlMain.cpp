/**
 * @file OpenGlMain
 * 
 * This is a sample to test the Viewer3D
 */

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string>

#include "robotTypes.h"
#include "viewer3D.h"
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"

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

int main(int argc, char **argv) {
    SquatchBall sBalls[BALLE_SQUATCH_NBR];
    GRSBall gBalls[BALLE_GRS_NBR];
    Skittle skittles[QUILLE_NBR];
    LOG_INFO("-- Coupe de Robotique 2005 -- \n"); 
 
    Viewer3D->createWindows(true, true);

    // definition des robots
    Viewer3D->setRobotModel(0, "Robot 0", ROBOT_MODEL_ATTACK, false);
    Viewer3D->setRobotModel(1, "Robot 1", ROBOT_MODEL_DEFENCE, false);
    Viewer3D->setRobotModel(2, "Robot 2", ROBOT_MODEL_ATTACK, false);
    Viewer3D->setRobotModel(3, "Robot 3", ROBOT_MODEL_DEFENCE, false);

    // objets fixes
    Viewer3D->setBridgePosition(BRIDGE_POS_BORDURE);
    Point support1(450, 450);
    Point support2(750, 1050);
    Point support3(1050, 750);
    Viewer3D->setSupportPosition(support1, support2);
    
    // quilles
    createSkittlePacket(&(skittles[0]), support1,
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
    createSkittlePacket(&(skittles[4]), Point(TERRAIN_X, TERRAIN_Y)-support1, 
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
    createSkittlePacket(&(skittles[8]), support2,
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, true);
    createSkittlePacket(&(skittles[12]), Point(TERRAIN_X, TERRAIN_Y)-support2, 
			TERRAIN_SUPPORT_QUILLE_HAUTEUR, true, false);
    createSkittlePacket(&(skittles[16]), support3,
			0, true, true);
    createSkittlePacket(&(skittles[20]), Point(TERRAIN_X, TERRAIN_Y)-support3, 
			0, true, false);
    createSkittlePacket(&(skittles[24]), Point(1522, 1050-75),
			0, false, true);
    createSkittlePacket(&(skittles[27]), Point(TERRAIN_X, TERRAIN_Y)-Point(1522, 1050-75), 
			0, false, false);
    Viewer3D->setSkittlePosition(skittles);
    
    // balles de grs
    gBalls[0].center = Point(1522, 1050+75);
    gBalls[1].center = Point(TERRAIN_X-1522, 1050-75);
    Viewer3D->setGRSBallsPosition(gBalls);
    
    // balles de squatch
    Viewer3D->setSquatchBallsPosition(sBalls);
    
    int x=200, y=200;
    double t=M_PI/4;
    Millisecond time=0;
    Millisecond deltaTime=20;
    while(x<3000) {
        Viewer3D->setRobotPosition(0, x,y,t);
        Viewer3D->setRobotPosition(1, x,TERRAIN_Y-y,t);
        x+=2; t+=0.02;
        usleep(20000);
        time+=deltaTime;
        Viewer3D->setTime(time);
    }
    sleep(30);
}
