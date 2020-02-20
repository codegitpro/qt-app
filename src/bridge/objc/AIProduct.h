// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import <Foundation/Foundation.h>

@interface AIProduct : NSObject
- (nonnull instancetype)initWithProductId:(nonnull NSString *)productId
                                    title:(nonnull NSString *)title
                       productDescription:(nonnull NSString *)productDescription
                                    price:(float)price
                           formattedPrice:(nonnull NSString *)formattedPrice;
+ (nonnull instancetype)productWithProductId:(nonnull NSString *)productId
                                       title:(nonnull NSString *)title
                          productDescription:(nonnull NSString *)productDescription
                                       price:(float)price
                              formattedPrice:(nonnull NSString *)formattedPrice;

@property (nonatomic, readonly, nonnull) NSString * productId;

@property (nonatomic, readonly, nonnull) NSString * title;

@property (nonatomic, readonly, nonnull) NSString * productDescription;

@property (nonatomic, readonly) float price;

@property (nonatomic, readonly, nonnull) NSString * formattedPrice;

@end