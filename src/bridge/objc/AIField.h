// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form2.djinni

#import "AIBlock.h"
#import "AIDataType.h"
#import "AIFieldType.h"
#import "AIStyle.h"
#import <Foundation/Foundation.h>

@interface AIField : NSObject
- (nonnull instancetype)initWithRect:(nonnull AIBlock *)rect
                             fieldId:(int32_t)fieldId
                               dataT:(AIDataType)dataT
                              fieldT:(AIFieldType)fieldT
                               title:(nonnull NSString *)title
                            required:(BOOL)required
                               style:(nonnull AIStyle *)style;
+ (nonnull instancetype)fieldWithRect:(nonnull AIBlock *)rect
                              fieldId:(int32_t)fieldId
                                dataT:(AIDataType)dataT
                               fieldT:(AIFieldType)fieldT
                                title:(nonnull NSString *)title
                             required:(BOOL)required
                                style:(nonnull AIStyle *)style;

@property (nonatomic, readonly, nonnull) AIBlock * rect;

@property (nonatomic, readonly) int32_t fieldId;

@property (nonatomic, readonly) AIDataType dataT;

@property (nonatomic, readonly) AIFieldType fieldT;

@property (nonatomic, readonly, nonnull) NSString * title;

@property (nonatomic, readonly) BOOL required;

@property (nonatomic, readonly, nonnull) AIStyle * style;

@end