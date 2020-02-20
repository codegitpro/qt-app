//
//  AIConfirmController.h
//  user
//
//  Created by Hans Malherbe on 2019/05/12.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AITheme.h"
#import "AIFormViewmodel.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIConfirmController : UIViewController
- (instancetype)initWithTitle:(nonnull NSString *)message
                          yes:(nonnull NSString *)yes
                           no:(nonnull NSString *)no
                        theme:(nonnull AITheme *)theme
                           vm:(AIFormViewmodel *)vm
;
@end

NS_ASSUME_NONNULL_END
