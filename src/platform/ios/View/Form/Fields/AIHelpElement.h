//
//  AIHelpElement.h
//  tech
//
//  Created by Hans Malherbe on 2019/05/26.
//  Copyright © 2019 DITS. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIInputElement.h"
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIHelpElement : UIView
- (instancetype)initWithTheme:(AITheme *)theme
                        label:(NSString *)label
               viewController:(UIViewController *)viewController
;
@end

NS_ASSUME_NONNULL_END
