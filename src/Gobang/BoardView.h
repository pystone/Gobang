//
//  BoardView.h
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#include "GBConfig.h"
@class BoardController;

@interface BoardView : NSView
{
    BoardController *_boardController;
    CGPoint _curClickedPt;
    NSMutableDictionary *_pieceImgs;
    NSImageView *_lastPieceImg;
    
    NSView *_pieceLayer;
    NSView *_lastPieceLayer;
}

@property(nonatomic, retain) IBOutlet NSTextFieldCell* _stepInput;
@property(nonatomic, retain) IBOutlet NSComboBox* _firstOrSecond;

-(IBAction) startGame;

-(GBIntPoint) getCheckPosByPoint:(CGPoint) pt;
-(CGRect) getPieceRectByCheckPos:(GBIntPoint)pos;
-(void) showPiece:(PieceTeam)team inPos:(GBIntPoint)pos;
-(void) showWin:(PieceTeam)winTeam;
-(void) resetForNewGame;
@end
