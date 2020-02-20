//
//  AIViewController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/02.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIView.h"
#import "AITheme.h"
#import "AIRootViewmodel.h"
#import "AIOsIoImpl.h"

#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMessageComposeViewController.h>
@import StoreKit;

NS_ASSUME_NONNULL_BEGIN

@interface AIViewController : UITabBarController<AIView, MFMessageComposeViewControllerDelegate, SKPaymentTransactionObserver, SKRequestDelegate, SKProductsRequestDelegate>
- (instancetype)initWithTheme:(AITheme *)theme
                           io:(AIOsIoImpl *)io
;
@property (nonatomic) AIRootViewmodel *vm;
@end

NS_ASSUME_NONNULL_END
