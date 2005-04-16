#include "strategyAttack.h"
#include "gridAttack.h"
#include "robotTimer.h"
#include "log.h"
#include "robotPosition.h"
#include "move.h"
#include "movementManager.h"
#include "events.h"
#include "geometry2D.h"

typedef GridAttack::GridUnit GridUnit;

// --------------------------------------------------------------------------
// Choisi une des 2 trajectoires predefinie et l'execute jusqu'� collision
// --------------------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration()
{
    bool result = false;
    if (RobotPos->y() > ATTACK_EXPLORE_METHOD_Y) {
        result = preDefinedSkittleExploration1();
    } else {
        result = preDefinedSkittleExploration2();
    }
    if (result) {
        LOG_OK("God Bless Odometry!\n");
        // si on arrive la je me coupe une...
        // vu qu'on a tout fait tomber sans probleme, on peut bien aller 
        // faire tomber les quilles du milieu
        result = killCenterSkittles();
    }
    return result;
}

// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1650
// ---------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration1()
{
    LOG_FUNCTION();
    Trajectory t;
    t.push_back(Point(2640, 1650)); 
    t.push_back(Point(3240, 1650)); 
    t.push_back(Point(3190, 650)); 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1100));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    t.clear();
    t.push_back(Point(2550, 1050)); 
    t.push_back(Point(2594, 1400)); 
    t.push_back(Point(3340, 1350)); 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1350));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    return true;
}

// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1350
// ---------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration2()
{
    LOG_FUNCTION();
    Trajectory t;
    t.push_back(Point(2640, 1350)); 
    t.push_back(Point(3240, 1350)); 
     MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    // on recule un petit peu car on ne sais pas ce qu'on va se prendre en
    // approchant du robot adverse!, mieux vaut tenir que courir
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1700));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    // on va droit sur l'adversaire
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(3190, 650));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1100));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    t.clear();
    t.push_back(Point(2550, 1050)); 
    t.push_back(Point(2594, 1700)); 
    t.push_back(Point(3340, 1650)); 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1650));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    return true;
}

// --------------------------------------------------------------------------
// fonction a appeler en boucle: elle explore le terrain dans une direction 
// puis quand on la rapelle elle explore le terrain dans une autre direction 
// perpendiculaire a la precedente
// --------------------------------------------------------------------------
bool StrategyAttackCL::basicSkittleExploration()
{
    updateGrid();
    GridPoint gpts[3];
    if (lastExplorationDir_ == ATTACK_EXPLORE_COL) {
        basicSkittleExplorationRow(gpts);
    } else {
        basicSkittleExplorationCol(gpts);
    }
    static char gptsTxt[128];
    strcpy(gptsTxt, "Exploration points");
    for(int i=0;i<3;i++) {
        sprintf(gptsTxt, "%s, gpts[%d]=(%d %d)", gptsTxt, i, gpts[i].x, gpts[i].y); 
    }
    LOG_INFO(gptsTxt);
    return exploreGrid(gpts);
}

// --------------------------------------------------------------------------
// genere 3 points qui permettent d'explorer une ligne (en x)
// gpts[0]=centre de la case la plus proche du robot
// gpts[1]=point ou il faut une rotation de 90degre
// gpts[2]=point cible au bout le la ligne commencant en gpts[1]
// --------------------------------------------------------------------------
void StrategyAttackCL::basicSkittleExplorationRow(GridPoint gpts[3])
{
    lastExplorationDir_ = ATTACK_EXPLORE_ROW;
    Millisecond time = Timer->time();
    gpts[0] = GridAttack::getGPoint(RobotPos->pt());
    
    int score[6];
    int scoreR[6];
    int bestScore=0;
    int bestLine=0;
    assert(grid_);

    for(int i=1; i<6; i++) {
        // TODO s'arreter de compter s'il y a un obstacle entre nous et la rangee
        int deltaScore=abs(i-(int)gpts[0].x)*50;
        score[i]  = grid_->scoreRow(gpts[0].x, (GridUnit)i, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreRow(gpts[0].x, (GridUnit)i, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
        
    }
    if (bestLine == 0) {
        // il n'y a pas de bonne ligne...
        // TODO: ne pas prendre la meme que la derniere fois
        if (RobotPos->x() < 2900) {
            bestLine = 4;
        } else {
            bestLine = -4;
        }
    }
    
    // point de depart de la colonne choisie
    gpts[1]=GridPoint(gpts[0].x, abs(bestLine));
    if (gpts[1].x <= 0) gpts[1].x = 1;
    if (gpts[1].x >= 4) gpts[1].x = 3;
    if (gpts[1].y <= 0) gpts[1].y = 1;
    if (gpts[1].y >= 6) gpts[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpts[2]=GridPoint(bestLine>0? 3 : 1, gpts[1].y);
}

// --------------------------------------------------------------------------
// genere 3 points qui permettent d'explorer une colonne (en y)
// gpts[0]=centre de la case la plus proche du robot
// gpts[1]=point ou il faut une rotation de 90degre
// gpts[2]=point cible au bout le la collone commencant en gpts[1]
// --------------------------------------------------------------------------
void StrategyAttackCL::basicSkittleExplorationCol(GridPoint gpts[3])
{
    lastExplorationDir_ = ATTACK_EXPLORE_COL;

    Millisecond time = Timer->time();
    gpts[0] = GridAttack::getGPoint(RobotPos->pt());
    assert(grid_);

    int score[4];
    int scoreR[4];
    int bestScore=0;
    int bestLine=0;
    for(int i=1; i<4; i++) {
        int deltaScore=abs(i-(int)gpts[0].y)*50;
        score[i]  = grid_->scoreCol((GridUnit)i, gpts[0].y, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreCol((GridUnit)i, gpts[0].y, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
    }
    if (bestLine == 0) {
        // il n'y a pas de bonne colonne, on prend celle du milieu
        // TODO il ne faudrait pas prendre la meme que la derniere fois
        if (RobotPos->y() < 1050) {
            bestLine = 2;
        } else {
            bestLine = -2;
        }
    } 

    // point de depart de la colonne choisie
    gpts[1]=GridPoint(abs(bestLine), gpts[0].y);
    if (gpts[1].x <= 0) gpts[1].x = 1;
    if (gpts[1].x >= 4) gpts[1].x = 3;
    if (gpts[1].y <= 0) gpts[1].y = 1;
    if (gpts[1].y >= 6) gpts[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpts[2]=GridPoint(gpts[1].x, bestLine>0? 5 : 1);
}

// ------------------------------------------------------------------------
// gere tous les deplacement pour suivre la trajectoire definie par les 3 
// points de passage
// ------------------------------------------------------------------------
bool StrategyAttackCL::exploreGrid(GridPoint gpts[3])
{
    bool needMoveBackward=false;
    if (!gotoGtps1(gpts)) 
        return false;
    if (!rotateOnGtps1(gpts)) 
        return false;
    if (!backBeforeGpts2(gpts)) 
        return false;
    if (!gotoGpts2(gpts, needMoveBackward)) 
        return false;
    if (needMoveBackward) {
        if (!goBackToGpts2(gpts)) 
            return false;
    }
    return true;
}

// ------------------------------------------------------------------------
// On va jusqu'au point gpts[1]
// ------------------------------------------------------------------------
bool StrategyAttackCL::gotoGtps1(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // --- le robot est en gpts[0], il faut aller vers gpts[1] ---
    Point pt1 = GridAttack::getPoint(gpts[1]); // conversion en mm
    // trouver le meilleur sens pour aller en pt1, il faut eviter de
    // faire un demi tour
    Radian dirPt1 = dir(RobotPos->pt(), pt1);
    if (isZeroAngle(RobotPos->thetaAbsolute() - dirPt1, 
                    M_PI/2)) {
        // il faut aller vers l'avant
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    } else {
        // il faut aller vers l'arriere
        MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    }

    // decaller le point d'arrivee pour etre sur d'etre au centre du 
    // case car on s'arrete toujours trop tot
    pt1.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt1);
    pt1.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt1);
    Move->go2Target(pt1);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    // TODO: manage events collision...
    return false;
}

// ---------------------------------------------------------------------
// On tourne autour du point gpts[1] pour s'orienter vers gpts[2]
// ---------------------------------------------------------------------
bool StrategyAttackCL::rotateOnGtps1(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // --- on est en pt1, tourner dans la direction de pt2 ---
    Point pt2 = GridAttack::getPoint(gpts[2]); // conversion en mm
    Radian dirPt2 = dir(RobotPos->pt(), pt2);
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->rotate(dirPt2);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    // TODO: manage events collision...
    return false;
}

// ---------------------------------------------------------------------
// On recule un peu pour visiter la case qui est derriere gpts[1]
// ---------------------------------------------------------------------
bool StrategyAttackCL::backBeforeGpts2(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // recule un peu a condition de ne pas risquer de s'envoyer dans le fosse
    // ou de ne pas foncer dans les robots adverses en debut de match
    Point targetPoint=RobotPos->pt();
    bool needMoveBackward = false;
    if (gpts[1].x > gpts[2].x) {
        targetPoint.x += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y > gpts[2].y) {
        targetPoint.y += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y < gpts[2].y) {
        if (attackPhase_ == ATTACK_GRAND_MENAGE) {
            targetPoint.y -= TERRAIN_CASE_LARGEUR;
            needMoveBackward = true;
        }
    }
    if (needMoveBackward) {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
        Move->go2Target(targetPoint);
        Events->wait(evtEndMove);
        // on a reussi ?
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO: manage events collision...
    } 
    return true;
}


// ---------------------------------------------------------------------
// On va vers gpts[2] et eventuellement on va une case plus loin pour 
// faire tomber les quilles qui sont pres des bords
// ---------------------------------------------------------------------
bool StrategyAttackCL::gotoGpts2(GridPoint gpts[3],
                               bool &needMoveBackward)
{
    LOG_FUNCTION();
    // on regard si on ne peut pas aller une plus loin que le point d'arrivee
    Point targetPoint=GridAttack::getPoint(gpts[2]);
    needMoveBackward = false;
    if (gpts[1].x < gpts[2].x) {
        targetPoint.x += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y < gpts[2].y) {
        targetPoint.y += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y > gpts[2].y) {
        if (attackPhase_ == ATTACK_GRAND_MENAGE) {
            targetPoint.y -= TERRAIN_CASE_LARGEUR;
            needMoveBackward = true;
        }
    } 
    if (!needMoveBackward) {
        // si on n'aura pas a reculer a la fin du mouvement
        // decaller le point d'arrivee pour etre sur d'etre au centre du 
        // case car on s'arrete toujours trop tot
        Radian dirPt2 = dir(RobotPos->pt(), targetPoint);
        targetPoint.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt2);
        targetPoint.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt2);
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(targetPoint);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    return false;
}
// ---------------------------------------------------------------------
// On a depasse Gpts2, on recule pour aller sur gpts2
// ---------------------------------------------------------------------
bool StrategyAttackCL::goBackToGpts2(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // on est alle trop loin, il faut reculer
    Point targetPoint = GridAttack::getPoint(gpts[2]);
    Radian dirPt2 = dir(RobotPos->pt(), targetPoint);
    // decaller le point d'arrivee pour etre sur d'etre au centre du 
    // case car on s'arrete toujours trop tot
    targetPoint.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt2+M_PI);
    targetPoint.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt2+M_PI);
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(targetPoint);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    return false;
}

// ------------------------------------------------------------------------
// enlever les zones dangereuses de match si on est passe sur toutes 
// les zones de quilles de depart
// ------------------------------------------------------------------------
void StrategyAttackCL::updateGrid()
{
    if ((attackPhase_ != ATTACK_GRAND_MENAGE) &&
        ((Timer->time() >= ATTACK_TIME_EXPLORE_ALL) ||
         (grid_ && 
          grid_->hasNoUnexploredSkittle()))) {
        setAttackPhase(ATTACK_GRAND_MENAGE);
    }
}