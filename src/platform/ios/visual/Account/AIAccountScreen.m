#import "AIAccountScreen.h"
#import "AIFormViewController.h"
#import "AIToolbarViewController.h"
#import <MessageUI/MFMessageComposeViewController.h>
#import <CommonCrypto/CommonCrypto.h>

@interface AIAccountScreen()
@property (nonatomic, readonly) id<AIAccountActions> actions;
@property (nonatomic) SKProductsRequest *productsRequest;
@end
@implementation AIAccountScreen

- (instancetype)initWithTheme:(AITheme *)theme actions:(id<AIAccountActions>)actions
{
    if (self = [super init])
    {
        _actions = actions;
        
        NSMutableArray<AIElement *> *elements = [NSMutableArray new];
        if ([MFMessageComposeViewController canSendText])
        {
            [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Please consider letting your friends know about Blomp." value:@""]];
            [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:4 title:@"Send Invite" value:@""]];
        }

        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"If you want to sign in with another account you must first sign out." value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:1 title:@"Sign Out" value:@""]];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Cancel downloads, uploads and delete operations currently in progress." value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:2 title:@"Cancel All" value:@""]];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Information about this app." value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:3 title:@"About" value:@""]];

        AIFormViewController *form = [[AIFormViewController alloc] initForm:theme title:@"Account" width:400 height:0 action:self elements:elements];
        
        _controller = form;
    }
    return self;
}

- (void)onResult:(int32_t)value
{
    switch (value) {
        case 1:
        {
            /*
            {
                NSData *dataReceipt = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
                NSString *receipt = [dataReceipt base64EncodedStringWithOptions:0];
                NSLog(@"%@",receipt);
            }
             */
//            NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
//            NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
//            NSUInteger len = receipt.length;
//            if (0 == len)
//            {
//                SKReceiptRefreshRequest *refresh = [[SKReceiptRefreshRequest alloc] init];
//                refresh.delegate = self;
//                [refresh start];
//                return;
//            }
//
//            NSString *s = [[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding];
//            NSData *decodedData = [[NSData alloc] initWithBase64EncodedData:receipt options:0];
//            NSString *decodedString = [[NSString alloc] initWithData:decodedData encoding:NSUTF8StringEncoding];
//            NSLog(@"%@", decodedString); // foo
                [_actions accountSignOut];
            break;

        }
        case 2:
            [_actions accountAbortNetworkTasks];
            break;
        case 3:
        {
            [_actions accountAbout];
//            _productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithArray:@[@"250GB_Auto_Renewal"]]];
//            _productsRequest.delegate = self;
//            [_productsRequest start];
            break;
        }
        case 4:
            [_actions sendInvite];
            break;
        default:
            break;
    }
}

- (void)requestDidFinish:(SKRequest *)request
{
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];

    if (!receipt || receipt.length == 0)
    {
        return;
    }
    NSData *decodedData = [[NSData alloc] initWithBase64EncodedData:receipt options:0];
    NSString *decodedString = [[NSString alloc] initWithData:decodedData encoding:NSUTF8StringEncoding];
    NSLog(@"%@", decodedString); // foo
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    
}

- (void)productsRequest:(nonnull SKProductsRequest *)request didReceiveResponse:(nonnull SKProductsResponse *)response
{
    
    NSArray<SKProduct *> *products = response.products;
    if (products.count == 0)
        return;
    SKProduct *product = products[0];
    SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:product];
    payment.quantity = 1;
    payment.applicationUsername = [self hashedValueForAccountName:@"hansie"];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
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
