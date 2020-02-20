//
//  AIUIThreadRunner.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/15.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIUIThreadRunner.h"
#import "AIVoidResult.h"

@implementation AIUIThreadRunner

- (void)guiThread:(nullable AIVoidResult *)cb
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [cb onResult];
    });
}

@end
