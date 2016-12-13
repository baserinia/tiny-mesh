#ifndef FILE_VERTEX_H_INCLUDED
#define FILE_VERTEX_H_INCLUDED

#include "vector.hpp"

class Vertex {
    Vector mPos; // position vector
    double mSpace; // spacing
public:
    Vertex() : mPos(), mSpacing(0) {}
    Vertex(const Vector& pos) : mPos(pos), mSpacing(0) {}
    Vertex(const Vector& pos, double sp) : mPos(pos), mSpacing(sp) {}
    const Vector& pos() { return mPos; }
    double space() { return mSpace; }
};

#endif // FILE_VERTEX_H_INCLUDED
