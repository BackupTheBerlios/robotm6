/**
 * @file geometry2D.cpp
 *
 * This file defines geometric functions for points, lines and circles
 */

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "geometry2D.h"

namespace {
    
    // ---------------------------------------------------------------------
    // validZone
    // --------------------------------------------------------------------- 
    bool validZone(Point pt)
    {
        return (pt.x >= 0);
    }

    // ---------------------------------------------------------------------
    // getLineCircleIntersection2
    // --------------------------------------------------------------------- 
    bool getLineCircleIntersection2(Line const& l, Circle const& c, 
                                    Point const& centerProjection,
                                    Point &pt1, Point &pt2)
    {
        double dist2;
        double d2;
        double r2 = c.radius*c.radius;
        dist2 = Geometry2D::getSquareDistance(centerProjection, c.center);
        if (dist2 > r2) return false;
        d2 = sqrt(r2 - dist2);
        pt1 = Geometry2D::getPointOnLine(l, centerProjection, d2);
        pt2 = Geometry2D::getPointOnLine(l, centerProjection, -d2);
        return true;
    }
}

// ---------------------------------------------------------------------
// Point functions
// ---------------------------------------------------------------------

/**
 * @brief This function returns the square of the distance between p1 and p2
 */
Millimeter Geometry2D::getSquareDistance(Point const& p1, Point const& p2)
{
    return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
}

/**
 * @brief This function returns an angle between mini and mini+2*pi
 */
Radian Geometry2D::normalizeAngle2PI(Radian const angle, Radian const mini)
{
  Radian maxi = mini + MM_2_PI;
  Radian result=angle;
  while(result<mini) result += MM_2_PI;
  while(result>=maxi) result -= MM_2_PI;
  return result;
}

/**
 * @brief This function returns an angle between mini and mini+pi
 */
Radian Geometry2D::normalizeAnglePI(Radian const angle, Radian const mini)
{
  Radian maxi = mini + MM_PI;
  Radian result=angle;
  while(result<mini) result += MM_PI;
  while(result>=maxi) result -= MM_PI;
  return result;
}

/**
 * @brief This function returns true if A is on right of vector (pt1 to pt2)
 */
bool Geometry2D::isPointOnRight(Point const& pt1, Point const& pt2, 
                                Point const& A)
{
  double v12x,v12y,v13x,v13y;
  v12x = pt2.x-pt1.x;
  v12y = pt2.y-pt1.y;
  v13x = A.x-pt1.x;
  v13y = A.y-pt1.y;
  if(v12x*v13y-v13x*v12y < 0) return true;
  else return false;
}

/**
 * @brief This function returns true if A is on left of vector (pt1 to pt2)
 */
bool Geometry2D::isPointOnLeft(Point const& pt1, Point const& pt2, 
                               Point const& A)
{
  double v12x,v12y,v13x,v13y;
  v12x = pt2.x-pt1.x;
  v12y = pt2.y-pt1.y;
  v13x = A.x-pt1.x;
  v13y = A.y-pt1.y;
  if(v12x*v13y-v13x*v12y > 0) return true;
  else return false;
}

/**
 * @brief This function returns the baricenter of nbrPt points
 */
Point Geometry2D::getBaricenter(Point const* pt, int const nbrPt)
{
  Point out;
  int i;
  assert(pt);
  out.x=0;
  out.y=0;
  for(i=0;i<nbrPt;++i){
    out.x+=pt[i].x;
    out.y+=pt[i].y;
  }
  out.x/=nbrPt;
  out.y/=nbrPt;
  return out;
}

// ---------------------------------------------------------------------
// class Line
// ---------------------------------------------------------------------

/**
 * @brief This function creates a line
 */
Line::Line(Millimeter const x1, Millimeter const y1, 
           Millimeter const x2, Millimeter const y2) : 
    type(LINE_NORMAL), a(0), b(0)
{
   Line l2(Point(x1,y1), Point(x2,y2));
   type=l2.type;
   a=l2.a;
   b=l2.b;
}

/**
 * @brief This function creates a line
 */
Line::Line(Point const& pt, Radian const theta) : 
    type(LINE_NORMAL), a(0), b(0)
{
  Line l2(pt.x, pt.y, 
          pt.x+cos(theta), pt.y+sin(theta));
  type=l2.type;
  a=l2.a;
  b=l2.b;
}

/**
 * @brief This function creates a line
 */
Line::Line(Point const& v1, Point const& v2) : 
    type(LINE_NORMAL), a(0), b(0)
{
  if(fabs(v1.x-v2.x) < Geometry2D::GEOM_SUPER_PRECISION) {
    type=LINE_VERTICAL;
    b=v1.x;
    return;
  }
  if(fabs(v1.y-v2.y) < Geometry2D::GEOM_SUPER_PRECISION) {
    type=LINE_HORIZONTAL;
    b=v1.y;
    return ;
  }
  type=LINE_NORMAL;
  a = (v2.y-v1.y)/(v2.x-v1.x);
  b = v1.y - (a)*v1.x;
  return;
}

void Line::print() const
{
    printf("t=%d a=%f b=%f\n",(int)type, (float)a, (float)b);
}

// ---------------------------------------------------------------------
// Line functions
// ---------------------------------------------------------------------

bool Geometry2D::getPointOnLineX(Line const& l, Millimeter const x, Point &pt)
{
    pt.x = x;
    switch(l.type){
    case LINE_VERTICAL:
        return false;
    case LINE_HORIZONTAL:
        pt.y = l.b;
        return true;
    case LINE_NORMAL:
    default:
        pt.y = l.a * pt.x + l.b;
        return true;
    }
}

bool Geometry2D::getPointOnLineY(Line const& l, Millimeter const y, Point &pt)
{
    pt.y = y;
    switch(l.type){
    case LINE_VERTICAL:
        pt.x = l.b;
        return false;
    case LINE_HORIZONTAL:
        return false;
    case LINE_NORMAL:
    default:
        assert(l.a != 0);
        pt.x = (y - l.b) / l.a;
        return true;
    }
}

void Geometry2D::getPointsOnLine(Line const& l, Point const& ptSrc, Millimeter const dist, 
                                 Point &pt1, Point &pt2)
{
    Point linePt2;
    if (!getPointOnLineX(l, ptSrc.x+1, linePt2)) {
        if (!getPointOnLineY(l, ptSrc.y+1, linePt2)) {
            assert(0);
        }
    }
    pt1 = getPointOnVector(ptSrc, linePt2, dist);
    pt2 = getPointOnVector(ptSrc, linePt2, -dist);
}

/**
 * @brief This function return the line that is orthogonal to line l and that pass through point pt
 */
Line Geometry2D::getLineOrthogonal(Line const& l, Point const& pt)
{
  Line l2;
  switch(l.type){
  case LINE_VERTICAL:
    l2.type=LINE_HORIZONTAL;
    l2.b=pt.y;
    break; 
  case LINE_HORIZONTAL:
    l2.type=LINE_VERTICAL;
    l2.b=pt.x;
    break;
  case LINE_NORMAL:
    l2.type=LINE_NORMAL;
    assert(l.a!=0);
    l2.a = -1/l.a;
    l2.b = pt.y - (l2.a)*pt.x;
  }
  return l2;
}

/**
 * @brief This function computes the intersection between 2 lines
 *
 * @param l1 Line 1
 * @param l2 Line 2
 * @param pt Intersection point that will be updated
 * @return true if the intersection exist
 */
bool Geometry2D::getLinesIntersection(Line const& l1, Line const& l2, Point &pt)
{
  if (l1.type==LINE_VERTICAL) {
    pt.x = l1.b;
    if (l2.type==LINE_VERTICAL) {
      if(l2.b==l1.b){
	pt.y = 0;
	return true;
      }
      else return false;
    } else if (l2.type==LINE_HORIZONTAL) {
      pt.y = l2.b;
      return true;
    } else { // l2.type==LINE_NORMAL;
      pt.y = l2.a*pt.x + l2.b;
      return true;
    } 
  } else if (l1.type==LINE_HORIZONTAL) {
    pt.y = l1.b;
    if (l2.type==LINE_VERTICAL) {
      pt.x = l2.b;
      return true;
    } else if (l2.type==LINE_HORIZONTAL) {
      if (l1.b==l2.b){
	pt.x=0;
	return true;
      } else return false;
    } else { // l2.type==LINE_NORMAL;
      assert(l2.a!=0);
      pt.x=(pt.y-l2.b)/l2.a;
      return true;
    } 
  } else { // l1.type==LINE_NORMAL;
    if (l2.type==LINE_VERTICAL) {
      pt.x = l2.b;
      pt.y = l1.a*pt.x + l1.b;
      return true;
    } else if (l2.type==LINE_HORIZONTAL) {
      pt.y = l2.b;
      assert(l1.a!=0);
      pt.x = (pt.y-l1.b)/l1.a;
      return true;
    } else { // l2.type==LINE_NORMAL;
      if (fabs(l2.a-l1.a)<GEOM_SUPER_PRECISION) 
	pt.x=0; // parallele lines
      else // normal case
	pt.x=(l2.b-l1.b)/(l1.a-l2.a);
      pt.y=l2.a*pt.x+l2.b;
      return true;
    } 
  } 
  return false;
}

/**
 * @brief This function returns the projection of point pt on line l
 */
Point Geometry2D::getOrthogonalProjection(Line const& l, Point const& pt)
{
  Point out;
  Line lineOrtho = getLineOrthogonal(l, pt);
  assert( getLinesIntersection(l, lineOrtho, out) );
  return out;
}

/**
 * @brief This function returns true if A is on segment (pt1 to pt2)
 */
bool Geometry2D::isPointOnSegment(Point const& pt1, Point const& pt2, 
                                  Point const& A)
{
  if (!arePointsCollinear(pt1,pt2,A)) return false;
  return isPointInRectangle(pt1, pt2, A);
}

/**
 * @brief return true if A is in the rectangle defined by s1, s2
 */
bool Geometry2D::isPointInRectangle(Point const& s1, Point const& s2, 
                                    Point const& A)
{
  if (fabs(s1.x-s2.x) > GEOM_SUPER_PRECISION) {
    if(A.x<s1.x && A.x<s2.x) return false;
    if(A.x>s1.x && A.x>s2.x) return false;
  }
  if (fabs(s1.y-s2.y) > GEOM_SUPER_PRECISION) {
    if(A.y<s1.y && A.y<s2.y) return false;
    if(A.y>s1.y && A.y>s2.y) return false;
  }
  return true;
}

/**
 * @brief This function returns pt1+a*(pt2-pt1)
 */
Point Geometry2D::getPointWithCoef(Point const& pt1, Point const& pt2, double const a)
{
  Point p;
  p.x = pt1.x + a*(pt2.x-pt1.x);
  p.y = pt1.y + a*(pt2.y-pt1.y);
  return p;
}

/**
 * @brief This function returns a point which is on vector (A,B), and at 
 * distance d from A
 */
Point Geometry2D::getPointOnVector(Point const& A, Point const& B, Millimeter const d)
{
  Millimeter q=dist(A, B);
  if (fabs(q) < GEOM_SUPER_PRECISION) return A;
  else return getPointWithCoef(A, B, d/q);
}

/**
 * @brief This function returns the point between pt1 and pt2
 */
Point Geometry2D::getPointMiddle(Point const& pt1, Point const& pt2)
{
  return getPointWithCoef(pt1, pt2, 0.5);
}

/**
 * @brief This function returns true if points are aligned
 */
bool Geometry2D::arePointsCollinear(Point const& pt1, Point const& pt2, Point const& pt3)
{
  double d = (pt1.y-pt2.y)*(pt1.x-pt3.x)-(pt1.x-pt2.x)*(pt1.y-pt3.y);
  if (fabs(d)<0.01) return true;
  else return false;
}


// ---------------------------------------------------------------------
// Class Circle
// ---------------------------------------------------------------------

/**
 * @brief This function creates a Circle
 */
Circle::Circle(Point      const& Center, 
               Millimeter const Radius) :
    center(Center), radius(Radius)
{
}

Circle::Circle(Millimeter const centerX, Millimeter const centerY, 
               Millimeter const Radius):
    center(centerX, centerY), radius(Radius)
{
}

void Circle::print() const
{ 
      printf("c.x=%f c.y=%f r=%f\n",
             (float)center.x, (float)center.y, (float)radius);
}

// ---------------------------------------------------------------------
// Circle functions
// ---------------------------------------------------------------------

/**
 * @brief This function computes the intersection between 2 circles
 *
 * @param c1 Circle 1
 * @param c2 Circle 2
 * @param pt1 first intesection point if it exists
 * @param pt2 second intersection point if it exists (if only one point, pt2=pt1)
 * @return true if the intersection exist, false in the other case
 */
bool Geometry2D::getCirclesIntersection(Circle const& c1, Circle const& c2, 
                                        Point &pt1, Point &pt2)
{
  Line l1, l2;
  Point ptMiddle;
  double d=dist(c1.center, c2.center);
  
  if (d > c1.radius+c2.radius || d==0) 
    return false;
  l1 = Line(c1.center, c2.center);
  double k = 0.5*((c2.radius*c2.radius-c1.radius*c1.radius)/(d*d)+1);
  ptMiddle = getPointWithCoef(c2.center, c1.center, k);
  l2 = getLineOrthogonal(l1, ptMiddle);
  // ptMiddle is the projection of c1 center on l2 
  return getLineCircleIntersection2(l2, c1, ptMiddle, pt1, pt2);
}

/**
 * @brief This function returns a point which is on line l, and at distance d from 
 * pt (pt must also be on l)
 */
Point Geometry2D::getPointOnLine(Line const& l, 
				 Point const& pt, 
				 double d)
{
  Point out;
  switch(l.type) {
  case LINE_VERTICAL:
    out.x=pt.x;
    out.y=pt.y+d;
    break; 
  case LINE_HORIZONTAL:
    out.x=pt.x+d;
    out.y=pt.y;
    break;
  case LINE_NORMAL:
    out.x = pt.x + sign(d)*sign(l.a)*sqrt(d*d/(l.a*l.a+1));
    out.y = l.a*out.x + l.b;
    break;
  }
  return out;
}

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
int Geometry2D::getSegmentCircleIntersection(Segment const& s, 
					      Circle const& c, 
					      Point &pt1, Point &pt2)
{
  int res=0;
  if (!getLineCircleIntersection2
        (s, c, getOrthogonalProjection(s, c.center), pt1, pt2))
    return res;
  if (pt1.x >= s.lowerLeft.x && pt1.x <= s.upperRight.x
      &&   pt1.y >= s.lowerLeft.y && pt1.y <= s.upperRight.y) res++;
  else pt1=pt2;
  if (pt2.x >= s.lowerLeft.x && pt2.x <= s.upperRight.x
      &&   pt2.y >= s.lowerLeft.y && pt2.y <= s.upperRight.y) res++;
  return res;
}

/**
 * @brief This function computes the intersection between a line and a circle
 *
 * @param l Line
 * @param c Circle
 * @param pt1 first intesection point if it exists
 * @param pt2 second intersection point if it exists (if only one point, pt2=pt1)
 * @return true if the intersection exist, false in the other case
 */
bool Geometry2D::getLineCircleIntersection(Line const& l, Circle const& c, 
                                           Point &pt1, Point &pt2)
{
  return getLineCircleIntersection2(l, c,
	      getOrthogonalProjection(l, c.center), pt1, pt2);
}

/**
 * @brief This function returns TRUE if the point is in the circle
 */
bool Geometry2D::isPointInCircle(Point const& pt, Circle const& c)
{
  return dist(pt, c.center) <= c.radius;
}

// ---------------------------------------------------------------------
// Class Rectangle
// ---------------------------------------------------------------------

/**
 * @brief This function creates a rectangle
 */
Rectangle::Rectangle(Point const& center, 
                     Radian const widthDir,
                     Millimeter const width, Millimeter const height)
{
  double thetaCorner=0;
  double distCorner=0;
  // get theta of a corner point so that points will be 
  // antitrigonometrically ordered, usefull for IsPointInRectangle
  if(width==0){
    thetaCorner=Geometry2D::MM_PI_2;
  } else {
    thetaCorner=fabs(atan(height/width));
  }
  distCorner  = sqrt(width*width+height*height)/2;
  pt[0].x = center.x+distCorner*cos(widthDir+thetaCorner);
  pt[0].y = center.y+distCorner*sin(widthDir+thetaCorner);
  pt[1].x = center.x+distCorner*cos(widthDir-thetaCorner);
  pt[1].y = center.y+distCorner*sin(widthDir-thetaCorner);
  pt[2].x = center.x+distCorner*cos(widthDir+thetaCorner+Geometry2D::MM_PI);
  pt[2].y = center.y+distCorner*sin(widthDir+thetaCorner+Geometry2D::MM_PI);
  pt[3].x = center.x+distCorner*cos(widthDir-thetaCorner+Geometry2D::MM_PI);
  pt[3].y = center.y+distCorner*sin(widthDir-thetaCorner+Geometry2D::MM_PI);
}

/**
 * @brief This function creates a rectangle
 */
Rectangle::Rectangle(Millimeter const centerX, Millimeter const centerY, 
                     Radian const widthDir,
                     Millimeter const width, Millimeter const height)
{
    Rectangle r2(Point(centerX, centerY), widthDir, width, height);
    pt[0]=r2.pt[0];
    pt[1]=r2.pt[1];
    pt[2]=r2.pt[2];
    pt[3]=r2.pt[3];
}

/**
 * @brief This function creates a rectangle
 */
Rectangle::Rectangle(Point const& topLeft, Point const& bottomRight)
{
  double tmp;
  // get points so that points will be 
  // antitrigonometrically ordered, usefull for IsPointInRectangle
  Point newTopLeft=topLeft;
  Point newBottomRight=bottomRight;
  if(newTopLeft.x>newBottomRight.x) {
    tmp=newTopLeft.x;
    newTopLeft.x=newBottomRight.x;
    newBottomRight.x=tmp;
  }
  if(newTopLeft.y<newBottomRight.y) {
    tmp=newTopLeft.y;
    newTopLeft.y=newBottomRight.y;
    newBottomRight.y=tmp;
  }
  pt[0]=newTopLeft;
  pt[1]=Point(newBottomRight.x, newTopLeft.y);
  pt[2]=newBottomRight;
  pt[3]=Point(newTopLeft.x, newBottomRight.y);
}

void Rectangle::print() const
{
    printf("pt[0]=(%f, %f) pt[1]=(%f, %f) pt[2]=(%f, %f) pt[3]=(%f, %f) \n",
           (float)pt[0].x, (float)pt[0].y,
           (float)pt[1].x, (float)pt[1].y,
           (float)pt[2].x, (float)pt[2].y,
           (float)pt[3].x, (float)pt[3].y);
}

void Segment::print() const
{
    printf("pt[0]=(%f, %f) pt[1]=(%f, %f)\n",
           (float)lowerLeft.x, (float)lowerLeft.y,
           (float)upperRight.x, (float)upperRight.y);
}

// ---------------------------------------------------------------------
// Rectangle functions
// ---------------------------------------------------------------------

/**
 * @brief This function computes the intersection of a rectangle and a circle.
 * It returns TRUE if the intersection exist
 */
bool Geometry2D::getRectangleCircleIntersection(Rectangle const& r, Circle const& c, 
                                                Point pt[8], int &nbrPt)
{ 
  int i;
  Line l2;
  nbrPt=0;
  for(i=0; i<4; ++i) {
    l2 = Line(r.pt[i], r.pt[(i+1)%4]);
    if (getLineCircleIntersection(l2, c, pt[nbrPt], pt[nbrPt+1])){
      if(isPointInRectangle(r.pt[i], r.pt[(i+1)%4], pt[nbrPt]))
	if(isPointInRectangle(r.pt[i], r.pt[(i+1)%4], pt[nbrPt+1])) nbrPt+=2;
	else ++ nbrPt;
      else 
	if(isPointInRectangle(r.pt[i], r.pt[(i+1)%4], pt[nbrPt+1])){ 
	  pt[nbrPt]=pt[nbrPt+1];
	  ++nbrPt;
	}
    }
  }
  return nbrPt>0;
}

/**
 * @brief This function computes the intersection of a rectangle and a line.
 * It returns TRUE if the intersection exist
 */
bool Geometry2D::getLineRectangleIntersection(Line const& l, Rectangle const& r,
                                              Point pt[2])
{
  int nbrPt=0;
  int i;
  Line l2;
  for(i=0; i<4; ++i) {
    l2 = Line(r.pt[i], r.pt[(i+1)%4]);
    if (getLinesIntersection(l, l2, pt[nbrPt])
	&& isPointInRectangle(r.pt[i], r.pt[(i+1)%4], pt[nbrPt])
	&& (nbrPt<1 || getSquareDistance(pt[nbrPt],pt[nbrPt-1])>GEOM_PRECISION)) 
      ++nbrPt;
    if (nbrPt==1) pt[1]=pt[0]; // to make sure there are 2 valid intersection points
    if (nbrPt>=2) return true;
  }
  return nbrPt>0;
}
/**
 * @brief This function computes the intersection of 2 rectangles
 * It returns TRUE if the intersection exist
 */
bool Geometry2D::getRectanglesIntersection(Rectangle const& r1, Rectangle const& r2,
                                           Point pt[8], int &nbrPt)
{
  /*	printf("pt=%lf %lf   %lf %lf %lf %lf   %lf %lf %lf %lf\n",pt[nbrPt].x,pt[nbrPt].y,
	       r1.pt[i].x, r1.pt[i].y, r1.pt[(i+1)%4].x, r1.pt[(i+1)%4].y,
	       r2.pt[j].x, r2.pt[j].y, r2.pt[(j+1)%4].x, r2.pt[(j+1)%4].y);*/
  int i, j;
  Line l1, l2;
  nbrPt=0;
  for(i=0; i<4; ++i) {
    l1 = Line(r1.pt[i], r1.pt[(i+1)%4]);
    for(j=0; j<4; ++j) {
      l2 = Line(r2.pt[j], r2.pt[(j+1)%4]);
      if (getLinesIntersection(l1, l2, pt[nbrPt])
	  && isPointInRectangle(r1.pt[i], r1.pt[(i+1)%4], pt[nbrPt])
	  && isPointInRectangle(r2.pt[j], r2.pt[(j+1)%4], pt[nbrPt])) 
	++nbrPt;
      if (nbrPt>=8) return true;
    }
  }
  return nbrPt>0;
}

/**
 * @brief This function returns true if the point is in the rectangle
 * 
 * Be sure that the rectangle's points are antitrigonometrically ordered!
 */
bool Geometry2D::isPointInRectangle(Point const& pt, 
                                    Rectangle const& r)
{
  int i;
  for(i=0;i<4;++i) {
    if (!isPointOnRight(r.pt[i], r.pt[(i+1)%4], pt)) 
      return false;
  }
  return true;
}

// ---------------------------------------------------------------------
// Tools
// ---------------------------------------------------------------------

/**
 * @brief This function returns the point which is on a spline of trajectory
 * at position t. t = [0..trajectory.size()-1]
 */
void Geometry2D::spline(Trajectory const& trajectory,
                        double            t,
                        Point           & point)
{
    assert(trajectory.size() >= 2);
    Point m0; //derivee au point 0 de notre sous-part de la courbe
    Point m1; //derivee au point 1 de notre sous-part de la courbe
    int i = (int)t; // sous-part de la courbe
    t -= i;         // l'endroit dans notre sous-part
    if (i >= (int)trajectory.size()-2) {
        point = trajectory[trajectory.size()-1];
        return;
    }
    // calcul la derivee du point 1
    if (i == 0) {
        m0 =  2.0*(trajectory[i+1] - trajectory[i]);
    } else {
        m0 =  2.0*(trajectory[i+1] - trajectory[i-1]);
    }

    // calcul la derivee du point 2
    m1 = 2.0*(trajectory[i+2] - trajectory[i]);
   
    // compute point on the spline
    point = ((2*t*t*t - 3*t*t + 1)*trajectory[i] + (t*t*t - 2*t*t + t)*m0
             + (t*t*t - t*t)*m1 + ((-2)*t*t*t + 3*t*t)*trajectory[i+1] );
    
}

/**
 * @brief This function returns the point which is on a segment of trajectory
 * at position t. t = [0..trajectory.size()-1]
 */
void Geometry2D::rectilinear(Trajectory const& trajectory,
                             double            index,
                             Point           & point)
{
    assert(trajectory.size() >= 2);

    if (index <= 0) {
        point = trajectory[0];
    } else if (index >= trajectory.size()-1) {
        point = trajectory[trajectory.size()-1];
    } else {
        int indexPt1 = (int)(index);
        int indexPt2 = (int)(index+1);
        index -= (double)indexPt1;
        point = ((1.-index)*trajectory[indexPt1]
                    + index*trajectory[indexPt2]);
    }
}

/**
 * @brief This function computes the position of a gonio
 *        from the direction of 3 targets
 */
bool Geometry2D::triangulation(Point const ptBalise1,  Point const ptBalise2,  Point const ptBalise3,
                               Radian const dirBalise1, Radian const dirBalise2, Radian const dirBalise3,
                               Point &ptGonio, Radian &dirGonio, 
                               TriangulationValidZoneFn zoneFn)
{
  Circle circle11;
  Circle circle21;
  Circle circle12;
  Circle circle22;
  Radian delta1 = dirBalise2 - dirBalise1;
  Radian delta2 = dirBalise3 - dirBalise2;
  if (delta1 == 0 || delta2 == 0) {
      return false;
  }
  // --------------------------------------------------------
  // Trouve les 2 cercles qui passent par le point recherche
  // --------------------------------------------------------
  Line line1(ptBalise1, ptBalise2);
  Line line2(ptBalise2, ptBalise3);
  Point ptMiddle1 = (ptBalise1+ptBalise2)/2.;
  Point ptMiddle2 = (ptBalise2+ptBalise3)/2.;
  Line line1o = getLineOrthogonal(line1, ptMiddle1);
  Line line2o = getLineOrthogonal(line2, ptMiddle2);
  Millimeter dist1 = dist(ptMiddle1, ptBalise1) / tan(delta1);
  Millimeter dist2 = dist(ptMiddle2, ptBalise2) / tan(delta2);
  getPointsOnLine(line1o, ptMiddle1, dist1, circle11.center, circle12.center);
  getPointsOnLine(line2o, ptMiddle2, dist2, circle21.center, circle22.center);
  if (!zoneFn(circle11.center)) {
      circle11.center = circle12.center;
      if (!zoneFn(circle11.center)) {
          return false;
      }
  }
  circle11.radius = dist(circle11.center, ptBalise1);
  if (!zoneFn(circle21.center)) {
      circle21.center = circle22.center;
      if (!zoneFn(circle21.center)) {
          return false;
      }
  }
  circle21.radius = dist(circle21.center, ptBalise3);

  // -----------------------------------------------------------------------
  // Trouve le meilleur point d'intersection des 2 cercles (pas la balise2)
  // -----------------------------------------------------------------------
  Point pt1, pt2;
  if (!getCirclesIntersection(circle11, circle21, pt1, pt2)) {
      return false;
  }
  if (dist(pt1, ptBalise2) > dist(pt2, ptBalise2)) {
      ptGonio = pt1;
  } else {
      ptGonio = pt2;
  }
  dirGonio = dirBalise1 - dir(ptGonio, ptBalise1);
  
  return true;
}
// ---------------------------------------------------------------------
// Segment::Segment
// ---------------------------------------------------------------------
Segment::Segment(Point const& v1, Point const& v2) : 
  Line(v1,v2)
{
  lowerLeft.x  = min(v1.x, v2.x);
  lowerLeft.y  = min(v1.y, v2.y);
  upperRight.x = max(v1.x, v2.x);
  upperRight.y = max(v1.y, v2.y);
}

/**
 * @brief This function computes the intersection between 2 segments
 *
 * @param s1 segment 1
 * @param s2 segment 2
 * @param pt Intersection point that will be updated
 * @return true if the intersection exist
 */
bool Geometry2D::getSegmentsIntersection(Segment const&s1, 
					 Segment const& s2, 
					 Point &pt)
{
  if (!getLinesIntersection(s1, s2, pt)) return false;
  return pt.x >= s1.lowerLeft.x && pt.x <= s1.upperRight.x
    &&   pt.x >= s2.lowerLeft.x && pt.x <= s2.upperRight.x
    &&   pt.y >= s1.lowerLeft.y && pt.y <= s1.upperRight.y
    &&   pt.y >= s2.lowerLeft.y && pt.y <= s2.upperRight.y;
}

// ---------------------------------------------------------------------
// Polygon
// ---------------------------------------------------------------------
Polygon::Polygon(Point* Pts, int NbrPts):
  nbrPts(NbrPts), pts(NULL)
{
  pts = new Point[nbrPts];
  memcpy(pts, Pts, sizeof(Point)*nbrPts);
}
Polygon::Polygon(Polygon const& p):
  nbrPts(p.nbrPts), pts(NULL)
{
    if (nbrPts!=0) {
        pts = new Point[nbrPts];
        memcpy(pts, p.pts, sizeof(Point)*nbrPts);
    }
}
Polygon& Polygon::operator=(Polygon const& p)
{
    if (nbrPts!=p.nbrPts) {
        if (pts) delete pts;
        pts=NULL;
        nbrPts=p.nbrPts;
        if (nbrPts!=0) pts = new Point[nbrPts];
    }
    if (nbrPts!=0) memcpy(pts, p.pts, sizeof(Point)*nbrPts);
    return *this;
    
}

Polygon::~Polygon()
{
  if (pts) delete[] pts;
  pts=NULL;
}

void Polygon::print() const
{
  int i=0;
  for(i=0;i<nbrPts-1;++i) {
    printf("  %d %d\n", (int)pts[i].x, (int)pts[i].y);
  }
}

/**
 * @brief This function returns true if the point is inside the polygon
 */
bool Geometry2D::isPointInPolygon(Polygon const& p1, Point const& pt1)
{
  int i=0;
  Point baricenter = getBaricenter(p1.pts, p1.nbrPts);
  for(i=0;i<p1.nbrPts-1;++i) {
    if ((isPointOnRight(p1.pts[i], p1.pts[i+1], pt1)
	 ^ isPointOnRight(p1.pts[i], p1.pts[i+1], baricenter))) 
      return false;
  }
  if ((isPointOnRight(p1.pts[p1.nbrPts-1], p1.pts[0], pt1)
       ^ isPointOnRight(p1.pts[p1.nbrPts-1], p1.pts[0], baricenter))) 
    return false;
  
  return true;
}

/**
 * @brief This function returns true if the circle is inside the polygon or 
 *        if it intersect it
 */
bool Geometry2D::isCircleInPolygon(Polygon const& p1, 
				   Circle const&  c1)
{
    int i=0;
    if (isPointInPolygon(p1, c1.center)) return true;
    for(i=0;i<p1.nbrPts;++i) {
        Segment s1(p1.pts[i%p1.nbrPts], p1.pts[(i+1)%p1.nbrPts]);
        Point pt1, pt2;
        if (getSegmentCircleIntersection(s1, c1, pt1, pt2)) return true;
    }
    return false;
}

/**
 * Conversion d'un point en coordonnees cylindriques
 */
Point& Geometry2D::convertToCylindricCoord(Point const& center, Point& pt)
{
    Millimeter d=dist(center, pt);
    Radian     r=dir( center, pt);
    pt.x=d;
    pt.y=r;
    return pt;
}
/**
 * Conversion d'un point en coordonnees orthogonales
 */
Point& Geometry2D::convertToOrthogonalCoord(Point const& center, 
                                            Radian direction,
                                            Point& pt)
{
    Millimeter x=pt.x*cos(pt.y+direction);
    Millimeter y=pt.x*sin(pt.y+direction);
    pt.x = center.x + x;
    pt.y = center.y + y;
    return pt;
}
// ---------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------

/**
 * @brief this function tests all functions of this lib
 */
bool Geometry2D::validate()
{
  Point A,B,C,D,O, E;
  Point pts[8], pt;
  Line l1,l2,l3,l4,l5;
  Circle c1,c2,c3,c4;
  Rectangle r1,r2,r3,r4;
  int nbrpt;
  
  test_begin();

  O=Point(0,0);
  A=Point(1,1);
  B=Point(0,1);
  C=Point(1,0);
  D=Point(2,1);
  E=Point(2,2);
  
  // point
  Point F;
  F=A+E;
  test_condition(F.x==3 && F.y==3);
  F-=E;
  test_condition(F.x==A.x && F.y==A.y);
  F=A-E;
  test_condition(F.x==-1 && F.y==-1);
  F+=E;
  test_condition(F.x==A.x && F.y==A.y);
  F=3.*A;
  test_condition(F.x==3 && F.y==3);
  F/=3.;
  test_condition(F.x==A.x && F.y==A.y);
  F=A*3.;
  test_condition(F.x==3 && F.y==3);
  F=F/3.;
  test_condition(F.x==A.x && F.y==A.y);
  test_condition(F==A);
  test_condition(A!=C);
  test_condition(A!=B);
  test_condition(A!=E);
  test_condition(D==D);
  F=-D;
  test_condition(F.x==-D.x && F.y==-D.y);
  
  // distance
  test_condition(dist(A,O)==sqrt(2.));
  test_condition(norme(A)==sqrt(2.));
  test_condition(getSquareDistance(A,O)==2);

  // directions
  test_condition(fabs(MM_PI-3.141)<0.02);
  test_condition(fabs(MM_2_PI-2*3.141)<0.04);
  test_condition(fabs(MM_PI_2-3.141/2)<0.01);
  test_condition(fabs(dir(O,B)-MM_PI_2)<0.001);
  test_condition(fabs(dir(B,O)+MM_PI_2)<0.001);
  test_condition(fabs(dir(O,C))<0.001);
  test_condition(fabs(dir(C,O)-MM_PI)<0.001);
  test_condition(fabs(angle(A)-MM_PI/4.)<GEOM_PRECISION);
  test_condition(fabs(angle(B)-MM_PI_2)<GEOM_PRECISION);
  test_condition(fabs(angle(C))<GEOM_PRECISION);
  test_condition(fabs(dir(O, A)-MM_PI/4.)<GEOM_PRECISION);
  test_condition(fabs(dir(A, O)-(MM_PI+MM_PI/4.))<GEOM_PRECISION);
 
  test_condition(normalizeAngle2PI(0,-1)==0);
  test_condition(normalizeAngle2PI(-1,-1)==-1);
  test_condition(normalizeAngle2PI(-1+MM_2_PI,-1)==-1);

  test_condition(normalizeAnglePI(-1+MM_2_PI,-1)==-1);
  test_condition(fabs(normalizeAnglePI(-0.9+MM_2_PI,-1)+0.9)<0.001);

  // points
  test_condition(isPointOnRight(O,A,C));
  test_condition(!isPointOnRight(O,A,B));
  test_condition(isPointOnLeft(O,A,B));
  test_condition(!isPointOnLeft(O,A,C));

  test_condition(!isPointInRectangle(O,A,D));
  test_condition(isPointInRectangle(O,A,B));

  pt=getPointWithCoef(O, A, 0);
  test_condition(pt.x==O.x && pt.y==O.y);
  pt=getPointWithCoef(O, A, 1);
  test_condition(pt.x==A.x && pt.y==A.y);
  pt=getPointWithCoef(O, A, 2);
  test_condition(pt.x==2 && pt.y==2);

  pt=getPointMiddle(D, A);
  test_condition(pt.x==1.5 && pt.y==1);

  pts[0]=O;
  pts[1]=B;
  pts[2]=A;
  pts[3]=C;
  pt=getBaricenter(pts, 4);
  test_condition(pt.x==0.5 && pt.y==0.5);

  test_condition(arePointsCollinear(O, A, pt));
  test_condition(!arePointsCollinear(O, A, D));

  // lines
  l1=Line(O,B); 
  l2=Line(B,A); 
  l3=Line(O,MM_PI_2/2);
  l4=getLineOrthogonal(l2, A); 
  l5=getLineOrthogonal(l1, O); 

  test_condition(l3.type==LINE_NORMAL     && fabs(l3.a-1)<GEOM_PRECISION && fabs(l3.b)<GEOM_PRECISION);
  test_condition(l1.type==LINE_VERTICAL   && fabs(l1.b)<GEOM_PRECISION);
  test_condition(l2.type==LINE_HORIZONTAL && fabs(l2.b-1)<GEOM_PRECISION);
  test_condition(l4.type==LINE_VERTICAL   && fabs(l4.b-1)<GEOM_PRECISION);
  test_condition(l5.type==LINE_HORIZONTAL && fabs(l5.b)<GEOM_PRECISION);
  
  test_condition(!getLinesIntersection(l1, l4, pt));
  test_condition(getLinesIntersection(l1, l2, pt));
  test_condition(pt.x==B.x && pt.y==B.y);
  test_condition(getLinesIntersection(l1, l3, pt));
  test_condition(pt.x==O.x && pt.y==O.y);
  test_condition(getLinesIntersection(l3, l4, pt));
  test_condition(fabs(pt.x-A.x)<GEOM_PRECISION && fabs(pt.y-A.y)<GEOM_PRECISION);
  test_condition(getLinesIntersection(l4, l5, pt));
  test_condition(fabs(pt.x-C.x)<GEOM_PRECISION && fabs(pt.y-C.y)<GEOM_PRECISION);

  pt=getPointOnLine(l3, A, sqrt(2.));
  //printf("pt=%lf %lf\n", pt.x, pt.y);
  test_condition(fabs(pt.x-E.x)<GEOM_PRECISION && fabs(pt.y-E.y)<GEOM_PRECISION);

  // circles
  c1=Circle(O, 1.5);
  c2=Circle(A, 2);
  pt=Point(1.5,0);
  c3=Circle(pt, 1);

  pt=Point(3,2);
  c4=Circle(pt, 1);

  test_condition(isPointInCircle(O,c1));
  test_condition(isPointInCircle(O,c2));
  test_condition(!isPointInCircle(O,c3));
  test_condition(isPointInCircle(A,c2));
  test_condition(isPointInCircle(A,c1));
  test_condition(!isPointInCircle(A,c3));
  test_condition(!isPointInCircle(D,c1));
  test_condition(isPointInCircle(D,c2));

  // geom_getCirclesIntersection also test_ circle lines intersections
  test_condition(getCirclesIntersection(c1,c3, pts[0], pts[1]));
  //printf("%lf %lf %lf %lf \n",pts[0].x, pts[0].y, pts[1].x, pts[1].y);
  test_condition(pts[0].x==pts[1].x && pts[0].y==-pts[1].y && fabs(pts[0].y-0.9428)<GEOM_PRECISION);
  test_condition(!getCirclesIntersection(c1,c4, pts[0], pts[1]));
  test_condition(getCirclesIntersection(c2,c4, pts[0], pts[1]));
  //printf("%lf %lf %lf %lf \n",pts[0].x, pts[0].y, pts[1].x, pts[1].y);
  test_condition((fabs(pts[1].x-3)<GEOM_PRECISION && fabs(pts[1].y-1)<GEOM_PRECISION
	 && fabs(pts[0].x-2.2)<GEOM_PRECISION && fabs(pts[0].y-2.6)<GEOM_PRECISION)
	 || (fabs(pts[1].x-3)<GEOM_PRECISION && fabs(pts[1].y-1)<GEOM_PRECISION
	 && fabs(pts[0].x-2.2)<GEOM_PRECISION && fabs(pts[0].y-2.6)<GEOM_PRECISION));
  test_condition(getCirclesIntersection(c2,c3, pts[0], pts[1]));
  test_condition((fabs(pts[1].x-1.293224)<GEOM_PRECISION && fabs(pts[1].y+0.978388)<GEOM_PRECISION
	 && fabs(pts[0].x-2.406776)<GEOM_PRECISION && fabs(pts[0].y+0.421612)<GEOM_PRECISION)
	 || (fabs(pts[0].x-1.293224)<GEOM_PRECISION && fabs(pts[0].y+0.978388)<GEOM_PRECISION
	 && fabs(pts[1].x-2.406776)<GEOM_PRECISION && fabs(pts[1].y+0.421612)<GEOM_PRECISION));

  test_condition(getLineCircleIntersection(l1,c2, pts[0], pts[1]));
  test_condition((fabs(pts[1].x)<GEOM_PRECISION && fabs(pts[1].y-2.732051)<GEOM_PRECISION
	 && fabs(pts[0].x)<GEOM_PRECISION && fabs(pts[0].y+0.732051)<GEOM_PRECISION)
	 || (fabs(pts[0].x)<GEOM_PRECISION && fabs(pts[0].y-2.732051)<GEOM_PRECISION
	 && fabs(pts[1].x)<GEOM_PRECISION && fabs(pts[1].y+0.732051)<GEOM_PRECISION));
  test_condition(getLineCircleIntersection(l5,c2, pts[0], pts[1]));
  test_condition((fabs(pts[1].x-2.732051)<GEOM_PRECISION && fabs(pts[1].y)<GEOM_PRECISION
	 && fabs(pts[0].x+0.732051)<GEOM_PRECISION && fabs(pts[0].y)<GEOM_PRECISION)
	 || (fabs(pts[0].x-2.732051)<GEOM_PRECISION && fabs(pts[0].y)<GEOM_PRECISION
	 && fabs(pts[1].x+0.732051)<GEOM_PRECISION && fabs(pts[1].y)<GEOM_PRECISION));
  test_condition(getLineCircleIntersection(l3,c2, pts[0], pts[1]));
  test_condition((fabs(pts[1].x-2.414214)<GEOM_PRECISION && fabs(pts[1].y-2.414214)<GEOM_PRECISION
	 && fabs(pts[0].x+0.414214)<GEOM_PRECISION && fabs(pts[0].y+0.414214)<GEOM_PRECISION)
	 || (fabs(pts[0].x-2.414214)<GEOM_PRECISION && fabs(pts[0].y-2.414214)<GEOM_PRECISION
	 && fabs(pts[1].x+0.414214)<GEOM_PRECISION && fabs(pts[1].y+0.414214)<GEOM_PRECISION));
  test_condition(!getLineCircleIntersection(l1,c3, pts[0], pts[1]));

  // rectangles
  r1=Rectangle(O,A);
  r2=Rectangle(A, MM_PI_2, 0.5, 2);
  r3=Rectangle(A, MM_PI_3, 1, 1);
  r4=Rectangle(2,2.5, 2*MM_PI_3, 4, 2);

  test_condition(!getLineRectangleIntersection(l5, r3, pts));
  test_condition(getLineRectangleIntersection(l4, r3, pts));
  //printf("%lf %lf %lf %lf \n",pts[0].x, pts[0].y, pts[1].x, pts[1].y);
  test_condition((fabs(pts[1].x-1)<GEOM_PRECISION && fabs(pts[1].y-1.577350)<GEOM_PRECISION
	 && fabs(pts[0].x-1)<GEOM_PRECISION && fabs(pts[0].y-0.422650)<GEOM_PRECISION)
	 || (fabs(pts[0].x-1)<GEOM_PRECISION && fabs(pts[0].y-1.577350)<GEOM_PRECISION
	 && fabs(pts[1].x-1)<GEOM_PRECISION && fabs(pts[1].y-0.422650)<GEOM_PRECISION));
  test_condition(getLineRectangleIntersection(l3, r1, pts));
  test_condition((fabs(pts[1].x-1)<GEOM_PRECISION && fabs(pts[1].y-1)<GEOM_PRECISION
	 && fabs(pts[0].x)<GEOM_PRECISION && fabs(pts[0].y)<GEOM_PRECISION)
	 || (fabs(pts[0].x-1)<GEOM_PRECISION && fabs(pts[0].y-1)<GEOM_PRECISION
	 && fabs(pts[1].x)<GEOM_PRECISION && fabs(pts[1].y)<GEOM_PRECISION));
  //printf("%lf %lf %lf %lf \n",pts[0].x, pts[0].y, pts[1].x, pts[1].y);
  
  test_condition(getRectanglesIntersection(r1, r3, pts, nbrpt));
  test_condition(nbrpt==2 && ((fabs(pts[1].x-0.422650)<GEOM_PRECISION && fabs(pts[1].y-1)<GEOM_PRECISION
			      && fabs(pts[0].x-1)<GEOM_PRECISION && fabs(pts[0].y-0.422650)<GEOM_PRECISION)
			     || (fabs(pts[0].x-0.422650)<GEOM_PRECISION && fabs(pts[0].y-1)<GEOM_PRECISION
				 && fabs(pts[1].x-1)<GEOM_PRECISION && fabs(pts[1].y-0.422650)<GEOM_PRECISION)));
  
  test_condition(getRectanglesIntersection(r2, r3, pts, nbrpt));
  test_condition(nbrpt==4);
  test_condition(getRectanglesIntersection(r4, r3, pts, nbrpt));
  test_condition(nbrpt==2);

  test_condition(!getRectangleCircleIntersection(r3, c2, pts, nbrpt));
  test_condition(!getRectangleCircleIntersection(r1, c2, pts, nbrpt));
  test_condition(!getRectangleCircleIntersection(r2, c2, pts, nbrpt));

  test_condition(!getRectangleCircleIntersection(r3, c4, pts, nbrpt));
  test_condition(!getRectangleCircleIntersection(r1, c4, pts, nbrpt));
  test_condition(!getRectangleCircleIntersection(r2, c4, pts, nbrpt));
  //printf("%d  %lf %lf %lf %lf \n",nbrpt, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
  test_condition(getRectangleCircleIntersection(r4, c4, pts, nbrpt));
  test_condition(nbrpt==2 && ((fabs(pts[1].x-2.870860)<GEOM_PRECISION && fabs(pts[1].y-2.991626)<GEOM_PRECISION
			      && fabs(pts[0].x-3.794204)<GEOM_PRECISION && fabs(pts[0].y-1.392348)<GEOM_PRECISION)
			     || (fabs(pts[0].x-2.870860)<GEOM_PRECISION && fabs(pts[0].y-2.991626)<GEOM_PRECISION
				 && fabs(pts[1].x-3.794204)<GEOM_PRECISION && fabs(pts[1].y-1.392348)<GEOM_PRECISION)));

  // triangulation
  Point  ptBalise1(0,0);  
  Point  ptBalise2(0,1050);  
  Point  ptBalise3(0,2100);
  Radian dirBalise1;
  Radian dirBalise2;
  Radian dirBalise3;
  Point  ptGonio; 
  Radian dirGonio;

  A=Point(1500,1050);
  dirBalise1=dir(A, ptBalise1);
  dirBalise2=dir(A, ptBalise2);
  dirBalise3=dir(A, ptBalise3);
  test_condition(triangulation(ptBalise1, ptBalise2, ptBalise3,
                               dirBalise1, dirBalise2, dirBalise3,
                               ptGonio, dirGonio, validZone));
  test_condition(dist(A, ptGonio)<GEOM_PRECISION);
  test_condition(fabs(normalizeAngle2PI(dirGonio,-MM_PI))<GEOM_PRECISION);

  A=Point(2100,1800);
  dirBalise1=dir(A, ptBalise1);
  dirBalise2=dir(A, ptBalise2);
  dirBalise3=dir(A, ptBalise3);
  test_condition(triangulation(ptBalise1, ptBalise2, ptBalise3,
                               dirBalise1, dirBalise2, dirBalise3,
                               ptGonio, dirGonio, validZone));
  test_condition(dist(A, ptGonio)<GEOM_PRECISION);
  test_condition(fabs(normalizeAngle2PI(dirGonio,-MM_PI))<GEOM_PRECISION);

  ptBalise2=Point(200,1350); 

  A=Point(1000,1550);
  dirBalise1=dir(A, ptBalise1);
  dirBalise2=dir(A, ptBalise2);
  dirBalise3=dir(A, ptBalise3);
  test_condition(triangulation(ptBalise1, ptBalise2, ptBalise3,
                               dirBalise1, dirBalise2, dirBalise3,
                               ptGonio, dirGonio, validZone));
  test_condition(dist(A, ptGonio)<GEOM_PRECISION);
  test_condition(fabs(normalizeAngle2PI(dirGonio,-MM_PI))<GEOM_PRECISION);

  A=Point(2800,330);
  dirBalise1=dir(A, ptBalise1);
  dirBalise2=dir(A, ptBalise2);
  dirBalise3=dir(A, ptBalise3);
  test_condition(triangulation(ptBalise1, ptBalise2, ptBalise3,
                               dirBalise1, dirBalise2, dirBalise3,
                               ptGonio, dirGonio, validZone));
  test_condition(dist(A, ptGonio)<GEOM_PRECISION);
  test_condition(fabs(normalizeAngle2PI(dirGonio,-MM_PI))<GEOM_PRECISION);

  test_end();

  return true;
}

#ifdef TEST_MAIN
int main()
{
    Geometry2D::validate();
    return 0;
}
#endif
