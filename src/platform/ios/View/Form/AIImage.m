//
//  AIImage.m
//  user
//
//  Created by Hans Malherbe on 2019/05/18.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIImage.h"
#import "PureLayout.h"

@implementation AIImage

- (instancetype)initWithImage:(UIImage *)image align:(AIAlign)align
{
    if (self = [super init])
    {
        [self configureForAutoLayout];
        self.image = image;
        switch (align)
        {
            case AIAlignLeft:
                self.contentMode = UIViewContentModeLeft;
                break;
            case AIAlignCenter:
                self.contentMode = UIViewContentModeScaleAspectFit;
                break;
            case AIAlignRight:
                self.contentMode = UIViewContentModeRight;
                break;
            case AIAlignNone:
                self.contentMode = UIViewContentModeScaleAspectFit;
                break;
        }
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
