// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIFileState.h"
#import <Foundation/Foundation.h>

@interface AIStateChange : NSObject
- (nonnull instancetype)initWithIndex:(int32_t)index
                                state:(AIFileState)state;
+ (nonnull instancetype)stateChangeWithIndex:(int32_t)index
                                       state:(AIFileState)state;

@property (nonatomic, readonly) int32_t index;

@property (nonatomic, readonly) AIFileState state;

@end