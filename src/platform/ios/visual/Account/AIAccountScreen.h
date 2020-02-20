#import <Foundation/Foundation.h>
#import "AIIntResult.h"
#import "AITheme.h"
#import "AIFormViewController.h"
#import "AIPortal.h"
#import "AIAccountActions.h"
#import <StoreKit/StoreKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AIAccountScreen : AIIntResult<SKProductsRequestDelegate, SKRequestDelegate>
@property (nonatomic) AIFormViewController *controller;
- (instancetype)initWithTheme:(AITheme *)theme actions:(id<AIAccountActions>)actions;
@end

NS_ASSUME_NONNULL_END
