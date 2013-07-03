//
//  AppDelegate.m
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import "AppDelegate.h"
#import "BoardController.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    NSLog(@"finish launch?");
    [[BoardController sharedBoardController] setBoardView:self.boardView];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showWinAlert:) name:@"ShowWinAlert" object:nil];
}

-(void) showWinAlert:(NSNotification *)note
{
    NSString *text = [[note userInfo] objectForKey:@"text"];
    NSAlert *alert = [NSAlert alertWithMessageText:text defaultButton:@"我知道了" alternateButton:nil otherButton:nil informativeTextWithFormat:@"点击\"我知道了\"开始新的一局"];
    
    [alert beginSheetModalForWindow:self.window modalDelegate:self didEndSelector:@selector(winAlertCallback) contextInfo:nil];
}

-(void) winAlertCallback
{
    NSLog(@"winAlertCallback");
    [[BoardController sharedBoardController] resetForNewGame];
}

@end
