#include "strategy2005.h"
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"
#include "motor.h"
#include "odometer.h"
//
//Gestion d'un fichier d'entree de commande
//
#include <stdio.h>
//
//
#include "robotConfig2005.h"
#include "robotMain2005.h"
#include "events.h"
#include "robotPosition.h"
#include "move.h"
#include "movementManager.h"
#include "strategyAttack.h"


// ----------------------------------------------------------------------------
// evtEndMove
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement
// ----------------------------------------------------------------------------
inline bool evtEndMove(bool evt[])
{
  return evt[EVENTS_MOVE_END];
}


/** @class TestMoveStrategy
 * Teste les déplacements du robot (MovementManager et Move)
 */
class TestMoveStrategy1CL : public Strategy2005CL
{
 public :
  TestMoveStrategy1CL(RobotMainCL* main): 
    Strategy2005CL("TestMove", "testMove1", CLASS_STRATEGY, main){}
  virtual void run(int argc, char*argv[]);
};

void TestMoveStrategy1CL::run(int argc, char* argv[])
{

    Lcd->print("SophiaTeam");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    sleep(5);

    // ====================    
    // HERE ICI HERE ICI HERE ICI
    // ====================    
    
    LOG_INFO("Debut de la sequence Gibs\n");

    //
    //Gestion d'un fichier d'entree de commande
    //
    FILE *fco;
    FILE *fcos;
    char *comm=NULL;
    size_t tailleLigne=0;
    fco=fopen("commande.gib","r");
    fcos=fco;
     if(fco==NULL){
      LOG_ERROR("pas de fichier de commande\n");
     }
     else{
       LOG_INFO("EXECUTION DU FICHIER DE COMMANDE GIB\n");
      //description du fichier de commande;
       while(getline(&comm,&tailleLigne,fco)!=-1){
	   //->enregistrer la ligne et la traiter.
	   LOG_INFO(comm);
	   //
	   //Evaluation de la chaine
	   if(*(comm)=='-')//commentaire-> non interprete
	     {
	       //LOG_INFO("commentaire");
	     }
	   else if(*comm=='T'){//Zone de Message:Titre
	     LOG_INFO(comm+1);//evite le premier caractere
	   }
	   //Pour des actions:
	   //posRob;
	   //avance;
	   //recule;
	   //avancR;//regule
	   //reculR;//regule
	   //tourne;
	   //tournR;//regule
	   //vaAuPt;//regule
	   //trajec;//regule abs/ord abs/ord ...
	   
	   else if(strncmp(comm,"avance",6)==0){//verifie la commande
	     int l;
	     comm+=6;
	     sscanf(comm," %d",&l);
	     Move->forward(l, 10);
	     
	     Events->wait(evtEndMove);
	     
	   }
	   else if(strncmp(comm,"recule",6)==0){//verifie la commande
	     int l;
	     comm+=6;
	     sscanf(comm," %d",&l);
	     Move->forward(-l, 10);
	     
	     Events->wait(evtEndMove);
	   }




	   else if(strncmp(comm,"avancR",6)==0){//verifie la commande Regule
	     // LOG_INFO("avancR demandee\n");

	     int l;
	     comm+=6;
	     sscanf(comm," %d",&l);
	     
	     RobotPos->set(0, 0, 0);
	     MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    	     Move->go2Target(Point(l, 0), -1, 10);

	     Events->wait(evtEndMove);
	   }
	   else if(strncmp(comm,"reculR",6)==0){//verifie la commande Regule
	     int l;
	     comm+=6;
	     sscanf(comm," %d",&l);
	     
	     RobotPos->set(0, 0, 0);
	     MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	     Move->go2Target(Point(l, 0), -1, 10);

	     Events->wait(evtEndMove);
	     
	   }


	   else if(strncmp(comm,"posRob",6)==0){//verifie la commande
	     int x,y,angl;
	     comm+=6;//passage de l'ordre
	     sscanf(comm," %d",&x);//abscisse(longueuer)
	     sscanf(comm," %d",&y);//ordonnee(largeur)
	     sscanf(comm," %d",&angl);//angle
	     
	     RobotPos->set(x, y, angl);
 
	   }

	   else if(strncmp(comm,"tourne",6)==0){//verifie la commande
	     int angl;
	     comm+=6;
	     sscanf(comm," %d",&angl);
	     Move->rotate(d2r(angl), -1, 10);

	     Events->wait(evtEndMove);
	   }
	   else if(strncmp(comm,"tournR",6)==0){//verifie la commande Regule
	     int angl;
	     comm+=6;
	     sscanf(comm," %d",&angl);
	     RobotPos->set(0,0,0);
	     Move->rotateFromAngle(d2r(angl), -1, 10);
	     
	     Events->wait(evtEndMove);
	   }
	   else if(strncmp(comm,"vaAuPt",6)==0){//verifie la commande
	     int x,y;
	     comm+=6;//passage de l'ordre
	     sscanf(comm," %d",&x);//abscisse(longueuer)
	     sscanf(comm," %d",&y);//ordonnee(largeur)
	     
	     Move->go2Target(Point(x,y), -1, 10);
	     Events->wait(evtEndMove);
	   }
	   //
	   //La position du robot devant etre initialise avant
	   //
	   else if(strncmp(comm,"trajec",6)==0){//verifie la commande
	     LOG_INFO("TRAJECTOIRE\n");
	     Trajectory t;
	     t.push_back(Point(RobotPos->x(), RobotPos->y()));
	     
	     int x,y;
	     comm+=6;//passage de l'ordre
	     
	     while(1){//tant que pas fin de fichier(chaine de char)
	       // s'arret si il y a un pb
	       if(sscanf(comm," %d:%d",&x,&y)!=2)break;//abscisse(longueuer)
	       t.push_back(Point(x,y));
	       LOG_INFO("1 PT de plus\n");
	       //
	       //Avancee du pointeur
	       comm++;
	       while((*comm)!=' ' && *comm!='\0')comm++;
	       if(comm=='\0')break;
	       //Sinon on retourne pour prendre le prochain point
	       //
	       //
	     }
	     
	     Move->followTrajectory(t, TRAJECTORY_RECTILINEAR, -1, 10);
	     Events->wait(evtEndMove);



	     
	   }
	   else{
	     LOG_ERROR("Erreur dans l'ecriture du fichier de commande");

	     break;//Probleme dans l'ecriture des lignes de commandes
	   }
	   //
	   //



	   
       }
       if(comm)
	 free(comm);
       LOG_INFO("Fin Fichier Commande Gibs");
     }


    //
    //



    //RobotPos->set(0,0,0);
    //Move->rotateFromAngle(d2r(720));
    //Move->go2Target(Point(1500, 0));
     //    Events->wait(evtEndMove);
    
    /*
    Move->rotate(d2r(-90)); // tourne de 90degre a droite
    Events->wait(evtEndMove);

    Move->forward(400); // avance de 20cm (pas de regulation)
    Events->wait(evtEndMove);
    */
    RobotPos->print();
    Move->stop();

    sleep(5);
    RobotPos->print();
    return;
}

/** @class TestMoveStrategy2CL
 * Teste les déplacements du robot (MovementManager et Move)
 */
class TestMoveStrategy2CL : public Strategy2005CL
{
 public :
  TestMoveStrategy2CL(RobotMainCL* main): 
    Strategy2005CL("TestMove", "testMove2", CLASS_STRATEGY, main){}
  virtual void run(int argc, char*argv[]);
};



void TestMoveStrategy2CL::run(int argc, char* argv[])
{
    Lcd->print("SophiaTeam");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    RobotPos->set(490, 1675, 0);
    Trajectory t;
    t.push_back(Point(RobotPos->x(), RobotPos->y()));
    t.push_back(Point(900,1675));
    t.push_back(Point(1200,1580));
    t.push_back(Point(1500,1580));
    t.push_back(Point(2140,1580));
    t.push_back(Point(2290,1580));
    t.push_back(Point(2590,1650));
    Move->followTrajectory(t/*, TRAJECTORY_SPLINE*/);
    Events->wait(evtEndMove);

    Move->go2Target(Point(3190,1650));
    Events->wait(evtEndMove);
    
    Move->go2Target(Point(3190,750));
    Events->wait(evtEndMove);
    
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1350));
    Events->wait(evtEndMove);

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2590, 1350));
    Events->wait(evtEndMove);

    Move->go2Target(Point(2250, 1270));
    Events->wait(evtEndMove);

    Move->stop();
    RobotPos->print();
    sleep(2);
    // detection du pont

    Log->emergencyStopPressed(true);
    sleep(5);
    return;
}

inline bool evtEndMovePwm(bool evt[])
{
  return evt[EVENTS_MOVE_END] || 
    evt[EVENTS_PWM_ALERT_LEFT] ||
    evt[EVENTS_PWM_ALERT_RIGHT];
}

/** @class TestMoveStrategy3CL
 * Teste les déplacements du robot (MovementManager et Move)
 */
class TestMoveStrategy3CL : public Strategy2005CL
{
 public :
  TestMoveStrategy3CL(RobotMainCL* main): 
    Strategy2005CL("TestMove", "testMove3", CLASS_STRATEGY, main){}
  virtual void run(int argc, char*argv[]);
};

void TestMoveStrategy3CL::run(int argc, char* argv[])
{
    Lcd->print("SophiaTeam2");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    RobotPos->set(0, 0, 0); 
    //Move->realign(0);
    //Move->rotateOnWheel(0, false);
    Move->go2Target(Point(1000,0), 10);
    ///*  Move->rotateFromAngle(-2*M_PI);
    Events->wait(evtEndMovePwm);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
      LOG_OK("Move end correct\n");
    }
    Move->rotateFromAngle(-M_PI);
    
    Events->wait(evtEndMovePwm);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
      LOG_OK("Move end correct\n");
    }
    Move->go2Target(Point(200,0));
    ///*  Move->rotateFromAngle(-2*M_PI);
    Events->wait(evtEndMovePwm);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
      LOG_OK("Move end correct\n");
    }
    Move->stop();
    RobotPos->print();
    sleep(5);
    RobotPos->print();
    while(1){sleep(1);}
    return; 

    Move->go2Target(Point(1000,0));
    Events->wait(evtEndMovePwm);
    if (Events->check(EVENTS_PWM_ALERT_LEFT)) {
      LOG_INFO ("Left\n"); 
      Move->stop();
      MvtMgr->motorReset();
      sleep(2);
      MvtMgr->setSpeed(-20, 20);
      usleep(1000000);
      MvtMgr->setSpeed(0, 0);
    } else if (Events->check(EVENTS_PWM_ALERT_RIGHT)) {
      LOG_INFO ("Rightt\n");
      Move->stop();
      MvtMgr->motorReset();
      sleep(2);
      MvtMgr->setSpeed(20, -20);
      sleep(2);
      usleep(1000000);
      MvtMgr->setSpeed(0, 0);
    } else {
      LOG_INFO ("Normal\n");
    }
    
    Move->forward(300);
    Events->wait(evtEndMovePwm);
/*
    Move->go2Target(Point(1000,0));
    Events->wait(evtEndMove);
    
    Move->go2Target(Point(1000,750));
    Events->wait(evtEndMove);
   */
    Move->stop();
    RobotPos->print();
    sleep(2);
    // detection du pont
    RobotPos->print();
    Log->emergencyStopPressed(true);
    sleep(5);
    return;
}

class Strategy1M : public Strategy2005CL
{
 public :
    Strategy1M(RobotMainCL* main): 
	Strategy2005CL("TestMove", "1Meter", CLASS_STRATEGY, main){}
    virtual void run(int argc, char*argv[]);
};

void Strategy1M::run(int argc, char* argv[])
{
    Lcd->print("1 Meter");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    RobotPos->set(0, 0, 0); 
    Move->go2Target(Point(1000,0));
    Events->wait(evtEndMovePwm);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
      LOG_OK("Move end correct\n");
    }
    RobotPos->print();
    sleep(5);
    RobotPos->print();
}


class Strategy2Turns : public Strategy2005CL
{
 public :
    Strategy2Turns(RobotMainCL* main, bool clockwise): 
	Strategy2005CL("TestMove", "2Turns", CLASS_STRATEGY, main),
	clockwise_(clockwise)
	{}
    virtual void run(int argc, char*argv[]);
    bool clockwise_;
};

void Strategy2Turns::run(int argc, char* argv[])
{
    Lcd->print("1 Meter");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);
    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);

    RobotPos->set(0, 0, 0); 
    Move->rotateFromAngle(clockwise_?2*M_PI:-2*M_PI);
    Events->wait(evtEndMovePwm);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
      LOG_OK("Move end correct\n");
    }
    RobotPos->print();
    sleep(5);
    RobotPos->print();
}


int main(int argc, char* argv[])
{
  RobotConfigCL*  config;
  RobotMainCL*    robotMain=NULL;
  StrategyCL*     strategy1=NULL;
  StrategyCL*     strategy2=NULL;
  StrategyCL*     strategy3=NULL;
  StrategyCL*     strategyAttack=NULL;

#ifndef SIMULATED
#define SIMULATED false
LOG_INFO("MODE REAL\n");
#else
LOG_INFO("SIMULATED\n");
#endif

#define ROBOT_DEFENCE

#ifdef ROBOT_DEFENCE
  LOG_INFO("ROBOT_DEFENCE\n");
  config = new RobotConfigDefence2005CL(SIMULATED);
#else
  LOG_INFO("ROBOT_ATTACK\n");
  config = new RobotConfigAttack2005CL(SIMULATED);
#endif

  robotMain = new RobotMain2005CL();
  strategy1 = new TestMoveStrategy1CL(robotMain);
  strategy2 = new TestMoveStrategy2CL(robotMain);
  strategy3 = new TestMoveStrategy3CL(robotMain);
  strategyAttack = new StrategyAttackCL(robotMain, "strategy attack");

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);
  //// ICI ICI ICI => strategy2 = traverse le pont
  //robotMain->run(strategy2, argc, argv); // traverse le pont
  //robotMain->run(strategyAttack, argc, argv); // traverse le pont
  robotMain->run(strategy3, argc, argv); // blocage des roues
  //robotMain->run(strategy1, argc, argv); //deplacement fichier  test des deplacements
 

  while(1) {sleep(1);}
  delete strategyAttack;
  delete strategy2;
  delete strategy1;
  delete robotMain;
  delete config;

  return 0;
}
