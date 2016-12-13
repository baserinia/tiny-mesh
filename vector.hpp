// Copyright (C) 1999--2016  Amir R. Baserinia 

#ifndef FILE_VECTOR_H_INCLUDED
#define FILE_VECTOR_H_INCLUDED

#include <cmath>

class Vector {
    double mX, mY;
public:
// default ctor
    Vector() : mX(0), mY(0) {}
// init ctor
    Vector(double x, double y) : mX(x), mY(y) {}
// copy ctor
    Vector(const Vector& v) : mX(v.mX), mY(v.mY) {}
// overloaded operators
    void operator=(const Vector &v) { mX = v.mX; mY = v.mY; }
// dot product
    double operator*(const Vector& v) { return mX*v.mX + mY*v.mY; }
// cross product
    double operator^(const Vector& v) { return mX*v.mY - mY*v.mX; }
// scalar product
    Vector operator*(double d) { return Vector(mX*d, mY*d); }
// scalar division
    Vector operator/(double d) { return Vector(mX/d, mY/d); }
// addition
    Vector operator+(const Vector& v) { return Vector(mX + v.mX, mY + v.mY); }
// subtraction
    Vector operator-(const Vector& v) { return Vector(mX - v.mX, mY - v.mY); }
// negation
    Vector operator-() { return Vector(-mX, -mY); }
// rotate 90 degrees counter-clockwise
    Vector rot90ccw() const { return Vector(-mY, mX); }
// normalize
    Vector normalize() const { double d = length(); return Vector(mX/d, mY/d); }
// set
    void set(double x, double y) { mX = x; mY = y; }
// get length squared
    double lengthSqr() const { return mX*mX + mY*mY; }
// get length
    double length() const { return sqrt(lengthSqr()); }
// dinstance squared between two points (avoid sqrt for performance)
    static double distSqr(const Vector& v1, const Vector& v2) { 
        double dx = v1.mX - v2.mX;
        double dy = v1.mY - v2.mY;
        return dx*dx + dy*dy;
    }
// distance between two points
    static double dist(const Vector& v1, const Vector& v2) { 
        return sqrt(distSqr(v1, v2));
    }
};

#endif // FILE_VECTOR_H_INCLUDED

