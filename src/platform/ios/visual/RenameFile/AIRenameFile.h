//
//  AIRenameFile.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/03/24.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIIntResult.h"
#import "AITheme.h"
#import "AIFormViewController.h"
#import "AIPortal.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIRenameFile : AIIntResult
@property (nonatomic) AIFormViewController *controller;
- (instancetype)initWithTheme:(AITheme *)theme fileId:(int)fileId fileName:(NSString *)fileName portal:(AIPortal *)portal;
@end

NS_ASSUME_NONNULL_END
