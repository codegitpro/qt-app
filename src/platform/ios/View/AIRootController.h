//
//  AIRootController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/06.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AINavigationController.h"
#import "AITheme.h"
#import "AIRootViewmodel.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIRootController : AINavigationController
- (instancetype)initWithTheme:(AITheme *)theme
                           vm:(AIRootViewmodel *)vm
                    menuIndex:(int)index
                        title:(NSString *)title
                         icon:(NSString *)icon
;
@end

NS_ASSUME_NONNULL_END
