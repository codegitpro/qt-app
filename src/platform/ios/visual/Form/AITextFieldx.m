#import "AITextFieldx.h"
#import "PureLayout.h"

@interface AITextFieldx()
@property (nonatomic, readonly) UIEdgeInsets padding;
@end

@implementation AITextFieldx

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
