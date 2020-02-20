//
//  AITextField.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AITextField.h"

@interface AITextField()
@property (nonatomic, readonly) UIEdgeInsets padding;
@end

@implementation AITextField

- (instancetype)init
{
    if (self = [super init])
    {
        _padding = UIEdgeInsetsMake(10, 15, 10, 10);
    }
    return self;
}

- (CGRect)textRectForBounds:(CGRect)bounds
{
    return UIEdgeInsetsInsetRect(bounds, _padding);
}

- (CGRect)editingRectForBounds:(CGRect)bounds
{
    return UIEdgeInsetsInsetRect(bounds, _padding);
}

- (CGRect)placeholderRectForBounds:(CGRect)bounds
{
    return UIEdgeInsetsInsetRect(bounds, _padding);
}


@end
