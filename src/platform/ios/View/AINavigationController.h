//
//  AINavigationController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/02.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AINavigationController : UINavigationController
- (instancetype)initWithTheme:(AITheme *)theme;
@end

NS_ASSUME_NONNULL_END
