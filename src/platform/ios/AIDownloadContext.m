//
//  AIDownloadContext.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIDownloadContext.h"

@implementation AIDownloadContext

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _contentLength = 0;
    }
    return self;
}

@end
