#include "implementation/motorIsa.h"
#ifdef TELECOMMAND_MAIN
#include <signal.h>
#include <stdlib.h>
#endif
#include "HCTLControl.h"

#include "log.h"
// ============================================================================
// ==============================  class MotorIsa   ==========================
// ============================================================================

/** Reset les hctl (gauche et droite) */
bool MotorIsa::reset()
{
#ifndef TELECOMMAND_MAIN
  LOG_FUNCTION();
  // on previent robot_pos qu'il y a un saut possible dans la valeur des 
  // codeurs avant et apres le reset
  MotorCL::reset();
  hctlSetMotorSpeed(0, 0);
  if (resetCallBack_) resetCallBack_();
  hctlIdle();
  if (resetCallBack_) resetCallBack_();
  hctlInit();
  if (resetCallBack_) resetCallBack_();

#else
  MotorCL::reset();
  hctlSetMotorSpeed(0, 0);
  hctlIdle();
  hctlInit();
#endif
  return true;
}

/** Defini la constante d'acceleration des moteurs */
void MotorIsa::setAcceleration(MotorAcceleration acceleration)
{
  hctlSetAcceleration(acceleration);
}

/** Specifie un consigne en vitesse pour les moteurs */
void MotorIsa::setSpeed(MotorSpeed left, 
			 MotorSpeed right)
{
    // on a change les moteurs et les codeurs, maintenant ca va 2 fois
    // moins vite. Pour pas changer les gains qu'on a teste, on change 
    // juste la consigne envoyee aux moteurs. A corrige en 2005 ;)
  hctlSetMotorSpeed( 2*right, 2*left);
}

/** Retourne la position des codeurs */
void MotorIsa::getPosition(MotorPosition &left,
			    MotorPosition &right)
{
  right=hctlGetRightPos();
  left =hctlGetLeftPos();
}

/** Retourne la consigne reellement envoyee au moteurs */
void MotorIsa::getPWM(MotorPWM &left,
		       MotorPWM &right)
{
  left =hctlGetLeftPWM();
  right=hctlGetRightPWM();
}

/** Desasservit les moteurs */
void MotorIsa::idle()
{
    hctlIdle();
}

/** Constructeur */
MotorIsa::MotorIsa(bool   automaticReset, 
                   MotorAlertFunction fn) : 
    MotorCL(automaticReset, fn), isStarted_(false)
{
}

void MotorIsa::start() 
{
#ifndef TELECOMMAND_MAIN
    LOG_FUNCTION();
#endif
    hctlInit();
//  reset();
#ifndef TELECOMMAND_MAIN
    LOG_OK("Initialisation terminée\n");
#endif  
    isStarted_ = true;
}

MotorIsa::~MotorIsa()
{
  hctlCleanUp();
}






#ifdef TELECOMMAND_MAIN

#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

struct termios confterm;

typedef struct MotorOrder {
    int speedRight;
    int speedLeft;
    MotorOrder(): speedRight(0), speedLeft(0){}
} MotorOrder;

MotorOrder order;

static const int MAX_NORMAL_SPEED=120;

void configureKeyboard()
{
    tcgetattr(0,&confterm);
    confterm.c_lflag = confterm.c_lflag & ~ICANON;
    confterm.c_cc[VMIN]=1;
    tcsetattr(0,TCSANOW,&confterm); 
}
/**
 * Check that motor order is in normal range
 */
void checkMotorOrder()
{
  if (order.speedLeft>MAX_NORMAL_SPEED)   
    order.speedLeft=MAX_NORMAL_SPEED;
  else if (order.speedLeft<-MAX_NORMAL_SPEED)  
    order.speedLeft=-MAX_NORMAL_SPEED;
  if (order.speedRight>MAX_NORMAL_SPEED)  
    order.speedRight=MAX_NORMAL_SPEED;
  else if (order.speedRight<-MAX_NORMAL_SPEED) 
    order.speedRight=-MAX_NORMAL_SPEED;
}
/**
 * Display programm usage
 */
void showUsage()
{
  printf("\nUsage:\n");
  printf("  Space : emergency stop\n");
  printf("  Use arrows to move the robot\n"); 
  printf("  r: reset motor\n");
  printf("  q : exit the application\n"); 
  printf("  h : display this message\n");
  printf("\n");
}

void setEmergencyStopOrder()
{
  order.speedRight=0;
  order.speedLeft =0;
  printf("setSpeed(%d, %d)\n", order.speedLeft, order.speedRight);
}

void handle8()
{
  order.speedLeft +=5;
  order.speedRight+=5;
  printf("setSpeed(%d, %d)\n", order.speedLeft, order.speedRight);
}

void handle2()
{
  order.speedLeft -=5;
  order.speedRight-=5;
  printf("setSpeed(%d, %d)\n", order.speedLeft, order.speedRight);
}

void handle4()
{
  order.speedLeft -=5;
  order.speedRight+=5;
  printf("setSpeed(%d, %d)\n", order.speedLeft, order.speedRight);
}

void handle6()
{
  order.speedLeft +=5;
  order.speedRight-=5;
  printf("setSpeed(%d, %d)\n", order.speedLeft, order.speedRight);
}

bool getKeyboardOrder(MotorIsa& motor)
{
  char rep;
  read(0,&rep,1);
  switch (rep) {
    // motor
  case 27 : 
    read(0,&rep,1); 
    read(0,&rep,1);
    switch (rep) {
      case 'A' : handle8(); break;
      case 'B' : handle2(); break;
      case 'D' : handle4(); break;
      case 'C' : handle6(); break;
      }
    break;
  case '8' : handle8(); break;
  case '2' : handle2(); break;
  case '4' : handle4(); break;
  case '6' : handle6(); break;
    // emergency stop
  case '5' :
  case ' ' : 
    setEmergencyStopOrder();
    break; 
  case 'r' :
  case 'R' : 
    motor.reset();
    order.speedLeft=0;
    order.speedRight=0;
    break;
  case 'c' :
  case 'C' : 
    MotorPosition posLeft, posRight;
    motor.getPosition(posLeft, posRight);
    printf("Coder position: left=%d,\tright=%d\n", posLeft, posRight);
    break; 
  case 'w' :
  case 'W' :
    MotorPWM pwmLeft, pwmRight;
    motor.getPWM(pwmLeft, pwmRight); 
    printf("PWM: left=%d,\tright=%d\n",pwmLeft, pwmRight);
    break;    
  case 'h' :
  case 'H' : 
    showUsage();
    break;  
    // exit
  case 'q': 
  case 'Q':
  case '-':
    return false;
    break;
  default:
      break;
  }
  checkMotorOrder();
  motor.setSpeed(order.speedLeft, order.speedRight);
  return true;
}

MotorIsa * pMotor=NULL;
void motorRealTelecommandSIGINT(int sig) {
  if (pMotor) pMotor->reset();
  usleep(10000);
  exit(-1);
}

int main() {
  showUsage();
  configureKeyboard();
  (void) signal(SIGINT, motorRealTelecommandSIGINT);

  MotorIsa motor;
  motor.start();
  pMotor=&motor;

  motor.reset();
  while(getKeyboardOrder(motor));
  motor.reset();

  pMotor = NULL;
  printf("Bye\n"); 
}

#endif // TELECOMMAND_MAIN
