//
//  AIMultilineButton.m
//  user
//
//  Created by Hans Malherbe on 2019/05/04.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIMultilineButton.h"
#import "PureLayout.h"

@implementation AIMultilineButton

- (instancetype)init
{
    if (self = [super init])
    {
        self.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
        self.titleLabel.numberOfLines = 0;
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)rect
{
    if (self = [super initWithFrame:rect])
    {
        self.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
        self.titleLabel.numberOfLines = 0;
    }
    return self;
}


- (CGSize)intrinsicContentSize
{
    CGSize size = self.titleLabel.intrinsicContentSize;
    return CGSizeMake(size.width + 40, size.height + 20);
//    return CGSizeMake(size.width + 20, size.height + 20); //NB crash
}
//
- (void)layoutSubviews
{
    [super layoutSubviews];
    if (self.titleLabel.preferredMaxLayoutWidth != self.titleLabel.frame.size.width)
    {
        self.titleLabel.preferredMaxLayoutWidth = self.titleLabel.frame.size.width;
        [super layoutSubviews];

    }
}

@end
