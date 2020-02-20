//
//  AIInputElement.h
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void (^ OnChange)(int inputId);

NS_ASSUME_NONNULL_BEGIN

@protocol AIInputElement <NSObject>
@property (nonatomic, readonly) int inputId;
@property (nonatomic) NSString *value;
- (UIView *)view;
- (void)enable:(BOOL)enabled;
- (void)setError:(NSString *)error;
- (BOOL)validate;
- (void)configure:(NSString *)config;
@end

NS_ASSUME_NONNULL_END
