#include <stdio.h>
#include <GL/glut.h>

#include "viewerPlayer.h"
#include "viewer3D.h"
#include "gltools.h"
#include "texture.h"
#include "text.h"
#include "button.h"

// ==========================================================================
// Player control
// ==========================================================================
ViewerColorST textColorGreen(0,0.9,0,1);
ViewerColorST textColorYellow(1,1,0,1);
ViewerColorST textColorWhite(1,1,1,1);
ViewerColorST textColorBlack(0,0,0,1);

static int winId_=0;
static bool init_=false;
static double scaleX=1;
static double scaleY=1;


// --------------------------------------------------------------------
// Viewer3DCL::drawPlayer
// --------------------------------------------------------------------
void Viewer3DCL::drawPlayer()
{
    glLoadIdentity();
    glClear (GL_COLOR_BUFFER_BIT);// Reset The Modelview Matrix
    glClear (GL_DEPTH_BUFFER_BIT);// Reset The Modelview Matrix
    drawPlayerControls();
    for(unsigned int i=0; i<VIEWER_MAX_ROBOT_NBR; i++) {
	glPushMatrix();
	glTranslatef(0, 120+i*50, 0);
	drawPlayerRobotInfo(i);
	glPopMatrix();
    }
    glutSwapBuffers();
}

// --------------------------------------------------------------------
// Viewer3DCL::drawPlayerControls
// --------------------------------------------------------------------
void Viewer3DCL::drawPlayerControls()
{
// draw buttons
  for(int i=0; i<CTRL_BTN_NBR; i++) {
      if (btn_[i]) btn_[i]->draw();
  }

  // draw timers
  ViewerText::setSize(10);
  ViewerText::setColor(textColorGreen);
  ViewerText::draw(5, 50, "Log Time: %d'%02d\"%03d", 
	      time_/60000, (time_/1000)%60, time_%1000);
  ViewerText::setColor(textColorYellow);
  ViewerText::draw(5, 65, "Match Time: %d'%02d\"%03d", 
	      matchTime_/60000, (matchTime_/1000)%60, matchTime_%1000);
  ViewerText::setColor(textColorWhite);
  ViewerText::draw(5, 80, "%s", title_.c_str());
}

// --------------------------------------------------------------------
// Viewer3DCL::drawPlayerRobotInfo
// --------------------------------------------------------------------
void Viewer3DCL::drawPlayerRobotInfo(int robotId)
{
    // boite lcd
    Texture->apply(TEX_LCD);
    glBegin(GL_QUADS);
    glTexCoord2f( 0.0, 0.0 ); glVertex2d(270, 0);
    glTexCoord2f( 0.0, 1.0 ); glVertex2d(270, 30);
    glTexCoord2f( 1.0, 1.0 ); glVertex2d(170, 30);	
    glTexCoord2f( 1.0, 0.0 ); glVertex2d(170, 0);	
    glEnd(); 
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_STRIP);
      glVertex2d(2, 0);
      glVertex2d(274, 0);
      glVertex2d(274, 48);
      glVertex2d(2, 48);
      glVertex2d(2, 0);
    glEnd();
    for(unsigned int i=0;i<5;i++) {
       if (btn_[CTRL_BTN_R0_JACK+robotId*5+i])	{
	   btn_[CTRL_BTN_R0_JACK+robotId*5+i]->setEnable(robotData_[robotId].exist);
       }
    }
    if (robotData_[robotId].exist) {
	ViewerText::setSize(7);
	ViewerText::setColor(textColorBlack);
	ViewerText::draw(175,14, "%s", robotData_[robotId].lcd.c_str());
	ViewerText::setSize(8);
	ViewerText::setColor(textColorWhite);
	ViewerText::draw(10, 10, "%s", robotData_[robotId].name.c_str());	
    } else {
	glPushMatrix();
	glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
          glVertex2d(3, 0);
	  glVertex2d(273, 0);
	  glVertex2d(273, 47);
	  glVertex2d(3, 47);
	glEnd();
	glPopMatrix();
	ViewerText::setSize(8);
	ViewerText::setColor(textColorWhite);
	ViewerText::draw(10, 10, "%s", "Waiting connection");
    }
}

// --------------------------------------------------------------------
// keyboardPlayerControl
// --------------------------------------------------------------------
void keyboardPlayerControl(unsigned char key, int x, int y)
{ 
  switch (key) {
  default:
    break;
  }
}

// --------------------------------------------------------------------
// menuPlayerControl
// --------------------------------------------------------------------
void menuPlayerControl(int value)
{
  switch(value) {
  default:
    break;
  }
}

// --------------------------------------------------------------------
// reshapePlayerControl
// --------------------------------------------------------------------
void reshapePlayerControl(int w, int h)
{
  glViewport(0,0,
	     (GLsizei) (w), 
	     (GLsizei) (h)); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  scaleX=276./w;
  scaleY=320./h;
  glOrtho(0.0f, scaleX*w, scaleY*h, 0.0f,-1.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// ---------------------------------------------------------------------------
// displayPlayerControl
// ---------------------------------------------------------------------------
void displayPlayerControl() 
{
  if (winId_==0) return;
  glutSetWindow(winId_);

  Viewer3D->drawPlayer();
}

// --------------------------------------------------------------------
// mousePlayerControl
// --------------------------------------------------------------------
void Viewer3DCL::mousePlayerControl(int button, int state, int x, int y)
{
  int i=0;
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      for(i=0;i<CTRL_BTN_NBR;i++) {
	  if (btn_[i]) btn_[i]->checkClick((int)(x*scaleX), (int)(y*scaleY));
      }
    } else {
      for(i=0;i<CTRL_BTN_NBR;i++) {
	  if (btn_[i]) btn_[i]->checkUnclick((int)(x*scaleX), (int)(y*scaleY));
      }
    }
  }
}

void mousePlayerControl(int button, int state, int x, int y)
{
    Viewer3D->mousePlayerControl(button, state, x, y);
}

// --------------------------------------------------------------------
// initPlayerControl
// --------------------------------------------------------------------
void initPlayerControl(int winId) 
{
  if (!init_) {
    winId_=winId; 
    glEnable(GL_BLEND);
    glShadeModel (GL_SMOOTH);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    init_=true;
  }
  resetPlayerControl();
}

// --------------------------------------------------------------------
// resetPlayerControl
// --------------------------------------------------------------------
void resetPlayerControl() 
{

}
