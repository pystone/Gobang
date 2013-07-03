//
//  BoardModel.h
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GBConfig.h"
@class BoardController;

@interface BoardModel : NSObject 
{
//    int _board[kBoardRow][kBoardCol];
    int **_board;
    GBIntPoint _lastPos;
}

+(BoardModel*) sharedBoard;
-(id) init;
-(void) resetForNewGame;

-(void) clearBoard;
-(void) setPiece:(PieceTeam)team inPos:(GBIntPoint)pos;
-(BOOL) pieceExistInPos:(GBIntPoint)pos;
-(BOOL) hasWinner;

-(int **) getBoardInfo;

@end

