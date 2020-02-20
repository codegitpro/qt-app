//
//  AICreateFolderScreen.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIIntResult.h"
#import "AITheme.h"
#import "AIFormViewController.h"
#import "AIPortal.h"

NS_ASSUME_NONNULL_BEGIN

@interface AICreateFolderScreen : AIIntResult
@property (nonatomic) AIFormViewController *controller;
- (instancetype)initWithTheme:(AITheme *)theme directoryId:(int)directoryId portal:(AIPortal *)portal title:(NSString *)title;
@end

NS_ASSUME_NONNULL_END
