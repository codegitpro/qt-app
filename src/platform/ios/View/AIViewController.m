//
//  AIViewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/02.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIViewController.h"
#import "AIPhoneFormController.h"
#import "AIVoidResult.h"
#import "AINavigationController.h"
#import "AIRootController.h"
#import "AIFolderController.h"
#import "AIToolbarController.h"
#import "AIThumbnails.h"
#import "AIProduct.h"
#import "AIStoreViewmodel.h"
#import "AITransactionDetail.h"
#import <CommonCrypto/CommonCrypto.h>

@interface AIViewController ()
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIThumbnails *thumbnails;
@property (nonatomic, readonly) AIOsIoImpl *io;
@property (nonatomic) BOOL menuVisible;
@property (nonatomic) int currentMenu;

@property (nonatomic) AIStoreViewmodel *storeVm;

@property (nonatomic) SKProductsRequest *productRequest;
@property (nonatomic) NSArray<SKProduct *> *availableProducts;
@property (nonatomic) NSArray<NSString *> *invalidProductIdentifiers;

@property (nonatomic) NSMutableDictionary<NSString *, AITransactionDetail *> *transactionStates;

@end

@implementation AIViewController

- (instancetype)initWithTheme:(AITheme *)theme
                           io:(AIOsIoImpl *)io
{
    _theme = theme;
    if (self = [super init])
    {
        _io = io;
        _menuVisible = NO;
        _currentMenu = -1;
        self.tabBar.hidden = YES;
        
        self.tabBar.tintColor = theme.tabBarSelectionColour;
        self.tabBar.barTintColor = theme.tabBarBackgroundColour;
        _thumbnails = [AIThumbnails new];
        
        _transactionStates = [NSMutableDictionary new];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = _theme.formBackground;
}

- (void)createMainMenu:(AIFormViewmodel *)rootFormVm rootFolderVm:(AIFolderViewmodel *)rootFolderVm userVm:(AIFormViewmodel *)userVm planVm:(AIFormViewmodel *)planVm settingsVm:(AIFormViewmodel *)settingsVm
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    AINavigationController *navFolder = [[AINavigationController alloc] initWithTheme:_theme];
    AINavigationController *navUser = [[AINavigationController alloc] initWithTheme:_theme];
    AINavigationController *navPlan = [[AINavigationController alloc] initWithTheme:_theme];
    AINavigationController *navSettings = [[AINavigationController alloc] initWithTheme:_theme];

    {
        AIToolbarController *controller = [[AIToolbarController alloc] initWithFormVm:rootFormVm folderVm:rootFolderVm];
        AIFolderController *folderController = [[AIFolderController alloc] initWithTheme:_theme
                                                                                  formVm:rootFormVm
                                                                                folderVm:rootFolderVm
                                                                                  parent:controller
                                                                              thumbnails:_thumbnails
                                                                                      io:_io
                                                ];
        [controller setViewController:folderController];
        [navFolder pushViewController:controller animated:NO];
    }

    {
        AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                      vm:userVm
                                                                              references:@[]
                                                                                showMenu:_menuVisible
                                                                                isDialog:NO
                                                                                onChange:nil
                                             ];
        [navUser pushViewController:controller animated:NO];
    }
    
    {
        AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                      vm:planVm
                                                                              references:@[]
                                                                                showMenu:_menuVisible
                                                                                isDialog:NO
                                                                                onChange:nil
                                             ];
        [navPlan pushViewController:controller animated:NO];
    }
    
    {
        AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                      vm:settingsVm
                                                                              references:@[]
                                                                                showMenu:_menuVisible
                                                                                isDialog:NO
                                                                                onChange:nil
                                             ];
        [navSettings pushViewController:controller animated:NO];
    }
    
    self.viewControllers = @[
                             navFolder,
                             navUser,
                             navPlan,
                             navSettings
                             ];
    
    _currentMenu = 0;
    self.tabBar.hidden = NO;
    _menuVisible = YES;

    navFolder.tabBarItem.title = @"Files";
    navFolder.tabBarItem.image = [[UIImage imageNamed:@"tree"] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
    navUser.tabBarItem.title = @"My Profile";
    navUser.tabBarItem.image = [[UIImage imageNamed:@"account"] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
    navPlan.tabBarItem.title = @"My Plan";
    navPlan.tabBarItem.image = [[UIImage imageNamed:@"basket"] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
    navSettings.tabBarItem.title = @"Settings";
    navSettings.tabBarItem.image = [[UIImage imageNamed:@"misc"] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];

}

- (void)selectMenu:(int32_t)index
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    _currentMenu = index;
    self.selectedIndex = index;
}

- (void)showMenu:(BOOL)visible
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    _menuVisible = visible;
    self.tabBar.hidden = !visible || -1 == _currentMenu;
}

- (void)sendInvite
{
    MFMessageComposeViewController *messageController = [[MFMessageComposeViewController alloc] init];
    messageController.body = @"Try Blomp for free cloud storage! Click https://sms.blomp.com";
    messageController.messageComposeDelegate = self;
    [self presentViewController:messageController animated:YES completion:nil];
}

- (BOOL)isAuthorizedForPayments
{
    BOOL r = [SKPaymentQueue canMakePayments];
    return r;
}

- (void)fetchProducts:(nonnull NSArray<NSString *> *)productIds
{
    NSSet *productIdentifiers = [NSSet setWithArray:productIds];
    
    // Initialize the product request with the above identifiers.
    self.productRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    self.productRequest.delegate = self;
    
    // Send the request to the App Store.
    [self.productRequest start];
}

- (void)registerStoreVm:(nullable AIStoreViewmodel *)vm
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    _storeVm = vm;

    if (_transactionStates.count > 0)
    {
        id states = _transactionStates;

        _transactionStates = [NSMutableDictionary new];
        
        [_storeVm onTransactionStates:states];
    }
}

- (void)subscriptionCancel:(nonnull NSString *)productId
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://apps.apple.com/account/subscriptions"]];
}

- (nonnull NSString *)subscriptionReceipt:(nonnull NSString *)productId
{
    NSData *dataReceipt = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
    NSString *receiptBody = [dataReceipt base64EncodedStringWithOptions:0];
    return receiptBody;
}

- (void)subscriptionPurchase:(nonnull NSString *)email productId:(nonnull NSString *)productId
{
    for (SKProduct *product in _availableProducts)
    {
        if ([product.productIdentifier isEqualToString:productId])
        {
            SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:product];
            payment.quantity = 1;
            payment.applicationUsername = [self hashedValueForAccountName:email];

            [[SKPaymentQueue defaultQueue] addPayment:payment];
            return;
        }
    }
}

- (void)subscriptionRestore:(nonnull NSString *)email
{
//    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactionsWithApplicationUsername:[self hashedValueForAccountName:email]];
}

- (void)subscriptionManage
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://apps.apple.com/account/subscriptions"]];
}

- (void)pushForm:(nullable AIFormViewmodel *)vm
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                   vm:vm
                                                                           references:@[]
                                                                             showMenu:_menuVisible
                                                                             isDialog:NO
                                                                             onChange:nil
                                         ];

    if (self.viewControllers.count == 0)
    {
        AINavigationController *nav = [[AINavigationController alloc] initWithTheme:_theme];
        self.viewControllers = @[nav];
    }
    [self.viewControllers.firstObject pushViewController:controller animated:YES];
    self.tabBar.hidden = YES;
}

- (void)replaceForm:(nullable AIFormViewmodel *)vm
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                   vm:vm
                                                                           references:@[]
                                                                             showMenu:_menuVisible
                                                                             isDialog:NO
                                                                             onChange:nil
                                         ];
// sign out breaks
//    if (self.viewControllers.count == 0)
//    {
        AINavigationController *nav = [[AINavigationController alloc] initWithTheme:_theme];
        self.viewControllers = @[nav];
        [nav pushViewController:controller animated:YES];
        self.tabBar.hidden = YES;
        nav.navigationBar.hidden = NO;
        return;
//    }
//
//    UINavigationController *nav = self.viewControllers.firstObject;
//    NSMutableArray<UIViewController *> *stack = [NSMutableArray new];
//    for (int i = 0; i < nav.viewControllers.count - 1; ++i)
//    {
//        [stack addObject:nav.viewControllers[i]];
//    }
//
//    [stack addObject:controller];
//    [nav setViewControllers:stack animated:NO];
}

- (void)copyToClipboard:(nonnull NSString *)content
{
    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string = content;
}

- (void)openLinkInBrowser:(nonnull NSString *)url
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
}

- (void)messageComposeViewController:(nonnull MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result
{
    [controller dismissViewControllerAnimated:NO completion:nil];
}

- (void)paymentQueue:(nonnull SKPaymentQueue *)queue updatedTransactions:(nonnull NSArray<SKPaymentTransaction *> *)transactions
{
    NSMutableDictionary<NSString *, AITransactionDetail *> *states = [NSMutableDictionary new];
    
    for(SKPaymentTransaction *transaction in transactions)
    {
        NSString *productId = transaction.payment.productIdentifier;
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchasing:
                NSLog(@"paymentQueue.updatedTransactions.SKPaymentTransactionStatePurchasing %@.", transaction.payment.productIdentifier);
                
                states[productId] = [AITransactionDetail transactionDetailWithState:AITransactionStateBusy errorMessage:@""];
                break;
            case SKPaymentTransactionStateDeferred:
                NSLog(@"paymentQueue.updatedTransactions.SKPaymentTransactionStateDeferred %@.", transaction.payment.productIdentifier);
                break;
            case SKPaymentTransactionStatePurchased:
            {
                NSLog(@"paymentQueue.updatedTransactions.SKPaymentTransactionStatePurchased %@.", transaction.payment.productIdentifier);
                
                states[productId] = [AITransactionDetail transactionDetailWithState:AITransactionStatePurchased errorMessage:@""];
                
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            }
            case SKPaymentTransactionStateFailed:
            {
                NSLog(@"paymentQueue.updatedTransactions.SKPaymentTransactionStateFailed %@.", transaction.payment.productIdentifier);
                
                if (transaction.error)
                {
                    NSError *error = transaction.error;
                    NSString *msg = SKErrorPaymentCancelled == error.code ? @"" : error.localizedDescription;
                    states[productId] = [AITransactionDetail transactionDetailWithState:AITransactionStateError errorMessage:msg];
                }
                else
                {
                    states[productId] = [AITransactionDetail transactionDetailWithState:AITransactionStateError errorMessage:@"Unknown error"];
                }
                
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                
                break;
            }
            case SKPaymentTransactionStateRestored:
            {
                NSLog(@"paymentQueue.updatedTransactions.SKPaymentTransactionStateRestored %@.", transaction.payment.productIdentifier);
                
                states[productId] = [AITransactionDetail transactionDetailWithState:AITransactionStateRestored errorMessage:@""];
                
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            }
            default:
                NSLog(@"paymentQueue.updatedTransactions.default %@.", transaction.payment.productIdentifier);
                break;
        }
    }
    if (states.count > 0)
    {
        __weak AIViewController *weakself = self;
        dispatch_async(dispatch_get_main_queue(), ^
        {
            if (weakself.storeVm)
            {
                [weakself.storeVm onTransactionStates:states];
            }
            else
            {
                [weakself.transactionStates addEntriesFromDictionary:states];
            }
        });
    }
}

/// Logs all transactions that have been removed from the payment queue.
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
{
    NSLog(@"paymentQueue.removedTransactions");
    for(SKPaymentTransaction *transaction in transactions)
    {
        NSLog(@"Removed %@", transaction.payment.productIdentifier);
    }
}

/// Called when an error occur while restoring purchases. Notify the user about the error.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    NSLog(@"paymentQueue.restoreCompletedTransactionsFailedWithError");
    if (error.code != SKErrorPaymentCancelled)
    {
        [_storeVm onRestoreFail:error.localizedDescription];
        //        self.status = PCSRestoreFailed;
        //        NSLog(@"Error restoring %@", error.localizedDescription);
        
        
        //        dispatch_async(dispatch_get_main_queue(), ^{
        //            [[NSNotificationCenter defaultCenter] postNotificationName:PCSPurchaseNotification object:self];
        //        });
    }
    else
    {
        [_storeVm onRestoreFail:@""];
    }
}

/// Called when all restorable transactions have been processed by the payment queue.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    NSLog(@"paymentQueueRestoreCompletedTransactionsFinished");
    [_storeVm onRestoreFinished];
    //    if (!self.hasRestorablePurchases) {
    //        self.status = PCSNoRestorablePurchases;
    //        self.message = [NSString stringWithFormat:@"%@\n%@", PCSMessagesNoRestorablePurchases, PCSMessagesPreviouslyBought];
    //
    //        dispatch_async(dispatch_get_main_queue(), ^{
    //            [[NSNotificationCenter defaultCenter] postNotificationName:PCSPurchaseNotification object:self];
    //        });
    //    }
}

#pragma mark - SKProductsRequestDelegate

- (void)productsRequest:(nonnull SKProductsRequest *)request didReceiveResponse:(nonnull SKProductsResponse *)response
{
    NSLog(@"productsRequest.didReceiveResponse");
    // products contains products whose identifiers have been recognized by the App Store. As such, they can be purchased.
    if ((response.products).count > 0)
    {
        _availableProducts = [NSMutableArray arrayWithArray:response.products];
        NSMutableArray<AIProduct *> *products = [NSMutableArray new];
        NSNumberFormatter *numberFormatter = [NSNumberFormatter new];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        
        for (SKProduct *product in _availableProducts)
        {
            //            NSString *groupId = product.subscriptionGroupIdentifier; //ios 12
            [numberFormatter setLocale:product.priceLocale];
            NSString *formattedString = [numberFormatter stringFromNumber:product.price];
            [products addObject:[AIProduct productWithProductId:product.productIdentifier title:product.localizedTitle productDescription:product.localizedDescription price:product.price.floatValue formattedPrice:formattedString]];
        }
        [_storeVm onProducts:products];
        //        [_productCallback onMyProducts:products];
    }
    
    // invalidProductIdentifiers contains all product identifiers not recognized by the App Store.
    if ((response.invalidProductIdentifiers).count > 0)
    {
        self.invalidProductIdentifiers = [NSMutableArray arrayWithArray:response.invalidProductIdentifiers];
    }
    
    //    if (self.storeResponse.count > 0) {
    //        self.status = PCSStoreResponse;
    //
    //        dispatch_async(dispatch_get_main_queue(), ^{
    //            [[NSNotificationCenter defaultCenter] postNotificationName:PCSProductRequestNotification object:self];
    //        });
    //    }
    
}

#pragma mark - SKRequestDelegate

/// Called when the product request failed.
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"request.didFailWithError");
    //    self.status = PCSRequestFailed;
    //    self.message = error.localizedDescription;
    //
    //    dispatch_async(dispatch_get_main_queue(), ^{
    //        [[NSNotificationCenter defaultCenter] postNotificationName:PCSProductRequestNotification object:self];
    //    });
}

- (void)requestDidFinish:(SKRequest *)request
{
    NSLog(@"requestDidFinish");
    //    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    //    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    //
    //    if (!receipt || receipt.length == 0)
    //    {
    //        return;
    //    }
    //    NSData *decodedData = [[NSData alloc] initWithBase64EncodedData:receipt options:0];
    //    NSString *decodedString = [[NSString alloc] initWithData:decodedData encoding:NSUTF8StringEncoding];
    //    NSLog(@"%@", decodedString); // foo
}

- (NSString *)hashedValueForAccountName:(NSString*)userAccountName
{
    const int HASH_SIZE = 32;
    unsigned char hashedChars[HASH_SIZE];
    const char *accountName = [userAccountName UTF8String];
    size_t accountNameLen = strlen(accountName);
    
    // Confirm that the length of the user name is small enough
    // to be recast when calling the hash function.
    if (accountNameLen > UINT32_MAX) {
        NSLog(@"Account name too long to hash: %@", userAccountName);
        return nil;
    }
    CC_SHA256(accountName, (CC_LONG)accountNameLen, hashedChars);
    
    // Convert the array of bytes into a string showing its hex representation.
    NSMutableString *userAccountHash = [[NSMutableString alloc] init];
    for (int i = 0; i < HASH_SIZE; i++) {
        // Add a dash every four bytes, for readability.
        if (i != 0 && i%4 == 0) {
            [userAccountHash appendString:@"-"];
        }
        [userAccountHash appendFormat:@"%02x", hashedChars[i]];
    }
    
    return userAccountHash;
}

@end
