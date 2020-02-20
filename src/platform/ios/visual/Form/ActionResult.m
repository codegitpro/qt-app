//
//  ActionResult.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "ActionResult.h"

@interface ActionResult()
@property (nonatomic, readonly) IntResult result;
@end

@implementation ActionResult

- (instancetype)initWithResult:(IntResult)result
{
    if (self = [super init])
    {
        _result = result;
    }
    return self;
}

- (void)onResult:(int32_t)value
{
    _result(value);
}

@end
