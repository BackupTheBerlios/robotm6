/**
 * @file robotPoint2D.h
 *
 * This file defines a template for 2Dimension points. It also defines 
 * functions to get the distance and the direction of 2 points
 *
 * @author Laurent Saint-Marcel
 * @date   2003/10/01
 */

#ifndef __ROBOT_POINT_2D_H__
#define __ROBOT_POINT_2D_H__

#include <math.h>
#include <stdio.h>

// ------------------------------------------------------------------------
// class RobotPoint2D
// ------------------------------------------------------------------------

template <typename T, typename TypeAngle>
class RobotPoint2D {
 public:
    // constructor
    RobotPoint2D() : x(0), y(0){}
    RobotPoint2D(RobotPoint2D<T, TypeAngle> const & pt) : x(pt.x), y(pt.y){}
    RobotPoint2D(T const& X, 
                 T const& Y): x(X), y(Y){}

 public:
    // functions
    T         dist(RobotPoint2D<T, TypeAngle> const& pt) const;
    T         norme() const;
    TypeAngle dir(RobotPoint2D<T, TypeAngle> const& pt) const;
    TypeAngle angle() const; 
    void      print() const;
    char*     txt() const;
 public:
    // members
    T x;
    T y;
};

// ------------------------------------------------------------------------
// RobotPoint2D::print
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline void 
RobotPoint2D<T, TypeAngle>::print() const
{
    printf("Point(%f, %f)\n", (float)x, (float)y);
}

// ------------------------------------------------------------------------
// RobotPoint2D::txt
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline char* 
RobotPoint2D<T, TypeAngle>::txt() const
{
    static char text[30];
    snprintf(text, 30, "(%f, %f)", (float)x, (float)y);
    return text;
}

// ------------------------------------------------------------------------
// RobotPoint2D::dist
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline T 
RobotPoint2D<T, TypeAngle>::dist(RobotPoint2D<T, TypeAngle> const& pt) const
{
    return (T)(sqrt((x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y)));
}

template <typename T, typename TypeAngle>
inline T 
dist(RobotPoint2D<T, TypeAngle> const& pt1,
     RobotPoint2D<T, TypeAngle> const& pt2)
{
    return pt1.dist(pt2);
}
// ------------------------------------------------------------------------
// RobotPoint2D::norme
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline T 
RobotPoint2D<T, TypeAngle>::norme() const
{
    return (T)(sqrt((x)*(x)+(y)*(y)));
}

template <typename T, typename TypeAngle>
inline T 
norme(RobotPoint2D<T, TypeAngle> const& pt)
{
    return pt.norme();
}

// ------------------------------------------------------------------------
// RobotPoint2D::dir2Pt
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline TypeAngle 
RobotPoint2D<T, TypeAngle>::dir(RobotPoint2D<T, TypeAngle> const& pt) const
{
    TypeAngle result=0;
    if ((pt.x - x) == 0) {
        if (pt.y - y > 0) return (TypeAngle)(M_PI/2.);
        else return (TypeAngle)(-M_PI/2.);
    } else {
        result = (TypeAngle)atan((pt.y-y)/(pt.x-x));
        if (pt.x - x > 0) return result;
        else return (TypeAngle)(M_PI)+result;
    }
}

template <typename T, typename TypeAngle>
inline TypeAngle 
dir(RobotPoint2D<T, TypeAngle> const& pt1,
    RobotPoint2D<T, TypeAngle> const& pt2)
{
    return pt1.dir(pt2);
}

// ------------------------------------------------------------------------
// RobotPoint2D::dir
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline TypeAngle 
RobotPoint2D<T, TypeAngle>::angle() const
{
    TypeAngle result=0;
    if (x == 0) {
        if (y > 0) return (TypeAngle)(M_PI/2.);
        else return (TypeAngle)(-M_PI/2.);
    } else {
        result = (TypeAngle)atan(y/x);
        if (x > 0) return result;
        else return (TypeAngle)(M_PI)+result;
    }
}

template <typename T, typename TypeAngle>
inline TypeAngle 
angle(RobotPoint2D<T, TypeAngle> const& pt)
{
    return pt.angle();
}

// ------------------------------------------------------------------------
// RobotPoint2D::operator+
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator+ (RobotPoint2D<T, TypeAngle> const& pt1,  
           RobotPoint2D<T, TypeAngle> const& pt2)
{
    RobotPoint2D<T, TypeAngle> result(pt1.x + pt2.x, 
                                      pt1.y + pt2.y);
    return result;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator-
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator- (RobotPoint2D<T, TypeAngle> const& pt1,  
           RobotPoint2D<T, TypeAngle> const& pt2)
{
    RobotPoint2D<T, TypeAngle> result(pt1.x - pt2.x, 
                                      pt1.y - pt2.y);
    return result;
}

// ------------------------------------------------------------------------
// RobotPoint2D::operator-
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator- (RobotPoint2D<T, TypeAngle> const& pt1)
{
    RobotPoint2D<T, TypeAngle> result(-pt1.x, -pt1.y);
    return result;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator+=
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator+= (RobotPoint2D<T, TypeAngle> & pt1, 
            RobotPoint2D<T, TypeAngle> const& pt2)
{
    pt1.x += pt2.x;
    pt1.y += pt2.y;
    return pt1;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator-=
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator-= (RobotPoint2D<T, TypeAngle> & pt1, 
            RobotPoint2D<T, TypeAngle> const& pt2)
{
    pt1.x -= pt2.x;
    pt1.y -= pt2.y;
    return pt1;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator==
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline bool      
operator== (RobotPoint2D<T, TypeAngle> const& pt1, 
            RobotPoint2D<T, TypeAngle> const& pt2)
{
    return (pt1.x == pt2.x) && (pt1.y == pt2.y);
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator!=
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline  bool       
operator!= (RobotPoint2D<T, TypeAngle> const& pt1, 
            RobotPoint2D<T, TypeAngle> const& pt2)
{
    return (pt1.x != pt2.x) || (pt1.y != pt2.y);
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator*
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
RobotPoint2D<T, TypeAngle> 
operator* (RobotPoint2D<T, TypeAngle> const& pt, 
           T a)
{
    RobotPoint2D<T, TypeAngle> result(pt.x * a, 
                                      pt.y * a);
    return result;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator*
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator* (T a,
           RobotPoint2D<T, TypeAngle> const& pt)
{
    RobotPoint2D<T, TypeAngle> result(pt.x * a, 
                                      pt.y * a);
    return result;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator*=
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator*= (RobotPoint2D<T, TypeAngle> & pt,
            T a)
{
    pt.x *= a;
    pt.y *= a;
    return pt;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator/
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator/ (RobotPoint2D<T, TypeAngle> const& pt,
           T a)
{
    RobotPoint2D<T, TypeAngle> result(pt.x / a, 
                                      pt.y / a);
    return result;
}
// ------------------------------------------------------------------------
// RobotPoint2D::operator/=
// ------------------------------------------------------------------------
template <typename T, typename TypeAngle>
inline RobotPoint2D<T, TypeAngle> 
operator/= (RobotPoint2D<T, TypeAngle> & pt,
            T a)
{
    pt.x /= a;
    pt.y /= a;
    return pt;
}

#endif // __ROBOT_POINT_2D_H__
