//
//  AIRenameFolder.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/03/25.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIIntResult.h"
#import "AITheme.h"
#import "AIFormViewController.h"
#import "AIPortal.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIRenameFolder : AIIntResult
@property (nonatomic) AIFormViewController *controller;
- (instancetype)initWithTheme:(AITheme *)theme folderId:(int)folderId folderName:(NSString *)folderName portal:(AIPortal *)portal;
@end

NS_ASSUME_NONNULL_END
