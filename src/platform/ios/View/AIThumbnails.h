//
//  AIThumbnails.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/16.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^ OnImage)(UIImage *image);

@interface AIThumbnails : NSObject
- (UIImage *)cachedImageForId:(int)iid;
- (void)createWithId:(int)iid
                data:(NSData *)data
             onImage:(OnImage)onImage
;
@end

NS_ASSUME_NONNULL_END
