#include "devices/implementation/motorOdom05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/motorOdomCom_05.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"

MotorOdom05::MotorOdom05() : 
    RobotDeviceCL("MotorOdom05", CLASS_MOTOR_ODOM), device_(NULL),
    motorPosLeft_(0), motorPosRight_(0),
    odomPosLeft_(0), odomPosRight_(0)
{
    device_ = IoManager->getIoDevice(IO_ID_MOTOR_ODOM_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for MotorOdom05 failed.\n");
	}
    } else {
        LOG_ERROR("MotorOdom05 device not found!\n");
    } 
}

MotorOdom05::~MotorOdom05()
{
    if (device_) device_->close();
}

/** Reset les hctl (gauche et droite) */
bool MotorOdom05::reset()
{
    return idle();
}

/** Desasservit les moteurs */
bool MotorOdom05::idle()
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

/** Defini la constante d'acceleration des moteurs */
bool MotorOdom05::setAcceleration(MotorAcceleration acceleration)
{
    if (!device_) return false; 
    bool status=false;
    // TODO
    return status;
}

/** Specifie un consigne en vitesse pour les moteurs */
bool MotorOdom05::setSpeed(MotorSpeed left, 
                           MotorSpeed right)
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

/** Retourne la position des codeurs */
bool MotorOdom05::getMotorPosition(MotorPosition &left,
                                   MotorPosition &right)
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

/** Retourne la position des codeurs */
bool MotorOdom05::getOdomPosition(CoderPosition &left,
                                  CoderPosition &right)
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

/** Retourne la consigne reellement envoyee au moteurs */
bool MotorOdom05::getMotorPwm(MotorPWM &left,
                              MotorPWM &right)
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

bool MotorOdom05::setSpeedAndCachePosition(MotorSpeed left, 
                                           MotorSpeed right)
{
    if (!device_) return false;
    bool status=false;
    // TODO
    return status;
}

void MotorOdom05::getCacheMotorPosition(MotorPosition &mLeft,
                                        MotorPosition &mRight)
{
    mLeft  = motorPosLeft_;
    mRight = motorPosRight_;
}

void MotorOdom05::getCacheOdomPosition(CoderPosition &oLeft,
                                       CoderPosition &oRight)
{
    oLeft  = odomPosLeft_;
    oRight = odomPosRight_;
}

void MotorOdom05::getCacheMotorPwm(MotorPWM &pLeft,
                                   MotorPWM &pRight)
{
    pLeft  = motorPwmLeft_;
    pRight = motorPwmRight_;
}

#ifdef TEST_MAIN

#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "io/serialPort.h"

struct termios confterm;

typedef struct MotorOrder {
    int speedRight;
    int speedLeft;
    MotorOrder(): speedRight(0), speedLeft(0){}
} MotorOrder;

MotorOrder order;
bool useCache_=false;

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
    printf("\nMotorOdom05 -- serial port motor&odom interface\n");
    printf("Usage:\n");
    printf("  Space : emergency stop\n");
    printf("  Use arrows to move the robot\n"); 
    printf("  p: get motor&odom Pos\n");
    printf("  c: use cache or not\n");
    printf("  w: get motor PWM\n");
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

bool getKeyboardOrder(MotorOdom05& motor)
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
  case 'p' :
  case 'P' :
      { 
    MotorPosition posLeft, posRight;
    CoderPosition codLeft, codRight;
    if (useCache_) {
        motor.getCacheMotorPosition(posLeft, posRight);
        motor.getCacheOdomPosition(codLeft, codRight);
    } else {
        motor.getMotorPosition(posLeft, posRight);
        motor.getOdomPosition(codLeft, codRight);
    }
    printf("Position: motor: l=%d,\tr=%d\t odometer:l=%d,\tr=%d\n", 
           posLeft, posRight, codLeft, codRight);
      }
    break; 
  case 'w' :
  case 'W' :
    MotorPWM pwmLeft, pwmRight;
    if (useCache_) {
        motor.getCacheMotorPwm(pwmLeft, pwmRight); 
    } else {
        motor.getMotorPwm(pwmLeft, pwmRight); 
    }
    printf("PWM: left=%d,\tright=%d\n",pwmLeft, pwmRight);
    break;    
  case 'c' :
  case 'C' :
      useCache_=!useCache_;
      printf("useCache=%s\n", useCache_?"TRUE":"FALSE");
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
  if (useCache_) {
      motor.setSpeedAndCachePosition(order.speedLeft, order.speedRight);
  } else {
      motor.setSpeed(order.speedLeft, order.speedRight);
  }
  return true;
}

MotorOdom05 * pMotor=NULL;
void motorRealTelecommandSIGINT(int sig) {
  if (pMotor) pMotor->reset();
  usleep(10000);
  exit(-1);
}

int main() {
  showUsage();
  configureKeyboard();
  (void) signal(SIGINT, motorRealTelecommandSIGINT);

  IoManager->submitIoHost(new SerialPort(0, false));
  IoManager->submitIoHost(new SerialPort(1, false));

  MotorOdom05 motor;
  pMotor=&motor;

  motor.reset();
  while(getKeyboardOrder(motor));
  motor.reset();

  pMotor = NULL;
  printf("Bye\n"); 
}

#endif // TEST_MAIN
