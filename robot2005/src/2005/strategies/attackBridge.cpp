#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "geometry2D.h"
#include "bumperMapping.h"
#include "env.h"
#include "envDetectorMapping.h"

// #define USE_SHARPS_FIRST  // ancien mode ou on utilise les
// sharpspour savoir ou est le pont. si cette ligne est commentee, on
// fait juste loguer l'info des sharps

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus si il y a un fosse devant
// ----------------------------------------------------------------------------
static bool evtEndMoveBridge(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_GROUP_BRIDGE];
}


// ---------------------------------------------------------------------------
// fonction qui boucle jusqu'a ce que le robot ait traverse le pont ou
// que ce soit la fin du match 
// --------------------------------------------------------------------------
bool StrategyAttackCL::findAndCrossBridge() 
{ 
    while (true) {   
        if (gotoBridgeEntry()) {
            if (crossBridge()) {
              if (bridgeDetectionByCenter_ &&
                    (bridge_ == BRIDGE_POS_CENTER ||
                     bridge_ == BRIDGE_POS_MIDDLE_CENTER) ) {
                    // on est passe par le pont du milieu, on fait tomber les 
                    // quilles qui y etaient
                    skittleMiddleProcessed_=true;
                }
                return true;
            }
        } 
        if (checkEndEvents()) return false;
        getNearestBridgeEntry(); // met a jour bridge avec une autre
				 // position de pont a explorer
    }
}


// --------------------------------------------------------------------------
// met a jour la variable bridge_ avec la position possible du pont la plus proche
// --------------------------------------------------------------------------
void StrategyAttackCL::getNearestBridgeEntry()
{
    unsigned char currentBit = getPosBit();

    // si on a deja tout teste on passe en mode sioux, on reteste a nouveau sauf l'endroit ou on est...
    if (!(bridgeAvailibility_ & 0x0F)) {
        if (bridgeBySharps_ != BRIDGE_POS_UNKNOWN) {
	    // utilise la position des capteurs sharps et desactive les
	    // capteurs de pont
	    disableBridgeCaptors();  
	    useBridgeBumpers_ = false;
	    bridge_ = bridgeBySharps_;
	    LOG_WARNING("Nearest bridge - Stop using bumpers, use sharps! goto: %s\n", 
			BridgePosTxt(bridge_));
	    Log->bridge(bridge_);
	    return;
	} else {
	    // sioux
	    disableBridgeCaptors();  
	    bridge_ = BRIDGE_POS_CENTER;
	    bridgeDetectionByCenter_ = true;
	    LOG_WARNING("Nearest bridge - Stop using bumpers, use sioux, I AM A KAMIKAZE! goto: %s\n", 
			BridgePosTxt(bridge_));
	    Log->bridge(bridge_);
	    return;
	    // on continue de tout essayer
	    bridgeAvailibility_ = 0x1F;
	    noBridgeHere();
	}
    }
    // trouve le pont non explore le plus proche
    printf("A: Ox%2.2x 0X%2.2x\n", bridgeAvailibility_, currentBit);
    // utilise le pont le voisin de la position actuelle
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case BRIDGE_ENTRY_BORDURE_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_BORDURE_BIT;
            break;
        case BRIDGE_ENTRY_MIDDLE_BORDURE_BIT:
            currentBit=BRIDGE_ENTRY_BORDURE_BIT;
            break;
        case BRIDGE_ENTRY_MIDDLE_CENTER_BIT:
            currentBit=BRIDGE_ENTRY_CENTER_BIT;
            break;
        case BRIDGE_ENTRY_CENTER_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_CENTER_BIT;
            break;
        case BRIDGE_ENTRY_SIOUX_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_CENTER_BIT;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    // verifie que la nouvelle position a essayer n'a pas encore
    // ete essayee, si c'est le cas, on essaye le pont d'a cote
    printf("A: Ox%2.2x 0X%2.2x\n", bridgeAvailibility_, currentBit);
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case BRIDGE_ENTRY_BORDURE_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_CENTER_BIT;
            break;
        case BRIDGE_ENTRY_MIDDLE_BORDURE_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_CENTER_BIT;
            break;
        case BRIDGE_ENTRY_MIDDLE_CENTER_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_BORDURE_BIT;
            break;
        case BRIDGE_ENTRY_CENTER_BIT:
            currentBit=BRIDGE_ENTRY_MIDDLE_BORDURE_BIT;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    // verifie que la nouvelle position du pont non exploree
    printf("A: Ox%2.2x 0X%2.2x\n", bridgeAvailibility_, currentBit);
    if (!(bridgeAvailibility_ & (1<<currentBit))) {
        switch(currentBit) {
        case BRIDGE_ENTRY_MIDDLE_BORDURE_BIT:
            currentBit=BRIDGE_ENTRY_BORDURE_BIT;
            break;
        case BRIDGE_ENTRY_MIDDLE_CENTER_BIT:
            currentBit=BRIDGE_ENTRY_CENTER_BIT;
            break;
        default:
            currentBit=(currentBit+1)%4;
        }
    }
    // la on devrait avoir trouve une position non exploree
    printf("A: Ox%2.2x 0X%2.2x\n", bridgeAvailibility_, currentBit);
    switch(currentBit) {
    case BRIDGE_ENTRY_BORDURE_BIT:
        bridge_ = BRIDGE_POS_BORDURE;
        break;
    case BRIDGE_ENTRY_MIDDLE_BORDURE_BIT:
        bridge_ = BRIDGE_POS_MIDDLE_BORDURE;
        break;
    case BRIDGE_ENTRY_MIDDLE_CENTER_BIT:
        bridge_ = BRIDGE_POS_MIDDLE_CENTER;
        break;
    case BRIDGE_ENTRY_CENTER_BIT:
    case BRIDGE_ENTRY_SIOUX_BIT:
        bridge_ = BRIDGE_POS_CENTER;
        break;
    default:
        {
            bridge_ = (BridgePosition)robotRand(1, 5);
            LOG_ERROR("On n'aurait jamais du passer la!, char* y sait pas coder!\n");
        }
    }
    LOG_INFO("Nearest bridge to try: %s Availibility:0x%2.2x\n", 
             BridgePosTxt(bridge_), bridgeAvailibility_);
    Log->bridge(bridge_);
}

// --------------------------------------------------------------------------
// renvoie le bit du pont en face duquel on est le plus proche
// --------------------------------------------------------------------------
unsigned char StrategyAttackCL::getPosBit() 
{
    unsigned char currentBit = BRIDGE_ENTRY_CENTER_BIT;
    if (bridgeDetectionByCenter_ 
        && RobotPos->y() < 1365) {
        currentBit = BRIDGE_ENTRY_SIOUX_BIT;
        return currentBit;  
    } 
    if (RobotPos->y() < 1500) {
        currentBit = BRIDGE_ENTRY_CENTER_BIT;
    } else if (RobotPos->y() < 1650) {
        currentBit = BRIDGE_ENTRY_MIDDLE_CENTER_BIT;
    } else if (RobotPos->y() < 1800) {
        currentBit = BRIDGE_ENTRY_MIDDLE_BORDURE_BIT;
    } else {
        currentBit = BRIDGE_ENTRY_BORDURE_BIT ;
    } 
    return currentBit;
}

// --------------------------------------------------------------------------
// pas de pont a l'endroit ou est le robot...
// --------------------------------------------------------------------------
void StrategyAttackCL::noBridgeHere()
{
    LOG_INFO("No bridgeHere:%d\n", getPosBit());
    bridgeAvailibility_ &= (~(1<<getPosBit()));
    if (bridgeDetectionByCenter_ && getPosBit() >= BRIDGE_ENTRY_MIDDLE_CENTER_BIT) {
      bridgeAvailibility_ &= ((1<<BRIDGE_ENTRY_BORDURE_BIT) 
			      | (1<<BRIDGE_ENTRY_MIDDLE_BORDURE_BIT));
    }
    LOG_INFO("0x%2.2x\n", bridgeAvailibility_);
}

// ======================================================================
// ================ Detection de pont par les capteurs ==================
// ======================================================================


// --------------------------------------------------------------------------
// va a l'endroit ou on detecte les pont par capteurs sharps 
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeDetection(bool gotoSiouxFirst)
{
  Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
  Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
  while(true) {
      LOG_INFO("gotoBridgeDetection(%s)\n",
               gotoSiouxFirst?"Passe par le milieu":"Passe par un pont normal");
      Trajectory t;
      t.push_back(Point(RobotPos->x(), RobotPos->y()));
      // if looping (while(true)..) be careful not to go too close.
      if (RobotPos->x() < BRIDGE_DETECT_SHARP_X - 500)
	  t.push_back(Point(RobotPos->x()+250, RobotPos->y()));
      if (gotoSiouxFirst) {
          // va vers le pont du milieu
          t.push_back(Point(BRIDGE_DETECT_SHARP_X-250, BRIDGE_ENTRY_SIOUX_Y));
          t.push_back(Point(BRIDGE_DETECT_SHARP_X,     BRIDGE_ENTRY_SIOUX_Y));
      } else {
          // va vers la gauche du terrain pour detecter la position du pont d'un coup
          t.push_back(Point(BRIDGE_DETECT_SHARP_X-250, BRIDGE_ENTRY_MIDDLE_BORDURE_Y));
          t.push_back(Point(BRIDGE_DETECT_SHARP_X,     BRIDGE_ENTRY_MIDDLE_BORDURE_Y));
      }
      Move->followTrajectory(t, TRAJECTORY_RECTILINEAR, -1, 40); // gain, vitesse max
      Events->wait(evtEndMove);
      if (checkEndEvents()) return false;
      if (Events->isInWaitResult(EVENTS_MOVE_END)) {
	Move->rotate(0);
	Move->stop();

         Events->wait(evtEndMove);
	 if (checkEndEvents()) return false;
	 if (Events->isInWaitResult(EVENTS_MOVE_END)) {
	   return true;
	 }
	 return true;
      }
      if (RobotPos->x()>600 || Timer->time() > 6000) {
	// collision: on recule et on essaye de repartir par un autre endroit...
	Move->backward(150);
	Events->wait(evtEndMoveNoCollision);
	gotoSiouxFirst = !gotoSiouxFirst;
      }
      if (isZeroAngle(RobotPos->thetaAbsolute() - M_PI, M_PI_2) &&
	  RobotPos->x() < 600)
      {
	Move->backward(700);
	Events->wait(evtEndMoveNoCollision);
	gotoSiouxFirst = !gotoSiouxFirst;
      }
  }
  return false;
}

// --------------------------------------------------------------------------
// Recupere la valeur des capteurs detecteurs de pont: verifie que la valeur ne 
// varie pas. si la valeur varie la fonction retourne false
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// verifie que tous les capteurs pont disent qu'il y a un pont en position 
// de depart. Si ce n'est pas le cas: si c'est un sharp qui merde, on 
// desactive la detection sharp, si c'est un bumper un demande de reessayer,
// si l'utilisateur skip, on desactive le capteur => risque de finir dans le 
// trou
// --------------------------------------------------------------------------
bool StrategyAttackCL::testBridgeCaptors()
{
    char txt[64];
    LOG_COMMAND("== testBridgeCaptors ==\n");
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    while(true) {
        // lit la valeur des capteurs sur la carte bumper
        while(true) {
            if (Bumper->getBridgeCaptors(captors)) break;
            LOG_ERROR("Cannot get bridge informations from bumpers\n");
            if (!menu("getBridge error\nRetry      Skip")) break;
        }
	for(int i=0;i<BRIDGE_CAPTORS_NBR;i++) {
	  sprintf(txt, "%s[%d.1]=%s, ", txt,
		  i, captors[i]==BRIDGE_NO?"trou":"pont");
	}
        // teste les sharps
        if (useSharpToDetectBridge_) {
            if (bridgeDetectionByCenter_) {
                if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_NO) {
                    LOG_ERROR("bridge sharp left does not work => disable bridge "
                              "sharps!\n");
                    //useSharpToDetectBridge_ = false;
                }
            } else {
                if (captors[BRIDGE_SHARP_LEFT] == BRIDGE_NO ||
                    captors[BRIDGE_SHARP_CENTER] == BRIDGE_NO ||
                    captors[BRIDGE_SHARP_RIGHT] == BRIDGE_NO) {
                    LOG_ERROR("At least one bridge sharp(%s) does not work => "
                              "disable bridge sharps!\n", BridgePosTxt(bridge_));
                    //useSharpToDetectBridge_ = false;
                }
            }
        }
        // teste les bumpers
        if (captors[BRIDGE_BUMPER_LEFT] == BRIDGE_NO) {
            LOG_ERROR("Left bridge bumper does not work\n");
            if (menu("Bump Bridge left\nErr   Retry Skip")) continue;
            else Bumper->disableCaptor(BRIDG_BUMP_LEFT);
        }
        if (captors[BRIDGE_BUMPER_RIGHT] == BRIDGE_NO) {
            LOG_ERROR("Right bridge bumper does not work\n");
            if (menu("Bump Bridge right\nErr   Retry Skip")) continue;
            else Bumper->disableCaptor(BRIDG_BUMP_RIGHT);
        }
        return true;
    }
    return true;
}

// --------------------------------------------------------------------------
// calcule la position du pont en fonction des sharps quand on est aligne
// pour traverser sur le pont du milieu
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// calcule la position du pont en fonction des sharps quand on est en face du 
// pont BRIDGE_POS_MIDDLE_BORDURE
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// met a jour la position du pont en fonction des sharps quand on est en face du 
// pont BRIDGE_POS_MIDDLE_BORDURE ou qu'on est aligne pour traverser sur le
// pont du milieu
// --------------------------------------------------------------------------
bool StrategyAttackCL::getBridgePosBySharp()
{
    LOG_FUNCTION();
    // on reccupere la valeur des capteurs 2 fois pour voir si on n'a 
    // pas un soucis de bruit
    bridgeBySharps_ =BRIDGE_POS_UNKNOWN;
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    if (!getBridgeCaptors(captors, true)) goto endGetBridgePosBySharp;

    if (bridgeDetectionByCenter_) {
        if (!getBridgePosBySharpFromCenter(captors)) goto endGetBridgePosBySharp;
    } else {
        if (!getBridgePosBySharpFromLeft(captors)) goto endGetBridgePosBySharp;
    }
    LOG_INFO("Bridge by Sharp=%s\n", BridgePosTxt(bridge_));
    Log->bridge(bridge_);
    bridgeBySharps_ = bridge_;
#ifdef USE_SHARPS_FIRST
    return true;
#endif

 endGetBridgePosBySharp:
#ifdef USE_SHARPS_FIRST
    return false;
#else
    LOG_INFO("Do not use sharps!\n");
    // pour aller tout droit!, on update bridge_
    getNearestBridgeEntry();
    return true;
#endif
}

// --------------------------------------------------------------------------
// verifie si les roues avant du robot ne sont pas dans le vide
// renvoie false si la carte bumper ne repond pas ou si l'info des bumpers
// n'est pas consistante
// --------------------------------------------------------------------------
bool StrategyAttackCL::getBridgePosByBumper(bool& bridgeInFront)
{  
    LOG_FUNCTION();
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    if (!getBridgeCaptors(captors, false)) return false; // on n'est pas dans la merde les bumpers ne marchent pas
    bridgeInFront = ((captors[BRIDGE_BUMPER_LEFT]==BRIDGE_DETECTED) &&
                     (captors[BRIDGE_BUMPER_RIGHT]==BRIDGE_DETECTED));
    return true;
}

// --------------------------------------------------------------------------
// return true if there is no bridge here
// dummyBumperEvt return false if it must continue the movement
// --------------------------------------------------------------------------
bool StrategyAttackCL::checkBridgeBumperEvent(bool& dummyBumperEvt) 
{
    dummyBumperEvt = false;
    bool holeLeft=Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_LEFT);
    bool holeRight=Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_RIGHT);
    if (holeLeft || holeRight) {
        dummyBumperEvt = true;
	if (RobotPos->x() < 1200 || RobotPos->x()> 1700) return false;
        // le pont n'est pas la! Faut vite s'arreter!
        Move->emergencyStop();
        usleep(500000); // attend 0.5s et regarde a nouveau les bumpers pour voir 
                        // si c'etait une fausse alerte
	bool bridgeDetected=false;
	BridgePosition oldBridgePos=bridge_;
        if (getBridgePosByBumper(bridgeDetected)
            && !bridgeDetected) {
            // il n'y a pas de pont ici, c'etait vrai!
	    LOG_WARNING("No bridge here! %s, current bridge=%s, holeLeft=%s holeRight=%s\n", 
			RobotPos->txt(),  BridgePosTxt(bridge_), b2s(holeLeft), b2s(holeRight));
            noBridgeHere(); 
	    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
	    if (getBridgeCaptors(captors, false)) { // on n'est pas dans la merde les bumpers ne marchent pas
		EnvDetectorDist leftSharpBridge = ENV_NO;
		EnvDetectorDist rightSharpBridge = ENV_NO;
		if (useEnvDetectorsForBridge_) {
		    if (!EnvDetector->getEnvDetector(ENV_DETECTOR_TOP_LEFT, leftSharpBridge)) {
			LOG_ERROR("Can't get left sharp-bridge detector.\n");
			leftSharpBridge = ENV_NO;
		    } else {
			LOG_INFO("left-sharp-bridge: %d\n", leftSharpBridge);
		    }
		    if (!EnvDetector->getEnvDetector(ENV_DETECTOR_TOP_RIGHT, rightSharpBridge)) {
			LOG_ERROR("Can't get right sharp-bridge detector.\n");
			rightSharpBridge = ENV_NO;
		    } else {
			LOG_INFO("right-sharp-bridge: %d\n", leftSharpBridge);
		    }
		}
	      if (captors[BRIDGE_BUMPER_LEFT] != BRIDGE_DETECTED && leftSharpBridge <= ENV_FAR) {
		// mark bridges, that are unavailable (current bridge
		// is going to get discarded elsewhere)
		if (oldBridgePos == BRIDGE_POS_MIDDLE_BORDURE) {
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_BORDURE_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_MIDDLE_BORDURE_BIT));
		}
		if (oldBridgePos == BRIDGE_POS_MIDDLE_CENTER) {
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_BORDURE_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_MIDDLE_BORDURE_BIT));
		}
	      } else if (captors[BRIDGE_BUMPER_RIGHT] != BRIDGE_DETECTED && rightSharpBridge <= ENV_FAR) {
		if (oldBridgePos == BRIDGE_POS_MIDDLE_BORDURE) {
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_CENTER_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_MIDDLE_CENTER_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_SIOUX_BIT));
		}
		if (oldBridgePos == BRIDGE_POS_MIDDLE_CENTER) {
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_CENTER_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_MIDDLE_CENTER_BIT));
		  bridgeAvailibility_ &= (~(1<<BRIDGE_ENTRY_SIOUX_BIT));
		}
	      } else {
		  LOG_INFO("bridge seems to be here");
		  dummyBumperEvt=true;
		  return false;
	      }
	    }
	    LOG_INFO("Availibility: 0x%2.2x\n", bridgeAvailibility_);
	    dummyBumperEvt=false;
            return true;
        }
    } 
    LOG_INFO("checkBridgeBumperEvent: dummy=%s\n", b2s(dummyBumperEvt));
	    
    return false;
}

// ======================================================================
// ================     Aller vers le pont             ==================
// ======================================================================

// --------------------------------------------------------------------------
// se place en face du pont ou on peut utiliser les bumpers pour 
// savoir si le pont est bien la. La variable bridge doit deja etre mise 
// a jour 
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// se place en face du pont en y ou on peut utiliser les bumpers pour 
// savoir si le pont est bien la. 
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeEntry(Millimeter y)
{
    Move->enableAccelerationController(true);
    if (fabs(RobotPos->y() - y) < BRIDGE_ENTRY_MARGIN) {
        if (!gotoBridgeEntryEasy(y)) return false;
    } else {
        // se debrouile pour que le robot ne fasse pas un demi tour sur lui meme
        if (RobotPos->x() > BRIDGE_ENTRY_NAVIGATE_X
            && isZeroAngle(RobotPos->thetaAbsolute(), M_PI/2)) {
            MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
        } else {
            MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        }
        
        if (fabs(RobotPos->y()-y)>220) {
            if (!gotoBridgeEntryFar(y)) return false;
	} else {
            if (!gotoBridgeEntryNear(y)) return false;
	}
    }
    // utiliser les bumpers events pour savoir si on tombe dans un
    // trou...
    bool bumperDummyEvt=true;
    while(bumperDummyEvt) {
      Events->wait(evtEndMoveBridge);
      Move->enableAccelerationController(false);
      if (checkBridgeBumperEvent(bumperDummyEvt)) {
        Move->backward(100);
        Events->wait(evtEndMoveNoCollision);
        return false;
      } 
    }
    if (Events->isInWaitResult(EVENTS_MOVE_END)) 
        return gotoBridgeEntryRotateToSeeBridge();
    if (checkEndEvents()) 
        return false;
    // collision
    Move->backward(100);
    Events->wait(evtEndMoveNoCollision);
    return false;
}

// --------------------------------------------------------------------------
// commence le movement qui va vers l'entree d'un pont en face du robot
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeEntryEasy(Millimeter y)
{
    if (RobotPos->x() > BRIDGE_DETECT_BUMP_X - 30) return true;
    enableBridgeCaptors();
    LOG_COMMAND("gotoBridgeEntry Easy:%d\n", (int)y);
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(BRIDGE_DETECT_BUMP_X, y),
                    ATTACK_BRIDGE_GAIN, ATTACK_BRIDGE_SPEED);
    return true;
}

// --------------------------------------------------------------------------
// commence le movement qui va vers l'entree d'un pont tres decalle en y (>20cm)
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeEntryFar(Millimeter y)
{
    disableBridgeCaptors();
    Millimeter y2 = RobotPos->y();
    LOG_COMMAND("gotoBridgeEntry Hard Far:%d\n", (int)y);
    // on va loin: on recule bettement puit on prend un point cible
    // on s'eloigne un peu du bord, pour aller en x qui nous permet
    //de nous promener tranquillement le long de la riviere
    Move->go2Target(BRIDGE_ENTRY_NAVIGATE_X, y2, 2, 40); // norotation
    Events->wait(evtEndMove);
    if (checkEndEvents() || 
        !Events->isInWaitResult(EVENTS_MOVE_END)) {
        Move->enableAccelerationController(false);
        return false;
    }
    
    // va en face du pont
    enableBridgeCaptors();
    Trajectory t;
    // Pour eviter les rotations finales on 
    if (y2 < y)
      t.push_back(Point(BRIDGE_ENTRY_NAVIGATE_X, y-75));
    else
      t.push_back(Point(BRIDGE_ENTRY_NAVIGATE_X, y+75));
    t.push_back(Point(BRIDGE_DETECT_BUMP_X, y));
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_RECTILINEAR, 2, 30, true); // norotation
    // dont wait events, it is done by the upper function
    return true;
}

// --------------------------------------------------------------------------
// commence le movement qui va vers l'entree d'un pont juste un peu decalle en y (~15cm)
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeEntryNear(Millimeter y)
{
    Trajectory t;
    disableBridgeCaptors();
    LOG_COMMAND("gotoBridgeEntry Hard Near:%d\n", (int)y);
    // on ne va pas loin en y donc on fait un joli mouvement en S
    // met les servos en position
    Millimeter y2 = RobotPos->y();
    if (y>1800) { // pont du bord
      Move->go2Target(Point(BRIDGE_ENTRY_NAVIGATE_X, y2), 3, 30);
    } else if (y<1500) { //  pont du bord ou pont contre le milieu
      Move->go2Target(Point(BRIDGE_ENTRY_NAVIGATE_X, y2), 3, 30);
    } else { // ponts au milieu
      t.push_back(Point(BRIDGE_DETECT_BUMP_X-100, y2));
      t.push_back(Point(BRIDGE_ENTRY_NAVIGATE_X, (2*y+y2)/3)); 
      Move->followTrajectory(t, TRAJECTORY_RECTILINEAR, 3, 30);
    }
     
    Events->wait(evtEndMove);
    Move->enableAccelerationController(false);
    if (checkEndEvents()) return false; 
    if(!Events->isInWaitResult(EVENTS_MOVE_END)) {
      // collision
      Move->backward(100);
      Events->wait(evtEndMoveNoCollision);
      return false;
    }
    
    // va en face du pont
    enableBridgeCaptors();
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(BRIDGE_DETECT_BUMP_X, y, 2, 30);
    // dont need to wait, it is done by the upper function
    return true;
}

// --------------------------------------------------------------------------
// Quand on est pret du pont, le robot tourne sur la droite pour voir s'il y 
// a vraiment un pont ou non, mais il ne met qu'une roue dans le trou! Ca 
// evite de tomber
// --------------------------------------------------------------------------
bool StrategyAttackCL::gotoBridgeEntryRotateToSeeBridge()
{
    bool result=true;
    if ((bridge_ == BRIDGE_POS_BORDURE)
        || (!bridgeDetectionByCenter_ 
            && bridge_ == BRIDGE_ENTRY_CENTER_Y)) {
        // il faut faire une rotation sur une roue pour voir si le pont 
        // et la et eviter de tomber comme une merde
        LOG_COMMAND("Rotate to detect Bridge\n");
	enableBridgeCaptors();
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD); 
        Move->enableAccelerationController(true);
        Move->rotateOnWheel(-M_PI_4/1.8 , false, -1, 15);
	bool dummyBumperEvt=true;
	while(dummyBumperEvt) {
	  Events->wait(evtEndMoveBridge);
	  Move->enableAccelerationController(false);
	  if (checkBridgeBumperEvent(dummyBumperEvt)) {
            result = false;
	    break;
	  } 
	}
	if (result && bridge_ == BRIDGE_POS_BORDURE) useBridgeBumpers_=false; // on sait que le pont est la
        if (checkEndEvents()) 
            return false;
	disableBridgeCaptors();
	Move->rotateOnWheel(0 , false, 2, 30);
        Events->wait(evtEndMove);
        if (checkEndEvents()) 
            return false;
    }
    return result;
}

bool StrategyAttackCL::crossBridgeDemo() 
{
  Move->go2Target(Point(1400, 1725));
  Events->wait(evtEndMove);
  MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
  Move->go2Target(Point(1200, 1700));
  Events->wait(evtEndMove);
  MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
  Move->go2Target(Point(1400, BRIDGE_ENTRY_MIDDLE_CENTER_Y));
  Events->wait(evtEndMove);
  bridge_ = BRIDGE_POS_MIDDLE_CENTER;
  bridgeDetectionByCenter_=false;
  return true; 
}

void StrategyAttackCL::enableBridgeCaptors() {
  LOG_WARNING("enableBridgeCaptors %s\n", b2s(useBridgeBumpers_));
  if (useBridgeBumpers_) {
    Events->enable(EVENTS_NO_BRIDGE_BUMP_LEFT);
    if (bridgeDetectionByCenter_ && (bridge_ == BRIDGE_POS_MIDDLE_CENTER || bridge_ == BRIDGE_POS_CENTER)) {
      // on n'utilise pas le capteur droit si on passe en sioux
    } else {
      Events->enable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
    }
  }
}

void StrategyAttackCL::disableBridgeCaptors() {
  LOG_WARNING("enableBridgeCaptors %s\n", b2s(useBridgeBumpers_));
  Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
  Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
}

// ======================================================================
// ================          Traverser le pont         ==================
// ======================================================================

// --------------------------------------------------------------------------
// quand on est aligne avec un pont et situe a son entree, on part de l'autre 
// cote du pont. Si les bumpers de pont disent qu'il y a un trou, on recule.
// --------------------------------------------------------------------------
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
    LOG_COMMAND("crossBridge: %s\n", tgt.txt());
    Move->enableAccelerationController(true);
    do {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->go2Target(tgt);
	enableBridgeCaptors();
	bool dummyBumperEvt=true;
	while(dummyBumperEvt) {
	  Events->wait(evtEndMoveBridge);
	  Move->enableAccelerationController(false);
	  if (checkBridgeBumperEvent(dummyBumperEvt)) {
	    if (RobotPos->x()> 1600) {
	      useBridgeBumpers_=false;
	      disableBridgeCaptors();
	      dummyBumperEvt = true;
	      continue;
	    } else {
	      Move->backward(100);
	      Events->wait(evtEndMoveNoCollision);
	      return false;
	    }
	  } else if (dummyBumperEvt) {
	    Move->enableAccelerationController(true);
	    Move->go2Target(tgt);
	    enableBridgeCaptors();
	  }
	}
	if (RobotPos->x()> 1600) {
	  useBridgeBumpers_=false;
	  disableBridgeCaptors();
	}
        // on a reussi a traverser?
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // on s'est plante, on recule un peu, on se reoriente et on
        // repart!
	Move->realign(0); 
	Events->wait(evtEndMoveNoCollision);
        if (checkEndEvents()) return false;
        Move->backward(100);
	// Events->wait(evtEndMoveNoCollision);
	// if (checkEndEvents()) return false;
	// Move->rotate(0);
        Events->wait(evtEndMoveNoCollision);
        if (checkEndEvents()) return false;
    } while (retry++<5);

    // on n'a pas reussi a traverser par ici... on recule et on essaye ailleurs
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    tgt.x=BRIDGE_DETECT_SHARP_X;
    Move->go2Target(tgt);
    Events->wait(evtEndMoveNoCollision);
    return false;
}
