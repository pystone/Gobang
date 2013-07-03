//
//  BoardController.h
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BoardModel.h"
#import "BoardView.h"

@interface BoardController : NSObject
{
    BoardModel *_boardModel;
    BoardView *_boardView;
    
    PieceTeam _curTeam;
    
    BOOL _isGaming;
    int _searchDepth;
    PieceTeam _aiTeam;
}

+(BoardController*) sharedBoardController;
-(id) init;
-(void) setBoardView:(BoardView*)view;
-(void) resetForNewGame;

-(void) startGameWith:(int)searchDepth aiTeam:(PieceTeam)team;
-(void) gameEnd;
-(void) boardClickedAtPos:(GBIntPoint)pos;

-(void) callAIPlacePiece;

@end
