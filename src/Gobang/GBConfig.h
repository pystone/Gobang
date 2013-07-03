//
//  GBConfig.h
//  Gobang
//
//  Created by PY on 13-6-10.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#ifndef Gobang_GBConfig_h
#define Gobang_GBConfig_h


#define kBoardRow 15
#define kBoardCol 15
#define kDefaultSearchDepth 3

typedef enum  {
    PieceTeamNone = 0,
    PieceTeamBlack = 1,
    PieceTeamWhite = 2
} PieceTeam;

static inline PieceTeam AnotherTeam(PieceTeam ori)
{
    if (ori == PieceTeamNone)
        return PieceTeamNone;
    return ori==PieceTeamBlack?PieceTeamWhite:PieceTeamBlack;
}

// GBIntPoint
typedef struct {
    int r;
    int c;
} GBIntPoint;
static inline GBIntPoint GBIntPointMake(int row, int col)
{
    GBIntPoint p;
    p.c = col; p.r = row;
    return p;
}

// 2-dimension array alloc in C style
static inline int ** new2DIntArray(int r, int c)
{
    int ** t;
    t = (int**)malloc(sizeof(int*)*r);
    t[0] = (int *)malloc(sizeof(int)*r*c);
    for(int i=1; i<r; i++)
    {
        t[i] = t[i-1] + c;
    }
    return t;
}
static inline void delete2DIntArray(int **arr)
{
    free(arr[0]);
    free(arr);
}
static inline int ** copy2DIntArray(int **ori, int r, int c)
{
    int **t = new2DIntArray(r, c);
//    printf("copy2DIntArray: r(%d) c(%d)", r, c);
    for (int i=0; i<r; ++i) {
        for (int j=0; j<c; ++j) {
            t[i][j] = ori[i][j];
        }
    }
    return t;
}

#endif
