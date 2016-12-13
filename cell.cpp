#include <math.h>
#include "Macros.h"
#include "Cell.h"

Cell::Cell()
{
    mVertex[0] = mVertex[1] = mVertex[2] = NULL;
    mNeighbor[0] = mNeighbor[1] = mNeighbor[2] = NULL;
}

Cell::Cell(Vertex *n0, Vertex *n1, Vertex *n2)
{
    mVertex[0] = n0; 
    mVertex[1] = n1; 
    mVertex[2] = n2; 
    mStatus = BADTRIANGLE;  // A cell is bad by default.
}

Vector cell::getSpacingGradient()
{
    double ld2S;
    Vector lvVec;
    Vector lvNorm;

    ld2S =fabs((mVertex[1]->m_vNode - mVertex[0]->m_vNode) ^
           (mVertex[2]->m_vNode - mVertex[0]->m_vNode));
    lvVec.SetVector(0,0);
    for (int i=0; i<3; i++)
    {
        lvNorm = mVertex[(i+1)%3]->m_vNode - mVertex[(i+2)%3]->m_vNode;
        lvNorm.MakeNormal();
        lvVec = lvVec + lvNorm * mVertex[i]->m_dSpacing;
    }
    lvVec = lvVec/ld2S;
    if ((mVertex[2]->m_vNode - mVertex[0]->m_vNode)*lvNorm <0)
        lvVec = -lvVec;
    return lvVec;
}


int cell::CheckGoodTriangle()
{
    double    ratio;

    ratio = FaceLength(FaceOrderIndx(0))/FaceLength(FaceOrderIndx(2));
    return (ratio > (BETAGOODTR)) ? GOODTRIANGLE : BADTRIANGLE;
}

// return the nearest vertex to a given point.
// input: 'point'
// output: 'distance' between 'point' and the nearest vertex
// return value: index of nearest vertex
int cell::getNearestVertex(Vector point, double& distance)
{
    distance = mVertex[0]->m_vNode.Distance(point);
    int index = 0;
    for (int i = 1; i < 3; i++) {
        double dist = mVertex[i]->m_vNode.Distance(point);
        if (dist < distance) {
            index = i;
            distance = dist;
        }
    }
    return index;
}

int cell::getOrderedEdgeLengths(int i)
{
    int min,mid,max,j;

    min = mid = max = 0;
    for (j=0; j<3; j++)
    {
        if (FaceLength(j)<FaceLength(min))
            min = j;
        if (FaceLength(j)>FaceLength(max))
            max = j;
    }

    mid = 3-min-max;

    switch (i)
    {
    case 0:
        return min;
    case 1:
        return mid;
    case 2:
        return max;
    }
}


double cell::getEdgeLength(int i)
{
    return Vector::distance(mVertex[(i+1)%3]->getPos(), mVertex[(i+2)%3]->getPos());
}


int cell::isObtuse()
{
    if (FaceLength(FaceOrderIndx(1))/FaceLength(FaceOrderIndx(2)) < 0.8/*BETA3*/)     
        return OBTUSETRIANGLE;
    else
        return ACUTETRIANGLE;
}

void addNeighbor(const Cell* cell)
{
    if (!cell) return;
    int i = 0;
    while (mNeighbor[i]) i++;
    if (i < 3) mNeighbor[i] = cell;
}

