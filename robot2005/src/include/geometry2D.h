/**
 * @file geometry2D.h
 *
 * This file defines 2D geometric functions for points, lines and circles,
 * segments
 *
 * @author Laurent Saint-Marcel
 * @date   2003/01/01
 */

#ifndef __GEOMETRY_2D_H__
#define __GEOMETRY_2D_H__

#include "robotTypes.h" // Point, Millimeter, Types 
#include <math.h> // M_PI, fabs, cos, sin, atan

#define na2PI(angle, start) Geometry2D::normalizeAngle2PI(angle, start)
#define naPI(angle, start)  Geometry2D::normalizeAnglePI(angle, start)
/** @brief return true if the angle is in the 0 direction +/- margin */
bool isZeroAngle(Radian angle, Radian margin);

typedef enum LineType {
  LINE_NORMAL,
  LINE_VERTICAL,
  LINE_HORIZONTAL
} LineType;

typedef bool (*TriangulationValidZoneFn)(Point);

/**
 * type = LINE_NORMAL     => y=a*x+b
 * type = LINE_VERTICAL   => x=b
 * type = LINE_HORIZONTAL => y=b
 */

/**
 * @class Line
 * @brief Structure pour definir une ligne
 */
class Line {
 public:
  LineType type;
  double   a;
  double   b;

  Line():type(LINE_HORIZONTAL), a(0), b(0){}
  Line(Point const& v1, Point const& v2);
  Line(Millimeter const x1, Millimeter const y1, 
       Millimeter const x2, Millimeter const y2);
  Line(Point const& pt, Radian const theta);

  void print() const;
};

/**
 * @class Segment
 * @brief Structure pour definir un segment
 */
class Segment : public Line {
 public:
  Point lowerLeft, upperRight;
  Segment():Line(){}
  Segment(Point const& v1, Point const& v2); 
  void print() const;
};

/**
 * @class Polygon
 * @brief Structure pour definir un polygon
 */
class Polygon {
 public:
  /** Points must be order in clockwise order */
  Polygon(Point* pts, int nbrPts);
  Polygon(Polygon const& p);
  Polygon():nbrPts(0), pts(NULL){}
  virtual ~Polygon();
  Polygon& operator=(Polygon const& p);
  
  int nbrPts;
  Point* pts;
  
  void print() const;
};

/**
 * @class Circle
 * @brief Structure pour definir un cercle
 */
class Circle {
 public:
  Point      center;
  Millimeter radius;

  Circle():radius(0){}
  Circle(Point const& center, Millimeter const radius);
  Circle(Millimeter const centerX, Millimeter const centerY, Millimeter const radius);
  
  void print() const;
};

/**
 * @class Rectangle
 * @brief Structure pour definir un rectangle
 */
class Rectangle {
 public:
  Point  pt[4];

  Rectangle(){}
  Rectangle(Point const& center, 
            Radian const widthDir,
            Millimeter const width, Millimeter const height);
  Rectangle(Millimeter const centerX, Millimeter const centerY, 
            Radian const widthDir,
            Millimeter const width, Millimeter const height);
  Rectangle(Point const& topLeft, Point const& BottomRight);

  void print() const;
};

/**
 * @namespace Geometry2D
 * Utility function to to do 2D computation
 */
namespace Geometry2D {
    const Radian     MM_PI                = (Radian)(M_PI);
    const Radian     MM_2_PI              = (Radian)(MM_PI*2.);
    const Radian     MM_PI_2              = (Radian)(MM_PI/2.);
    const Radian     MM_PI_3              = (Radian)(MM_PI/3.);
    const Radian     ANGLE_DIAGONALE      = (Radian)(0.96007036240568800268);
    const Millimeter GEOM_PRECISION       = Millimeter(0.0001);
    const Millimeter GEOM_SUPER_PRECISION = GEOM_PRECISION*GEOM_PRECISION;

/**
 * @brief This function returns the distance™ between 2 points
 */
Millimeter getSquareDistance(Point const& p1, Point const& p2);
    
/**
 * @brief This function returns an angle between mini and mini+2*pi
 */
Radian normalizeAngle2PI(Radian const angle, Radian const mini);
    
/**
 * @brief This function returns an angle between mini and mini+pi
 */
Radian normalizeAnglePI(Radian const angle, Radian const mini);

/**
 * @brief This function returns true if A is on right of vector (pt1 to pt2)
 */
bool isPointOnRight(Point const& pt1, Point const& pt2, 
                    Point const& A);

/**
 * @brief This function returns true if A is on left of vector (pt1 to pt2)
 */
bool isPointOnLeft(Point const& pt1, Point const& pt2, 
                   Point const& A);

/**
 * @brief This function returns true if A is on segment (pt1 to pt2)
 */
bool isPointOnSegment(Point const& pt1, Point const& pt2, 
                      Point const& A);

/**
 * @brief return true if A is in the rectangle defined by s1, s2
 */
bool isPointInRectangle(Point const& s1, Point const& s2, 
                        Point const& A);

/**
 * @brief This function returns pt1+a*(pt2-pt1)
 */
Point getPointWithCoef(Point const& pt1, Point const& pt2, double const a);

/**
 * @brief This function returns the point between pt1 and pt2
 */
Point getPointMiddle(Point const& pt1, Point const& pt2);

/**
 * @brief This function returns the baricenter of nbrPt points
 */
Point getBaricenter(Point const* pt, int const nbrPt);

/**
 * @brief This function returns true if points are aligned
 */
bool arePointsCollinear(Point const& pt1, Point const& pt2, Point const& pt3);


/**
 * @brief This function return the line that is orthogonal to line l and that pass through point pt
 */
Line getLineOrthogonal(Line const& l, Point const& pt);

/**
 * @brief This function computes the intersection between 2 lines
 *
 * @param l1 Line 1
 * @param l2 Line 2
 * @param pt Intersection point that will be updated
 * @return true if the intersection exist
 */
bool getLinesIntersection(Line const& l1, Line const& l2, Point &pt);

/**
 * @brief This function returns the projection of point pt on line l
 */
Point getOrthogonalProjection(Line const& l, Point const& pt);

/**
 * @brief This function returns a point which is on line l, and at distance dist from 
 * pt (which must also be on l)
 */
Point getPointOnLine(Line const& l, Point const& pt, double d);

bool getPointOnLineX(Line const& l, Millimeter const x, Point &pt);

bool getPointOnLineY(Line const& l, Millimeter const y, Point &pt);

void getPointsOnLine(Line const& l, Point const& ptSrc, Millimeter const dist, 
                     Point &pt1, Point &pt2);

/**
 * @brief This function returns a point which is on vector (A,B), and at 
 * distance d from A
 */
Point getPointOnVector(Point const& A, Point const& B,  Millimeter const d);

/**
 * @brief This function computes the intersection between 2 segments
 *
 * @param s1 segment 1
 * @param s2 segment 2
 * @param pt Intersection point that will be updated
 * @return true if the intersection exist
 */
bool getSegmentsIntersection(Segment const& s1, Segment const& s2, Point &pt);

/**
 * @brief This function computes the intersection between a segment and a 
 * circle
 *
 * @param s segment
 * @param c Circle
 * @param pt1 first intesection point if it exists
 * @param pt2 second intersection point if it exists (if only one point, 
 *        pt2=pt1)
 * @return the number of intersection points:0,1,2. If 1 then pt1 is the 
 *         only valid point
 */
int  getSegmentCircleIntersection(Segment const& s, 
				  Circle const& c, 
				  Point &pt1, Point &pt2);
/**
 * @brief This function computes the intersection between 2 circles
 *
 * @param c1 Circle 1
 * @param c2 Circle 2
 * @param pt1 first intesection point if it exists
 * @param pt2 second intersection point if it exists (if only one point, pt2=pt1)
 * @return true if the intersection exist, false in the other case
 */
bool getCirclesIntersection(Circle const& c1, Circle const& c2, 
                            Point &pt1, Point &pt2);

/**
 * @brief This function returns true if the point is in the circle
 */
bool isPointInCircle(Point const& pt, Circle const& c);

/**
 * @brief This function computes the intersection between a line and a circle
 *
 * @param l Line
 * @param c Circle
 * @param pt1 first intesection point if it exists
 * @param pt2 second intersection point if it exists (if only one point,
 *        pt2=pt1)
 * @return true if the intersection exist, false in the other case
 */
bool getLineCircleIntersection(Line const& l, Circle const& c, 
                               Point &pt1, Point &pt2);

/**
 * @brief This function computes the intersection of a rectangle and and 
 * circle. It returns true if the intersection exist
 */
bool getRectangleCircleIntersection(Rectangle const& r, Circle const& c, 
                                    Point pt[8], int &nbrpt);

/**
 * @brief This function computes the intersection of a rectangle and a line
 * If there is only one intersection point, pt[0]=pt[1]
 */
bool getLineRectangleIntersection(Line const& l, Rectangle const& r,
                                  Point pt[2]);

/**
 * @brief This function computes the intersection of 2 rectangles
 */
bool getRectanglesIntersection(Rectangle const& r1, Rectangle const& r2,
                               Point pt[8], int &nbrpt);


/**
 * @brief This function returns true if the point is in the rectangle
 * 
 * Be sure that the rectangle's points are antitrigonometrically ordered!
 */
bool isPointInRectangle(Point const& pt, Rectangle const& r);

/**
 * @brief This function returns true if the point is in a circle
 */
bool isPointInCircle(Point const& pt, Circle const& c);

/**
 * @brief This function returns true if the point is inside the polygon
 */
bool isPointInPolygon(Polygon const& p1, Point const& pt1);

/**
 * @brief This function returns true if the circle is inside the polygon or 
 *        if it intersect it
 */
 bool isCircleInPolygon(Polygon const& p1, Circle const& c1);

// ---------------------------------------------------------------------------
// TOOLS
// ---------------------------------------------------------------------------

/**
 * @brief This function computes the position of a gonio
 *        from the direction of 3 targets
 */
bool triangulation(Point const ptBalise1,  Point const ptBalise2,  Point const ptBalise3,
                   Radian const dirBalise1, Radian const dirBalise2, Radian const dirBalise3,
                   Point &ptGonio, Radian &dirGonio,
                   TriangulationValidZoneFn zoneFn);

/**
 * @brief This function returns the point which is on a spline of trajectory
 * at position t. t = [0..trajectory.size()-1]
 */
void spline(Trajectory const& trajectory,
            double            t,
            Point           & point);
/**
 * @brief This function returns the point which is on a segment of trajectory
 * at position t. t = [0..trajectory.size()-1]
 */
void rectilinear(Trajectory const& trajectory,
                 double            index,
                 Point           & point);

/**
 * Conversion d'un point en coordonnees cylindriques
 */
Point& convertToCylindricCoord(Point const& center, 
                               Point&       pt);
/**
 * Conversion d'un point en coordonnees orthogonales
 */
Point& convertToOrthogonalCoord(Point const& center, 
                                Radian       direction,
                                Point&       pt);
/**
 * @brief this function tests all functions of this lib
 */
bool validate();

}

// ---------------------------------------------------------------------------
// isZeroAngle
// ---------------------------------------------------------------------------
inline bool isZeroAngle(Radian angle, Radian margin) {
    return (fabs(na2PI(angle, -M_PI)) < fabs(margin));
}

inline Millimeter minDist(Millimeter a, Millimeter b) {
    if (a < 0) return b; // INFINITE_DIST
    else if (b < 0) return a;
    else return (a<b) ? a:b;
}

#endif // __GEOMETRY_2D_H__
