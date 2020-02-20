//
//  AIMultilineTextInput.h
//  user
//
//  Created by Hans Malherbe on 2019/05/04.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIInputElement.h"
#import "AISize.h"
#import "AIColour.h"
#import "AITheme.h"
#import "AIElementType.h"
#import "AIAlign.h"
#import "AIWeight.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIMultilineTextInput : UIView<AIInputElement, UITextViewDelegate>
@property (nonatomic, readonly) int inputId;
- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                     isDialog:(BOOL)isDialog
                   isRequired:(BOOL)isRequired
                        align:(AIAlign)align
                     maxWidth:(int)maxWidth
                        lines:(int)lines
                     scroller:(UIScrollView *)scroller
                     onChange:(OnChange)onChange
;
@end

NS_ASSUME_NONNULL_END
