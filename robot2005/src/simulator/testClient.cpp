#include <unistd.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include "simulatorClient.h"

#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"

struct termios confterm;

// --------------------------------------------------------------------------
// BasicClientMode
// --------------------------------------------------------------------------
typedef enum BasicClientMode {
    MODE_BRICK,  // boite qui peut se deplacer n'importe comment
    MODE_ATTACK, // robot d'attaque
    MODE_DEFENCE // robot de defense
} BasicClientMode;

// --------------------------------------------------------------------------
// struct Order
// --------------------------------------------------------------------------
typedef struct Order {
    MotorSpeed speedRight;
    MotorSpeed speedLeft;
    char motorReset;
    bool resetRobotPos;
    Position pos; // utilise quand mode brick
  
  Order(): speedRight(0), speedLeft(0), motorReset(0), resetRobotPos(false)
    {}

  void reset() {
     motorReset=0;
     resetRobotPos=false;
     speedRight=0;
     speedLeft=0;
  }

  bool operator != (Order const & o) const {
    return (speedRight != o.speedRight) 
        || (speedLeft != o.speedLeft)
        || (motorReset)
        || (resetRobotPos)
        || (pos != o.pos)
        ;
  }
} Order;

// --------------------------------------------------------------------------
// static
// --------------------------------------------------------------------------
static const int MAX_NORMAL_SPEED = 120;
static const Position startPositionBrick(1500,0,0);
static const Position startPositionAttack(400,2000,0);
static const Position startPositionDefence(200,1800,-M_PI/2);

static bool quitApplication=false;
static BasicClientMode mode=MODE_BRICK;

// --------------------------------------------------------------------------
// FullTelecommand::checkMotorOrder
// --------------------------------------------------------------------------
void checkMotorOrder(Order& order)
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

// --------------------------------------------------------------------------
// FullTelecommand::applyOrder
// --------------------------------------------------------------------------
// Send an order to the API robot
// --------------------------------------------------------------------------
void applyOrder(Order& order)
{
    if (order.motorReset) {
        order.speedLeft=0;
        order.speedRight=0;
    } 
    if (order.resetRobotPos) {
         switch(mode) {
         case MODE_BRICK:
             Simulator->setRobotPosition(startPositionBrick);
             break;
         case MODE_ATTACK:
             Simulator->setRobotPosition(startPositionAttack);
             break;
         case MODE_DEFENCE:
             Simulator->setRobotPosition(startPositionDefence);
             break;
         }
    }
    if (mode == MODE_BRICK) {
        Simulator->setRobotPosition(order.pos);
    } else {
        checkMotorOrder(order);
        Simulator->setSpeed(order.speedLeft, order.speedRight);
    }
}


// --------------------------------------------------------------------------
// FullTelecommand::showUsage
// --------------------------------------------------------------------------
void showUsage()
{
    printf("\nUsage:\n");
    printf("  Space : emergency stop\n");
    printf("  Use arrows to move the robot\n"); 
    printf("  r: reset robot Position\n");
    printf("  q : exit the application\n"); 
    printf("  h : display this message\n");
    printf("\n");
}

// --------------------------------------------------------------------------
// Send an order to the API robot
// --------------------------------------------------------------------------
//--------------------------------------------------------------------
// FullTelecommand::setEmergencyStopOrder
//--------------------------------------------------------------------
void setEmergencyStopOrder(Order& order)
{
    order.speedRight=0;
    order.speedLeft =0;
}
 
//--------------------------------------------------------------------
// FullTelecommand::handleForward
//--------------------------------------------------------------------
void handleForward(Order& order)
{
    if (mode == MODE_BRICK) {
        order.pos.center.y+=20;
    } else {
        order.speedLeft +=5;
        order.speedRight+=5;
    }
}

//--------------------------------------------------------------------
// FullTelecommand::handleBackward
//--------------------------------------------------------------------
void handleBackward(Order& order)
{
    if (mode == MODE_BRICK) {
        order.pos.center.y-=20;
    } else {
        order.speedLeft -=5;
        order.speedRight-=5;
    }
}

//--------------------------------------------------------------------
// FullTelecommand::handleLeft
//--------------------------------------------------------------------
void handleLeft(Order& order)
{
    if (mode == MODE_BRICK) {
        order.pos.center.x-=20;
    } else {
        order.speedLeft -=2;
        order.speedRight+=2;
    }
}

//--------------------------------------------------------------------
// FullTelecommand::handleRight
//--------------------------------------------------------------------
void handleRight(Order& order)
{
    if (mode == MODE_BRICK) {
        order.pos.center.x+=20;
    } else {
        order.speedLeft +=2;
        order.speedRight-=2;
    }
}

//--------------------------------------------------------------------
// FullTelecommand::handleMaxSpeed
//--------------------------------------------------------------------
void handleMaxSpeed(Order& order)
{
    order.speedLeft =100;
    order.speedRight=100;
}

//--------------------------------------------------------------------
// FullTelecommand::handleNormalSpeed
//--------------------------------------------------------------------
void handleNormalSpeed(Order& order)
{
    order.speedLeft =30;
    order.speedRight=30;
}

//--------------------------------------------------------------------
// configureKeyboard
//--------------------------------------------------------------------
void configureKeyboard()
{
    tcgetattr(0,&confterm);
    confterm.c_lflag = confterm.c_lflag & ~ICANON;
    confterm.c_cc[VMIN]=1;
    tcsetattr(0,TCSANOW,&confterm); 
}

//--------------------------------------------------------------------
// getKeyboardOrder
//--------------------------------------------------------------------
bool getKeyboardOrder(Order &order)
{
    order.reset();
    char rep;
    read(0,&rep,1);
  //  printf("read=%d\n", rep);
    switch (rep) {
        // motor
    case 27 : 
        read(0,&rep,1); 
        read(0,&rep,1);
        switch (rep) {
	case 'A' : handleForward(order); break;
	case 'B' : handleBackward(order); break;
	case 'D' : handleLeft(order); break;
	case 'C' : handleRight(order); break;
        }
        break;
    case '8' : handleForward(order); break;
    case '2' : handleBackward(order); break;
    case '4' : handleLeft(order); break;
    case '6' : handleRight(order); break;
        // emergency stop
    case '5' :
    case ' ' : 
        setEmergencyStopOrder(order);
        break; 
    case 'r' :
    case 'R' :    
	order.resetRobotPos=true;
        order.motorReset=1;
        order.speedLeft=0;
        order.speedRight=0;
        break;  
    case 'h' :
    case 'H' : 
        showUsage();
        break;   
    case 'q': 
    case 'Q': 
    case '-':
        quitApplication = true;
        break;
    default:
        break;
    }
    checkMotorOrder(order);
    return true;
}


//--------------------------------------------------------------------
// main
//--------------------------------------------------------------------
int main(int argc, char* argv[]) {
    configureKeyboard();
    
    if (!Simulator->connectToServer()) {
	LOG_ERROR("Cannot connect to server\n");
	return -1;
    }
    if (argc >= 2) {
        if (strncmp(argv[1], "1", 1) == 0) {
            mode=MODE_ATTACK;
            Simulator->setRobotName("Tele Attack");
            Simulator->setRobotModel(ROBOT_MODEL_ATTACK);
        } else if (strncmp(argv[1], "2", 1) == 0) {
            mode=MODE_DEFENCE;
            Simulator->setRobotName("Tele Defence");
            Simulator->setRobotModel(ROBOT_MODEL_DEFENCE);
        } else {
            mode=MODE_BRICK;
            Simulator->setRobotName("Tele Brique");
            Simulator->setModeBrick(true);
            Simulator->setRobotModel(ROBOT_MODEL_DEFENCE);
        }
    } else {
        mode=MODE_BRICK;
	Simulator->setRobotName("Tele Brique");
        Simulator->setModeBrick(true);
        Simulator->setRobotModel(ROBOT_MODEL_DEFENCE);
    }

    Order oldOrder;
    Order newOrder;
    newOrder.resetRobotPos = true;
    newOrder.pos = startPositionBrick;
    applyOrder(newOrder);
    while(!quitApplication) {
        getKeyboardOrder(newOrder);
        if(newOrder != oldOrder) {
            applyOrder(newOrder);
            oldOrder = newOrder;
        }
    }

    delete Simulator;
    return 0;
};
