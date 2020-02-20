//
//  AICertCheck.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/18.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AICertCheck.h"

@implementation AICertCheck
-   (void)URLSession:(NSURLSession *)session
 didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge
   completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition, NSURLCredential *))completionHandler
{
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust])
    {
        NSURLCredential *credential = [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust];
        completionHandler(NSURLSessionAuthChallengeUseCredential,credential);
    }
}

@end
