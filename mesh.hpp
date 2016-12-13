#ifndef FILE_MESH_H_INCLUDED
#define FILE_MESH_H_INCLUDED

#include "Node.h"
#include "Cell.h"
#include "Macros.h"
#include "VisibleRect.h"

class Mesh {
    int mCount;
    std::vector<Cell*> mCells; // triangular cells
    std::vector<Edge*> mEdges; // edges
    std::vector<Vertex*> mVerts; // vertexes
    std::list< std::vector<Vertex*> > mBounds; // boundaries
    Cell* mCurrentCell;
    bool mMeshingFinished;

public:
    Mesh() {}
    ~Mesh() { DeleteMesh(); }

    void importCase(const std::string& filename);
    void exportPoints(const std::string& filename);
    void exportMesh(const std::string& filename);
    
    void addNode(const Node*);
    void addBound(CNode* pNode,int i);
    void addCell(Cell *pCell);
    void addFront();

    void deleteInBound();
    void deleteMesh();

    void releaseMesh();
    void releaseBound(int i);
    void releaseAllBounds();
    void releaseNodes();

    void postProcess();
    void renumberMesh();
    void triangulate();


    void mergeNewPoint();
    void omitNewPoint();
    void findBadTriangles();
    void constructNode();
    void detectInBound();
    void initialize(CVisibleRect rect);

    void generateRandomCloud();
    Cell* swapFace(Cell *pCell1, CNode *pNode);
    void enforceDelaunay(Cell *pCell, CNode *pNode);
    int checkInCircle(Cell *pCell, CNode *pNode);
    int checkMinLength(Cell *pCell, CNode *pNode);
    Cell* walkSearch(CVector vec);
    int getCellCount();
    void insertNewNode(CVector vec,double space);
};

#endif // FILE_MESH_H_INCLUDED
