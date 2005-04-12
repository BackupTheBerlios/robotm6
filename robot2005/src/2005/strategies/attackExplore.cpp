#include "strategyAttack.h"
#include "gridAttack.h"
#include "robotTimer.h"
#include "log.h"
#include "robotPosition.h"
#include "move.h"
#include "events.h"

typedef GridAttack::GridUnit GridUnit;

bool StrategyAttackCL::basicSkittleExploration()
{
    if (lastExplorationDir_ == ATTACK_EXPLORE_COL) {
        return basicSkittleExplorationRow();
    } else {
        return basicSkittleExplorationCol();
    }
}

bool StrategyAttackCL::basicSkittleExplorationRow()
{
    lastExplorationDir_ = ATTACK_EXPLORE_ROW;
    Millisecond time = Timer->time();
    GridPoint gpt[3];
    gpt[0] = GridAttack::getGPoint(RobotPos->pt());
    
    int score[6];
    int scoreR[6];
    int bestScore=0;
    int bestLine=0;
    assert(grid_);

    for(int i=1; i<6; i++) {
        // TODO s'arreter de compter s'il y a un obstacle entre nous et la rangee
        int deltaScore=abs(i-(int)gpt[0].x)*50;
        score[i]  = grid_->scoreRow(gpt[0].x, (GridUnit)i, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreRow(gpt[0].x, (GridUnit)i, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
        
    }
    if (bestLine == 0) {
        // il n'y a pas de bonne ligne...
        if (RobotPos->x() < 2900) {
            bestLine = 4;
        } else {
            bestLine = -4;
        }
    }
    
    // point de depart de la colonne choisie
    gpt[1]=GridPoint(gpt[0].x, abs(bestLine));
    if (gpt[1].x <= 0) gpt[1].x = 1;
    if (gpt[1].x >= 4) gpt[1].x = 3;
    if (gpt[1].y <= 0) gpt[1].y = 1;
    if (gpt[1].y >= 6) gpt[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpt[2]=GridPoint(bestLine>0? 3 : 1, gpt[1].y);
    
    return false;
}

bool StrategyAttackCL::basicSkittleExplorationCol()
{
    lastExplorationDir_ = ATTACK_EXPLORE_COL;

    Millisecond time = Timer->time();
    GridPoint gpt[3];
    gpt[0] = GridAttack::getGPoint(RobotPos->pt());
    assert(grid_);

    int score[4];
    int scoreR[4];
    int bestScore=0;
    int bestLine=0;
    for(int i=1; i<4; i++) {
        int deltaScore=abs(i-(int)gpt[0].y)*50;
        score[i]  = grid_->scoreCol((GridUnit)i, gpt[0].y, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreCol((GridUnit)i, gpt[0].y, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
    }
    if (bestLine == 0) {
        // il n'y a pas de bonne ligne...
        if (RobotPos->y() < 1050) {
            bestLine = 2;
        } else {
            bestLine = -2;
        }
    } 

    // point de depart de la colonne choisie
    gpt[1]=GridPoint(abs(bestLine), gpt[0].y);
    if (gpt[1].x <= 0) gpt[1].x = 1;
    if (gpt[1].x >= 4) gpt[1].x = 3;
    if (gpt[1].y <= 0) gpt[1].y = 1;
    if (gpt[1].y >= 6) gpt[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpt[2]=GridPoint(gpt[1].x, bestLine>0? 5 : 1);
    
    return basicSkittleExplorationGrid(gpt);
}

bool StrategyAttackCL::basicSkittleExplorationGrid(GridPoint gpts[3])
{
    // TODO
    return false;
}
