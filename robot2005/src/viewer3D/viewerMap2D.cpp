#include <stdio.h>
#ifdef linux
#include <GL/glut.h>
#endif
#include <string>
#include "viewerMap2D.h"
#include "viewer3D.h"
#include "text.h"
#include "log.h"

// ---------------------------------------------------------------
// SPECIAL COLORS for the map
// ---------------------------------------------------------------
#define COLOR_TARGET_TRAJECTORY 1.0f, 0.0f, 0.0f
#define COLOR_REAL_TRAJECTORY   0.0f, 1.0f, 0.0f

#define COLOR_BUMPER_OBSTACLE  0.0f,0.0f,1.0f, 0.5f
#define COLOR_ENV_OBSTACLE     0.5f,0.0f,1.0f, 0.5f
#define COLOR_USER_OBSTACLE    1.0f,0.0f,0.7f, 0.5f
#define COLOR_TRAJEC_OBSTACLE  0.5f,0.5f,0.5f, 0.5f 

// ---------------------------------------------------------------
// STATIC
// ---------------------------------------------------------------
static const double SCALE =0.1;
static const int    MARGIN=5;
static int    winId_=0;
static double mouseScaleX_=1, mouseScaleY_=1;
static bool   init_=false;

// ---------------------------------------------------------------
// Viewer3DCL::draw2D
// ---------------------------------------------------------------
// Dessin de la scene en 2D
// ---------------------------------------------------------------
void Viewer3DCL::draw2D()
{
    drawBG2D();
    drawBridges2D();
    drawSupports2D();

    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawTrajectories2D();
    drawSquatchBalls2D();
    drawSkittles2D();
    drawGRSBalls2D();
    drawObstacles2D();
    drawTrajectories2D();
    drawRobots2D();
    drawLegend2D();

    movieScreenShot(VIEWER_SCREEN_MAP);

    glutSwapBuffers();
}

// ---------------------------------------------------------------
// Viewer3DCL::drawRobots2D
// ---------------------------------------------------------------
// Dessin de la scene en 2D
// ---------------------------------------------------------------
void Viewer3DCL::drawRobots2D()
{
    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
        drawRobot2D(i, false);
        if (enableEstimatedDisplay_) drawRobot2D(i, true);
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawRobot2D
// ---------------------------------------------------------------
// Dessine le robot en 2D
// ---------------------------------------------------------------
void Viewer3DCL::drawRobot2D(int robotId, bool estimated)
{
    Position pos = estimated?robotData_[robotId].estimatedPos:robotData_[robotId].pos;
    if (!robotData_[robotId].exist || !isInField(pos.center)) return;
    glPushMatrix();
    robotData_[robotId].color.setGL(estimated?0.5:1.);
    if (robotData_[robotId].isBrick() ||
        robotData_[robotId].getTeam() == TEAM_RED) {
	glTranslatef(MARGIN+pos.center.x*SCALE, 
		     MARGIN+TERRAIN_Y*SCALE-pos.center.y*SCALE,
		     0);
	glRotatef(-r2d(pos.direction),0,0,1);
    } else {
	glTranslatef(MARGIN+(TERRAIN_X-pos.center.x)*SCALE, 
		     MARGIN+pos.center.y*SCALE,
		     0);
	glRotatef(-r2d(M_PI-pos.direction),0,0,1);
    }
    switch(robotData_[robotId].model) {
    case ROBOT_MODEL_ATTACK:
	glBegin(GL_LINE_STRIP);
	glVertex2d(-60*SCALE, 170*SCALE);
	glVertex2d(80*SCALE, 170*SCALE);
	glVertex2d(140*SCALE, 140*SCALE);
	glVertex2d(140*SCALE, -140*SCALE);
	glVertex2d(80*SCALE, -170*SCALE);
	glVertex2d(-60*SCALE, -170*SCALE);
	glVertex2d(-60*SCALE, 170*SCALE);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(140*SCALE, 0*SCALE);
	glVertex2d(-10*SCALE, 50*SCALE);
	glVertex2d(-10*SCALE, -50*SCALE);
	glVertex2d(140*SCALE, 0*SCALE);
	glEnd();
	break;
    case ROBOT_MODEL_DEFENCE:
	glBegin(GL_LINE_STRIP);
	glVertex2d(130*SCALE, 130*SCALE);
	glVertex2d(-130*SCALE, 130*SCALE);
	glVertex2d(-130*SCALE, -130*SCALE);
	glVertex2d(130*SCALE, -130*SCALE);
	glVertex2d(130*SCALE, 130*SCALE);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(130*SCALE, 0*SCALE);
	glVertex2d(-20*SCALE, 50*SCALE);
	glVertex2d(-20*SCALE, -50*SCALE);
	glVertex2d(130*SCALE, 0*SCALE);
	glEnd();
    break;
    default:
	glBegin(GL_LINE_STRIP);
	glVertex2d(150*SCALE, 150*SCALE);
	glVertex2d(-150*SCALE, 150*SCALE);
	glVertex2d(-150*SCALE, -150*SCALE);
	glVertex2d(150*SCALE, -150*SCALE);
	glVertex2d(150*SCALE, 150*SCALE);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(150*SCALE, 0*SCALE);
	glVertex2d(0*SCALE, 50*SCALE);
	glVertex2d(0*SCALE, -50*SCALE);
	glVertex2d(150*SCALE, 0*SCALE);
	glEnd();
    break;
    }
    glPopMatrix();
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBG2D
// ---------------------------------------------------------------
// Dessine le terrain et la salle en 2D 
// ---------------------------------------------------------------
void Viewer3DCL::drawBG2D()
{
    glLoadIdentity();
    glClear (GL_COLOR_BUFFER_BIT);// Reset The Modelview Matrix
    glClear (GL_DEPTH_BUFFER_BIT);// Reset The Modelview Matrix
    int i,j;
    
    // dessin du contour
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_LINE_STRIP);
    glVertex2d(MARGIN, MARGIN);	
    glVertex2d(MARGIN, MARGIN+TERRAIN_Y*SCALE);	
    glVertex2d(MARGIN+TERRAIN_X*SCALE, MARGIN+TERRAIN_Y*SCALE);
    glVertex2d(MARGIN+TERRAIN_X*SCALE, MARGIN);
    glVertex2d(MARGIN, MARGIN);
    glEnd();	
    
    //dessin de la grille
    int caseWidth = (int)(TERRAIN_CASE_LARGEUR*SCALE);
    for(i=0;i<5;++i) {
        for(j=0;j<7;++j) {
            if ((i+j)%2==0) {
                glColor3f(COLOR_YELLOW_GRID_1);
            } else {
                glColor3f(COLOR_YELLOW_GRID_2);
            }
            glBegin(GL_QUADS);
              glVertex2d(MARGIN+i*caseWidth,
                         MARGIN+j*caseWidth);	
              glVertex2d(MARGIN+i*caseWidth,
                         MARGIN+(j+1)*caseWidth);	
              glVertex2d(MARGIN+(i+1)*caseWidth,
                         MARGIN+(j+1)*caseWidth);
              glVertex2d(MARGIN+(i+1)*caseWidth,
                         MARGIN+j*caseWidth);	
            glEnd();

            glBegin(GL_QUADS);
              glVertex2d(MARGIN+TERRAIN_X*SCALE-i*caseWidth,
                         MARGIN+j*caseWidth);	
              glVertex2d(MARGIN+TERRAIN_X*SCALE-i*caseWidth,
                         MARGIN+(j+1)*caseWidth);	
              glVertex2d(MARGIN+TERRAIN_X*SCALE-(i+1)*caseWidth,
                         MARGIN+(j+1)*caseWidth);
              glVertex2d(MARGIN+TERRAIN_X*SCALE-(i+1)*caseWidth,
                         MARGIN+j*caseWidth);	
            glEnd();		
        }
    }

    // ligne blanche avant le fosse
    glColor3f(COLOR_WHITE_LINE);
    glBegin(GL_QUADS);
      glVertex2d(MARGIN+5*caseWidth, 
                 MARGIN);	
      glVertex2d(MARGIN+5*caseWidth+TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN);	
      glVertex2d(MARGIN+5*caseWidth+TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN+TERRAIN_Y*SCALE);	
      glVertex2d(MARGIN+5*caseWidth, 
                 MARGIN+TERRAIN_Y*SCALE);	   
    glEnd();
    glBegin(GL_QUADS);
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth, 
                 MARGIN);	
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth-TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN);	
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth-TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN+TERRAIN_Y*SCALE);	
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth, 
                 MARGIN+TERRAIN_Y*SCALE);	   
    glEnd();

    // fosse
    glColor3f(COLOR_WATER);
    glBegin(GL_QUADS);
      glVertex2d(MARGIN+5*caseWidth+TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN);	
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth-TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN);	
      glVertex2d(MARGIN+TERRAIN_X*SCALE-5*caseWidth-TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN+TERRAIN_Y*SCALE);
      glVertex2d(MARGIN+5*caseWidth+TERRAIN_BORDURE_LARGEUR*SCALE, 
                 MARGIN+TERRAIN_Y*SCALE);	
    glEnd();

    glDisable(GL_DEPTH_TEST);
}
// ---------------------------------------------------------------
// Viewer3DCL::
// ---------------------------------------------------------------
// Dessine toutes les quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSkittles2D()
{
    if (!skittles_) return;
    for(unsigned int i=0; i<QUILLE_NBR; i++) {
        drawSkittle2D(&(skittles_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSkittle2D
// ---------------------------------------------------------------
// Dessine une quille 
// ---------------------------------------------------------------
void Viewer3DCL::drawSkittle2D(Skittle* skittle)
{
    if (!skittle || !isInField(skittle->center)) return;
    if (skittle->color == COLOR_RED) {
        glColor4f(COLOR_SKITTLE_RED, 1.0f);
    } else {
        glColor4f(COLOR_SKITTLE_GREEN, 1.0f);
    }
    if (skittle->status == SKITTLE_DOWN) {
        glPushMatrix ();
        glTranslatef(MARGIN+skittle->center.x*SCALE, 
                     MARGIN+(TERRAIN_Y-skittle->center.y)*SCALE,0); 
        glRotatef(-r2d(skittle->direction)-90,0,0,1);
        glBegin(GL_QUADS);
        glVertex2d(-(TERRAIN_SUPPORT_QUILLE_HAUTEUR/2)*SCALE, 
                   -(TERRAIN_SUPPORT_QUILLE_RAYON)*SCALE);
        glVertex2d(-(TERRAIN_SUPPORT_QUILLE_HAUTEUR/2)*SCALE, 
                   (TERRAIN_SUPPORT_QUILLE_RAYON)*SCALE);
        glVertex2d((TERRAIN_SUPPORT_QUILLE_HAUTEUR/2)*SCALE, 
                   (TERRAIN_SUPPORT_QUILLE_RAYON)*SCALE);
        glVertex2d((TERRAIN_SUPPORT_QUILLE_HAUTEUR/2)*SCALE, 
                   -(TERRAIN_SUPPORT_QUILLE_RAYON)*SCALE);
        glEnd();
        glPopMatrix();
    } else {
        GLUquadric* quad=gluNewQuadric();
        glPushMatrix ();
        glTranslatef(MARGIN+skittle->center.x*SCALE, 
                     MARGIN+(TERRAIN_Y-skittle->center.y)*SCALE,0); 
        gluDisk(quad, 0, QUILLE_RAYON*SCALE, 10,10);
        glPopMatrix();
    
        gluDeleteQuadric(quad);
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawGRSBalls2D
// ---------------------------------------------------------------
// Dessine toutes les balles 
// ---------------------------------------------------------------
void Viewer3DCL::drawGRSBalls2D()
{
    if (!grsBalls_) return;
    for(unsigned int i=0; i<BALLE_GRS_NBR; i++) {
        drawGRSBall2D(&(grsBalls_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawGRSBall2D
// ---------------------------------------------------------------
// Dessine une balle de GRS 
// ---------------------------------------------------------------
void Viewer3DCL::drawGRSBall2D(GRSBall* ball)
{
    if (!isInField(ball->center)) return;
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glTranslatef(MARGIN+ball->center.x*SCALE, 
                 MARGIN+(TERRAIN_Y-ball->center.y)*SCALE,0); 
    glColor4f(COLOR_GRS_BALL, 1.0f);
    gluDisk(quad, 0, BALLE_GRS_RAYON*SCALE, 10,10);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSquatchBalls2D
// ---------------------------------------------------------------
// Dessine toutes les balles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSquatchBalls2D()
{
    if (!squatchBalls_) return;
    for(unsigned int i=0; i<BALLE_SQUATCH_NBR; i++) {
        drawSquatchBall2D(&(squatchBalls_[i]));
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawSquatchBall2D
// ---------------------------------------------------------------
// Dessine une balle de Squatch 
// ---------------------------------------------------------------
void Viewer3DCL::drawSquatchBall2D(SquatchBall* ball)
{
    if (!isInField(ball->center)) return;
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glTranslatef(MARGIN+ball->center.x*SCALE, 
                 MARGIN+(TERRAIN_Y-ball->center.y)*SCALE,0); 
    glColor4f(COLOR_SQUATCH_BALL, 1.0f);
    gluDisk(quad, 0, BALLE_SQUATCH_RAYON*SCALE, 5,5);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBridges2D
// ---------------------------------------------------------------
// Dessine tous les ponts 
// ---------------------------------------------------------------
void Viewer3DCL::drawBridges2D()
{
    Millimeter y=TERRAIN_Y/2 
                 +TERRAIN_BORDURE_PONT_LARGEUR
                 +TERRAIN_PONT_FIXE_LARGEUR/2;
    drawBridge2D(y, false);
    drawBridge2D(TERRAIN_Y-y, false);
    switch(bridge_) {
    case BRIDGE_POS_BORDURE: 
        y = TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.;
        drawBridge2D(y, true);  
        drawBridge2D(TERRAIN_Y-y, true);  
        break;
    case BRIDGE_POS_MIDDLE_BORDURE:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR*0.5;
        drawBridge2D(y, true);  
        drawBridge2D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_MIDDLE_CENTER:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR;
        drawBridge2D(y, true);  
        drawBridge2D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_CENTER:
        y = (TERRAIN_BORDURE_PONT_LARGEUR+TERRAIN_PONT_LARGEUR/2.)
            +TERRAIN_CASE_LARGEUR*1.5;
        drawBridge2D(y, true);  
        drawBridge2D(TERRAIN_Y-y, true); 
        break;
    case BRIDGE_POS_UNKNOWN:
    default:
        break;
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawBridge2D
// ---------------------------------------------------------------
// Dessine un pont 
// ---------------------------------------------------------------
void Viewer3DCL::drawBridge2D(Millimeter y, bool big)
{
    Millimeter largeur=big?TERRAIN_PONT_LARGEUR:TERRAIN_PONT_FIXE_LARGEUR;

    glPushMatrix ();
    glTranslatef(MARGIN+(5*TERRAIN_CASE_LARGEUR+TERRAIN_BORDURE_LARGEUR)*SCALE, 
                 MARGIN+(TERRAIN_Y-(y+largeur/2+TERRAIN_BORDURE_PONT_LARGEUR))*SCALE,
                 0); 

    glColor3f(COLOR_YELLOW_GRID_2);
    glBegin(GL_QUADS);
      glVertex2d(0, 
                 TERRAIN_BORDURE_PONT_LARGEUR*SCALE);	
      glVertex2d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);	
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 (TERRAIN_BORDURE_PONT_LARGEUR)*SCALE); 
    glEnd();
    glColor3f(COLOR_WHITE_BORDER);
    glBegin(GL_QUADS);
      glVertex2d(0, 
                 0);
      glVertex2d(0, 
                 TERRAIN_BORDURE_PONT_LARGEUR*SCALE);
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 TERRAIN_BORDURE_PONT_LARGEUR*SCALE);
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 0);
    glEnd();
    glBegin(GL_QUADS);
      glVertex2d(0, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);
      glVertex2d(0, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 (largeur+2*TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);
      glVertex2d(TERRAIN_PONT_LONGUEUR*SCALE, 
                 (largeur+TERRAIN_BORDURE_PONT_LARGEUR)*SCALE);
    glEnd();

    glPopMatrix();
}

// ---------------------------------------------------------------
// Viewer3DCL::drawSupports2D
// ---------------------------------------------------------------
// Dessine tous les supports de quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSupports2D()
{
    drawSupport2D(support1_);
    drawSupport2D(support2_);
    drawSupport2D(Point(TERRAIN_X, TERRAIN_Y) - support1_);
    drawSupport2D(Point(TERRAIN_X, TERRAIN_Y) - support2_);
}

// ---------------------------------------------------------------
// Viewer3DCL::drawSupport2D
// ---------------------------------------------------------------
// Dessine un support de quilles 
// ---------------------------------------------------------------
void Viewer3DCL::drawSupport2D(Point pt)
{
    if (!isInField(pt)) return;
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix();
    glTranslatef(MARGIN+pt.x*SCALE, 
                 MARGIN+(TERRAIN_Y-pt.y)*SCALE,0); 
    glColor4f(COLOR_WHITE_BORDER, 1.0f);
    gluDisk(quad, 0, TERRAIN_SUPPORT_QUILLE_RAYON*SCALE, 15,15);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
}

// ---------------------------------------------------------------
// Viewer3DCL::drawObstacles2D
// ---------------------------------------------------------------
// Dessine tous les obstacles 
// ---------------------------------------------------------------
void Viewer3DCL::drawObstacles2D()
{
    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
	for(unsigned int t=0; t<OBSTACLE_TYPE_NBR; t++) {
	    ObstacleIter iter(robotData_[i].obstacles_[t]);
	    Obstacle obstacle;
	    while(iter.getNext(obstacle)) {
		drawObstacle2D(&obstacle, i);
	    }
	}
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawObstacle2D
// ---------------------------------------------------------------
// Dessine un obstacle 
// ---------------------------------------------------------------
void Viewer3DCL::drawObstacle2D(Obstacle * obstacle, int robotId)
{
    if (!obstacle || !isInField(obstacle->center)) return;
    
    //glEnable(GL_ALPHA_TEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    switch(obstacle->type) {
    case ENV_OBSTACLE:
        glColor4f(COLOR_ENV_OBSTACLE);
        break;
    case BUMPER_OBSTACLE:
        glColor4f(COLOR_BUMPER_OBSTACLE);
        break;
    case USER_OBSTACLE:
        glColor4f(COLOR_USER_OBSTACLE);
        break;
    case TRAJECTORY_OBSTACLE:
        glColor4f(COLOR_TRAJEC_OBSTACLE);
        break;
    case ALL_OBSTACLES:
    default:
        glColor4f(COLOR_TRAJEC_OBSTACLE);
        break;
    }
    GLUquadric* quad=gluNewQuadric();
    glPushMatrix ();
    glTranslatef(MARGIN+obstacle->center.x*SCALE, 
                 MARGIN+TERRAIN_Y*SCALE-obstacle->center.y*SCALE,0);
    
    gluDisk(quad, 0, obstacle->radius*SCALE, 10,5);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
    //glDisable(GL_ALPHA_TEST);
}
// ---------------------------------------------------------------
// Viewer3DCL::drawTrajectories2D
// ---------------------------------------------------------------
// Dessine toutes les trajectoires des robots 
// ---------------------------------------------------------------
void Viewer3DCL::drawTrajectories2D()
{
    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
        drawTrajectory2D(&(robotData_[i].targetTrajectory), true);
        drawTrajectory2D(&(robotData_[i].currentTrajectory), false);
    }
}
// ---------------------------------------------------------------
// Viewer3DCL::drawTrajectory2D
// ---------------------------------------------------------------
// Dessine une trajectoire 
// ---------------------------------------------------------------
void Viewer3DCL::drawTrajectory2D(Trajectory * t, bool target)
{
    if (!t) return;
    if (target) glColor3f(COLOR_TARGET_TRAJECTORY);
    else glColor3f(COLOR_REAL_TRAJECTORY);
    glBegin(GL_LINE_STRIP);	
    Trajectory::iterator it=t->begin();
    for(;it!=t->end();it++) {
        glVertex2d(MARGIN+(*it).x*SCALE, 
                   TERRAIN_Y*SCALE+MARGIN-(*it).y*SCALE);
    }
    glEnd();
}

// ---------------------------------------------------------------
// Viewer3DCL::drawLegend2D
// ---------------------------------------------------------------
// Dessine la legende 
// ---------------------------------------------------------------
void Viewer3DCL::drawLegend2D() 
{
    int X=0, Y=220;
    int CIRCLE_RAYON=30;
    GLUquadric* quad=gluNewQuadric();
    static ViewerColorST textColorRed(1,0,0,1);
    ViewerText::setColor(textColorRed);
    ViewerText::setSize(10);
   /*ViewerText::draw(5,(int)(TERRAIN_Y*SCALE+4*MARGIN-3),
                "x=%3d, y=%3d, t=%3d, time=%d",
                (int)x/10, (int)y/10, r2d(t)%360, 
                (int)(time_/1000.));*/
    
    X+=100;
    glPushMatrix ();
    glColor4f(COLOR_ENV_OBSTACLE);
    glTranslatef(X,Y-MARGIN,0);
    gluDisk(quad, 0, CIRCLE_RAYON*SCALE, 10,5);
    glPopMatrix();
    ViewerText::draw(X+20,Y-3, "env");
    
    X=5; Y+=2*MARGIN;
    glPushMatrix ();
    glColor4f(COLOR_BUMPER_OBSTACLE);
    glTranslatef(X,Y-MARGIN,0);
    gluDisk(quad, 0, CIRCLE_RAYON*SCALE, 10,5);
    glPopMatrix();
    ViewerText::draw(X+20,Y-2, "Bump");
    
    X+=100;
    glPushMatrix ();
    glColor4f(COLOR_USER_OBSTACLE);
    glTranslatef(X,Y-MARGIN,0);
    gluDisk(quad, 0, CIRCLE_RAYON*SCALE, 10,5);
    glPopMatrix();
    ViewerText::draw(X+20,Y-2, "User");
    
    X+=100;
    glPushMatrix ();
    glColor4f(COLOR_TRAJEC_OBSTACLE);
    glTranslatef(X,Y-MARGIN,0);
    gluDisk(quad, 0, CIRCLE_RAYON*SCALE, 10,5);
    glPopMatrix();
    ViewerText::draw(X+20,Y-2, "Trajec");
  
    gluDeleteQuadric(quad);
}

// ====================================================================
// C API for openGL
// ====================================================================


// --------------------------------------------------------------------
// initViewerMap2D
// --------------------------------------------------------------------
void initViewerMap2D(int winId) 
{
    if (!init_) {
        winId_=winId;
	glEnable(GL_BLEND);
        glShadeModel (GL_SMOOTH);
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        //glutInitDisplayMode( GLUT_RGB );
        glutCreateMenu (menuViewerMap2D);
        //glutAddMenuEntry ("Afficher la zone couverte", 1);
        //glutAddMenuEntry ("Afficher la trajectoire du robot", 2);
        //glutAddMenuEntry ("Afficher la destination du robot", 3);
        glutAttachMenu (GLUT_RIGHT_BUTTON);
        init_=true;
    }
    resetViewerMap2D();
}

// --------------------------------------------------------------------
// resetViewerMap2D
// --------------------------------------------------------------------
void resetViewerMap2D()
{
}

// --------------------------------------------------------------------
// displayViewerMap2D
// --------------------------------------------------------------------
void displayViewerMap2D(void) 
{
    if (winId_==0) return;
    glutSetWindow(winId_);
    Viewer3D->draw2D();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void keyboardViewerMap2D(unsigned char key, int x, int y)
{
    Point pt((mouseScaleX_*x-MARGIN)/SCALE,
	     (MARGIN+TERRAIN_Y*SCALE-mouseScaleY_*y)/SCALE);
  
    printf("Point(%d %d) -> point(%d %d)\n",
	   x, y,(int) pt.x, (int)pt.y);
    switch (key) {
    case 'c':
	Viewer3D->screenShot("viewerMap2D");
	break;
    case 'a':
	break;
    case 'z':
	break;
    case 'e':
	break;
    default:
	break;
    }
}

// --------------------------------------------------------------------
// menuViewerMap2D
// --------------------------------------------------------------------
void menuViewerMap2D(int value)
{
    switch(value) {
    case 1:
        break;
    case 2:
        break;
    case 3:
         break;
    case 4:
        break;
    }
}

// --------------------------------------------------------------------
// reshapeViewerMap2D
// --------------------------------------------------------------------
void reshapeViewerMap2D(int w, int h)
{
    if (winId_==0) return;
  
    glutSetWindow(winId_);

    glViewport(0,0,
               (GLsizei) (w), 
               (GLsizei) (h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(0.0, (GLfloat) 310.f, (GLfloat) 220.f, 1.f);
    glOrtho(0.0f,
            (2*MARGIN+TERRAIN_X*SCALE), 
            (10*MARGIN+TERRAIN_Y*SCALE),
            0.0f,-1.0f,1.0f);
    /* gluLookAt (1500*SCALE, 1050*SCALE, 300, 
	       1500*SCALE, 1050*SCALE, 0, 
	       1, 0, 0); */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mouseScaleX_=(2.*MARGIN+TERRAIN_X*SCALE)/w;
    mouseScaleY_=(8.*MARGIN+TERRAIN_Y*SCALE)/h;
   
}

// --------------------------------------------------------------------
// idleViewerMap2D
// --------------------------------------------------------------------
void idleViewerMap2D(void)
{
    
}

// --------------------------------------------------------------------
// timerViewerMap2D
// --------------------------------------------------------------------
void timerViewerMap2D(int v)
{
 
}
