#include <assert.h>

#include "strategy2005.h"
#include "robotConfig2005.h"
#include "robotMain.h"
#include "log.h"

#include "motor.h"
#include "lcd.h"
#include "alim.h"
#include "crane.h"
#include "bumper.h"
#include "env.h"
#include "skittleDetector.h"
#include "tesla.h"
#include "events.h"
#include "sound.h"
#include "robotDevices.h"
#include "movementManager.h"

extern bool evtDialogButtonReleasedFilter(bool evt[]);

// ============================================================================
// ==============================   class Strategy2005CL   ====================
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

// ---------------------------------------------------------------------------
// Strategy2005CL::checkRebootSwitch
// ---------------------------------------------------------------------------
bool Strategy2005CL::checkRebootSwitch()
{
    LOG_COMMAND("== checkRebootSwitch ==\n");
    bool reboot;
    while (true) { 
        if (!Bumper->getRebootSwitch(reboot)) return true;
        if (reboot) return true;
	LOG_WARNING("Reboot switch not set\n");
	Sound->play(SOUND_WAIT_REBOOT_SWITCH, SND_PRIORITY_URGENT);
	Lcd->print("Reboot switch\nRetry      Skip");
	Events->wait(evtDialogButtonReleasedFilter);
	Events->wait(evtRebootSwitch);
	if (Events->isInWaitResult(EVENTS_BUTTON_YES)){}
	else if (Events->isInWaitResult(EVENTS_BUTTON_NO)) return true;
	else if (Events->isInWaitResult(EVENTS_SWITCH_REBOOT)) return true;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Strategy2005CL::testMove
// ---------------------------------------------------------------------------
bool Strategy2005CL::testMove()
{
    LOG_COMMAND("== testMove ==\n");
    bool retry = true;
    while (retry) {
        Lcd->print("Test move");
        RobotPos->set(0,0,0);
        // on avance de 10cm
        Move->forward(100);
        Events->wait(evtEndMove);
        if (Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT)) {
            LOG_ERROR("TestMove:EVENTS_PWM_ALERT_LEFT\n");
            retry = menu("PWM Alert left\nRetry     Skip");
            continue;
        } else if (Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT)) {
            LOG_ERROR("TestMove:EVENTS_PWM_ALERT_RIGHT\n");
            retry = menu("PWM Alert right\nRetry     Skip");
            continue;
        } else if (dist(RobotPos->pt(), Point(100,0)) > 30) {
            LOG_ERROR("TestMove: endPos error: real %s - expected (100, 0, 0)\n",
                      RobotPos->txt());
            retry = menu("RobotPos alert\nRetry     Skip");
            continue;
        } 

        // on recule de 10cm
        Move->backward(100);
        Events->wait(evtEndMove);
        if (Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT)) {
            LOG_ERROR("TestMove:EVENTS_PWM_ALERT_LEFT\n");
            retry = menu("PWM Alert left\nRetry     Skip");
            continue;
        } else if (Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT)) {
            LOG_ERROR("TestMove:EVENTS_PWM_ALERT_RIGHT\n");
            retry = menu("PWM Alert right\nRetry     Skip");
            continue;
        } else if (dist(RobotPos->pt(), Point(0,0)) > 30) {
            LOG_ERROR("TestMove: endPos error: real %s - expected (0, 0, 0)\n",
                      RobotPos->txt());
            retry = menu("RobotPos alert\nRetry     Skip");
            continue;
        }

        retry = false;
    }
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkLcd
// -------------------------------------------------------------------------
bool Strategy2005CL::checkLcd()
{
    LOG_COMMAND("== checkLcd ==\n");
    if (!Lcd->exists()) {
        LOG_ERROR("LCD not found\n");
        Sound->play(SOUND_ALERT_1, SND_PRIORITY_URGENT);
        Sound->play(SOUND_CARTE_LCD);
	Sound->play(SOUND_NE_REPOND_PAS);
        return false;
    } else {
        return true;
    }
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkBumper
// -------------------------------------------------------------------------
bool Strategy2005CL::checkBumper()
{
    LOG_COMMAND("== checkBumper ==\n");
    if (!Bumper->exists()) {
        LOG_ERROR("Bumper device not found\n");
        Sound->play(SOUND_ALERT_1, SND_PRIORITY_URGENT);
        Sound->play(SOUND_CARTE_BUMPER);
	Sound->play(SOUND_NE_REPOND_PAS);
        Lcd->print("Bumper not ping");
        sleep(5);
        return false;
    } else {
        return true;
    }
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkMove
// -------------------------------------------------------------------------
bool Strategy2005CL::checkMove()
{
    LOG_COMMAND("== checkMove ==\n");
    RobotDevicesCL* rd=RobotDevicesCL::lastInstance();
    assert(rd);
    while(!rd->getMotor()->exists()) {
        LOG_ERROR("Move device not found\n");
        Sound->play(SOUND_CARTE_MOVE);
	Sound->play(SOUND_NE_REPOND_PAS);
        Lcd->print("Move not ping\nConnect & Reboot");
        while(1) sleep(1);
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkServo
// -------------------------------------------------------------------------
bool Strategy2005CL::checkServo()
{
    LOG_COMMAND("== checkServo ==\n");
    while(!Servo->exists()) {
        LOG_ERROR("Servo device not found\n");
        Sound->play(SOUND_CARTE_SERVO);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Servo not ping\nretry      skip")) {
	  main_->robotDevices()->rescan();
        } else {
            return false;
        }
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkEnv
// -------------------------------------------------------------------------
bool Strategy2005CL::checkEnv()
{
    LOG_COMMAND("== checkEnv ==\n");
    while(!EnvDetector->exists()) {
        LOG_ERROR("Env device not found\n");
        Sound->play(SOUND_CARTE_ENV_DETECTOR);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Env not ping\nretry      skip")) {
            // Flo todo: reping a device...
        } else {
            return false;
        }
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkCrane
// -------------------------------------------------------------------------
bool Strategy2005CL::checkCrane()
{
    LOG_COMMAND("== checkCrane ==\n");
    while(!Crane->exists()) {
        LOG_ERROR("Crane device not found\n");
        Sound->play(SOUND_CARTE_GRUE);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Grue not ping\nretry      skip")) {
            // Flo todo: reping a device...
        } else {
            return false;
        }
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkSkittleDetector
// -------------------------------------------------------------------------
bool Strategy2005CL::checkSkittleDetector()
{
    LOG_COMMAND("== checkSkittleDetector ==\n");
    while(!SkittleDetector->exists()) {
        LOG_ERROR("SkittleDetector device not found\n");
        Sound->play(SOUND_CARTE_RATEAU);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Rateau not ping\nretry      skip")) {
            // Flo todo: reping a device...
        } else {
            return false;
        }
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkTesla
// -------------------------------------------------------------------------
bool Strategy2005CL::checkTesla()
{
    LOG_COMMAND("== checkTesla ==\n");
    while(!Tesla->exists()) {
        LOG_ERROR("Tesla device not found\n");
        Sound->play(SOUND_CARTE_RATEAU);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Aimant not ping\nretry      skip")) {
            // Flo todo: reping a device...
        } else {
            return false;
        }
    }      
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::checkAlim
// -------------------------------------------------------------------------
bool Strategy2005CL::checkAlim()
{
    LOG_COMMAND("== checkAlim ==\n");
    while(!Alim->exists()) {
        LOG_ERROR("Alim device not found\n");
        Sound->play(SOUND_CARTE_ALIM);
	Sound->play(SOUND_NE_REPOND_PAS);
        if(menu("Alim not ping\nretry      skip")) {
            // Flo todo: reping a device...
        } else {
            return false;
        }
    }      
    return true;
}



// ---------------------------------------------------------------------------
// Strategy2005CL::testDevicesConnection
// ---------------------------------------------------------------------------
// Verifie que les cartes sont connectees
// ---------------------------------------------------------------------------
void Strategy2005CL::testDevicesConnection()
{
    if (RobotConfig2005->hasLcd)    checkLcd();
    if (RobotConfig2005->hasBumper) checkBumper();
    if (RobotConfig2005->hasMotor)  checkMove();
    if (RobotConfig2005->hasEnv)    checkEnv();
    if (RobotConfig2005->hasServo)  checkServo();
    if (RobotConfig2005->hasCrane)  checkCrane();
    if (RobotConfig2005->hasSkittleDetector)  checkSkittleDetector();
    if (RobotConfig2005->hasTesla)  checkTesla();
    if (RobotConfig2005->hasAlim)   checkAlim();
}

// -------------------------------------------------------------------------
// Strategy2005CL::autoCheck
// -------------------------------------------------------------------------
bool Strategy2005CL::autoCheck()
{
    LOG_COMMAND("== Auto check begin ==\n");
    testDevicesConnection();
    checkRebootSwitch();
    unlockEmergencyStop();
    return true;
}

bool Strategy2005CL::resetMotorForPrepareRobot()
{
  MvtMgr->motorReset();
  menu("Press any key\nTo asservir");
  MvtMgr->motorAsservi();
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
	Lcd->print("Jack already in\nWait jack out");
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
    Lcd->setLed(LCD_LED_RED,   LCD_LED_OFF);
    Lcd->setLed(LCD_LED_GREEN, LCD_LED_ON);
    Events->waitNot(EVENTS_JACKIN);

    Lcd->print("Go, go, go...");
    LOG_WARNING("Go, go, go...\n");
    return true;
}


