// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIHeader.h"
#import "AIProgress.h"
#import <Foundation/Foundation.h>


@interface AIHttpProgressResult : NSObject

- (void)onProgress:(nonnull AIProgress *)at;

- (void)onResult:(int32_t)statusCode
         headers:(nonnull NSArray<AIHeader *> *)headers
   contentLength:(int64_t)contentLength;

@end
