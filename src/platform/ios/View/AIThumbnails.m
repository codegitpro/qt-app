//
//  AIThumbnails.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/16.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIThumbnails.h"
@interface AIThumbnails()
@property (nonatomic, readonly) NSMutableDictionary<NSNumber *, UIImage *> *cache;
@property (nonatomic, readonly) NSOperationQueue *imageQueue;
@end

@implementation AIThumbnails

- (instancetype)init
{
    if (self = [super init])
    {
        _cache = [NSMutableDictionary new];
        _imageQueue = [NSOperationQueue new];

    }
    return self;
}

- (UIImage *)cachedImageForId:(int)iid
{
    return _cache[[NSNumber numberWithInt:iid]];
}

- (void)createWithId:(int)iid
                data:(NSData *)data
             onImage:(OnImage)onImage
{
    __weak AIThumbnails *weakself = self;
    [_imageQueue addOperationWithBlock:^{
        UIImage *thumbnail = [UIImage imageWithData:data];
        dispatch_async(dispatch_get_main_queue(), ^{
            if (thumbnail)
                weakself.cache[[NSNumber numberWithInt:iid]] = thumbnail;
            onImage(thumbnail);
        });
    }];

}

@end
