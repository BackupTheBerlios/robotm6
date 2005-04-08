#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "geometry2D.h"

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus les collisions basees sur les bumper
// ----------------------------------------------------------------------------
bool evtEndMoveBridge(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_GROUP_BRIDGE];
}


bool StrategyAttackCL::findAndCrossBridge() 
{ 
    while (true) {   
        if (gotoBridgeEntry()) {
            if (crossBridge()) {
                Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
                Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
                return true;
            }
        } 
        if (checkEndEvents()) return false;
        getNearestBridgeEntry();
    }
}


// met a jour la variable bridge_ avec la position possible du pont la plus proche
void StrategyAttackCL::getNearestBridgeEntry()
{
    unsigned char currentBit = getPosBit();

    // si on a deja tout teste, on reteste a nouveau...
    if (!(bridgeAvailibility_&0x0F)) bridgeAvailibility_ = 0x0F;
    // trouve le pont non explore le plus proche
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case 0:
            currentBit=1;
            break;
        case 1:
            currentBit=0;
            break;
        case 2:
            currentBit=3;
            break;
        case 3:
            currentBit=2;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case 0:
            currentBit=2;
            break;
        case 1:
            currentBit=2;
            break;
        case 2:
            currentBit=1;
            break;
        case 3:
            currentBit=1;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case 1:
            currentBit=0;
            break;
        case 2:
            currentBit=3;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    // should be ok...
    switch(currentBit) {
        case 0:
            bridge_ = BRIDGE_POS_BORDURE;
            break;
        case 1:
            bridge_ = BRIDGE_POS_MIDDLE_BORDURE;
            break;
        case 2:
            bridge_ = BRIDGE_POS_MIDDLE_CENTER;
            break;
        case 3:
            bridge_ = BRIDGE_POS_CENTER;
            break;
        default:
            {
                bridge_ = (BridgePosition)robotRand(1, 5);
                LOG_ERROR("On n'aurait jamais du passer la!, char* y sait pas coder!\n");
            }
    }
    LOG_INFO("Nearest bridge to try: %s Availibility:0x%2.2x\n", BridgePosTxt(bridge_), bridgeAvailibility_);
    Log->bridge(bridge_);
}

// renvoie le bit du pont en face duquel on est le plus proche
unsigned char StrategyAttackCL::getPosBit() 
{
    unsigned char currentBit = 4;
    if (useLeftBridge_) {
        if (RobotPos->y() < 1500) {
            currentBit = 3;
        } else if (RobotPos->y() < 1650) {
            currentBit = 2;
        } else if (RobotPos->y() < 1800) {
            currentBit = 1;
        } else {
            currentBit = 0;
        } 
    } else {
        if (RobotPos->y() > 600) {
            currentBit = 3;
        } else if (RobotPos->y() > 450) {
            currentBit = 2;
        } else if (RobotPos->y() > 300) {
            currentBit = 1;
        } else {
            currentBit = 0;
        } 
    }
    return currentBit;
}

// pas de pont a l'endroit ou est le robot...
void StrategyAttackCL::noBridgeHere()
{
    bridgeAvailibility_ &= (~(1<<getPosBit()));
    if (bridgeDetectionByCenter_ && getPosBit() >= 2) {
        bridgeAvailibility_ &= 0x03;
    }
    bridgeDetectionByCenter_ = false; 
}

// ================ Detection et traversee du pont ==================

/** @brief va a l'endroit ou on detecte les pont par capteurs sharps */
bool StrategyAttackCL::gotoBridgeDetection()
{
  Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
  Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
  while(true) {
      LOG_INFO("gotoBridgeDetection(%s)\n",
               bridgeDetectionByCenter_?"Passe par le milieu":"Passe par un pont normal");
      Trajectory t;
      t.push_back(Point(RobotPos->x(), RobotPos->y()));
      t.push_back(Point(RobotPos->x()+250, RobotPos->y()));
      if (bridgeDetectionByCenter_) {
          // va vers le pont du milieu
          t.push_back(Point(BRIDGE_DETECT_SHARP_X-250, BRIDGE_ENTRY_SIOUX_Y));
          t.push_back(Point(BRIDGE_DETECT_SHARP_X,     BRIDGE_ENTRY_SIOUX_Y));
      } else {
          // va vers la gauche du terrain pour detecter la position du pont d'un coup
          t.push_back(Point(BRIDGE_DETECT_SHARP_X-250, BRIDGE_ENTRY_MIDDLE_BORDURE_Y));
          t.push_back(Point(BRIDGE_DETECT_SHARP_X,     BRIDGE_ENTRY_MIDDLE_BORDURE_Y));
      }
      Move->followTrajectory(t, TRAJECTORY_RECTILINEAR, 1, 40); // gain, vitesse max
      Events->wait(evtEndMove);
      if (checkEndEvents()) return false;
      if (Events->isInWaitResult(EVENTS_MOVE_END)) {
          Move->stop();
          return true;
      }
      // collision: on recule et on essaye de repartir par un autre endroit...
      Move->backward(300);
      Events->wait(evtEndMoveNoCollision);
      bridgeDetectionByCenter_ = !bridgeDetectionByCenter_;
  }
  return false;
}

// Recupere la valeur des sharps detecteurs de pont: verifie que la valeur ne 
// varie pas
bool StrategyAttackCL::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR],
                                        bool checkSharp)
{
    BridgeCaptorStatus captors1[BRIDGE_CAPTORS_NBR];
    if (!Bumper->getBridgeCaptors(captors1)) {
        LOG_ERROR("Cannot get bridge position by captors");
        return false;
    }

    usleep(300000);

    BridgeCaptorStatus captors2[BRIDGE_CAPTORS_NBR];
    if (!Bumper->getBridgeCaptors(captors2)) {
        LOG_ERROR("Cannot get bridge position by captors");
        return false;
    }
    bool result = true;
    char txt[256]; txt[0]=0;
    for(int i=0;i<BRIDGE_CAPTORS_NBR;i++) {
        if (i != BRIDGE_BUMPER_LEFT && i != BRIDGE_BUMPER_RIGHT) {
            if (checkSharp && captors1[i] != captors2[i]) result = false;
        } else {
            if (!checkSharp && captors1[i] != captors2[i]) result = false;
        }
        if (captors1[i] != captors2[i]) sprintf(txt, "%s%s", txt, KB_RED); 
        sprintf(txt, "%s[%d.1]=%s [%d.2]=%s, ", txt,
                i, captors1[i]==BRIDGE_NO?"trou":"pont", 
                i, captors2[i]==BRIDGE_NO?"trou":"pont");
        if (captors1[i] != captors2[i]) sprintf(txt, "%s%s", txt, KB_RESTORE); 
    }
    if (result) {
        LOG_INFO("Status bridge captors: %s\n",txt);
    } else  {
        LOG_ERROR("Status bridge captors: %s\n",txt);
    }
    memcpy(captors, captors2, sizeof(BridgeCaptorStatus)*BRIDGE_CAPTORS_NBR);
    return result;
}

// calcule la position du pont en fonction des sharps quand on est aligne
// pour traverser sur lo pont du milieu
bool StrategyAttackCL::getBridgePosBySharpFromCenter
      (BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    // on doit arrive vers le pont du milieu
    if (fabs(RobotPos->y() -  BRIDGE_ENTRY_SIOUX_Y) > BRIDGE_ENTRY_MARGIN) 
        return false;
    if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_DETECTED
        && captors[BRIDGE_SHARP_CENTER] == BRIDGE_DETECTED) {
        // pont contre le pont fixe
        bridge_=BRIDGE_POS_CENTER;
    } else if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_DETECTED
               && captors[BRIDGE_SHARP_CENTER] == BRIDGE_NO) {
        // pont au milieu, cote pont fixe, va y avoir du spectacle!
        bridge_=BRIDGE_POS_MIDDLE_CENTER;
    } else {
        // 2 positions possibles pres du bord du terrain
        bridge_=BRIDGE_POS_UNKNOWN;
        bridgeAvailibility_ &= 0x03;
        return false;
    }    
    return true;
}

// calcule la position du pont en fonction des sharps quand on est en face du 
// pont BRIDGE_POS_MIDDLE_BORDURE
bool StrategyAttackCL::getBridgePosBySharpFromLeft
      (BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    if (fabs(RobotPos->y() - BRIDGE_ENTRY_MIDDLE_BORDURE_Y) 
        > BRIDGE_ENTRY_MARGIN) 
        return false;
    if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_DETECTED
        && captors[BRIDGE_SHARP_CENTER] == BRIDGE_DETECTED
        && captors[BRIDGE_SHARP_RIGHT] == BRIDGE_DETECTED) {
        // pont en face!
        bridge_=BRIDGE_POS_MIDDLE_BORDURE;
    } else if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_DETECTED
               && captors[BRIDGE_SHARP_RIGHT] == BRIDGE_NO) {
        
        bridge_=BRIDGE_POS_BORDURE;
    } else if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_NO
               && captors[BRIDGE_SHARP_CENTER] == BRIDGE_DETECTED
               && captors[BRIDGE_SHARP_RIGHT] == BRIDGE_DETECTED) {
        
        bridge_=BRIDGE_POS_MIDDLE_CENTER;
    } else if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_NO
               && captors[BRIDGE_SHARP_CENTER] == BRIDGE_NO
               && captors[BRIDGE_SHARP_RIGHT] == BRIDGE_DETECTED) {
        bridge_=BRIDGE_POS_CENTER;
    } else {
        // pas de bol, les sharps merdouillent...
        bridge_=BRIDGE_POS_UNKNOWN;
        return false;
    }
    return true;
}

// met a jour la position du pont en fonction des sharps quand on est en face du 
// pont BRIDGE_POS_MIDDLE_BORDURE ou qu'on est aligne pour traverser sur le
// pont du milieu
bool StrategyAttackCL::getBridgePosBySharp()
{
    LOG_FUNCTION();
    // on reccupere la valeur des capteurs 2 fois pour voir si on n'a 
    // pas un soucis de bruit
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    if (!getBridgeCaptors(captors, true)) return false;

    if (bridgeDetectionByCenter_) {
        if (!getBridgePosBySharpFromCenter(captors)) return false;
    } else {
        if (!getBridgePosBySharpFromLeft(captors)) return false;
    }
    LOG_INFO("Bridge by Sharp=%s\n", BridgePosTxt(bridge_));
    Log->bridge(bridge_);
    return true;
}

/** @brief se place en face du pont en y ou on peut utiliser les bumpers pour 
    savoir si le pont est bien la. */
bool StrategyAttackCL::gotoBridgeEntry(Millimeter y, 
				       bool rotateLeft, 
				       bool rotateRight)
{
    if (fabs(RobotPos->y() - y) < BRIDGE_ENTRY_MARGIN) {
        Events->enable(EVENTS_NO_BRIDGE_BUMP_LEFT);
        Events->enable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        LOG_INFO("gotoBridgeEntry Easy:%d\n", (int)y);
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->go2Target(Point(BRIDGE_DETECT_BUMP_X, y));
    } else {
        LOG_INFO("gotoBridgeEntry Hard:%d\n", (int)y);
        // se debrouile pour que le robot ne fasse pas un demi tour sur lui meme
        if (RobotPos->x() > BRIDGE_ENTRY_NAVIGATE_X
            && isZeroAngle(RobotPos->theta(), M_PI/2)) {
            MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
        } else {
            MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        }
        
        // quand on recule, on ne regarde pas les evenements des bumpers pont
        // car on etait peut etre dans le trou...
        Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
        Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        // on s'eloigne un peu du bord, pour aller en x qui nous permet
        //de nous promener tranquillement le long de la riviere
        Move->go2Target(BRIDGE_ENTRY_NAVIGATE_X, RobotPos->y());
        Events->wait(evtEndMove);
        if (checkEndEvents()) return false;
        if (!Events->isInWaitResult(EVENTS_MOVE_END)) return false;

        // va en face du pont
        Events->enable(EVENTS_NO_BRIDGE_BUMP_LEFT);
        Events->enable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        Trajectory t;
        t.push_back(Point(BRIDGE_ENTRY_NAVIGATE_X, y));
        t.push_back(Point(BRIDGE_DETECT_BUMP_X, y));
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->followTrajectory(t);
    }
    // utiliser les bumpers events pour savoir si on tombe dans un trou...
    Events->wait(evtEndMoveBridge);
    if (Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_LEFT) ||
        Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_RIGHT)) {
        // le pont n'est pas la! Faut vite s'arreter!
        Move->emergencyStop();
        noBridgeHere();
        Move->backward(100);
        Events->wait(evtEndMoveNoCollision);
        return false;
    } 
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    return false;
}

/** @brief se place en face du pont ou on peut utiliser les bumpers pour 
    savoir si le pont est bien la. La variable bridge doit deja etre mise 
    a jour */
bool StrategyAttackCL::gotoBridgeEntry()
{
    Millimeter y=0;
    if (bridgeDetectionByCenter_) {
        if (bridge_ == BRIDGE_POS_CENTER || 
            bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
            y = BRIDGE_ENTRY_SIOUX_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
            y = BRIDGE_ENTRY_MIDDLE_BORDURE_Y;
        } else if (bridge_ == BRIDGE_POS_BORDURE) {
            y = BRIDGE_ENTRY_BORDURE_Y;
        }
    } else {
        if (bridge_ == BRIDGE_POS_CENTER) {
            y = BRIDGE_ENTRY_CENTER_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
            y = BRIDGE_ENTRY_MIDDLE_CENTER_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
            y = BRIDGE_ENTRY_MIDDLE_BORDURE_Y;
        } else if (bridge_ == BRIDGE_POS_BORDURE) {
            y = BRIDGE_ENTRY_BORDURE_Y;
        }
    }
    if (y<1) return false;
    if (!useLeftBridge_) {
        y = TERRAIN_Y - y; 
    }
    return gotoBridgeEntry(y);
}

// verifie si les roues avant du robot ne sont pas dans le vide
// renvoie false si la carte bumper ne repond pas ou si l'info des bumpers
// n'est pas consistante
bool StrategyAttackCL::getBridgePosByBumper(bool& bridgeInFront)
{  
    LOG_FUNCTION();
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    if (!getBridgeCaptors(captors, false)) return false; // on n'est pas dans la merde les bumpers ne marchent pas
    bridgeInFront = ((captors[BRIDGE_BUMPER_LEFT]==BRIDGE_DETECTED) &&
                     (captors[BRIDGE_BUMPER_RIGHT]==BRIDGE_DETECTED));
    return true;
}

// traverse un pont et gere les evenements possibles
bool StrategyAttackCL::crossBridge()
{
    Point tgt(BRIDGE_CROSS_BRIDGE_X, RobotPos->y());
    if (bridgeDetectionByCenter_) {
        if (bridge_ == BRIDGE_POS_CENTER || 
            bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
            tgt.y=BRIDGE_ENTRY_SIOUX_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
            tgt.y=BRIDGE_ENTRY_MIDDLE_BORDURE_Y;
        } else if (bridge_ == BRIDGE_POS_BORDURE) {
            tgt.y=BRIDGE_ENTRY_BORDURE_Y;
        }
    } else {
        if (bridge_ == BRIDGE_POS_CENTER) {
            tgt.y=BRIDGE_ENTRY_CENTER_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_CENTER) {
            tgt.y=BRIDGE_ENTRY_MIDDLE_CENTER_Y;
        } else if (bridge_ == BRIDGE_POS_MIDDLE_BORDURE) {
            tgt.y=BRIDGE_ENTRY_MIDDLE_BORDURE_Y;
        } else if (bridge_ == BRIDGE_POS_BORDURE) {
            tgt.y=BRIDGE_ENTRY_BORDURE_Y;
        }
    }
    if (!useLeftBridge_) {
        tgt.y = TERRAIN_Y - tgt.y; 
    }
    // verifie qu'on est bien aligne
    if (fabs(tgt.y-RobotPos->y()) > BRIDGE_ENTRY_MARGIN) 
        return false; 
    
    int retry=0;
    LOG_INFO("crossBridge: %d\n", (int)tgt.y);
    do {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->go2Target(tgt);
        Events->enable(EVENTS_NO_BRIDGE_BUMP_LEFT);
        Events->enable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        Events->wait(evtEndMoveBridge);
        if ((Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_LEFT) ||
             Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_RIGHT)) && RobotPos->x()<1500) {
            // le pont n'est pas la! c'est un trou!
            Move->emergencyStop();
            noBridgeHere();
            Move->backward(100);
            Events->wait(evtEndMoveNoCollision);
            return false;
        } 
        // on a reussi a traverser?
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // on s'est plante, on recule un peu, on se reoriente et on repart!
        Move->backward(100);
        Events->wait(evtEndMoveNoCollision);
        if (checkEndEvents()) return false;
        Move->rotate(0);
        Events->wait(evtEndMoveNoCollision);
        if (checkEndEvents()) return false;
    } while (retry++<5);

    // on n'a pas reussi a traverser par ici... on recule et on essaye ailleurs
    noBridgeHere();
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    tgt.x=BRIDGE_DETECT_SHARP_X;
    Move->go2Target(tgt);
    Events->wait(evtEndMoveNoCollision);
    return false;
}

