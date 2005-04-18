
#pragma once
#include "robotTypes.h"

class GridAttack
{
 public:
    typedef char GridUnit;
    typedef GridPoint GPoint;

    struct GridElement {
        DangerLevel    danger;
        bool           skittleAtBegining;
        Millisecond    obstacleTime;
        Millisecond    lastVisitTime;
        
        GridElement(): 
            danger(DANGER_NONE), 
            skittleAtBegining(false), 
            obstacleTime(TIME_NEVER), 
            lastVisitTime(TIME_NEVER) {}
    };

    GridAttack();
    virtual ~GridAttack();
    void reset();

    void setVisitTime(Millisecond time, 
                      Position pos);

    void setObstacleTime(Millisecond time, 
                         Point pt);
    void setSkittleDetected(Point pt);
    int  scoreRow(GridUnit    row, 
                  GridUnit    col, 
                  bool        dir,
                  Millisecond time);
    int  scoreCol(GridUnit    row, 
                  GridUnit    col, 
                  bool        dir,
                  Millisecond time);

    bool hasNoUnexploredSkittle();

    static GPoint getGPoint(Point const& pt);
    static Point  getPoint(GPoint const& gpt);

    static int width()  { return sizeX_; }
    static int height() { return sizeY_; }
    
 private:
    int score(GridUnit    row, 
              GridUnit    col,
              Millisecond time);
    int getIndex(GPoint & pt);
    GridElement& element(GPoint & gpt);
    GridElement& element(GridUnit i, GridUnit  j) {
        GPoint pt(i, j);
        return element(pt); 
    }
 private:
    GridElement* elements_;
    static const GridUnit sizeX_=5; 
    static const GridUnit sizeY_=7;

    static const int GRID_BEGIN_POINT_X=2144;
    static const int GRID_BEGIN_POINT_Y=0;
    static const int GRID_DELTA_POINT_X=300;
    static const int GRID_DELTA_POINT_Y=300;
};

inline GridAttack::GPoint GridAttack::getGPoint(Point const& pt)
{
    GPoint gpt;
    gpt.x=(GridUnit)(((int)pt.x-GRID_BEGIN_POINT_X)/GRID_DELTA_POINT_X);
    gpt.y=(GridUnit)(((int)pt.y-GRID_BEGIN_POINT_Y)/GRID_DELTA_POINT_Y);
    return gpt;
}

inline Point GridAttack::getPoint(GridAttack::GPoint const& gpt)
{
    Point pt;
    pt.x = (gpt.x+0.5)*GRID_DELTA_POINT_X + GRID_BEGIN_POINT_X;
    pt.y = (gpt.y+0.5)*GRID_DELTA_POINT_Y + GRID_BEGIN_POINT_Y;
    return pt;
    
}

inline int GridAttack::getIndex(GridAttack::GPoint & pt) 
{
    if (pt.x < 0) pt.x=0; 
    else if (pt.x >= sizeX_) pt.x = sizeX_-1;  
    if (pt.y < 0) pt.y=0;  
    else if(pt.y >= sizeY_) pt.y = sizeY_-1;
    return pt.y*sizeX_ + pt.x;
}

inline GridAttack::GridElement& GridAttack::element(GridAttack::GPoint & gpt)
{
    int id = getIndex(gpt);
    return elements_[id]; 
}

