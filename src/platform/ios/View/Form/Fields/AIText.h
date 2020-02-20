//
//  AIText.h
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
#import "AIAlign.h"
#import "AIWeight.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIText : UIView<AIInputElement>
- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                        align:(AIAlign)align
                     isDialog:(BOOL)isDialog
                     maxWidth:(int)maxWidth
;
@property (nonatomic, readonly) int inputId;
@property (nonatomic, readonly) UILabel *label;
@end

NS_ASSUME_NONNULL_END
