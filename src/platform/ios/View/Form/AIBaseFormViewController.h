//
//  AIBaseFormViewController.h
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIGenericForm.h"
#import "AITheme.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIBaseFormViewController : UIViewController<AIGenericForm>
@property (nonatomic, readonly) UILabel* error;
@end

NS_ASSUME_NONNULL_END
