//
//  AITransactionObserver.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/05/06.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AITransactionObserver.h"

@implementation AITransactionObserver

- (void)paymentQueue:(nonnull SKPaymentQueue *)queue updatedTransactions:(nonnull NSArray<SKPaymentTransaction *> *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
                // Call the appropriate custom method for the transaction state.
            case SKPaymentTransactionStatePurchasing:
                [self showTransactionAsInProgress:transaction deferred:NO];
                break;
            case SKPaymentTransactionStateDeferred:
                [self showTransactionAsInProgress:transaction deferred:YES];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            default:
                // For debugging
                NSLog(@"Unexpected transaction state %@", @(transaction.transactionState));
                break;
        }
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions
{
    
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    
}

- (void)showTransactionAsInProgress:(SKPaymentTransaction *)transaction deferred:(BOOL)deferred
{
    
}
- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}
- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
//    NSData *receipt = transaction.transactionReceipt;
//    if (receipt && receipt.length > 0)
//    {
//        NSString *s = [[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding];
//        NSData *decodedData = [[NSData alloc] initWithBase64EncodedData:receipt options:0];
//        NSString *decodedString = [[NSString alloc] initWithData:decodedData encoding:NSUTF8StringEncoding];
//        NSLog(@"%@", decodedString); // foo
//    }
   [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}
- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    
}
@end
