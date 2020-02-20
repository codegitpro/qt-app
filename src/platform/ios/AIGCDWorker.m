//
//  AIGCDWorker.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/25.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIGCDWorker.h"
#import "AITask.h"

@interface AIGCDWorker()
@property (nonatomic, readonly) NSOperationQueue* q;
@end

@implementation AIGCDWorker

- (void)schedule:(AITask *)todo
{
    [_q addOperationWithBlock:^{
        [todo run];
    }];
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _q = [NSOperationQueue new];
    }
    return self;
}
@end
