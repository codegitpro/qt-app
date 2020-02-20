//
//  AIHelp.h
//  tech
//
//  Created by Hans Malherbe on 2019/05/26.
//  Copyright © 2019 DITS. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIHelp : UIImageView
- (instancetype)initWithText:(NSString *)text
                       theme:(AITheme *)theme
              viewController:(UIViewController *)viewController
;
@end

NS_ASSUME_NONNULL_END
