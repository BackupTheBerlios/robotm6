#include "robotMain.h"
#include "strategyDefence.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "tesla.h"
#include "skittleDetector.h"
#include "crane.h"
#include "robotTimer.h"

StrategyDefenceCL::StrategyDefenceCL(RobotMainCL* main): 
    Strategy2005CL("StrategyDefence", "Robot defence", CLASS_STRATEGY, main),
    modeBridgeDefence_(DEFENCE_MODE_FOLLOW_ATTACK)
{

}

// --------------------------------------------------------------------------
// Verification que tout va bien dans le robot
// --------------------------------------------------------------------------
bool StrategyDefenceCL::autoCheck()
{
    Strategy2005CL::autoCheck();
    // on suit le robot d'attack ou on fait le tour du terrain?
    if (menu("Va pont    Longe\nMilieu      Bord")) {
        modeBridgeDefence_ = DEFENCE_MODE_FOLLOW_ATTACK;
    } else {
        modeBridgeDefence_ = DEFENCE_MODE_FOLLOW_BORDER;
    }

    // verifier les sharps detecteurs d'environnement
    // TODO
    // verifier l'asservissement
    testMove();
    // met l'electroaimant en position et verifie le detecteur de quilles
    preparePullupMechanism();
    // maintenant il n'y a plus qu'a bien placer le robot sur la table
    // et mettre les balles dans les catapultes
    // et attendre la jack de depart
    return true;
}

// --------------------------------------------------------------------------
// Met la grue en position de depart et verifie le detecteur de quilles
// --------------------------------------------------------------------------
bool StrategyDefenceCL::preparePullupMechanism()
{
    Crane->setPosition(CRANE_STARTING_POS_X, CRANE_STARTING_POS_Z);
    Tesla->stopTesla();
    Tesla->disableDetector();

    SkittlePosition posSkittle;
    // verifie que le detecteur de quille rateau n'a pas de faut contact
    while (SkittleDetector->getBasicValue(posSkittle) 
           && posSkittle != 0) {
        LOG_ERROR("Skittle Detector has detected something before start\n");
        if (!menu("Rateau court jus\nRetry     Skip")) {
            SkittleDetector->setMask(posSkittle);
            break;
        }
    }

    usleep(CRANE_WAIT_GOTO_START_POS*1000);
    Crane->disableMotors();
    return true;
}


// --------------------------------------------------------------------------
// Point d'entree du programme du robot: commande haut niveau du robot
// --------------------------------------------------------------------------
void StrategyDefenceCL::run(int argc, char* argv[])
{
    Lcd->print("SophiaTeam\nDefence");
    RobotPos->setOdometerType(ODOMETER_MOTOR);
    //RobotPos->setOdometerType(ODOMETER_UART_MANUAL);
    setStartingPosition();
    waitStart(INIT_NONE);

    Move->enableAccelerationController(false);
    MvtMgr->enableAutomaticReset(false);


    switch(modeBridgeDefence_) {
    case DEFENCE_MODE_FOLLOW_ATTACK:
        usleep(DEFENCE_DELAY_AT_START*1000);
        gotoBridgeByLeft();
        break;
    case DEFENCE_MODE_FOLLOW_BORDER:
        gotoBridgeFollowBorder();
        break;
    case DEFENCE_MODE_HOMOLOGATION:
        usleep(DEFENCE_DELAY_HOMOLOGATION*1000);
        gotoBridgeHomologation();
        break;
    }

    if (checkEndEvents()) return; // fin du match

    // pendant un certain temps on essaye d'empecher l'aute de venir 
    // chez nous ou de tirer sur nos balles
    if (RobotPos->x()>1250) {
        while(Timer->time() < DEFENCE_TIME_PROTECTION_BEFORE_EXPLORE) {
            doOneProtectionLoop();
            if (checkEndEvents()) return; // fin du match
        }
    }
    
    // jusqu'a la fin du match on fait le tour de nos quilles pour renverser
    // celles qui sont tombees
    while(true) {
        exploreAroundSkittles(); 
        if (checkEndEvents()) return; // fin du match
    }   


    return;
}

//essayer de traverser par le pont du mileu, s'il n'y a pas de pont, 
// ca envoie la balle de GRS sur les quilles... et on tombe dans le trou
bool StrategyDefenceCL::gotoBridgeHomologation()
{
    Trajectory t;
    t.push_back(Point(600, RobotPos->y()));
    t.push_back(Point(1050, 1500));
    t.push_back(Point(1250, 1265));
    t.push_back(Point(2500, 1265));
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t);
    Events->wait(evtEndMoveNoCollision);
 
    Move->stop();
    // on a terminer la tracjectoire anormalement
    if (checkEndEvents()) return false;
    LOG_INFO("Wait match end\n");
    // attendre la fin du match
    while(!checkEndEvents()) sleep(1);
    return false;
}

// aller vers le pont du milieu en suivant le robot d'attack
// va dans une position qui enpeche l'adversaire de viser nos quilles avec des catapultes
bool StrategyDefenceCL::gotoBridgeByLeft()
{
    Trajectory t;
    t.push_back(Point(600, RobotPos->y()));
    t.push_back(Point(900, 1500));
    t.push_back(Point(1050, 1265));
    t.push_back(Point(1200, 1050));
    t.push_back(Point(1350, 1050));
    t.push_back(Point(1350, 750));
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t);
    Events->wait(evtEndMove);
    if (checkEndEvents()) return false;
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;

    // collision: on attend un peu  et on va au point voulu
    Move->stop();
    t.clear(); 
    t.push_back(Point(1300, 1500));
    t.push_back(Point(1350, 750));
    while(true) {
        // recule un peu
        Move->backward(70);
        Events->wait(evtEndMoveNoCollision);
        Move->stop();
        usleep(DEFENCE_COLLISION_DELAY*1000);  // attends 3 secondes pour gener l'adversaire
        if (checkEndEvents()) return false;
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->followTrajectory(t);
        Events->wait(evtEndMove);
        if (checkEndEvents()) return false;
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true; 
        // collision: on attend un peu  et on va au point voulu
    }
}

// quand on est devant le pont adverse, on bouge un peu pour empecher
// le robot adverse de passer ou de tirer des balles sur nos quilles
bool StrategyDefenceCL::doOneProtectionLoop()
{
    if (RobotPos->x()<1250) return false;
    
    Point target(1300, 300);
    if (RobotPos->y()<600) target.y=1050;
    // la cible est devant ou derriere? 
    if (RobotPos->isTargetForward(target)) {        
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    } else {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    }
    // on vient de se mettre dans la direction qui minimise la rotation du robot
    Move->go2Target(target);
    Events->wait(evtEndMove);
    if (checkEndEvents()) return false;
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true; 
    // collision: on attend un peu  et on va au point voulu 
    Move->backward(70);
    Events->wait(evtEndMoveNoCollision);
    Move->stop();
    usleep(DEFENCE_COLLISION_DELAY*1000); // attend 3 secondes pour gener l'adversaire
    return false;
}

// fait un tour du terrain pour relever les quilles
bool StrategyDefenceCL::exploreAroundSkittles()
{
    return false;
}

// aller vers le pont du milieu en longean le bord du terrain
bool StrategyDefenceCL::gotoBridgeFollowBorder()
{
    Trajectory t;
    t.push_back(Point(170, 200));
    t.push_back(Point(1350, 200));
    t.push_back(Point(1300, 1050));
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (checkEndEvents()) return false;
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // collision...
    while(true) {
        if (RobotPos->x()>1200) return true;
        else if (RobotPos->x()<300 && RobotPos->y()>300) {
            // on est coince contre le bord de note terrain
            Move->realign(-M_PI/2);
            Events->wait(evtEndMoveNoCollision);
            Move->stop();
            t.clear();
            t.push_back(Point(170, 200));
            t.push_back(Point(1350, 200));
            t.push_back(Point(1300, 1050));
            MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
            Move->followTrajectory(t, TRAJECTORY_BASIC);
        } else {
            Move->realign(0);
            Events->wait(evtEndMoveNoCollision);
            Move->stop();
            t.clear();
            t.push_back(Point(1350, 200));
            t.push_back(Point(1300, 1050));
            MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
            Move->followTrajectory(t, TRAJECTORY_BASIC);
        }
        Events->wait(evtEndMove);
        if (checkEndEvents()) return false;
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    }
    
}
