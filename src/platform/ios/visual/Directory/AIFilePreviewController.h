//
//  AIFilePreviewController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIPortal.h"
#import "AITheme.h"

@interface AIFilePreviewController : UIViewController
- (instancetype)initWithFilePath:(NSString*)path title:(NSString *)title size:(int64_t)size fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme;
@end
