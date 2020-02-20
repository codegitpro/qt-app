//
//  AIPicker.h
//  user
//
//  Created by Hans Malherbe on 2019/05/01.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIInputElement.h"
#import "AISize.h"
#import "AIColour.h"
#import "AITheme.h"
#import "AIAlign.h"
#import "AIWeight.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIPicker : UIView<AIInputElement, UIPickerViewDataSource, UIPickerViewDelegate>
@property (nonatomic, readonly) int inputId;
- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                      options:(NSArray<NSString *> *)options
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                     isDialog:(BOOL)isDialog
                   isRequired:(BOOL)isRequired
                        align:(AIAlign)align
                     maxWidth:(int)maxWidth
         parentViewController:(UIViewController *)parentViewController
                     onChange:(OnChange)onChange
;
+ (NSArray<NSString *> *)optionsFromString:(NSString *)pipeDelimitedOptions;
@end

NS_ASSUME_NONNULL_END
