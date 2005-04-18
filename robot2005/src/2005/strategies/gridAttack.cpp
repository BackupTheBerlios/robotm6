#include "gridAttack.h"

const GridAttack::GridUnit GridAttack::sizeX_; 
const GridAttack::GridUnit GridAttack::sizeY_;

const int GridAttack::GRID_BEGIN_POINT_X;
const int GridAttack::GRID_BEGIN_POINT_Y;
const int GridAttack::GRID_DELTA_POINT_X;
const int GridAttack::GRID_DELTA_POINT_Y;

GridAttack::GridAttack() : elements_(NULL) 
{
    elements_ = new GridElement[sizeX_*sizeY_];
    reset();
}

GridAttack::~GridAttack() 
{
    if (elements_) delete[] elements_;
}

void GridAttack::reset()
{
    for(int i=0; i<sizeX_*sizeY_; i++) {
        elements_[i] = GridElement();
    }
    element(1,5).skittleAtBegining=true;
    element(2,5).skittleAtBegining=true;
    element(3,5).skittleAtBegining=true;
    element(1,4).skittleAtBegining=true;
    element(2,4).skittleAtBegining=true;
    element(3,4).skittleAtBegining=true;
    element(2,3).skittleAtBegining=true;
    element(3,3).skittleAtBegining=true;
    element(3,2).skittleAtBegining=true;
}

void GridAttack::setVisitTime(Millisecond time, 
                              Position pos)
{
    GPoint pt=getGPoint(pos.center);
    GridElement& elem=element(pt);
    elem.lastVisitTime=time; 
    elem.skittleAtBegining=false;
}

void GridAttack::setObstacleTime(Millisecond time, 
                                 Point pt)
{
    GPoint gpt=getGPoint(pt);
    element(gpt).obstacleTime=time;
}

void GridAttack::setSkittleDetected(Point pt)
{
    GPoint gpt=getGPoint(pt);
    element(gpt).skittleAtBegining=true;
}

int GridAttack::scoreRow(GridUnit    row, 
                         GridUnit    col, 
                         bool        dir,
                         Millisecond time)
{
    int score=0;
    GridUnit i=row;
    while(true) {
        dir?(i++):(i--);
        if (i <= 0 || i >= sizeX_-1) break;
        int scr = GridAttack::score(i, col, time);
        if (scr<0) break; // on s'arrete si on tombe sur un obstacle
        else score += scr;
    }
    return score;
}

int GridAttack::scoreCol(GridUnit    row, 
                         GridUnit    col, 
                         bool        dir,
                         Millisecond time)
{
    int score=0;
    GridUnit i=col;
    while(true) {
        dir?(i++):(i--);
        if (i <= 0 || i >= sizeY_-1) break;
        int scr = GridAttack::score(row, i, time);
        if (scr<0) break; // on s'arrete si on tombe sur un obstacle
        else score += scr;
    }
    return score;
}

int GridAttack::score(GridUnit    row, 
                      GridUnit    col,
                      Millisecond time)
{
    GridElement const& elem = element(row, col);
    if (elem.skittleAtBegining) return 100;
    if (elem.obstacleTime > 0 && 
        time - elem.obstacleTime < 20000) return -1;
    if (elem.lastVisitTime > 0) 
        return ((time - elem.lastVisitTime)/900)/(elem.danger+1);
    else 
        return 90/(elem.danger+1);    
}

bool GridAttack::hasNoUnexploredSkittle()
{
    for(int i=1;i<4;i++) {
        for(int j=2;j<6;j++) {
            if (element(i,j).skittleAtBegining) return false;
        }
    }
    return true;
}
