#define ATTACK_CATAPULT_INFO

#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "servo.h"
#include "gridAttack.h"
#include "robotTimer.h"
#include "bumperMapping.h"
#include "alim.h"

void gridAttackPeriodicTask(void *userData, 
                            Millisecond time)
{
    if (!userData) return;
    ((StrategyAttackCL*)userData)->periodicTask(time);
}

void StrategyAttackEnvDetectorCallBack(void* userData,
                                       EventsEnum evt)
{
    if (!userData) return;
    ((StrategyAttackCL*)userData)->envDetectorCallBack(evt);
}

// --------------------------------------------------------------------------
// Initialisation 
// --------------------------------------------------------------------------
StrategyAttackCL::StrategyAttackCL(RobotMainCL* main, const char* menuName): 
    Strategy2005CL("StrategyAttack", menuName, CLASS_STRATEGY, main),
    bridgeAvailibility_(0xFF), bridge_(BRIDGE_POS_UNKNOWN), 
    useLeftBridge_(true), bridgeDetectionByCenter_(true), 
    useSharpToDetectBridge_(true),
    grid_(NULL), lastExplorationDir_(ATTACK_EXPLORE_COL),
    attackPhase_(ATTACK_WAIT_START), skittleMiddleProcessed_(false),
    isProcessingMiddleSkittles_(false), bridgeBySharps_(BRIDGE_POS_UNKNOWN), 
    useBridgeBumpers_(true), demoMode_(0)
{
    grid_ = new GridAttack();
    Timer->registerPeriodicFunction(gridAttackPeriodicTask,
                                    "gridAttackPeriodicTask",
                                    this);
}
// --------------------------------------------------------------------------
// Destruction 
// --------------------------------------------------------------------------
StrategyAttackCL::~StrategyAttackCL()
{
    if (grid_) delete grid_;
}

// --------------------------------------------------------------------------
// Verification que tout va bien dans le robot
// --------------------------------------------------------------------------
bool StrategyAttackCL::autoCheck()
{
    Strategy2005CL::autoCheck();
    // est ce qu'on essaye de passer par le pont du milieu ?
    demoMode_ = menu("Bridge mid cent\nDemo    Normal")?1:0;
    bridgeDetectionByCenter_ = !menu("Strategy\nGauche    Sioux");
    Lcd->print("Catapult armed\n++ Test Move ++"); 
    // met les servos en position
    sleep(1);
    prepareCatapults();
    // verifier les capteurs sharps et bumpers detecteurs de pont
    sleep(1);
    testBridgeCaptors();
    // verifier les sharps detecteurs d'environnement
    // TODO
    // verifier l'asservissement
    sleep(2);
    testMove();
    // maintenant il n'y a plus qu'a bien placer le robot sur la table
    // et mettre les balles dans les catapultes
    // et attendre la jack de depart
    resetMotorForPrepareRobot();
    return true;
}

// --------------------------------------------------------------------------
// Point d'entree du programme du robot: commande haut niveau du robot
// --------------------------------------------------------------------------
void StrategyAttackCL::run(int argc, char* argv[])
{
    if (!grid_) {
        grid_ = new GridAttack();
    }
    grid_->reset();
    Lcd->print("SophiaTeam\nAttack");
    //RobotPos->setOdometerType(ODOMETER_MOTOR);
    RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    Events->registerImmediatCallback(EVENTS_ENV_SIDE_RIGHT, this,
                                     StrategyAttackEnvDetectorCallBack,
                                     "StrategyAttackEnvDetectorCallBack");
    Events->registerImmediatCallback(EVENTS_ENV_SIDE_RIGHT, this,
                                     StrategyAttackEnvDetectorCallBack,
                                     "StrategyAttackEnvDetectorCallBack");
    setStartingPosition();
    bridgeDetectionByCenter_=true;
    waitStart(INIT_COMPLETE);

    Move->enableAccelerationController(false);
    // active le desasservissement automatique des moteurs en
    // cas de PWM alerte
    MvtMgr->enableAutomaticReset(true);  
    
    // tire les balles!
    fireCatapults();

    // va vers le pont
    setAttackPhase(ATTACK_CROSS_BRIDGE);
    Lcd->print("Goto bridge");
    if (demoMode_==1) {
      crossBridgeDemo();
    } else {
      gotoBridgeDetection();
      Move->stop();
      // sleep(5);
      if (checkEndEvents()) return; // fin du match
      
      // utilise les capteurs pour trouver le pont et traverse le pont
      if (!useSharpToDetectBridge_ || 
	  !getBridgePosBySharp()) {
        // les sharps ne marchent pas, on va tout droit pour voir...
        getNearestBridgeEntry();
	
      }
    }
    // sleep(5);
    Lcd->print("Find and cross\nBridge");
    findAndCrossBridge();
    if (checkEndEvents()) return; // fin du match

    // utilise une trajectoire d'exploration predefinie
    setAttackPhase(ATTACK_PREDEFINED_TRAJEC);
    Lcd->print("Pre-defined expl");
    preDefinedSkittleExploration();
    if (checkEndEvents()) return; // fin du match

    // utilise un super algo d'exploration du terrain
    setAttackPhase(ATTACK_ONLY_SKITTLES);
    Lcd->print("Explore a donf");
    int counterTryMiddle=0;
    while(true) {
        basicSkittleExploration();
        if (checkEndEvents()) return; // fin du match
        if (!isProcessingMiddleSkittles_ 
            && (--counterTryMiddle < 0)) {
            killCenterSkittles();
            if (checkEndEvents()) return; // fin du match
            counterTryMiddle = 3;
        }
    }
    return;
}

// ========================= Tire des catapultes ========================

// ------------------------------------------------------------------------
// Met les servos des catapultes dans la position de depart
// ------------------------------------------------------------------------
bool StrategyAttackCL::prepareCatapults()
{
    LOG_FUNCTION();
    for(unsigned int i=0; i<ATTACK_CATAPULT_SERVO_NBR; i++) {
      Servo->setServoPosition(catapultServos[i].id, 
			      catapultServos[i].posArmed);
    }
    usleep(CATAPULT_AWAIT_DELAY*1000);
    Servo->disableAll();
   return true;
}

// ------------------------------------------------------------------------
// Tire des balles avec les catapultes 
// ------------------------------------------------------------------------
void StrategyAttackCL::fireCatapults()
{
    // pour tirer plus vite que notre ombre on commande le servo en premier,
    // les logs viendront ensuite
    // on envoie 2 fois l'ordre pour asser car c'est une action hyper
    // importante!
    for(unsigned int i=0; i<ATTACK_CATAPULT_SERVO_NBR; i++) {
        Servo->setServoPosition(catapultServos[i].id, 
				catapultServos[i].posFire);
    }
    LOG_COMMAND("Fire Catapults\n");
    setAttackPhase(ATTACK_FIRE_CATAPULT);
    Lcd->print("Fire");
    Log->fireBalls();
    // on attend un peut pour etre sur qu'on a tirer avant de couper les
    // moteurs et de bouger
    usleep(CATAPULT_AWAIT_DELAY*1000);

    Servo->disableAll();
}

// ================ Exploration de la zone ennemie ==================

// ------------------------------------------------------------------------
// tache periodique qui met a jour la grille pour dire ou on est deja passe
// ------------------------------------------------------------------------
void StrategyAttackCL::periodicTask(Millisecond time)
{
    if (grid_ && 
        attackPhase_ >= ATTACK_PREDEFINED_TRAJEC) {
        grid_->setVisitTime(time, RobotPos->pos());
    }
    if (isProcessingMiddleSkittles_) {
        if (RobotPos->x() < 2450) {
            Bumper->enableCaptor(BRIDG_BUMP_LEFT);
            Bumper->enableCaptor(BRIDG_BUMP_RIGHT);
        } else {
            Bumper->disableCaptor(BRIDG_BUMP_LEFT);
            Bumper->disableCaptor(BRIDG_BUMP_RIGHT);
        }
    }
}

// -------------------------------------------------------------------------------
// Met a jour la grille quand on detect un true sur les cotes du robot
// -------------------------------------------------------------------------------
void StrategyAttackCL::envDetectorCallBack(EventsEnum evt) 
{
    Point pt;
    switch(evt) {
    case EVENTS_ENV_SIDE_LEFT:
        pt = RobotPos->pt()+300.*Point(cos(RobotPos->thetaAbsolute()+M_PI/2),
                                       sin(RobotPos->thetaAbsolute()+M_PI/2));
        if (pt.x < 2500) return; // on ne detecte pas les quilles chez nous!
        Log->skittle(pt);
        if (grid_) grid_->setSkittleDetected(pt);
        break;
    case EVENTS_ENV_SIDE_RIGHT:
        pt = RobotPos->pt()+300.*Point(cos(RobotPos->thetaAbsolute()-M_PI/2),
                                       sin(RobotPos->thetaAbsolute()-M_PI/2));
        if (pt.x < 2500) return; // on ne detecte pas les quilles chez nous!
        Log->skittle(pt);
        if (grid_) grid_->setSkittleDetected(pt);
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------------
// Petit message pour les log quand change de phase de jeu
// -------------------------------------------------------------------------------
void StrategyAttackCL::setAttackPhase(AttackPhase phase)
{
    attackPhase_ = phase;

    switch (attackPhase_) { 
    case ATTACK_WAIT_START:
        { LOG_OK("Change Attack Phase to ATTACK_WAIT_START\n"); }
    break;
    case ATTACK_FIRE_CATAPULT:
        { LOG_OK("Change Attack Phase to ATTACK_FIRE_CATAPULT\n"); }
    break;
    case ATTACK_CROSS_BRIDGE:
        { LOG_OK("Change Attack Phase to ATTACK_CROSS_BRIDGE\n"); }
    break;
    case ATTACK_PREDEFINED_TRAJEC:
        { LOG_OK("Change Attack Phase to ATTACK_PREDEFINED_TRAJEC\n"); }
    break;
    case ATTACK_ONLY_SKITTLES:
        { LOG_OK("Change Attack Phase to ATTACK_ONLY_SKITTLES\n"); }
    break;
    case ATTACK_GRAND_MENAGE:       
        { LOG_OK("Change Attack Phase to ATTACK_GRAND_MENAGE\n"); }
    break;
    }
}


Point getAttackWheelPos(bool left, bool forward) 
{
    Point currentPos = RobotPos->pt();
    Point leftWheel(currentPos);
    Radian theta = RobotPos->thetaAbsolute();
    double c = cos(theta);
    double s = sin(theta);
    // TODO: get real constants
    const Millimeter LEFT = left?170:-170;
    const Millimeter FRONT = forward?210:150;
    leftWheel.x += FRONT * c - LEFT * s;
    leftWheel.y += FRONT * s + LEFT * c;
    return leftWheel;
}

bool StrategyAttackCL::calcSupportCenterCollision(Point pos, Point& supportCenter) const 
{
    GridAttack::GPoint gridPoint = grid_->getGPoint(pos);
    bool canContainSupport = grid_->element(gridPoint).skittleAtBeginning;
    if (!canContainSupport)
	return false;
    else {
	supportCenter = grid_->getPoint(gridPoint);
	return true;
    }
}
