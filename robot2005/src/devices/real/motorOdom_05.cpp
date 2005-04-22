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
    LOG_FUNCTION();
    return device_->write(MOTOR_ODOM_REQ_RESET);
}

/** Defini la constante d'acceleration des moteurs */
bool MotorOdom05::setAcceleration(MotorAcceleration acceleration)
{
    if (!device_) return false; 
    unsigned char buf[2];
    unsigned int l=2;
    buf[0] = MOTOR_ODOM_REQ_SET_ACCELERATION;
    buf[1] = acceleration;
    return device_->write(buf, l);
}

/** Specifie un consigne en vitesse pour les moteurs */
bool MotorOdom05::setSpeed(MotorSpeed left, 
                           MotorSpeed right)
{
    if (!device_) return false;
    static unsigned char buf[3];
    unsigned int l=3;
    buf[0] = MOTOR_ODOM_REQ_SET_SPEED;
    buf[1] = left;
    buf[2] = right;
    return device_->write(buf, l);
}

/** Retourne la position des codeurs */
bool MotorOdom05::getMotorPosition(MotorPosition &left,
                                   MotorPosition &right)
{
    if (!device_) return false;
    static unsigned char buf[8];
    unsigned int l=6;
    if (device_->writeRead(MOTOR_ODOM_REQ_GET_HCTL_CODER, buf, l)) {
        left  = (((int)buf[0])<<16)+(((int)buf[1])<<8)+(int)buf[2];
        right = (((int)buf[3])<<16)+(((int)buf[4])<<8)+(int)buf[5];
        return true;
    } else {
        return false;
    }
}

/** Retourne la position des codeurs */
bool MotorOdom05::getOdomPosition(CoderPosition &left,
                                  CoderPosition &right)
{
    if (!device_) return false;
    static unsigned char buf[8];
    unsigned int l=4;
    if (device_->writeRead(MOTOR_ODOM_REQ_GET_ODOM_CODER, buf, l)) {
        left  = (((int)buf[0])<<8)+(int)buf[1];
        right = (((int)buf[2])<<8)+(int)buf[3];
        return true;
    } else {
        return false;
    }
}

/** Retourne la consigne reellement envoyee au moteurs */
bool MotorOdom05::getMotorPwm(MotorPWM &left,
                              MotorPWM &right)
{
    if (!device_) return false;
    static unsigned char buf[8];
    unsigned int l=2;
    if (device_->writeRead(MOTOR_ODOM_GET_PWM, buf, l)) {
        left  = buf[0];
        right = buf[1];
        return true;
    } else {
        return false;
    }
}

bool MotorOdom05::setSpeedAndCachePosition(MotorSpeed left, 
                                           MotorSpeed right)
{
    if (!device_) return false;
    static unsigned char buf[8];
    unsigned int l=3;
    buf[0] = MOTOR_ODOM_REQ_SET_SPEED_AND_GET_POS;
    buf[1] = left;
    buf[2] = right;

    static unsigned char buf2[20];
    unsigned int l2=13;
    if (device_->writeRead(buf, l, buf2, l2)) {
        unsigned char checksum=buf[0]; 
        for(int k=1; k<12;k++)  checksum ^= buf[k];
        if (checksum != buf[13]) {
	  //bad checksum, flush the buffer!
	  l2=20;
	  device_->read(buf, l2);
	  LOG_ERROR("Checksum error!\n");
	} else {
	  odomPosLeft_   = (((int)buf2[0])<<8)+(int)buf2[1];
	  odomPosRight_  = (((int)buf2[1])<<8)+(int)buf2[3];
	  motorPosLeft_  = (((int)buf2[4])<<16)+(((int)buf2[5])<<8)+(int)buf2[6];
	  motorPosRight_ = (((int)buf2[7])<<16)+(((int)buf2[8])<<8)+(int)buf2[9];
	  motorPwmLeft_  = buf2[10];
	  motorPwmRight_ = buf2[11];
	}
#ifdef TEST_MAIN
	LOG_DEBUG("ol(0x2.2x 0x2.2x) or(0x2.2x 0x2.2x) "
		  "pl(0x2.2x0x2.2x 0x2.2x) pr(0x2.2x 0x2.2x 0x2.2x) "
		  "wl=0x2.2x, wr=0x2.2x\n",
		  buf2[0], buf2[1], 
		  buf2[2], buf2[3], 
		  buf2[4], buf2[5], buf2[6],
		  buf2[7], buf2[8], buf2[9],
		  buf2[10], buf2[11]);
#endif
        return true;
    } else {
        LOG_ERROR("Com Error send%d/3, received:%d/12\n", l, l2);
        return false;
    }
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
    return true;
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

  IoManager->submitIoHost(new SerialPort(0));
  IoManager->submitIoHost(new SerialPort(1));

  MotorOdom05 motor;
  pMotor=&motor;

  motor.reset();
  while(getKeyboardOrder(motor));
  motor.reset();

  pMotor = NULL;
  printf("Bye\n"); 
}

#endif // TEST_MAIN
