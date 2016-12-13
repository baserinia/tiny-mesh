#ifndef FILE_EDGE_H_INCLUDED
#define FILE_EDGE_H_INCLUDED

#include "vertex.hpp"

class Edge {
    Vertex* mV1, mV2; // end vertices
public:
    Edge() : mV1(NULL), mV2(NULL) {}
    double length() { return Vector::distance(mV1.pos(), mV2.pos()); }
};

#endif // FILE_EDGE_H_INCLUDED
