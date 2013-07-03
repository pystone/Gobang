//
//  BoardModel.m
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import "BoardModel.h"
#import "BoardController.h"

@implementation BoardModel

static BoardModel* _sharedBoard;

+(BoardModel*) sharedBoard
{
    if (_sharedBoard == nil)
        _sharedBoard = [[BoardModel alloc] init];
    return _sharedBoard;
}

-(id) init
{
    if(self = [super init]){
        _board = new2DIntArray(kBoardRow, kBoardCol);
        [self resetForNewGame];
    }
    return self;
}
-(void) resetForNewGame
{
    [self clearBoard];
    _lastPos = GBIntPointMake(0, 0);
}
-(void) clearBoard
{
    for (int i=0; i<kBoardRow; ++i) {
        for (int j=0; j<kBoardCol; ++j) {
            _board[i][j] = PieceTeamNone;
        }
    }
}

-(void) setPiece:(PieceTeam)team inPos:(GBIntPoint)pos
{
    if ([self pieceExistInPos:pos] == YES)
        return;
    if (pos.r<0 || pos.r>=kBoardRow || pos.c<0 || pos.c>=kBoardCol)
        return;
    
    _board[pos.r][pos.c] = team;
    _lastPos = pos;
    return;
}

-(BOOL) pieceExistInPos:(GBIntPoint)pos
{
    if (_board[pos.r][pos.c] != PieceTeamNone)
        return YES;
    return NO;
}

-(int) getSamePieceInDirection:(GBIntPoint)oriPos andDelta:(GBIntPoint)delta withDir:(int)dir 
{
    int sum = 0;
    for (int i=1; i<5; ++i) {
        int cr = oriPos.r + delta.r*i*dir;
        int cc = oriPos.c + delta.c*i*dir;
        if(cr<0 || cr>=kBoardRow || cc<0 || cc>=kBoardCol){
            break;
        }
        if (_board[cr][cc]==PieceTeamNone || _board[cr][cc]!=_board[_lastPos.r][_lastPos.c]){
            break;
        }
        ++sum;
    }
    return sum;
}
-(BOOL) hasWinner
{
    int sum = 0;
    int dr[] = { 1, 1, 0,-1,-1,-1, 0, 1}; //从正上方开始顺时针
    int dc[] = { 0, 1, 1, 1, 0,-1,-1,-1};
    for (int i=0; i<4; ++i) {   //4个方向
        sum = 0;
        // 正向
        sum += [self getSamePieceInDirection:_lastPos andDelta:GBIntPointMake(dr[i], dc[i]) withDir:1];
        // 反向
        sum += [self getSamePieceInDirection:_lastPos andDelta:GBIntPointMake(dr[i], dc[i]) withDir:-1];
        if (sum >= 4) {
            return YES;
        }
    }
    return NO;
}

-(int **) getBoardInfo
{
    int **t = new2DIntArray(kBoardRow, kBoardCol);
//    memcpy(t, _board, sizeof(int)*kBoardRow*kBoardCol);
    for (int i=0; i<kBoardRow; ++i) {
        for (int j=0; j<kBoardCol; ++j) {
            t[i][j] = _board[i][j];
        }
    }
    return t;
//    return _board;
}

@end

