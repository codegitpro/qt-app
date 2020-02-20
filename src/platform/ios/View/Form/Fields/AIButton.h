//
//  AIButton.h
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIInputElement.h"
#import "AISize.h"
#import "AIColour.h"
#import "AITheme.h"
#import "AIValidatedForm.h"
#import "AIAlign.h"
#import "AIWeight.h"

NS_ASSUME_NONNULL_BEGIN

typedef void (^ OnClick)(int inputId, BOOL mustValidate);

@interface AIButton : UIView<AIInputElement>
@property (nonatomic, readonly) int inputId;
- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                       colour:(AIColour)colour
                     isDialog:(BOOL)isDialog
                 mustValidate:(BOOL)mustValidate
                     maxWidth:(int)maxWidth
                        align:(AIAlign)align
                    hasBorder:(BOOL)hasBorder
                    multiline:(BOOL)multiline
                       action:(OnClick)action
;

@end

NS_ASSUME_NONNULL_END
