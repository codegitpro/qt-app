//
//  AIPhotoController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/15.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIPreViewmodel.h"
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIPhotoController : UIViewController
- (instancetype)initWithTitle:(NSString *)title
                              vm:(AIPreViewmodel *)vm
                           theme:(AITheme *)theme
                            path:(NSString *)path
;
@end

NS_ASSUME_NONNULL_END
