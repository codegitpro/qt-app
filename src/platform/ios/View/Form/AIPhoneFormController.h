//
//  AIPhoneFormController.h
//  user
//
//  Created by Hans Malherbe on 2019/05/04.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIField.h"
#import "AITheme.h"
#import "AIFormViewmodel.h"
#import "AIGenericForm.h"
#import "AIReference.h"
#import "AIInputElement.h"
#import "AIFormView.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIPhoneFormController : UIViewController<AIGenericForm, AIFormView>
- (instancetype)initWithTheme:(nonnull AITheme *)theme
                            vm:(nullable AIFormViewmodel *)vm
                    references:(nonnull NSArray<AIReference *> *)references
                      showMenu:(BOOL)showMenu
                      isDialog:(BOOL)isDialog
                      onChange:(nullable OnChange)onChange
;
@end

NS_ASSUME_NONNULL_END
