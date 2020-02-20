//
//  AIVerticalSpace.m
//  user
//
//  Created by Hans Malherbe on 2019/05/18.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIVerticalSpace.h"
#import "PureLayout.h"
#import "AITheme.h"

@interface AIVerticalSpace()
@property (nonatomic, readonly) CGFloat height;

@property (nonatomic) BOOL didSetupConstraints;
@end
@implementation AIVerticalSpace

- (instancetype)initWithSize:(CGFloat)height
{
    if (self = [super init])
    {
        [self configureForAutoLayout];
        _height = height;
        _didSetupConstraints = NO;
        [self setNeedsUpdateConstraints];
    }
    return self;
}

- (void)updateConstraints
{
    if (_didSetupConstraints)
    {
        [super updateConstraints];
        return;
    }
    
    _didSetupConstraints = YES;
    
    [self autoSetDimension:ALDimensionHeight toSize:_height];
    [super updateConstraints];
}

@end
