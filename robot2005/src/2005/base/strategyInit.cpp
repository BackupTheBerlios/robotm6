#include <assert.h>

#include "strategy2005.h"
#include "robotMain.h"
#include "log.h"

#include "odometer.h"
#include "lcd.h"
#include "events.h"
#include "movementManager.h"


// ============================================================================
// ==============================   class Strategy2005CL   ==========================
// ============================================================================

// -------------------------------------------------------------------------
// Strategy2005CL::unlockEmergencyStop
// -------------------------------------------------------------------------
void Strategy2005CL::unlockEmergencyStop()
{
  if (Events->check(EVENTS_EMERGENCY_STOP)) { 
	LOG_WARNING("EMERGENCY_STOP is pressed\n");
	Lcd->print("Unlock \nemergency stop");
	Sound->play(SOUND_RELACHE_ARRET_URGENCE, SND_PRIORITY_URGENT);
	MvtMgr->motorReset();
	Events->waitNot(EVENTS_EMERGENCY_STOP);
	MvtMgr->motorReset();
    }
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkLcd
// -------------------------------------------------------------------------
bool Strategy2005CL::checkLcd()
{
    LOG_COMMAND("== checkLcd ==\n");
#ifdef LSM_TODO
    if (Lcd->isSimu()) {
        LOG_ERROR("LCD not found\n");
        //SOUND->play(SOUND_ALERT_1, SND_PRIORITY_URGENT);
        //SOUND->play(SOUND_CARTE_LCD);
	//SOUND->play(SOUND_NE_REPOND_PAS);
        return false;
    } else {
        return true;
    }
#else 
    return true;
#endif
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkOdometer
// -------------------------------------------------------------------------
bool Strategy2005CL::checkOdometer()
{
    LOG_COMMAND("== checkOdometer ==\n");
    bool retry=true;
#ifdef LSM_TODO
    while(retry && ODOMETER->isSimu()) {
        LOG_ERROR("ODOMETER not found\n");
        SOUND->play(SOUND_CARTE_ODOMETER, SND_PRIORITY_URGENT);
	SOUND->play(SOUND_NE_REPOND_PAS);
        retry = HLI->menu("Odometer ping\nerr  Retry Skip");
        if (retry) UARTMGR->scanAndAlloc();
    }
    if (retry) {
        // l'odometre ping ! on le passe en mode automatique
        if (ODOMETER->setMode(ODOMETER_AUTOMATIC)) {
            ROBOT_POS->setOdometerType(ODOMETER_UART_AUTOMATIC);
        } else {
            // on n'a pas reussi a le passer en mode automatique, on 
            // utilise les hctl
            ROBOT_POS->setOdometerType(ODOMETER_MOTOR);
        }
    } else {
        // l'odometre ne ping pas, on utilise les hctl a la place
        ROBOT_POS->setOdometerType(ODOMETER_MOTOR);
    }
#endif
    return retry;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkBumper
// -------------------------------------------------------------------------
bool Strategy2005CL::checkBumper()
{
#ifdef LSM_TODO
    LOG_COMMAND("== checkBumper ==\n");
    unsigned int i;
    for(i=0; i< DRIDAQ_BUMPER_NBR; i++) {
	if (!EVENTS->isEnabled(dridaqButtonMapping_[i].evt) || 
	    !dridaqButtonMapping_[i].isActive) continue;
	bool correctValue=false;
	// on verifie que le capteur est dans le bon etat
	bool retry = true;
	while (retry && 
	       EVENTS->check(dridaqButtonMapping_[i].evt) != correctValue) {
	    SOUND->play(SOUND_BUMPER, SND_PRIORITY_URGENT);
	    if (dridaqButtonMapping_[i].evt 
		== EVENTS_BUMPER_BORDURE_RR) {
		SOUND->play(SOUND_REAR_RIGHT);
	    } else if (dridaqButtonMapping_[i].evt 
		       == EVENTS_BUMPER_BORDURE_RL) {
		SOUND->play(SOUND_REAR_LEFT);
	    } else {
		SOUND->play((SoundId)(SOUND_0+i));
	    }
	    SOUND->play(SOUND_NE_FONCTIONNE_PAS);
	    char name[32];
	    strncpy(name, dridaqButtonMapping_[i].name, 16);
	    name[16]=0;
	    if (HLI->menu("%s\nerr   Retry Skip", name)) {
		retry = true;
	    } else {
		EVENTS->disable(dridaqButtonMapping_[i].evt);
		retry = false;
	    }
	}
    }
#endif
    return true;
}

// ---------------------------------------------------------------------------
// Strategy2005CL::checkRebootSwitch
// ---------------------------------------------------------------------------
bool Strategy2005CL::checkRebootSwitch()
{
#ifdef LSM_TODO
    LOG_COMMAND("== checkRebootSwitch ==\n");
    bool retry=true;
    while (!Strategy2005CLDegraded::isMatchSwitchSet() && retry) { 
	LOG_WARNING("Reboot switch not set\n");
	SOUND->play(SOUND_WAIT_REBOOT_SWITCH, SND_PRIORITY_URGENT);
	LCD->print("Reboot switch\nRetry      Skip");
	EVENTS->wait(evtRebootSwitch);
	retry = true;
	if (EVENTS->isInWaitResult(EVENTS_BUTTON_YES)) retry = true;
	else if (EVENTS->isInWaitResult(EVENTS_BUTTON_NO))  retry = false;
	else if (EVENTS->isInWaitResult(EVENTS_SWITCH_DEGRADED)) retry = false;
    }
#endif
    return true;
}

// ---------------------------------------------------------------------------
// Strategy2005CL::testUARTCom
// ---------------------------------------------------------------------------
// Verifie que les cartes UART pinguent
// ---------------------------------------------------------------------------
void Strategy2005CL::testUARTCom()
{
    checkLcd();
    checkOdometer();
    checkBumper();
    checkRebootSwitch();
}

// ---------------------------------------------------------------------------
// Strategy2005CL::testMove
// ---------------------------------------------------------------------------
// Test les deplacements du robot
// ---------------------------------------------------------------------------
void Strategy2005CL::testMove()
{
#ifdef LSM_TODO
    LOG_COMMAND("== testMove ==\n");
    if (!ODOMETER->isSimu()) {
        Position pos = ROBOT_POS->pos();
    testMoveCmd:
        ROBOT_POS->set(0,0,0);
        HLI->menu("Push robot 15cm\nthen press a key");
        Position posEndHctl = ROBOT_POS->getHctlPosition();
        Position posEndOdom = ROBOT_POS->getOdometerPosition();
        checkBumper();
        LOG_INFO("Odom position: %d %d %d, Hctl position: %d %d %d, "
		 "theorie:150 0 0\n",
                 (int)posEndOdom.center.x, (int)posEndOdom.center.y, 
		      r2d(posEndOdom.direction),
                 (int)posEndHctl.center.x, (int)posEndHctl.center.y, 
		      r2d(posEndHctl.direction)
	    );
        if (fabs(posEndOdom.center.x) < 100 
            || fabs(posEndOdom.center.x) > 200
            || fabs(posEndOdom.center.y) > 50
            || r2d(fabs(na2PI(posEndOdom.direction, -M_PI))) > 15) {
            if (HLI->menu("Odometer error\nRetry      Skip")) {
                goto testMoveCmd;
            } else {
                ROBOT_POS->setOdometerType(ODOMETER_MOTOR);
            }
        }
        HLI->menu("Move robot back\nthen press a key");
        ROBOT_POS->set(pos.center.x, pos.center.y, pos.direction);   
    }
#endif
}



// -------------------------------------------------------------------------
// Strategy2005CL::autoCheck
// -------------------------------------------------------------------------
bool Strategy2005CL::autoCheck()
{
    LOG_COMMAND("== Auto check begin ==\n");
    

    testUARTCom();

    if (testMove_)        testMove();
    
    unlockEmergencyStop();
    
    LOG_OK("== Auto check DONE ==\n");
    
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::waitJackout
// -------------------------------------------------------------------------
bool Strategy2005CL::waitJackout()
{
    RobotConfig->disableUartAnswerRequest=true;
  
    LOG_COMMAND("== waitJackout ==\n");
    unlockEmergencyStop();
    if (Events->check(EVENTS_JACKIN)) {
	LOG_WARNING("JACK is already in, wait out\n");
	Lcd->print("Wait jack out");
	Sound->play(SOUND_WAIT_JACKOUT, SND_PRIORITY_URGENT);
	Events->waitNot(EVENTS_JACKIN);
	sleep(1);
    }
    
    LOG_COMMAND("WAIT JACKIN\n");
    Lcd->print("Wait jack in");
    Sound->play(SOUND_WAIT_JACKIN, SND_PRIORITY_URGENT);
    Events->wait(EVENTS_JACKIN);

    // on attends un peu car en general on a du mal a enfoncer
    // la jack et ca fait des faux contacts
    sleep(3);

    setStartingPosition();

    LOG_COMMAND("WAIT START\n");
    Lcd->print("Ready to start\nWait jack out");
    Sound->play(SOUND_GO_FOR_LAUNCH, SND_PRIORITY_URGENT);
    Events->waitNot(EVENTS_JACKIN);
   
    Lcd->print("Go, go, go...");
    LOG_WARNING("Go, go, go...\n");
    return true;
}


