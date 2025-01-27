// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form.djinni

#import "AIElementType.h"
#import <Foundation/Foundation.h>

@interface AIElement : NSObject
- (nonnull instancetype)initWithGroup:(int32_t)group
                                 type:(AIElementType)type
                              inputId:(int32_t)inputId
                                title:(nonnull NSString *)title
                                value:(nonnull NSString *)value;
+ (nonnull instancetype)elementWithGroup:(int32_t)group
                                    type:(AIElementType)type
                                 inputId:(int32_t)inputId
                                   title:(nonnull NSString *)title
                                   value:(nonnull NSString *)value;

@property (nonatomic, readonly) int32_t group;

@property (nonatomic, readonly) AIElementType type;

@property (nonatomic, readonly) int32_t inputId;

@property (nonatomic, readonly, nonnull) NSString * title;

@property (nonatomic, readonly, nonnull) NSString * value;

@end
