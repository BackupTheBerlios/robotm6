#include "simulatorSkittle.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"
#include "simulator.h"

SimulatorGrsBall::SimulatorGrsBall() : 
  RobotBase("SimulatorGrsBall", CLASS_SIMULATOR), 
  ball_(NULL), speedX_(0), speedY_(0), isValid_(true), hasMoved_(false), newPos_(){}
    
SimulatorGrsBall::~SimulatorGrsBall() {}
  
// -------------------------------------------------------------------
// update the simulated robot position
// -------------------------------------------------------------------
void SimulatorGrsBall::updatePosition()
{ 
    if (!ball_) return;
    newPos_.center.x = ball_->center.x+speedX_;
    newPos_.center.y = ball_->center.y+speedY_;
    if (fabs(speedX_)>0.05) speedX_/=1.001;
    else speedX_ = 0;
    if (fabs(speedY_)>0.05) speedY_/=1.001;
    else speedY_ = 0;
}
void SimulatorGrsBall::setNewPositionValid()
{
    hasMoved_= (fabs(speedX_)>0.05) || (fabs(speedY_)>0.05);
    ball_->center = newPos_.center;
}
void SimulatorGrsBall::checkPosAndWall()
{
    if (!hasMoved_) return;
    // bord du terrain
    if (newPos_.center.x<BALLE_GRS_RAYON) {
        if (speedX_<0) speedX_=-speedX_;
        newPos_.center.x = BALLE_GRS_RAYON;
    } else if (newPos_.center.x > TERRAIN_X - BALLE_GRS_RAYON) {
        if (speedX_>0) speedX_=-speedX_;
        newPos_.center.x = TERRAIN_X - BALLE_GRS_RAYON;
    }
    if (newPos_.center.y<BALLE_GRS_RAYON) {
        if (speedY_<0) speedY_=-speedY_;
        newPos_.center.y = BALLE_GRS_RAYON;
    } else if (newPos_.center.y > TERRAIN_Y - BALLE_GRS_RAYON) {
        if (speedY_>0) speedY_=-speedY_;
        newPos_.center.y = TERRAIN_Y - BALLE_GRS_RAYON;
    }
}
void SimulatorGrsBall::checkPosAndBridge(BridgePosition const& bridge)
{
    if (!hasMoved_) return;
    Millimeter XRiver=(TERRAIN_X-TERRAIN_PONT_LONGUEUR)/2;
    if(SimulatorCL::instance()->isInRiver(ball_->center)) {
        // on etait dans la riviere, on y reste!
        if (newPos_.center.x<XRiver+BALLE_GRS_RAYON) {
            if (speedX_<0) speedX_=-speedX_;
            newPos_.center.x = XRiver+BALLE_GRS_RAYON;
        } else if (newPos_.center.x > TERRAIN_X - XRiver - BALLE_GRS_RAYON) {
            if (speedX_>0) speedX_=-speedX_;
            newPos_.center.x = TERRAIN_X - XRiver - BALLE_GRS_RAYON;
        }
        switch(bridge) {
        case BRIDGE_POS_UNKNOWN:
        case BRIDGE_POS_BORDURE:
            if (newPos_.center.y<(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+BALLE_GRS_RAYON
                && ball_->center.y>TERRAIN_Y/2) {
                if (speedY_<0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            } else if (newPos_.center.y>(TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON
                && ball_->center.y<TERRAIN_Y/2) {
                if (speedY_>0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            }  
        break;
        case BRIDGE_POS_MIDDLE_BORDURE:
            if (ball_->center.y<TERRAIN_CASE_LARGEUR) {
                speedY_=0;
                newPos_.center.y = BALLE_GRS_RAYON;
            } else if (ball_->center.y>TERRAIN_Y-TERRAIN_CASE_LARGEUR) {
                speedY_=0;
                newPos_.center.y = TERRAIN_Y-BALLE_GRS_RAYON;
            } else if (newPos_.center.y<(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+BALLE_GRS_RAYON
                && ball_->center.y>(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+BALLE_GRS_RAYON) {
                if (speedY_<0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            } else if (newPos_.center.y>(TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON
                && ball_->center.y<(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+BALLE_GRS_RAYON) {
                if (speedY_>0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            }  
        break;  
        case BRIDGE_POS_MIDDLE_CENTER: 
            if (ball_->center.y>TERRAIN_Y/2 && ball_->center.y<TERRAIN_Y*3/4) {
                speedY_=0;
                newPos_.center.y = (TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+BALLE_GRS_RAYON;
            } else if (ball_->center.y<TERRAIN_Y/2 && ball_->center.y>TERRAIN_Y*1/4) {
                speedY_=0;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            } else if (newPos_.center.y<(TERRAIN_Y-TERRAIN_CASE_LARGEUR)+BALLE_GRS_RAYON
                && ball_->center.y>(TERRAIN_Y-TERRAIN_CASE_LARGEUR)) {
                if (speedY_<0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-BALLE_GRS_RAYON;
            } else if (newPos_.center.y>(TERRAIN_CASE_LARGEUR-BALLE_GRS_RAYON)
                && ball_->center.y<(TERRAIN_CASE_LARGEUR)) {
                if (speedY_>0) speedY_=-speedY_;
                newPos_.center.y = TERRAIN_CASE_LARGEUR-BALLE_GRS_RAYON;
            }  
        break;  
        case BRIDGE_POS_CENTER:
            if (newPos_.center.y<(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+TERRAIN_PONT_LARGEUR+BALLE_GRS_RAYON
                && ball_->center.y>TERRAIN_Y/2) {
                if (speedY_<0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2+TERRAIN_PONT_LARGEUR+BALLE_GRS_RAYON;
            } else if (newPos_.center.y>(TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-TERRAIN_PONT_LARGEUR-BALLE_GRS_RAYON
                && ball_->center.y<TERRAIN_Y/2) {
                if (speedY_>0) speedY_=-speedY_;
                newPos_.center.y = (TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2-TERRAIN_PONT_LARGEUR-BALLE_GRS_RAYON;
            }  
        break;
        }
    } else {
        if (newPos_.center.x>XRiver && newPos_.center.x<TERRAIN_X-XRiver
            && ball_->center.x>XRiver && ball_->center.x<TERRAIN_X-XRiver) {
            // sur un pont
            if (ball_->center.y>(TERRAIN_Y-TERRAIN_CASE_LARGEUR)/2 &&
                ball_->center.y<(TERRAIN_Y)/2) {
                if (speedY_>0) speedY_=0;
                newPos_.center.y = TERRAIN_Y/2-BALLE_GRS_RAYON;
            } else if (ball_->center.y<(TERRAIN_Y+TERRAIN_CASE_LARGEUR)/2 &&
                       ball_->center.y>(TERRAIN_Y)/2) {
                if (speedY_>0) speedY_=0;
                newPos_.center.y = TERRAIN_Y/2+BALLE_GRS_RAYON;
            } else {
                Point* bridgePts=SimulatorCL::instance()->getBridgePts();
                if (ball_->center.y<TERRAIN_Y/2) {
                    // pont du bas
                    if (newPos_.center.y<bridgePts[6].y+BALLE_GRS_RAYON) {
                        if (speedY_<0) speedY_=-speedY_;
                        newPos_.center.y = bridgePts[6].y+BALLE_GRS_RAYON;
                    } else if (newPos_.center.y>bridgePts[10].y-BALLE_GRS_RAYON) {
                        if (speedY_>0) speedY_=-speedY_;
                        newPos_.center.y = bridgePts[10].y-BALLE_GRS_RAYON;
                    } 
                } else {
                    // pont du haut
                    if (newPos_.center.y<bridgePts[12].y+BALLE_GRS_RAYON) {
                        if (speedY_<0) speedY_=-speedY_;
                        newPos_.center.y = bridgePts[12].y+BALLE_GRS_RAYON;
                    } else if (newPos_.center.y>bridgePts[8].y-BALLE_GRS_RAYON) {
                        if (speedY_>0) speedY_=-speedY_;
                        newPos_.center.y = bridgePts[8].y-BALLE_GRS_RAYON;
                    }
                }
            }
        }
    }
}
// do not check collision with bigger objects, they already did it
//bool checkPosAndOtherRobot(SimulatorRobot* other);
void SimulatorGrsBall::checkPosAndGRSBall(SimulatorGrsBall* other)
{ 
}
void SimulatorGrsBall::checkPosAndSkittle(SimulatorSkittle* skittle)
{
}
void SimulatorGrsBall::setRobotCollision(Point& newPoint)
{
    newPos_.center = newPoint;
    speedX_=(newPos_.center.x-ball_->center.x)/30.;
    speedY_=(newPos_.center.y-ball_->center.y)/30.;
}

bool SimulatorGrsBall::getIntersection(Point const&   captor, 
                                     Segment const& captorVision, 
                                     Millimeter     zPosCaptor, 
                                     Point&         intersectionPt)
{
    return false;
}
