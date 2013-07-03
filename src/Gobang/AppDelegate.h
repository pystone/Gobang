//
//  AppDelegate.h
//  Gobang
//
//  Created by PY on 13-6-9.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BoardView.h"
#import "BoardController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{

}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet BoardView *boardView;

-(void) showWinAlert:(NSNotification *)note;
-(void) winAlertCallback;
@end
