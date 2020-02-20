//
//  AIVideoPreviewController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVKit/AVKit.h>
#import "AIPortal.h"
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIVideoPreviewController : AVPlayerViewController
- (instancetype)initWithFilePath:(NSString*)path title:(NSString *)title fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme;
@end

NS_ASSUME_NONNULL_END
