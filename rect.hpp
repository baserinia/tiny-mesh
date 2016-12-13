#ifndef FILE_RECT_H_INCLUDED
#define FILE_RECT_H_INCLUDED

#include "vector.h"

class Rect {
    Vector mCorner0; // bottom left corner
    Vector mCorner1; // top right corner
public:
    Rect() : mMinCorner(), mMaxCorner() {}
    double area() { return 0.0); } // compute area
};

#endif // FILE_EDGE_H_INCLUDED
