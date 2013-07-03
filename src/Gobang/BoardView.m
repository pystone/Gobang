//
//  BoardView.m
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import "BoardView.h"
#import "BoardController.h"

#define kBoardViewLineGap 40.0f
#define kPiecePicSideLen 36.0f

@implementation BoardView

-(id) init
{
    if (self = [super init]){
        NSLog(@"init!");
    }
    return self;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
        NSLog(@"initwithframe!");
        _lastPieceImg = nil;
        _boardController = [BoardController sharedBoardController];
        _pieceImgs = [NSMutableDictionary dictionaryWithCapacity:kBoardRow];
        for (int i=0; i<kBoardRow; ++i){
            [_pieceImgs setObject:[NSMutableDictionary dictionaryWithCapacity:kBoardCol] forKey:[NSNumber numberWithInt:i]];
        }
        _pieceLayer = [[NSView alloc] initWithFrame:[self bounds]];
        _lastPieceLayer = [[NSView alloc] initWithFrame:[self bounds]];
        [self addSubview:_pieceLayer];
        [self addSubview:_lastPieceLayer];
    }
    
    return self;
}

-(IBAction)startGame
{
    
}

// 绘制棋盘
- (void)drawRect:(NSRect)dirtyRect
{
    NSLog(@"BoardView:drawRect");
    // Drawing code here.
//    NSRect frame = [self frame];
//    NSRect bounds = [self bounds];
//    NSLog(@"%f,%f %f,%f", frame.size.width, frame.size.height, bounds.size.width, bounds.size.height);
//    NSLog(@"%f, %f, %f, %f", frame.origin.x, frame.origin.y, bounds.origin.x, bounds.origin.y);
//    [[NSColor greenColor] set];
    
    int wid = (kBoardCol-1) * kBoardViewLineGap;
    int hei = (kBoardRow-1) * kBoardViewLineGap;
    float delta = kBoardViewLineGap/2.0;
    for (int i=0; i<kBoardRow; ++i) {
        CGPoint beg = CGPointMake(0+delta, i*kBoardViewLineGap+delta);
        CGPoint end = CGPointMake(wid+delta, i*kBoardViewLineGap+delta);

        [NSBezierPath strokeLineFromPoint:beg toPoint:end];
    }
    for (int i=0; i<kBoardCol; ++i) {
        CGPoint beg = CGPointMake(i*kBoardViewLineGap + delta, 0+delta);
        CGPoint end = CGPointMake(i*kBoardViewLineGap + delta, hei + delta);
        [NSBezierPath strokeLineFromPoint:beg toPoint:end];
    }
//    [NSBezierPath fillRect:CGRectMake(50, 100, 23, 40)];
//    [NSBezierPath fillRect:bounds];
//    [NSBezierPath strokeLineFromPoint:CGPointMake(50, 100) toPoint:CGPointMake(300, 400)];
}

-(void) mouseDown:(NSEvent *)theEvent
{
    NSLog(@"%f, %f", [theEvent locationInWindow].x, [theEvent locationInWindow].y);
    _curClickedPt = [theEvent locationInWindow];
}

-(void) mouseUp:(NSEvent *)theEvent
{
    NSLog(@"%f, %f", [theEvent locationInWindow].x, [theEvent locationInWindow].y);
    GBIntPoint begPos = [self getCheckPosByPoint:_curClickedPt];
    GBIntPoint endPos = [self getCheckPosByPoint:[theEvent locationInWindow]];
    NSLog(@"%d,%d %d,%d", begPos.r, begPos.c, endPos.r, endPos.c);
    if(begPos.r==endPos.r && begPos.c==endPos.c){
        [_boardController boardClickedAtPos:endPos];
    }
    
    _curClickedPt = CGPointZero;
}

-(GBIntPoint) getCheckPosByPoint:(CGPoint)pt
{
    NSRect frame = [self frame];
    pt.x -= frame.origin.x;
    pt.y -= frame.origin.y;
    
    int row = (int)(pt.y/kBoardViewLineGap);
    int col = (int)(pt.x/kBoardViewLineGap);

    return GBIntPointMake(row, col);
}

-(CGRect) getPieceRectByCheckPos:(GBIntPoint)pos
{
    float x = kBoardViewLineGap/2 + pos.c*kBoardViewLineGap;
    float y = kBoardViewLineGap/2 + pos.r*kBoardViewLineGap;
    x -= kPiecePicSideLen/2;
    y -= kPiecePicSideLen/2;
    return CGRectMake(x, y, kPiecePicSideLen, kPiecePicSideLen);
}

-(void) showPiece:(PieceTeam)team inPos:(GBIntPoint)pos
{
    if (pos.r<0 || pos.r>kBoardRow || pos.c<0 || pos.c>kBoardCol){
        NSLog(@"(%d, %d) out of range!", pos.r, pos.c);
        return;
    }
    NSMutableDictionary *curRow = _pieceImgs[[NSNumber numberWithInt:pos.r]];
    
    NSString *imgName = @"";
    if(team == PieceTeamBlack){
        imgName = @"black_piece.png";
    }
    else{
        imgName = @"white_piece.png";
    }
    
    CGRect rect = [self getPieceRectByCheckPos:pos];
    NSImageView *piece = [[NSImageView alloc] initWithFrame:rect];
    [piece setImage:[NSImage imageNamed:imgName]];
    [_pieceLayer addSubview:piece];
    [curRow setObject:piece forKey:[NSNumber numberWithInt:pos.c]];
    
    // 标识最后落下的棋子
    rect.size = CGSizeMake(27.0f, 18.0f);
    if (_lastPieceImg == nil){
        _lastPieceImg = [[NSImageView alloc] initWithFrame:rect];
        [_lastPieceImg setImage:[NSImage imageNamed:@"last_piece.png"]];
        [_lastPieceLayer addSubview:_lastPieceImg];
    }
    [_lastPieceImg setFrame:rect];
}

-(void) showWin:(PieceTeam)winTeam
{
    NSString *text = @"";
    if (winTeam == PieceTeamWhite)
        text = @"白子获胜！";
    else
        text = @"黑子获胜!";
    [[NSNotificationCenter defaultCenter] postNotificationName:@"ShowWinAlert" object:nil userInfo:@{@"text":text}];
}

-(void) resetForNewGame
{
    for (id key in [_pieceImgs allKeys]) {
        NSMutableDictionary *dic = [_pieceImgs objectForKey:key];
        for (id ckey in [dic allKeys]) {
            NSImageView *img = [dic objectForKey:ckey];
            [img removeFromSuperview];
            [dic removeObjectForKey:ckey];
        }
    }
    _curClickedPt = CGPointZero;
    [_lastPieceImg removeFromSuperview];
    _lastPieceImg = nil;
}

@end
