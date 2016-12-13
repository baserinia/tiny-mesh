#ifndef FILE_CELL_H_INCLUDED
#define FILE_CELL_H_INCLUDED

#include "Vector.h"
#include "Vertex.h"

class Cell {
    int mIndex;
    Vertex* mVertex[3];
    Edge*  mEdge[3];
    Cell* mNeighbor[3];
    bool mStatus;
public:
    Cell();
    Cell(const Vertex* n0, const Vertex* n1, const Vertex* n2);
    ~Cell() {}
 
    int isObtuse();
    double edgeLength(int i);
    int edgeOrderIndx(int i);
    int MinDistIndx(const Vector& vec);
    double MinDist(const Vector& vec);
    int isGoodTriangle();
    Vector getSpacingGradient();
    void addNeighbor(const Cell*);
};

#endif // FILE_CELL_H_INCLUDED
