//
//  AIBaseForm.h
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AIGenericForm.h"
#import "AITheme.h"
#import "AIElement.h"
#import "AIFormViewmodel.h"
#import "AIValidatedForm.h"
#import "AIReference.h"
#import "AIField.h"
#import "AIInputElement.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIBaseForm : NSObject<AIGenericForm, AIValidatedForm>
- (instancetype)initWithTheme:(AITheme *)theme
                           vm:(AIFormViewmodel *)vm
                   references:(nonnull NSArray<AIReference *> *)references
             parentController:(UIViewController *)parentController
                     scroller:(UIScrollView *)scroller
                     onChange:(nullable OnChange)onChange
;

- (UIView *)addFields:(NSArray<AIField *> *)fields
               parent:(UIView *)parent
                 prev:(nullable UIView *)prev
             isDialog:(BOOL)isDialog
            multiline:(BOOL)multiline
;
- (UIView *)addCenteredFields:(NSArray<AIField *> *)fields content:(UIView *)content prev:(nullable UIView *)prev isDialog:(BOOL)isDialog;

@property (nonatomic, readonly) UILabel* error;
@end

NS_ASSUME_NONNULL_END
