//
//  BoardController.m
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import "BoardController.h"
#include "GBAI.h"

static BoardController *_sharedBoardController;

@implementation BoardController

+(BoardController*) sharedBoardController
{
    if (_sharedBoardController == nil)
        _sharedBoardController = [[BoardController alloc] init];
    return _sharedBoardController;
}

-(id) init
{
    if (self = [super init]){
        _boardModel = [BoardModel sharedBoard];
        _curTeam = PieceTeamBlack;
        
        _isGaming = NO;
        _searchDepth = kDefaultSearchDepth;
        _aiTeam = PieceTeamWhite;   // 默认玩家先手
    }
    return self;
}
-(void) setBoardView:(BoardView*) view
{
    _boardView = view;
}
-(void) resetForNewGame
{
    _curTeam = PieceTeamBlack;
    [_boardView resetForNewGame];
    [_boardModel resetForNewGame];
}

//-(void) startGameWith:(int)searchDepth aiTeam:(PieceTeam)team
//{
//    _isGaming = YES;
//    _searchDepth = searchDepth;
//    _aiTeam = team;
//    
//    [self resetForNewGame];
//}
//-(void) gameEnd
//{
//    _isGaming = NO;
//}

-(void) boardClickedAtPos:(GBIntPoint)pos
{
    if ([_boardModel pieceExistInPos:pos] == NO){
        [_boardModel setPiece:_curTeam inPos:pos];
        [_boardView showPiece:_curTeam inPos:pos];
        
        if ([_boardModel hasWinner] == YES){
            [_boardView showWin:_curTeam];
            return;
        }
        
        // Only for test
//        aiTest([_boardModel getBoardInfo], _curTeam);
        
        if (_curTeam == PieceTeamWhite){
            _curTeam = PieceTeamBlack;
        }
        else{
            _curTeam = PieceTeamWhite;
            
            // 延时调用ai
            [NSTimer scheduledTimerWithTimeInterval:0.1f
                                             target:self
                                           selector:@selector(callAIPlacePiece)
                                           userInfo:nil
                                            repeats:NO];
        }
    }
}

-(void) callAIPlacePiece
{
    NSLog(@"callAIPlacePiece");
    GBIntPoint blackPos = getAIPos([_boardModel getBoardInfo]);
    if ([_boardModel pieceExistInPos:blackPos])
        ;
    [self boardClickedAtPos:blackPos];
}

@end
