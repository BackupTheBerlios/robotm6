#include <string>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "mthread.h"

#ifndef M_PI
#define M_PI (3.14)
#define M_PI_2 (M_PI/2.)
#define M_3_PI_2 (M_PI_2*3.)
#endif

#include "viewerMap3D.h"
#include "viewer3D.h"
#include "texture.h"

#define SCALE3D 0.1

Millimeter Viewer3DCL::getGroundLevel(Point const& pt)
{
    if (pt.x<(TERRAIN_X-TERRAIN_PONT_LONGUEUR)/2) return 0;
    if (pt.x>(TERRAIN_X+TERRAIN_PONT_LONGUEUR)/2) return 0;
    if (pt.y<(TERRAIN_Y-TERRAIN_PONT_FIXE_LARGEUR)/2 && 
	pt.y>(TERRAIN_Y+TERRAIN_PONT_FIXE_LARGEUR)/2) return 0;
    switch(bridge_) {
    case BRIDGE_POS_BORDURE:  
	if (pt.y < (TERRAIN_PONT_LARGEUR) || 
	    pt.y > (TERRAIN_Y-TERRAIN_PONT_LARGEUR)) 
	    return -TERRAIN_RIVIERE_PROFONDEUR;
	break;      
    case BRIDGE_POS_MIDDLE_BORDURE:
	if ((pt.y > TERRAIN_CASE_LARGEUR/2 &&
	     pt.y < TERRAIN_CASE_LARGEUR/2 + TERRAIN_PONT_LARGEUR) || 
	    (pt.y < TERRAIN_Y - TERRAIN_CASE_LARGEUR/2 &&
	     pt.y > TERRAIN_Y - TERRAIN_CASE_LARGEUR/2 - TERRAIN_PONT_LARGEUR))
	    return -TERRAIN_RIVIERE_PROFONDEUR;
	break;
    case BRIDGE_POS_MIDDLE_CENTER:
	if ((pt.y > TERRAIN_CASE_LARGEUR &&
	     pt.y < TERRAIN_CASE_LARGEUR + TERRAIN_PONT_LARGEUR) || 
	    (pt.y < TERRAIN_Y - TERRAIN_CASE_LARGEUR &&
	     pt.y > TERRAIN_Y - TERRAIN_CASE_LARGEUR - TERRAIN_PONT_LARGEUR))
	    return -TERRAIN_RIVIERE_PROFONDEUR;
	break;
    case BRIDGE_POS_CENTER:
	if ((pt.y > 1.5*TERRAIN_CASE_LARGEUR &&
	     pt.y < TERRAIN_Y - 1.5*TERRAIN_CASE_LARGEUR))
	    return -TERRAIN_RIVIERE_PROFONDEUR;
	break;
    default:
        break;
    }
    return 0;
}

// =========================================================================
// CAMERA
// =========================================================================
namespace {
    GLdouble xEye=0*SCALE3D, yEye=-1000*SCALE3D, zEye=1000*SCALE3D;
    GLdouble xCenter=1500*SCALE3D, yCenter=1050*SCALE3D, zCenter=0*SCALE3D; 
    GLdouble xUp=0.1, yUp=0.1, zUp=1, dirR=M_PI_2;
    int W, H;
#define ZNEAR 1
#define ZFAR 10000
#define FOV_START 60
    int FOVY = FOV_START;
    GLdouble dAspect=1;

    int winId_=0;
    bool init_=false;
}

// -----------------------------------------------------------------------
// setCameraTargetPosition3D -- Eyes direction
// -----------------------------------------------------------------------
// Defini le robot que la camera suit (-1 = camera regarde le centre du 
// terrain)
// -----------------------------------------------------------------------
void Viewer3DCL::setCameraTarget3D(int robotId) 
{
    targetRobotId3D_ = robotId;
}

// -----------------------------------------------------------------------
// setCameraTargetPosition3D -- Eyes direction
// -----------------------------------------------------------------------
// La camera choisi automatiquement le meilleur angle de vue 
// -----------------------------------------------------------------------
void Viewer3DCL::changeCameraAuto3D()
{
    cameraModeAuto3D_ = !cameraModeAuto3D_;
}

// -----------------------------------------------------------------------
// setCameraTargetPosition3D -- Eyes direction
// -----------------------------------------------------------------------
// Active les textures dans la vue 3D 
// -----------------------------------------------------------------------
void Viewer3DCL::changeTexture3D()
{
    useTexture_ = !useTexture_;
}

// -----------------------------------------------------------------------
// setCameraTargetPosition3D -- Eyes direction
// -----------------------------------------------------------------------
void Viewer3DCL::setCameraTargetPosition3D(Millimeter cx, 
                                           Millimeter cy, 
                                           Millimeter cz)
{
    xCenter = cx*SCALE3D;
    yCenter = cy*SCALE3D;
    zCenter = cz*SCALE3D;
    
    if(xEye != xCenter){
        dirR = atan((yEye-yCenter)/(xEye-xCenter));
        if(xCenter<xEye) dirR += M_PI;
    }
    else{
        if(yCenter>yEye) dirR = M_PI_2;
        else dirR = M_PI_2*3;
    }
} // setTargetPosition

// -------------------------------------------------------------------------
// Viewer3DCL::setCameraPosition3D
// -------------------------------------------------------------------------
void Viewer3DCL::setCameraPosition3D(Millimeter cx, 
                                     Millimeter cy, 
                                     Millimeter cz)
{
    xEye = cx*SCALE3D;
    yEye = cy*SCALE3D;
    zEye = cz*SCALE3D;
} // setTargetPosition

// -------------------------------------------------------------------------
// Viewer3DCL::cameraPositionRotate3D
// -------------------------------------------------------------------------
void Viewer3DCL::cameraPositionRotate3D(int mode)
{
    double d=sqrt((xEye-xCenter)*(xEye-xCenter)+(yEye-yCenter)*(yEye-yCenter));
    switch(mode) {
    case 0:
        dirR+=M_PI/16.;
        xEye = xCenter+d*cos(dirR+M_PI);
        yEye = yCenter+d*sin(dirR+M_PI);
        break;
    case 1:
        dirR-=M_PI/16.;
        xEye = xCenter+d*cos(dirR+M_PI);
        yEye = yCenter+d*sin(dirR+M_PI);
        break;
    case 2:
        d*=1.1;
        xEye = xCenter+d*cos(dirR+M_PI);
        yEye = yCenter+d*sin(dirR+M_PI);
        break;
    case 3:
        d*=0.8;
        xEye = xCenter+d*cos(dirR+M_PI);
        yEye = yCenter+d*sin(dirR+M_PI);
        break;
    case 4:
        dirR+=M_PI/16.;
        xCenter = xEye +d*cos(dirR);
        yCenter = yEye +d*sin(dirR);
        setCameraTargetPosition3D(xCenter/SCALE3D, yCenter/SCALE3D, zCenter/SCALE3D);
        break;
    case 5:
        dirR-=M_PI/16.;
        xCenter = xEye +d*cos(dirR);
        yCenter = yEye +d*sin(dirR);
        setCameraTargetPosition3D(xCenter/SCALE3D, yCenter/SCALE3D, zCenter/SCALE3D);
        break;
    case 6:
        d*=1.1;
        xCenter = xEye +d*cos(dirR);
        yCenter = yEye +d*sin(dirR);
        setCameraTargetPosition3D(xCenter/SCALE3D, yCenter/SCALE3D, zCenter/SCALE3D);
        break;
    case 7:
        d*=0.8;
        xCenter = xEye +d*cos(dirR);
        yCenter = yEye +d*sin(dirR);
        setCameraTargetPosition3D(xCenter/SCALE3D, yCenter/SCALE3D, zCenter/SCALE3D);
        break;
    
    }
} // setTargetPosition

// -------------------------------------------------------------------------
// Viewer3DCL::cameraNearestCorner3D
// -------------------------------------------------------------------------
void Viewer3DCL::cameraNearestCorner3D() 
{
    Point c1(-300,-300);
    Point c2(TERRAIN_X+300,-300);
    Point c3(TERRAIN_X+300,TERRAIN_Y+300);
    Point c4(-300,TERRAIN_Y+300);
    Point c5(-300,TERRAIN_Y/2);
    Point c6(TERRAIN_X/2,-300);
    Point c7(TERRAIN_X/2,2400);
    Point c8(3300,1050);
    Point t(xCenter/SCALE3D, yCenter/SCALE3D);
    
    Millimeter dist1=t.dist(c1);
    xEye=c2.x*SCALE3D;
    yEye=c2.y*SCALE3D;
    zEye=700*SCALE3D;
    Millimeter bestDist=dist1;
    
    dist1=t.dist(c2);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c2.x*SCALE3D;
        yEye=c2.y*SCALE3D;
    }
    
    dist1=t.dist(c3);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c3.x*SCALE3D;
        yEye=c3.y*SCALE3D;
    }
    
    dist1=t.dist(c4);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c4.x*SCALE3D;
        yEye=c4.y*SCALE3D;
    }
    
    dist1=t.dist(c5);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c5.x*SCALE3D;
        yEye=c5.y*SCALE3D;
    }
    
    dist1=t.dist(c6);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c6.x*SCALE3D;
        yEye=c6.y*SCALE3D;
    }
    
    dist1=t.dist(c7);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c7.x*SCALE3D;
        yEye=c7.y*SCALE3D;
    }
    
    dist1=t.dist(c8);
    if (dist1<bestDist) {
        bestDist=dist1;
        xEye=c8.x*SCALE3D;
        yEye=c8.y*SCALE3D;
    }
    setCameraPosition3D(xEye/SCALE3D, yEye/SCALE3D, zEye/SCALE3D);
}

// =========================================================================
// Dessin de la scene
// =========================================================================

// ---------------------------------------------------------------
// Viewer3DCL::draw3D
// ---------------------------------------------------------------
// Dessin de la scene en 3D
// ---------------------------------------------------------------
void Viewer3DCL::draw3D()
{
    if (!cameraModeAuto3D_) {
//	cameraNearestCorner3D(); 
        if (targetRobotId3D_<0 || targetRobotId3D_>VIEWER_MAX_ROBOT_NBR) {
            setCameraTargetPosition3D(TERRAIN_X/2, TERRAIN_Y/2, 0); 
        } else {
            if (!isInField(robotData_[targetRobotId3D_].pos.center)) {
                setCameraTargetPosition3D(TERRAIN_X/2, TERRAIN_Y/2, 0);
            } else {
                if (robotData_[targetRobotId3D_].isBrick() || 
                    robotData_[targetRobotId3D_].getTeam() == TEAM_RED) {
                    setCameraTargetPosition3D(robotData_[targetRobotId3D_].pos.center.x,
                                              robotData_[targetRobotId3D_].pos.center.y,
                                              0);
                } else {
                    setCameraTargetPosition3D(TERRAIN_X-robotData_[targetRobotId3D_].pos.center.x,
                                              TERRAIN_Y-robotData_[targetRobotId3D_].pos.center.y,
                                              0);
                }
            }
        }
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOVY, dAspect, ZNEAR, ZFAR);
    gluLookAt (xEye, yEye, zEye, 
	       xCenter, yCenter, zCenter, 
	       cos(dirR), sin(dirR), 0);
    //gluLookAt (xEye, yEye, zEye, xCenter, yCenter, zCenter, xUp, yUp, zUp);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();
    
    drawBG3D();
    drawBridges3D();
    drawSupports3D();

    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawSquatchBalls3D();
    drawSkittles3D();
    drawGRSBalls3D();
    drawRobots3D();
    drawLegend3D();

    movieScreenShot(VIEWER_SCREEN_3D);

    glutSwapBuffers();
}

// ---------------------------------------------------------------
// Viewer3DCL::drawRobots3D
// ---------------------------------------------------------------
// Dessin de la scene en 3D
// ---------------------------------------------------------------
void Viewer3DCL::drawRobots3D()
{
    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
        drawRobot3D(i);
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawRobot3D
// ---------------------------------------------------------------
// Dessine le robot en 3D
// ---------------------------------------------------------------
void Viewer3DCL::drawRobot3D(int robotId)
{
    if (!robotData_[robotId].exist || 
	!isInField(robotData_[robotId].pos.center)) { 
	return;
    }
    glPushMatrix();
    if (robotData_[robotId].isBrick() || 
        robotData_[robotId].getTeam() == TEAM_RED) {
	glTranslatef(robotData_[robotId].pos.center.x*SCALE3D, 
		     robotData_[robotId].pos.center.y*SCALE3D, 
		     0);
	glRotatef(r2d(robotData_[robotId].pos.direction), 
		  0, 
		  0,
		  1);
    } else {
	glTranslatef((TERRAIN_X-robotData_[robotId].pos.center.x)*SCALE3D, 
		     (TERRAIN_Y-robotData_[robotId].pos.center.y)*SCALE3D, 
		     0);
	glRotatef(r2d(M_PI-robotData_[robotId].pos.direction), 
		  0, 
		  0,
		  1);
    }
    glScalef(SCALE3D, SCALE3D, SCALE3D);
    if (robotData_[robotId].dead) {
        // le robot est tombe dans la riviere
        if(robotData_[robotId].dead == 1) {
            glRotatef(-15, 1, 0, 0);
        } else if(robotData_[robotId].dead == 2) {
            glRotatef(15, 1, 0, 0);
        } else {
            glRotatef(24, 0, 1, 0);
        } 
    }
    switch(robotData_[robotId].model) {
    case ROBOT_MODEL_ATTACK:
	drawRobot3DAttack(robotData_[robotId].color);
	break;
    case ROBOT_MODEL_DEFENCE:
    default:
	drawRobot3DDefence(robotData_[robotId].color);
	break;
    }
    glPopMatrix();
}
void Viewer3DCL::drawRobot3DAttack(ViewerColorST& color)
{
    Millimeter L=170, L2=140;
    Millimeter HR=-60, HF=80, HFF=140;
    Millimeter Z=400;

     glColor4f(1,1,1,1);
        Texture->apply(TEX_ATTACK_SIDE_R1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HF, -L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HF, -L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR, -L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HR, -L, Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_SIDE_L1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HR, L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR, L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HF, L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HF, L, Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_SIDE_R2);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HFF, -L2, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HFF, -L2, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HF,  -L,  0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HF,  -L,  Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_SIDE_L2);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HF,  L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HF,  L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HFF, L2, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HFF, L2, Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_SIDE_F);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HFF, L2, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HFF, L2, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HFF,-L2,  0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HFF,-L2,  Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_SIDE_R);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HR, -L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR, -L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR,  L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HR,  L, Z);		
        glEnd();
        Texture->apply(TEX_ATTACK_TOP);
        glBegin(GL_POLYGON);
        glTexCoord2f( 0.1, 0.0 ); glVertex3f(HFF, -L2, Z);
        glTexCoord2f( 0.0, 0.4 ); glVertex3f(HF,  -L,  Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR,  -L,  Z);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR,   L,  Z);
        glTexCoord2f( 1.0, 0.4 ); glVertex3f(HF,   L,  Z);
        glTexCoord2f( 0.9, 0.0 ); glVertex3f(HFF,  L2, Z);		
        glEnd();
        Texture->disable();
    color.setGL();	
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glTranslatef(0,0,450);
    gluSphere(quad, 30, 9,4);
    glPopMatrix(); 
    
    if (useTexture_) {
        glPushMatrix();
	glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,0.3); 
        glBegin(GL_QUADS);
         glVertex3f(HFF+50, L, 5);
         glVertex3f(HR+50,  L, 5);
         glVertex3f(HR+50, -L,  5);
         glVertex3f(HFF+50,-L,  5);		
        glEnd();
	glPopMatrix(); 
    }
}

void Viewer3DCL::drawRobot3DDefence(ViewerColorST& color)
{
    Millimeter L=140;
    Millimeter HR=-140, HF=140;
    Millimeter Z=400;

     glColor4f(1,1,1,1);
        Texture->apply(TEX_DEFENCE_SIDE_R);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HF, -L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HF, -L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR, -L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HR, -L, Z);		
        glEnd();
        Texture->apply(TEX_DEFENCE_SIDE_L);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HR, L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR, L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HF, L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HF, L, Z);		
        glEnd();
        Texture->apply(TEX_DEFENCE_SIDE_F);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HF, L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HF, L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HF,-L,  0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HF,-L,  Z);		
        glEnd();
        Texture->apply(TEX_DEFENCE_SIDE_RR);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HR, -L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR, -L, 0);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR,  L, 0);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(HR,  L, Z);		
        glEnd();
        Texture->apply(TEX_DEFENCE_TOP);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(HF, -L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HR, -L, Z);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(HR,  L, Z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(HF,  L, Z);
        glEnd();
        Texture->disable();
    color.setGL();	
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glTranslatef(0,0,450);
    gluSphere(quad, 30, 9,4);
    glPopMatrix();

    if (useTexture_) {
	glPushMatrix ();
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,0.3); 
        glBegin(GL_QUADS);
         glVertex3f(HF+50, L, 5);
         glVertex3f(HR+50,  L, 5);
         glVertex3f(HR+50, -L,  5);
         glVertex3f(HF+50,-L,  5);		
        glEnd();
	glPopMatrix();
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBG3D
// ---------------------------------------------------------------
// Dessine le terrain et la salle en 3D 
// ---------------------------------------------------------------
void Viewer3DCL::drawBG3D()
{
    // dessin rebord
    glPushMatrix ();
    glEnable(GL_DEPTH_TEST);
    glScalef(SCALE3D, SCALE3D, SCALE3D);
    int i,j;
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);	
    glVertex3d(0,TERRAIN_Y,TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(0,TERRAIN_Y,-4*TERRAIN_BORDURE_HAUTEUR);	
    glVertex3d(TERRAIN_X,TERRAIN_Y,-4*TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(TERRAIN_X,TERRAIN_Y,TERRAIN_BORDURE_HAUTEUR);	
    glEnd();
    glBegin(GL_QUADS);	
    glVertex3d(0,0,TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(0,0,-4*TERRAIN_BORDURE_HAUTEUR);	
    glVertex3d(TERRAIN_X,0,-4*TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(TERRAIN_X,0,TERRAIN_BORDURE_HAUTEUR);	
    glEnd();
    glBegin(GL_QUADS);	
    glVertex3d(TERRAIN_X,TERRAIN_Y,TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(TERRAIN_X,TERRAIN_Y,-4*TERRAIN_BORDURE_HAUTEUR);	
    glVertex3d(TERRAIN_X,0,-4*TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(TERRAIN_X,0,TERRAIN_BORDURE_HAUTEUR);	
    glEnd();
    glBegin(GL_QUADS);	
    glVertex3d(0,TERRAIN_Y,TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(0,TERRAIN_Y,-4*TERRAIN_BORDURE_HAUTEUR);	
    glVertex3d(0,0,-4*TERRAIN_BORDURE_HAUTEUR);
    glVertex3d(0,0,TERRAIN_BORDURE_HAUTEUR);	
    glEnd();

    // dessin damier
    glColor3f(COLOR_YELLOW_GRID_1);
    for(i=0;i<5;++i) {
        for(j=0;j<7;++j) {
            if ((i+j)%2==1) continue;
            glBegin(GL_QUADS);	
              glVertex3d(i*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(i*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);	
              glVertex3d((i+1)*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);
              glVertex3d((i+1)*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);	
            glEnd();
	    glBegin(GL_QUADS);	
              glVertex3d(TERRAIN_X-i*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(TERRAIN_X-i*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);	
              glVertex3d(TERRAIN_X-(i+1)*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(TERRAIN_X-(i+1)*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);	
            glEnd();			
        }
    }
    glColor3f(COLOR_YELLOW_GRID_2);
    for(i=0;i<5;++i) {
        for(j=0;j<7;++j) {
            if ((i+j)%2==0) continue;
            glBegin(GL_QUADS);	
              glVertex3d(i*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(i*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);	
              glVertex3d((i+1)*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);
              glVertex3d((i+1)*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);	
            glEnd();
	    glBegin(GL_QUADS);	
              glVertex3d(TERRAIN_X-i*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(TERRAIN_X-i*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);	
              glVertex3d(TERRAIN_X-(i+1)*TERRAIN_CASE_LARGEUR,
			 (j+1)*TERRAIN_CASE_LARGEUR,0);
              glVertex3d(TERRAIN_X-(i+1)*TERRAIN_CASE_LARGEUR,
			 j*TERRAIN_CASE_LARGEUR,0);	
            glEnd();			
        }
    }
    // ligne blanche avant le fosse
    glColor3f(COLOR_WHITE_LINE);
    glBegin(GL_QUADS);	
    glVertex3d(5*TERRAIN_CASE_LARGEUR, TERRAIN_Y, 0);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, TERRAIN_Y, 0);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, 0, 0);
    glVertex3d(5*TERRAIN_CASE_LARGEUR, 0, 0);	
    glEnd();
    glBegin(GL_QUADS);	
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR), TERRAIN_Y, 0);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), TERRAIN_Y, 0);	
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), 0, 0);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR), 0, 0);	
    glEnd();

    // bordure du fosse
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);	
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, TERRAIN_Y, 0);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, TERRAIN_Y, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, 0, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, 0, 0);	
    glEnd();
    glBegin(GL_QUADS);	
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), TERRAIN_Y, 0);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), TERRAIN_Y, -TERRAIN_RIVIERE_PROFONDEUR);	
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), 0, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), 0, 0);	
    glEnd();

    glColor3f(COLOR_WATER);
    glBegin(GL_QUADS);	
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, TERRAIN_Y, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR, 0, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), 0, -TERRAIN_RIVIERE_PROFONDEUR);
    glVertex3d(TERRAIN_X-(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR), TERRAIN_Y, -TERRAIN_RIVIERE_PROFONDEUR);	
    glEnd();
    
    // dessin des supports de balise 
    /*
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);
    glVertex3d(34, 0, 45);
    glVertex3d(26, 0, 45);
    glVertex3d(26, -8, 45);
    glVertex3d(34, -8, 45);

    glVertex3d(30, 0, 45);
    glVertex3d(30, 0, 0);
    glVertex3d(30, -2, 0);
    glVertex3d(30, -2, 45);

    glVertex3d(34, 75, 45);
    glVertex3d(26, 75, 45);
    glVertex3d(26, 67, 45);
    glVertex3d(34, 67, 45);

    glVertex3d(34, 135, 45);
    glVertex3d(26, 135, 45);
    glVertex3d(26, 143, 45);
    glVertex3d(34, 143, 45);

    glVertex3d(34, 210, 45);
    glVertex3d(26, 210, 45);
    glVertex3d(26, 218, 45);
    glVertex3d(34, 218, 45);

    glVertex3d(30, 210, 45);
    glVertex3d(30, 210, 0);
    glVertex3d(30, 212, 0);
    glVertex3d(30, 212, 45);

    glEnd();

    // dessin des supports de balise 
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);
    glVertex3d(274, 0, 45);
    glVertex3d(266, 0, 45);
    glVertex3d(266, -8, 45);
    glVertex3d(274, -8, 45);

    glVertex3d(270, 0, 45);
    glVertex3d(270, 0, 0);
    glVertex3d(270, -2, 0);
    glVertex3d(270, -2, 45);

    glVertex3d(274, 75, 45);
    glVertex3d(266, 75, 45);
    glVertex3d(266, 67, 45);
    glVertex3d(274, 67, 45);

    glVertex3d(274, 135, 45);
    glVertex3d(266, 135, 45);
    glVertex3d(266, 143, 45);
    glVertex3d(274, 143, 45);

    glVertex3d(274, 210, 45);
    glVertex3d(266, 210, 45);
    glVertex3d(266, 218, 45);
    glVertex3d(274, 218, 45);

    glVertex3d(270, 210, 45);
    glVertex3d(270, 210, 0);
    glVertex3d(270, 212, 0);
    glVertex3d(270, 212, 45);

    glEnd();
    */
    if (useTexture_) {  
        glColor4f(1,1,1,1);
        Texture->apply(TEX_LOGO);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(TERRAIN_X/2+100, -10,  50);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(TERRAIN_X/2+100, -10, -100);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(TERRAIN_X/2-100, -10, -100);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(TERRAIN_X/2-100, -10,  50);		
        glEnd();
        Texture->disable();
  
        Texture->apply(TEX_SALLE_SIDE3);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(-1000, 3500,  2000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(-1000, 3500,  -1000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(-1000, -2000, -1000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(-1000, -2000, 2000);	
        glEnd(); 

        Texture->apply(TEX_SALLE_SIDE1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(4000, -2000, 2000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(4000, -2000, -1000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(4000, 3500,  -1000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(4000, 3500,  2000);	
        glEnd(); 

        Texture->apply(TEX_SALLE_SIDE4);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(4000,  3500, 2000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(4000,  3500, -1000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(-1000, 3500,  -1000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(-1000, 3500,  2000);	
        glEnd();

        Texture->apply(TEX_SALLE_SIDE2);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(-1000,  -2000, 2000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(-1000,  -2000, -1000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(4000, -2000, -1000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(4000, -2000, 2000);	
        glEnd(); 
        Texture->disable(); 

        // texture_->apply(TEX_SALLE_SOL);
        glColor4f(0.43,0.33,0.24,1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(-1000,  3500, -1000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(-1000,  -2000, -1000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(4000, -2000, -1000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(4000, 3500, -1000);	
        glEnd();

	// ombre sous le terrain
        glColor4f(0.40,0.30,0.20,1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(-100,  2000, -990);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(-100,  -100, -990);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(TERRAIN_X-100, -100, -990);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(TERRAIN_X-100, 2000, -990);	
        glEnd(); 
        // Texture->apply(TEX_SALLE_PLAFOND);
        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
        glTexCoord2f( 0.0, 0.0 ); glVertex3d(-1000,  3500, 2000);
        glTexCoord2f( 0.0, 1.0 ); glVertex3d(-1000,  -2000, 2000);
        glTexCoord2f( 1.0, 1.0 ); glVertex3d(4000, -2000, 2000);	
        glTexCoord2f( 1.0, 0.0 ); glVertex3d(4000, 3500, 2000);	
        glEnd(); 


        // pieds de la table
        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
        glVertex3d(100, 120, -20);
        glVertex3d(150, 120, -20);
        glVertex3d(150, 120, -1000);	
        glVertex3d(100, 120, -1000);	

        glVertex3d(120, 100, -20);
        glVertex3d(120, 150, -20);
        glVertex3d(120, 150, -1000);	
        glVertex3d(120, 100, -1000);

        glVertex3d(100, 2020, -20);
        glVertex3d(150, 2020, -20);
        glVertex3d(150, 2020, -1000);	
        glVertex3d(100, 2020, -1000);	

        glVertex3d(120, 2000, -20);
        glVertex3d(120, 2050, -20);
        glVertex3d(120, 2050, -1000);	
        glVertex3d(120, 2000, -1000);


        glVertex3d(1800, 120, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1850, 120, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1850, 120, -1000);	
        glVertex3d(1800, 120, -1000);	

        glVertex3d(1820, 100, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1820, 150, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1820, 150, -1000);	
        glVertex3d(1820, 100, -1000);

        glVertex3d(1800, 2020, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1850, 2020, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1850, 2020, -1000);	
        glVertex3d(1800, 2020, -1000);	

        glVertex3d(1820, 2000, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1820, 2050, -20-TERRAIN_RIVIERE_PROFONDEUR);
        glVertex3d(1820, 2050, -1000);	
        glVertex3d(1820, 2000, -1000);


        glVertex3d(3500, 120, -20);
        glVertex3d(3550, 120, -20);
        glVertex3d(3550, 120, -1000);	
        glVertex3d(3500, 120, -1000);	

        glVertex3d(3520, 100, -20);
        glVertex3d(3520, 150, -20);
        glVertex3d(3520, 150, -1000);	
        glVertex3d(3520, 100, -1000);

        glVertex3d(3500, 2020, -20);
        glVertex3d(3550, 2020, -20);
        glVertex3d(3550, 2020, -1000);	
        glVertex3d(3500, 2020, -1000);	

        glVertex3d(3520, 2000, -20);
        glVertex3d(3520, 2050, -20);
        glVertex3d(3520, 2050, -1000);	
        glVertex3d(5320, 2000, -1000);	
        glEnd(); 
    }

    glPopMatrix ();
}
// ---------------------------------------------------------------
// Viewer3DCL::
// ---------------------------------------------------------------
// Dessine toutes les quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSkittles3D()
{
    if (!skittles_) return;
    for(unsigned int i=0; i<QUILLE_NBR; i++) {
        drawSkittle3D(&(skittles_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSkittle3D
// ---------------------------------------------------------------
// Dessine une quille 
// ---------------------------------------------------------------
void Viewer3DCL::drawSkittle3D(Skittle* skittle)
{
    if (!skittle || !isInField(skittle->center)) return;
#ifndef VIEWER_DISACTIVATED 
    glPushMatrix ();
    GLUquadric* quad=gluNewQuadric();
    glTranslatef(skittle->center.x*SCALE3D, 
		 skittle->center.y*SCALE3D, 
		 skittle->altitude*SCALE3D);
    glScalef(SCALE3D, SCALE3D, SCALE3D);
    switch(skittle->status) {
    case SKITTLE_UP:
	break;
    case SKITTLE_REVERSED:
	glScalef(1, 1, -1);
	break;
    case SKITTLE_DOWN:
	glRotatef(r2d(skittle->direction), 
		  0, 
		  0,
		  1);
        glRotatef(90, 0, 1, 0);
	break;
    }
    if (useTexture_) {
        glColor4f(COLOR_SKITTLE_METAL, 1.0f);
        glPushMatrix ();
        glTranslatef(QUILLE_RAYON/2.,0,0);
        gluCylinder(quad, 0, QUILLE_TVIS_R, 
                    QUILLE_H_TVIS, 10, 1);
        glTranslatef(-5./6.*QUILLE_RAYON,QUILLE_RAYON/2.,0);
        gluCylinder(quad, 0, QUILLE_TVIS_R, 
                    QUILLE_H_TVIS, 10, 1);
        glTranslatef(0,-QUILLE_RAYON,0);
        gluCylinder(quad, 0, QUILLE_TVIS_R, 
                    QUILLE_H_TVIS, 10, 1);
        glPopMatrix();
        glTranslatef(0,0,QUILLE_H_TVIS);
        
        glPushMatrix ();
        glTranslatef(QUILLE_RAYON/2.,0,0);
        gluCylinder(quad, QUILLE_VIS_R, QUILLE_VIS_R, 
                    QUILLE_H_VIS, 10, 1);
        gluDisk(quad, 0, QUILLE_TVIS_R, 10,10);   
        glTranslatef(-5./6.*QUILLE_RAYON,QUILLE_RAYON/2.,0);
        gluCylinder(quad, QUILLE_VIS_R, QUILLE_VIS_R, 
                    QUILLE_H_VIS, 10, 1);
        gluDisk(quad, 0, QUILLE_TVIS_R, 10,10);
        glTranslatef(0,-QUILLE_RAYON,0);
        gluCylinder(quad, QUILLE_VIS_R, QUILLE_VIS_R, 
                    QUILLE_H_VIS, 10, 1);
        gluDisk(quad, 0, QUILLE_TVIS_R, 10,10);
        glPopMatrix();
        glTranslatef(0,0,QUILLE_H_VIS);
        
        if (skittle->color==COLOR_RED) glColor4f(COLOR_SKITTLE_RED, 1.0f);
        else glColor4f(COLOR_SKITTLE_GREEN, 1.0f);
        gluDisk(quad, 0, QUILLE_RAYON, 10,10);   
        gluCylinder(quad, QUILLE_RAYON, QUILLE_RAYON, QUILLE_H_BOIS, 10, 1);
        
        glTranslatef(0,0,QUILLE_H_BOIS);
        glColor4f(COLOR_SKITTLE_REFLECT, 1.0f);
        gluCylinder(quad, QUILLE_RAYON, QUILLE_RAYON, QUILLE_H_REFLE, 10, 1);
        
        glTranslatef(0, 0, QUILLE_H_REFLE);
        if (skittle->color==COLOR_RED) glColor4f(COLOR_SKITTLE_RED, 1.0f);
        else glColor4f(COLOR_SKITTLE_GREEN, 1.0f);
        gluDisk(quad, 0, QUILLE_RAYON, 16,16);
        glPopMatrix();
        if (useTexture_) {
            glPushMatrix();
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0,0,0,0.3); 
            Millimeter z=getGroundLevel(skittle->center);
            if(skittle->altitude>10) {
                z = TERRAIN_SUPPORT_QUILLE_HAUTEUR+5;
            }
            glTranslatef((skittle->center.x-20)*SCALE3D, 
                         (skittle->center.y-20)*SCALE3D, 
                         (z+5)*SCALE3D);
            switch(skittle->status) {
            case SKITTLE_UP:
            case SKITTLE_REVERSED:
                {
                    gluDisk (quad, 0, QUILLE_RAYON*SCALE3D, 15, 15);
                }
                break;
            case SKITTLE_DOWN:
                glRotatef(r2d(skittle->direction), 
                          0, 
                          0,
                          1);
                glBegin(GL_QUADS);
                glVertex3d(QUILLE_RAYON,  0, 0);
                glVertex3d(-QUILLE_RAYON, 0, 0);
                glVertex3d(-QUILLE_RAYON, QUILLE_HAUTEUR, 0);	
                glVertex3d(QUILLE_RAYON,  QUILLE_HAUTEUR, 0);
                glEnd();
                break;
            }
            glPopMatrix();
        }
    } else {
        if (skittle->color==COLOR_RED) glColor4f(COLOR_SKITTLE_RED, 1.0f);
        else glColor4f(COLOR_SKITTLE_GREEN, 1.0f);
        gluDisk(quad, 0, QUILLE_RAYON, 10,10);   
        gluCylinder(quad, QUILLE_RAYON, QUILLE_RAYON, QUILLE_HAUTEUR, 10, 1);
        glTranslatef(0, 0, QUILLE_HAUTEUR);
        gluDisk(quad, 0, QUILLE_RAYON, 10,10);
    }
    gluDeleteQuadric(quad);
    glPopMatrix();
#endif // VIEWER_DISACTIVATED 
}
// ---------------------------------------------------------------
// Viewer3DCL::drawGRSBalls3D
// ---------------------------------------------------------------
// Dessine toutes les balles 
// ---------------------------------------------------------------
void Viewer3DCL::drawGRSBalls3D()
{
    if (!grsBalls_) return;
    for(unsigned int i=0; i<BALLE_GRS_NBR; i++) {
        drawGRSBall3D(&(grsBalls_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawGRSBall3D
// ---------------------------------------------------------------
// Dessine une balle de GRS 
// ---------------------------------------------------------------
void Viewer3DCL::drawGRSBall3D(GRSBall* ball)
{
    if (!isInField(ball->center)) return;
#ifndef VIEWER_DISACTIVATED 
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glColor4f(COLOR_GRS_BALL, 1.0f);
    glTranslatef(ball->center.x*SCALE3D, 
		 ball->center.y*SCALE3D, 
		 ball->altitude*SCALE3D);
    gluSphere(quad, BALLE_GRS_RAYON*SCALE3D, 9,4);
    Millimeter z = getGroundLevel(ball->center);
    z=ball->altitude-z;
    if (useTexture_) {
        glTranslatef(-z*SCALE3D/10,-z*SCALE3D/10,-z*SCALE3D+0.5);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,0.3); 
        gluDisk (quad, 0, BALLE_GRS_RAYON*SCALE3D, 15, 15);
    }
    glPopMatrix();
    gluDeleteQuadric(quad);
#endif // VIEWER_DISACTIVATED 
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSquatchBalls3D
// ---------------------------------------------------------------
// Dessine toutes les balles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSquatchBalls3D()
{
    if (!squatchBalls_) return;
    for(unsigned int i=0; i<BALLE_SQUATCH_NBR; i++) {
        drawSquatchBall3D(&(squatchBalls_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSquatchBall3D
// ---------------------------------------------------------------
// Dessine une balle de Squatch 
// ---------------------------------------------------------------
void Viewer3DCL::drawSquatchBall3D(SquatchBall* ball)
{
    if (!isInField(ball->center)) return;
#ifndef VIEWER_DISACTIVATED 
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glColor4f(COLOR_SQUATCH_BALL, 1.0f);
    glTranslatef(ball->center.x*SCALE3D, 
		 ball->center.y*SCALE3D, 
		 ball->altitude*SCALE3D);
    gluSphere(quad, BALLE_SQUATCH_RAYON*SCALE3D, 9,4);
    if (useTexture_) {
        Millimeter z = getGroundLevel(ball->center);
	z=ball->altitude-z;
	glTranslatef(-z*SCALE3D/10,-z*SCALE3D/10,-z*SCALE3D+0.5);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,0.3); 
        gluDisk (quad, 0, BALLE_SQUATCH_RAYON*SCALE3D, 9, 4);
    }
    glPopMatrix();
    gluDeleteQuadric(quad);
#endif // VIEWER_DISACTIVATED 
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBridges3D
// ---------------------------------------------------------------
// Dessine tous les ponts 
// ---------------------------------------------------------------
void Viewer3DCL::drawBridges3D()
{
    Millimeter y=TERRAIN_Y/2 
                 +TERRAIN_BORDURE_PONT_LARGEUR
                 +TERRAIN_PONT_FIXE_LARGEUR/2;
    drawBridge3D(y, false);
    drawBridge3D(TERRAIN_Y-y, false);
    switch(bridge_) {
    case BRIDGE_POS_BORDURE: 
        y = TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.;
        drawBridge3D(y, true);  
        drawBridge3D(TERRAIN_Y-y, true);  
        break;
    case BRIDGE_POS_MIDDLE_BORDURE:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR*0.5;
        drawBridge3D(y, true);  
        drawBridge3D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_MIDDLE_CENTER:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR;
        drawBridge3D(y, true);  
        drawBridge3D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_CENTER:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR*1.5;
        drawBridge3D(y, true);  
        drawBridge3D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_UNKNOWN:
        break;
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBridge3D
// ---------------------------------------------------------------
// Dessine un pont 
// ---------------------------------------------------------------
void Viewer3DCL::drawBridge3D(Millimeter y, bool big)
{
    Millimeter largeur=big?TERRAIN_PONT_LARGEUR:TERRAIN_PONT_FIXE_LARGEUR;

    glPushMatrix ();
    glTranslatef((5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR)*SCALE3D, 
                 (TERRAIN_Y-(y+largeur/2+TERRAIN_BORDURE_PONT_LARGEUR))*SCALE3D,
                 0); 
    glScalef(SCALE3D, SCALE3D, SCALE3D);
    glColor3f(COLOR_YELLOW_GRID_2);
    glBegin(GL_QUADS);	
      glVertex3d(0, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 0);
      glVertex3d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 0);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 0);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 0);
    glEnd();
    
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);	
      glVertex3d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR) ,
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR),
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
    glEnd();
    glBegin(GL_QUADS);	
      glVertex3d(0, 
                 0, 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (TERRAIN_BORDURE_PONT_LARGEUR),
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 0, 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
    glEnd();

    glBegin(GL_QUADS);	
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (TERRAIN_BORDURE_PONT_LARGEUR),
		 -TERRAIN_RIVIERE_PROFONDEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (TERRAIN_BORDURE_PONT_LARGEUR), 
		 -TERRAIN_RIVIERE_PROFONDEUR);
    glEnd();
    glBegin(GL_QUADS);	
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 0, 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 0, 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0,
		 0,
		 -TERRAIN_RIVIERE_PROFONDEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 0, 
		 -TERRAIN_RIVIERE_PROFONDEUR);
    glEnd();

    glBegin(GL_QUADS);	
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR),
		 -TERRAIN_RIVIERE_PROFONDEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR), 
		 -TERRAIN_RIVIERE_PROFONDEUR);
    glEnd();
    glBegin(GL_QUADS);	
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR), 
		 TERRAIN_BORDURE_PONT_HAUTEUR);
      glVertex3d(0, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR),
		 -TERRAIN_RIVIERE_PROFONDEUR);
      glVertex3d(TERRAIN_PONT_LONGUEUR, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR), 
		 -TERRAIN_RIVIERE_PROFONDEUR);
    glEnd();
    glPopMatrix();
}

// ---------------------------------------------------------------
// Viewer3DCL::drawSupports3D
// ---------------------------------------------------------------
// Dessine tous les supports de quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSupports3D()
{
    drawSupport3D(support1_);
    drawSupport3D(support2_);
    drawSupport3D(Point(TERRAIN_X, TERRAIN_Y) - support1_);
    drawSupport3D(Point(TERRAIN_X, TERRAIN_Y) - support2_);
}

// ---------------------------------------------------------------
// Viewer3DCL::drawSupport3D
// ---------------------------------------------------------------
// Dessine un support de quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSupport3D(Point pt)
{
    if (!isInField(pt)) return;
#ifndef VIEWER_DISACTIVATED 
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glColor4f(COLOR_WHITE_BORDER, 1.0f);
    glTranslatef(pt.x*SCALE3D, 
		 pt.y*SCALE3D, 
		 0*SCALE3D);
    gluCylinder(quad, TERRAIN_SUPPORT_QUILLE_RAYON*SCALE3D, TERRAIN_SUPPORT_QUILLE_RAYON*SCALE3D, 
		TERRAIN_SUPPORT_QUILLE_HAUTEUR*SCALE3D, 16, 1);
    glTranslatef(0, 0, TERRAIN_SUPPORT_QUILLE_HAUTEUR*SCALE3D);
    gluDisk(quad, 0, TERRAIN_SUPPORT_QUILLE_RAYON*SCALE3D, 16,16);
    Millimeter z = TERRAIN_SUPPORT_QUILLE_HAUTEUR;
    if (useTexture_) {
        glTranslatef(-5*z*SCALE3D/10,-5*z*SCALE3D/10,-z*SCALE3D+0.5);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,0.3); 
        gluDisk (quad, 0, TERRAIN_SUPPORT_QUILLE_RAYON*SCALE3D, 15, 15);
    }
    glPopMatrix();
    gluDeleteQuadric(quad);
#endif // VIEWER_DISACTIVATED 
}

// ---------------------------------------------------------------
// Viewer3DCL::drawLegend3D
// ---------------------------------------------------------------
// Dessine la legende 
// ---------------------------------------------------------------
void Viewer3DCL::drawLegend3D() 
{
 
}

// ---------------------------------------------------------------
// Viewer3DCL::keyboardMap3D
// ---------------------------------------------------------------
void Viewer3DCL::keyboardMap3D(unsigned char key, int x, int y)
{
    if (map3DKeyBoardCB_) map3DKeyBoardCB_(VIEWER_SCREEN_3D, key, x, y);
}
// ---------------------------------------------------------------
// Viewer3DCL::registerKeyboardMap3D
// ---------------------------------------------------------------
void Viewer3DCL::registerKeyboardMap3D(ViewerKeyboardFn cb) 
{
    map3DKeyBoardCB_ = cb;
}


// =========================================================================
// C API for openGL
// =========================================================================

// -------------------------------------------------------------------------
// initViewerMap3D
// -------------------------------------------------------------------------
void initViewerMap3D(int winId) {
    if (!init_) {
	winId_=winId;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    
	glShadeModel (GL_SMOOTH);
    
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClear (GL_COLOR_BUFFER_BIT);
	glClear (GL_DEPTH_BUFFER_BIT);
    
	glutCreateMenu (menuViewerMap3D);
	glutAddMenuEntry ("Vue du coin le plus proche", 1);
	glutAddMenuEntry ("Camera automatique/fixe",    2);
	glutAddMenuEntry ("Textures / pas de texture",  3);
	glutAttachMenu (GLUT_RIGHT_BUTTON);
	init_=true;
    }
    resetViewerMap3D();
}

// -------------------------------------------------------------------------
// resetViewerMap3D
// -------------------------------------------------------------------------
void resetViewerMap3D() 
{
  
}

// -------------------------------------------------------------------------
// displayViewerMap3D
// -------------------------------------------------------------------------
void displayViewerMap3D(void) 
{
    if (winId_==0) return;  
    glutSetWindow(winId_);
    Viewer3D->draw3D();
}

// -------------------------------------------------------------------------
// keyboardViewerMap3D
// -------------------------------------------------------------------------
void keyboardViewerMap3D (unsigned char key, int x, int y)
{ 
    switch (key) {
    case '6':
    case 'd':  
	Viewer3D->cameraPositionRotate3D(0);
	glutPostRedisplay();
	break;
    case '4':
    case 'q':  
	Viewer3D->cameraPositionRotate3D(1);
	glutPostRedisplay();
	break;
    case '8':
    case '+':
    case 'z':  
	Viewer3D->cameraPositionRotate3D(3);
	glutPostRedisplay();
	break;
    case '2':
    case '-':
    case 's':  
	Viewer3D->cameraPositionRotate3D(2);
	glutPostRedisplay();
	break;
    case '1':
   	Viewer3D->cameraPositionRotate3D(4);
	glutPostRedisplay();
	break;
    case '3':
   	Viewer3D->cameraPositionRotate3D(5);
	glutPostRedisplay();
	break;
    case '7':
   	Viewer3D->cameraPositionRotate3D(6);
	glutPostRedisplay();
	break;
    case '9':
   	Viewer3D->cameraPositionRotate3D(7);
	glutPostRedisplay();
	break;
    case 'c':  
	Viewer3D->screenShot("viewerMap3d");
	break;
    default:
	break;
    }
    Viewer3D->keyboardMap3D(key, x, y);
}

// -------------------------------------------------------------------------
// menuViewerMap3D
// -------------------------------------------------------------------------
void menuViewerMap3D (int value)
{
    if (value==1) {
	Viewer3D->cameraNearestCorner3D();
    }
    if (value==2) {
        Viewer3D->changeCameraAuto3D();
    }
    if (value==3) {
	Viewer3D->changeTexture3D();
    }
    
}


// -------------------------------------------------------------------------
// reshapeViewerMap3D
// -------------------------------------------------------------------------
void reshapeViewerMap3D(int w, int h)
{
    if (winId_==0) return;
    glutSetWindow(winId_);
    
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(30.0, (GLfloat) w/(GLfloat) h, 1.0, 800.0);
  
    glMatrixMode(GL_MODELVIEW);
}

// -------------------------------------------------------------------------
// idleViewerMap3D
// -------------------------------------------------------------------------
void idleViewerMap3D(void)
{

}

// -------------------------------------------------------------------------
// timerViewerMap3D
// -------------------------------------------------------------------------
void timerViewerMap3D(int v)
{

}


