#ifndef FILE_MACROS_H_INCLUDED
#define FILE_MACROS_H_INCLUDED

template<typename T> T pow2(T val) { return val*val;}
#define SQR(X) ((X)*(X))
#define RELEASE(X) { delete X; X = NULL;}
#define SAFERELEASE(X)          { if(X) delete X; X=NULL; }
#define NEXT(X)                 ((X)+4)%3
#define PREV(X)                 ((X)+2)%3

#define PI                      3.141592653589794
#define INVALID_DATA            0
#define BAD_CIRCLE              1
#define GOOD_CIRCLE             2

#define	ZOOMINRATE              0.9
#define ZOOMOUTRATE             1.1

#define LINESCROLL              0.02
#define PAGESCROLL              0.9

#define MAXZOOMIN               30
#define MAXZOOMOUT              0.5

#define BAD_TRIANGLE             0
#define GOODTRIANGLE            1
#define	BOUNDTRIANGLE           2
#define OBTUSETRIANGLE          3
#define	ACUTETRIANGLE           4

#define MAX_BOUNDARY_NUM        100

#define	BETA                    0.7
#define	BETA_LIMIT_LOW             0.5
#define	BETA_LIMIT_HI             1.4
#define BETA_GOOD_TRI              0.6
#define BETAMERGE               0.8
#define BETAOMIT                0.6
#define	ROUNDOFF                1E-10

#endif	// FILE_MACROS_H_INCLUDED
