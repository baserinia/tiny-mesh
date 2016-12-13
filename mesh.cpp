#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "Macros.h"
#include "Mesh.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Loads boundary data from file.
void Mesh::importCase(const std::string& filename)
{
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) return;

    Vector vec;
    char  s1[20], s2[20];
    Vertex *lnVertex;
    int j,Num;
    int liNumBound[BOUNDARY_LIMIT];
    CObList lobBounds[BOUNDARY_LIMIT];

    mIsComplete = false;
    int i = 0;

    s1[0] = 'N';
    ZeroMemory(liNumBound, sizeof(liNumBound));
    while (s1[0] == 'N')   
    {
        fscanf( pFile , "%s %s\n" , s1 ,s2);
        if (s1[0] == 'N') {
            liNumBound[i] = atoi(s2);
            i++;
        }
        if (i >= BOUNDARY_LIMIT) {
            fprintf(stderr, "Too many boundaries have been defined!");
            return;
        }
    }
    Num = i;
    
    for (i=0; i<Num; i++) {
        for (j=0; j<liNumBound[i]; j++) {
            if (feof(pFile)) {
                AfxMessageBox("Data file is invalid!", MB_ICONSTOP | MB_OK);
                deleteMesh();
                break;
            }
            if ((i != 0) || (j != 0)) {
                fscanf( pFile , "%s %s\n" , s1 ,s2);
            }
            vec.m_x = atof(s1);
            vec.m_y = atof(s2);
            if ((i == 0) && (j==0)) {
                lBound.m_dx1 = lBound.m_dx2 = vec.m_x;
                lBound.m_dy1 = lBound.m_dy2 = vec.m_y;
            }
            if (vec.m_x > lBound.m_dx2) lBound.m_dx2 = vec.m_x;
            if (vec.m_x < lBound.m_dx1) lBound.m_dx1 = vec.m_x;
            if (vec.m_y > lBound.m_dy2) lBound.m_dy2 = vec.m_y;
            if (vec.m_y < lBound.m_dy1) lBound.m_dy1 = vec.m_y;
            lnVertex = new Vertex(vec);
            addBound(lnVertex,i);
        }
    }
    fclose(pFile);
    Initialize(lBound);

    POSITION pos;
    Vertex* node;
    Vertex* pPrevVertex;
    Vertex* pNextVertex;
    for (i=0;!m_olBound[i].IsEmpty(); i++) {
        pPrevVertex = (Vertex*)(m_olBound[i].GetTail());   
        for( pos = m_olBound[i].GetHeadPosition(); pos != NULL; ) {
            node = (Vertex*)(m_olBound[i].GetNext( pos ));   
            if (pos!=NULL) {
                pNextVertex = (Vertex*)(m_olBound[i].GetAt( pos ));   
            } else {
                pNextVertex = (Vertex*)(m_olBound[i].GetHead());
            }
            
            node->m_dSpacing = 0.5 * (
                pNextVertex->m_vVertex.Distance(node->m_vVertex) +
                pPrevVertex->m_vVertex.Distance(node->m_vVertex) );
            InsertNewVertex(node->m_vVertex,node->m_dSpacing);
            pPrevVertex = node;
        }
    }
    DetectInBound();
    InitialVertexCorrection();
    releaseAllBounds();
    m_iCount = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initializes mesh by generating suitable phantom points.
void Mesh::initialize(CVisibleRect rect)
{
    Vertex* vert[4];
    Cell* cell[2];
    Vector vec;
    vec.SetVector(rect.m_dx1,rect.m_dy1);
    vert[0] = new Vertex(vec);
    vec.SetVector(rect.m_dx2,rect.m_dy1);
    vert[1] = new Vertex(vec);
    vec.SetVector(rect.m_dx1,rect.m_dy2);
    vert[2] = new Vertex(vec);
    vec.SetVector(rect.m_dx2,rect.m_dy2);
    vert[3] = new Vertex(vec);
    cell[0] = new Cell(vert[0], vert[1], vert[2]);
    cell[1] = new Cell(vert[3], vert[1], vert[2]);
    cell[1]->addNeighbor(cell[0]);
    cell[0]->addNeighbor(cell[1]);
    for (int i = 0; i < 4; i++) addVertex(vert[i]);
    for (int i = 0; i < 2; i++) addCell(cell[i]);
    mCurrentCell = cell[0];

    time_t lstTime;
    time(&lstTime);
    srand(lstTime);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Add a new cell to the mesh
void Mesh::addCell(Cell* cell)
{
    try {
        mCells.push_back(cell);
    } catch (CMemoryException* perr) {
        fprintf(stderr, "Out of Memory", MB_ICONSTOP | MB_OK);
        if (cell) {
            delete cell;
            cell = NULL;
        }
        perr->Delete();
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Add a new vertex to the mesh
void Mesh::addVertex(const Vertex* vert)
{
    try {
        mVertex.push_back(node);
    } catch (CMemoryException* perr) {
        fprintf(stderr, "Out of Memory");
        if (node) {
            delete node;
            node = NULL;
        }
        perr->Delete();
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::addBound(Vertex* node,int i)
{
    try {
        m_olBound[i].AddTail(node);
    } catch (CMemoryException* perr) {
        fprintf(stderr, "Out of Memory");
        if (node) {
            delete node;
            node = NULL;
        }
        perr->Delete();
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::releaseVertexs()
{
    POSITION pos;
    Vertex* node;
    for ( pos = m_olVertex.GetHeadPosition(); pos != NULL; ) {
        node = (Vertex*)(m_olVertex.GetNext( pos ));   
        delete node;
        node=NULL;
    }
    m_olVertex.RemoveAll();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::releaseMesh()
{
    POSITION pos;
    Cell* cell;
    for (auto iter = mCells.begin(); item != mCells.end(); iter++) {
        delete (*iter);   
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::releaseBound(int i)
{
    POSITION pos;
    Vertex* node;
    for( pos = m_olBound[i].GetHeadPosition(); pos != NULL; ) {
        node = (Vertex*)(m_olBound[i].GetNext( pos ));   
        delete node;
        node = NULL;
    }
    m_olBound[i].RemoveAll();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::releaseAllBounds()
{
    for (int i = 0; i < BOUNDARY_LIMIT; i++) {
        releaseBound(i);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Number of Cells on the Mesh
int Mesh::getCellCount()
{
    return m_olMesh.GetCount();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given the point 'pt', return the cell containing the point of NULL if no
// cell contains it.
Cell* Mesh::walkSearch(const Vector& pt)
{
    Vector faceNormal;    // Normal Unit Vector of the Face
    Vector lvBP;    // Vector Connecting Face Bisection to the Point P
    Vector lvTemp;
    int lbCur,lbNext,lbPrev,lbIMax;
    double ldMax;

    lbIMax = -1;
    ldMax = 0;
    for (int i = 0; i < 3; i++) {
        lbCur = i;
        lbNext = NEXT(i);
        lbPrev = PREV(i);
        faceNormal = (m_CellCurrent->m_VertexVertex[lbCur]->m_vVertex-
                m_CellCurrent->m_VertexVertex[lbNext]->m_vVertex);
        faceNormal.MakeNormal();
        lvTemp = m_CellCurrent->m_VertexVertex[lbPrev]->m_vVertex - 
                m_CellCurrent->m_VertexVertex[lbCur]->m_vVertex;

        if (lvTemp*faceNormal > 0) {
            faceNormal = -faceNormal;
        }
        lvBP = vec - (m_CellCurrent->m_VertexVertex[lbCur]->m_vVertex+
            0.5 * m_CellCurrent->m_VertexVertex[lbNext]->m_vVertex);
        if (lvBP*faceNormal > ldMax) {
            lbIMax = lbPrev;
            ldMax = lvBP*faceNormal;
        }
    }
    if (lbIMax == -1) {
        return m_CellCurrent;
    } else if (m_CellCurrent->m_CellNeighbor[lbIMax] != NULL) {
        m_CellCurrent = m_CellCurrent->m_CellNeighbor[lbIMax];
        return walkSearch(vec);
    }
    return NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Insert a new point in the mesh and update the connectivity.
void Mesh::insertVertex(const Vector& vec, double sp)
{
    Cell* lcFound;
    Cell* lcNew[3];
    Vertex* lnNewVertex;
    POSITION pos;
    BYTE lbCur,lbNxt,lbPrv;
    lcFound = walkSearch(vec);
    if ((lcFound == NULL) || (lcFound->mStatus == BOUNDTRIANGLE)) return;
    lnNewVertex = new Vertex(vec,space);
    addVertex(lnNewVertex);
    for (int i = 0; i < 3; i++) {
        lbCur = i;
        lbNxt = NEXT(i);
        lbPrv = PREV(i);
        lcNew[lbCur] = new Cell(
            lcFound->m_VertexVertex[0],
            lcFound->m_VertexVertex[1],
            lcFound->m_VertexVertex[2],
            NULL, NULL, NULL);
        lcNew[lbCur]->m_VertexVertex[lbCur] = lnNewVertex;
        lcNew[lbCur]->mStatus = BAD_TRIANGLE;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                lcNew[i]->m_CellNeighbor[j] = lcFound->m_CellNeighbor[j];
            } else {
                lcNew[i]->m_CellNeighbor[j] = lcNew[j];
            }
            
            if (lcFound->m_CellNeighbor[i]!=NULL) {
                if (lcFound->m_CellNeighbor[i]->m_CellNeighbor[j] == lcFound) {
                    lcFound->m_CellNeighbor[i]->m_CellNeighbor[j] = lcNew[i];
                }
            }
        }
        addCell(lcNew[i]);
    }
    pos = m_olMesh.Find(lcFound);
    RELEASE(lcFound);
    m_olMesh.RemoveAt(pos);
    m_CellCurrent = lcNew[0];
    for (int i = 0; i < 3; i++) {
        enforceDelaunay(lcNew[i], lnNewVertex);
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given a triangular mesh cell and a mesh vertex, determine if the vertex lies 
// inside the circumcircle of the triangle. The result will be used to determine
// if mesh edges need swapping.
int Mesh::CheckInCircle(Cell *cell, Vertex *node)
{
    int i,liIndx1,liIndx2;
    Cell* lcpNeighbor;
    Vector lvVec1,lvVec2;
    double ldAng1,ldAng2;

    if (!cell) return INVALID_DATA;
    liIndx1 = -1;
    for (int i = 0; i < 3; i++) {
        if (cell->m_VertexVertex[i] == node) {
            liIndx1 = i;
        }
    }
    if (liIndx1 == -1) {
        return INVALID_DATA;
    }
    liIndx2 = -1;
    lcpNeighbor = cell->m_CellNeighbor[liIndx1];
    if (lcpNeighbor == NULL) {
        return INVALID_DATA;
    }
    for (i = 0; i < 3; i++) {
        if (lcpNeighbor->m_CellNeighbor[i] == cell) {
            liIndx2 = i;
        }
    }
    if ((liIndx2 == -1) || (lcpNeighbor == NULL)) return INVALID_DATA;
    lvVec1 = cell->m_VertexVertex[NEXT(liIndx1)]->m_vVertex -
             cell->m_VertexVertex[liIndx1]->m_vVertex;
    lvVec2 = cell->m_VertexVertex[PREV(liIndx1)]->m_vVertex -
             cell->m_VertexVertex[liIndx1]->m_vVertex;
    lvVec1.MakeUnit();
    lvVec2.MakeUnit();
    ldAng1 = acos(lvVec1 * lvVec2);
    lvVec1 = lcpNeighbor->m_VertexVertex[NEXT(liIndx2)]->m_vVertex -
             lcpNeighbor->m_VertexVertex[liIndx2]->m_vVertex;
    lvVec2 = lcpNeighbor->m_VertexVertex[PREV(liIndx2)]->m_vVertex -
             lcpNeighbor->m_VertexVertex[liIndx2]->m_vVertex;
    lvVec1.MakeUnit();
    lvVec2.MakeUnit();
    ldAng2 = acos(lvVec1 * lvVec2);
    return ((ldAng1 + ldAng2) < PI) ? GOOD_CIRCLE : BAD_CIRCLE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Mesh::CheckMinLength(Cell *cell, Vertex *node)
{
    int i,liIndx1,liIndx2;
    Cell* lcpNeighbor;
    Vector lvVec1,lvVec2;
    double ldAng1,ldAng2;
    if (cell == NULL) return INVALID_DATA;
    liIndx1 = -1;
    for (i = 0; i < 3; i++) {
        if (cell->m_VertexVertex[i] == node) {
            liIndx1 = i;
        }
    }
    if (liIndx1 == -1) return INVALID_DATA;
    liIndx2 = -1;
    lcpNeighbor = cell->m_CellNeighbor[liIndx1];
    if (lcpNeighbor == NULL) {
        return INVALID_DATA;
    }
    for (i = 0; i < 3; i++) {
        if (lcpNeighbor->m_CellNeighbor[i] == cell) {
            liIndx2 = i;
        }
    }
    if ((liIndx2 == -1) || (lcpNeighbor == NULL)) {
        return INVALID_DATA;
    }
    lvVec1 = cell->m_VertexVertex[NEXT(liIndx1)]->m_vVertex -
             cell->m_VertexVertex[PREV(liIndx1)]->m_vVertex;
    lvVec2 = cell->m_VertexVertex[liIndx1]->m_vVertex -
             lcpNeighbor->m_VertexVertex[liIndx2]->m_vVertex;
    return (lvVec1.Distance() < lvVec2.Distance()) ? GOOD_CIRCLE : BAD_CIRCLE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Checks the Delaunay condition for two neighbor cells.
void Mesh::enforceDelaunay(Cell *cell1, Vertex *node)
{
    if (CheckInCircle(cell1, node) == BAD_CIRCLE) {
        Cell *cell2 = swapEdge(cell1, node);
        if (!cell2) return;
        enforceDelaunay(cell1, node);
        enforceDelaunay(cell2, node);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cell* Mesh::swapEdge(Cell *cell1, Vertex *node)
{
    int liCur1,liCur2;
    Cell *cell2;
    Cell *lcpTemp1,*lcpTemp2;
    if (cell1 == NULL) return NULL;
    liCur1 = -1;
    for (i = 0; i < 3; i++) {
        if (cell1->m_VertexVertex[i] == node) liCur1 = i;
    }
    if ((liCur1 == -1) || (cell1->m_CellNeighbor[liCur1] == NULL)) {
        return NULL;
    }
    cell2 = cell1->m_CellNeighbor[liCur1];
    for (i=0; i<3; i++) {
        if (cell2->m_CellNeighbor[i] == cell1) liCur2 = i;
    }
    if ((cell1->mStatus == BOUNDTRIANGLE)||(cell2->mStatus == BOUNDTRIANGLE)) {
        return NULL;
    }
    lcpTemp1 = new Cell(
        cell1->m_VertexVertex[liCur1],
        cell1->m_VertexVertex[NEXT(liCur1)],
        cell2->m_VertexVertex[liCur2], NULL,
        cell2, cell1->m_CellNeighbor[PREV(liCur1)] );

    lcpTemp2 = new Cell(
        cell1->m_VertexVertex[liCur1],
        cell1->m_VertexVertex[PREV(liCur1)],
        cell2->m_VertexVertex[liCur2], NULL,
        cell1,
        cell1->m_CellNeighbor[NEXT(liCur1)] );
    if (cell1->m_VertexVertex[NEXT(liCur1)] == cell2->m_VertexVertex[NEXT(liCur2)]) {
        lcpTemp1->m_CellNeighbor[0] = cell2->m_CellNeighbor[PREV(liCur2)];
        lcpTemp2->m_CellNeighbor[0] = cell2->m_CellNeighbor[NEXT(liCur2)];
        for (i = 0; i < 3; i++) {
            if ((cell2->m_CellNeighbor[PREV(liCur2)] != NULL) &&
                    (cell2->m_CellNeighbor[PREV(liCur2)]->m_CellNeighbor[i] == cell2)) {
                cell2->m_CellNeighbor[PREV(liCur2)]->m_CellNeighbor[i] = cell1;
            }
            if ((cell1->m_CellNeighbor[NEXT(liCur1)] != NULL) &&
                    (cell1->m_CellNeighbor[NEXT(liCur1)]->m_CellNeighbor[i] == cell1)) {
                cell1->m_CellNeighbor[NEXT(liCur1)]->m_CellNeighbor[i] = cell2;
            }
        }
    } else {
        lcpTemp1->m_CellNeighbor[0] = cell2->m_CellNeighbor[NEXT(liCur2)];
        lcpTemp2->m_CellNeighbor[0] = cell2->m_CellNeighbor[PREV(liCur2)];
        for (i = 0; i < 3; i++) {
            if ((cell2->m_CellNeighbor[NEXT(liCur2)] != NULL) &&
                    (cell2->m_CellNeighbor[NEXT(liCur2)]->m_CellNeighbor[i] == cell2)) {
                cell2->m_CellNeighbor[NEXT(liCur2)]->m_CellNeighbor[i] = cell1;
            }
            if ((cell1->m_CellNeighbor[NEXT(liCur1)] != NULL) &&
                    (cell1->m_CellNeighbor[NEXT(liCur1)]->m_CellNeighbor[i] == cell1)) {
                cell1->m_CellNeighbor[NEXT(liCur1)]->m_CellNeighbor[i] = cell2;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        cell1->m_VertexVertex[i] = lcpTemp1->m_VertexVertex[i];
        cell1->m_CellNeighbor[i] = lcpTemp1->m_CellNeighbor[i];
        cell2->m_VertexVertex[i] = lcpTemp2->m_VertexVertex[i];
        cell2->m_CellNeighbor[i] = lcpTemp2->m_CellNeighbor[i];
    }
    cell1->mStatus = cell2->mStatus = BAD_TRIANGLE;
    delete lcpTemp1;
    delete lcpTemp2;
    return cell2;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::generateRandomCloud()
{
    Vector lvVec;
    double ldx,ldy;
    CVisibleRect lrect(0.0, 0.0, 1.0, 1.0);
    deleteMesh();
    Initialize(lrect);
    for (int i = 0; i < 100; i++) {
        ldx = (double)rand()/RAND_MAX;
        ldy = (double)rand()/RAND_MAX;
        lvVec.SetVector(ldx,ldy);
        if (getCellCount()>0) {
            InsertNewVertex(lvVec,0);
        }
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Release memory allocated to the mesh
void Mesh::deleteMesh()
{
    releaseMesh();
    releaseVertexs();
    releaseAllBounds();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Extracts triangles which are in the inner boundaries.
void Mesh::DetectInBound()
{
    POSITION pos;
    Vertex* node;
    Vertex* pPrevVertex;
    Cell* cell;
    Vector vec1,vec2;
    int i,j,indx;
    BOOL lbStatus;
    for (int i = 0; !m_olBound[i].IsEmpty(); i++) {
        pPrevVertex = (Vertex*)(m_olBound[i].GetTail());   
        for( pos = m_olBound[i].GetHeadPosition(); pos != NULL; ) {
            node = (Vertex*)(m_olBound[i].GetNext( pos ));   
            vec1 = (node->m_vVertex - pPrevVertex->m_vVertex);
            vec1.MakeNormal();
            cell =  walkSearch( (node->m_vVertex+pPrevVertex->m_vVertex)/2 );
            if (cell == NULL) return;
            for (j = 0; j < 3; j++) {
                if ((cell->m_VertexVertex[j]->m_vVertex != node->m_vVertex) &&
                        (cell->m_VertexVertex[j]->m_vVertex != pPrevVertex->m_vVertex)) {
                    indx = j;
                }
            }
            vec2 = cell->m_VertexVertex[indx]->m_vVertex - node->m_vVertex;
            if ((vec1 * vec2 < 0) && (i!=0) ) {
                cell = cell->m_CellNeighbor[indx];
            }
            if ((vec1 * vec2 > 0) && (i==0)) {
                cell = cell->m_CellNeighbor[indx];
            }
            cell->mStatus = BOUNDTRIANGLE;
            pPrevVertex = node;
        }
    }

    for ( pos = m_olMesh.GetHeadPosition(); pos != NULL; ) {
        cell = (Cell*)(m_olMesh.GetNext( pos ));   
        lbStatus = GOOD_TRIANGLE;
        for (i=0; i<3; i++) {
            if ((cell->m_CellNeighbor[i] != NULL) && 
                (cell->m_CellNeighbor[i]->mStatus == BAD_TRIANGLE)) {
                lbStatus = BAD_TRIANGLE;
            }
        }    
        if (cell->mStatus == BAD_TRIANGLE) {
            cell->mStatus = lbStatus;
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Genearting new nodes.
void Mesh::constructNewVertex()
{
    POSITION pos;
    Cell* cell, cell2;
    Vertex* node, node2;
    Vector vec1,vec2;
    double ldhM,ldTemp,ldl,dist,min;
    char buffer[20];
    for (pos = m_olMesh.GetHeadPosition(); pos != NULL;) {
        cell = (Cell*)m_olMesh.GetNext( pos );   
        for (int j = 0; j < 2; j++) {
            if (j == 0) {
                i = cell->FaceOrderIndx(0);
            } else {
                if ( cell->isObtuse() ) {
                    i = cell->FaceOrderIndx(1);
                } else {
                    continue;
                }
            }
            if ((cell->mStatus == BAD_TRIANGLE) &&
                    (cell->m_CellNeighbor[i] != NULL) && 
                    (cell->m_CellNeighbor[i]->mStatus != BAD_TRIANGLE)) {
                ldhM = (cell->m_VertexVertex[NEXT(i)]->m_dSpacing + 
                        cell->m_VertexVertex[PREV(i)]->m_dSpacing)/2;
                vec1 = (cell->m_VertexVertex[NEXT(i)]->m_vVertex - 
                         cell->m_VertexVertex[PREV(i)]->m_vVertex);
                vec1.MakeNormal();
                vec1.MakeUnit();
                vec2 =  cell->m_VertexVertex[i]->m_vVertex - 
                        cell->m_VertexVertex[NEXT(i)]->m_vVertex;
                if (vec1 * vec2 < 0) {
                    vec1 = -vec1;
                }
                vec2 = (cell->m_VertexVertex[NEXT(i)]->m_vVertex + 
                        cell->m_VertexVertex[PREV(i)]->m_vVertex) / 2 +
                        (vec1 *cell->FaceLength(i)*0.866);
                cell2 = walkSearch(vec2);
                if (cell2 == NULL) {
                    continue;
                }
                ldTemp = cell2->SpacingGrad() * vec1;
                ldl = ldhM/(2/sqrt(3) - ldTemp/2);
                if ((ldl/cell->FaceLength(i))<BETALIMITDN) {
                    ldl = BETALIMITDN*cell->FaceLength(i);
                }
                if ((ldl/cell->FaceLength(i))>(BETALIMITUP)) {
                    ldl =cell->FaceLength(i)/BETALIMITUP;
                }
                vec2 = (cell->m_VertexVertex[NEXT(i)]->m_vVertex + 
                        cell->m_VertexVertex[PREV(i)]->m_vVertex) / 2 +
                        (vec1 *ldl);
                node = new Vertex(vec2);
                node->m_dSpacing = cell2->m_VertexVertex[0]->m_dSpacing +
                    cell2->SpacingGrad() * ( node->m_vVertex - cell2->m_VertexVertex[0]->m_vVertex);
                addBound(node,0);
            }
        }
    }    
    MergeNewPoint();
    OmitNewPoint();
    for( pos = m_olBound[0].GetHeadPosition(); pos != NULL; ) {
        node = (Vertex*)(m_olBound[0].GetNext( pos ));
        InsertNewVertex(node->m_vVertex,node->m_dSpacing);
    }
    releaseAllBounds();
    markBadTris();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::markBadTris()
{
    for (auto iter = mCells.begin(); item != mCells.end(); iter++) {
        Cell* cell = *iter;
        if ( (cell->mStatus == BAD_TRIANGLE) && (cell->CheckGoodTriangle() != BAD_TRIANGLE) ) {
            cell->setStatus( GOOD_TRIANGLE );
        }
    }    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::OmitNewPoint()
{
    POSITION pos,minpos;
    Cell* cell;
    Vertex* node, node2;
    int i,j,k,indx;
    double ldmin,ld,ldspace,min,dist;
    Vector vec;
    for (pos = m_olBound[0].GetHeadPosition(); pos != NULL;) {
        minpos = pos;
        node = (Vertex*)(m_olBound[0].GetNext( pos ));   
        vec = node->m_vVertex;
        cell = walkSearch(vec);
        if (cell == NULL) return ;

        int minIndex = getNearestVertex(vec, min);
        node2 = cell->m_VertexVertex[minIndex];
        for (int i = 0; i < 3; i++) {
            if (cell->m_CellNeighbor[i] != NULL) {
                int index = cell->m_CellNeighbor[i]->getNearestVertex(vec, dist);
                if (dist <min) {
                    min = dist;
                    node2 = cell->m_CellNeighbor[i]->m_VertexVertex[index];
                }
            }
        }
        if (node2 != NULL) {
            if (min < BETAOMIT*(node->m_dSpacing+node2->m_dSpacing)/2) {
                delete node;
                m_olBound[0].RemoveAt(minpos);
            }
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::MergeNewPoint()
{
    POSITION pos,pos2,minpos,prev;
    Vertex* node;
    Vertex* node2;
    double dist,min;

    for( pos = m_olBound[0].GetHeadPosition(); pos != NULL; )   
    {
        node = (Vertex*)(m_olBound[0].GetNext( pos ));   
        min = m_vrTotRect.Width();    // very large value
        dist = min;
        minpos = NULL;
        for( pos2 = m_olBound[0].GetHeadPosition(); pos2 != NULL; )   
        {
            prev=pos2;
            node2 = (Vertex*)(m_olBound[0].GetNext( pos2 ));   
            if (pos != pos2) dist=node->m_vVertex.Distance(node2->m_vVertex);
            if (dist < min) 
            {
                min = dist;
                minpos = prev;
            }
        }
        if (minpos != NULL)
        {
            node2 = (Vertex*)(m_olBound[0].GetAt( minpos ));   
            if (min < BETAMERGE*(node->m_dSpacing+node2->m_dSpacing)/2)
            {
                node->m_vVertex = ( node->m_vVertex + node2->m_vVertex)/2;
                node->m_dSpacing = ( node->m_dSpacing + node2->m_dSpacing)/2;
                if (minpos == pos)
                    node = (Vertex*)(m_olBound[0].GetNext( pos ));   
                delete node2;
                m_olBound[0].RemoveAt(minpos);
            }
        }
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::DeleteInBound()
{
    POSITION pos,curpos,pos2,curpos2;
    Cell* cell;
    Vertex* node;
    int i;
    if ((m_olVertex.IsEmpty()) || !mIsComplete) return;
    for (pos = m_olMesh.GetHeadPosition(); pos != NULL;) {
        curpos = pos;
        cell = (Cell*)m_olMesh.GetNext( pos );   
        if ((cell != NULL) && (cell->mStatus == BOUNDTRIANGLE)) {
            RELEASE(cell);
            m_olMesh.RemoveAt(curpos);
        }
    }    
    for (pos = m_olVertex.GetHeadPosition(), i = 0; i < 4; i++) {
        curpos = pos;
        node = (Vertex*)m_olVertex.GetNext( pos );   
        for (pos2 = m_olMesh.GetHeadPosition(); pos2 != NULL;) {
            curpos2 = pos2;
            cell = (Cell*)m_olMesh.GetNext( pos2 );   
            if ((cell->m_VertexVertex[0] == node) ||
                    (cell->m_VertexVertex[1] == node) ||
                    (cell->m_VertexVertex[2] == node)) {
                RELEASE(cell);
                m_olMesh.RemoveAt(curpos2);
            }
        }    
    }
    for (pos = m_olVertex.GetHeadPosition(), i = 0; i < 4; i++) {
        curpos = pos;
        node = (Vertex*)m_olVertex.GetNext( pos );   
        RELEASE(node);
        m_olVertex.RemoveAt(curpos);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::addFront()
{
    int Num;
    if (mIsComplete) return;
    Num = m_olVertex.GetCount();
    ConstructVertex();
    if (Num == m_olVertex.GetCount()) {
        mIsComplete = true;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::triangulate()
{
    int Num;
    if (mIsComplete) return;
    Num = 0;
    for (; Num != m_olVertex.GetCount();) {
        Num = m_olVertex.GetCount();
        ConstructVertex();
    }
    mIsComplete = true;
    DeleteInBound();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::renumberMesh()
{
    Vertex* node;
    Cell* cell;
    POSITION    pos;
    int i;
    for (pos = m_olVertex.GetHeadPosition(),i = 1 ; pos != NULL; i++) {
        node = (Vertex*)m_olVertex.GetNext( pos );   
        node->m_iIndex = i;
    }
    for (pos = m_olMesh.GetHeadPosition(),i = 1 ; pos != NULL; i++) {
        cell = (Cell*)m_olMesh.GetNext( pos );   
        cell->m_iIndex = i;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::PostProcess()
{
    POSITION pos,pos2;
    Vertex* node;
    Vertex* pPrevVertex;
    Cell* cell;
    Cell* pfCell;
    Cell* plCell;
    Vector vec1,vec2,vec;
    int indx,num;
    BOOL lbStatus;
    for( pos = m_olVertex.GetHeadPosition(); pos != NULL; ) {
        node = (Vertex*)(m_olVertex.GetNext( pos ));   
        vec.SetVector(0,0);
        num = 0;
        lbStatus = false;
        for( pos2 = m_olMesh.GetHeadPosition(); pos2 != NULL; ) {
            cell = (Cell*)(m_olMesh.GetNext( pos2 ));
            for (i = 0; i < 3; i++) {
                if (cell->m_VertexVertex[i] == node) {
                    if (cell->mStatus == BOUNDTRIANGLE) {
                        lbStatus = true;
                    }
                    vec1 = (cell->m_VertexVertex[NEXT(i)]->m_vVertex +
                        cell->m_VertexVertex[PREV(i)]->m_vVertex);
                    vec = vec + vec1;
                    num++;
                }
            }    
        }
        if (lbStatus) continue;
        vec = vec / (double)(2*num);
        node->m_vVertex = vec * 0.8 +  node->m_vVertex * 0.2;
    }    
    markBadTris();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::exportPoints(const std::string& name)
{
    FILE* pFile;
    char  s1[20],s2[20];
    POSITION pos;
    Vertex* node;
    Cell* cell;
    pFile = fopen(name.c_str(),"w");
    for( pos = m_olBound[0].GetHeadPosition(); pos != NULL; ) {
        node = (Vertex*)(m_olBound[0].GetNext( pos ));   
        fprintf(pFile,"%8.4f %8.4f \n",node->m_vVertex.m_x,node->m_vVertex.m_y);
    }
    fclose(pFile);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Mesh::ExportMesh(const std::string& filename)
{
    FILE* pFile;
    char  s1[20],s2[20];
    POSITION pos;
    Vertex* node;
    Cell* cell;
    Numbering();
    pFile = fopen(LPCTSTR(str),"w");
    fprintf(pFile,"VARIABLES = \"X\" , \"Y\" , \"Z\"\n");
    fprintf(pFile,"ZONE N=%d, E=%d, F=FEPOINT, ET=TRIANGLE \n",
        m_olVertex.GetCount(), m_olMesh.GetCount());
    for (pos = m_olVertex.GetHeadPosition(); pos != NULL;) {
        node = (Vertex*)(m_olVertex.GetNext( pos ));   
        fprintf(pFile,"%8.4f %8.4f %8.4f\n",node->m_vVertex.m_x,node->m_vVertex.m_y,node->m_dSpacing);
    }
    for (pos = m_olMesh.GetHeadPosition(); pos != NULL;) {
        cell = (Cell*)(m_olMesh.GetNext( pos ));   
        fprintf(pFile,"%5d %5d %5d \n",
            cell->m_VertexVertex[0]->m_iIndex,
            cell->m_VertexVertex[1]->m_iIndex,
            cell->m_VertexVertex[2]->m_iIndex);
    }
    fclose(pFile);
}

