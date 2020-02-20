//
//  AIText.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIText.h"
#import "PureLayout.h"

@interface AIText()
@property (nonatomic, readonly) AIColour colour;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIAlign align;
@property (nonatomic, readonly) int maxWidth;

@property (nonatomic) BOOL didSetupConstraints;
@end
@implementation AIText

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                        align:(AIAlign)align
                     isDialog:(BOOL)isDialog
                     maxWidth:(int)maxWidth
{
    if (self = [super initForAutoLayout])
    {
        _inputId = inputId;
        _colour = textColour;
        _theme = theme;
        _align = align;
        _maxWidth = maxWidth;
        
        _didSetupConstraints = NO;

        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.text = label;
        _label.font = [theme fontWithSize:size weight:weight];
        _label.textColor = [theme colour:textColour];
        _label.textAlignment = [AITheme convertTextAlign:align];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;
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

    [_label autoPinEdgeToSuperviewEdge:ALEdgeTop withInset:_theme.margins];
    [_label autoPinEdgeToSuperviewEdge:ALEdgeBottom withInset:_theme.margins];
    
    switch (_align)
    {
        case AIAlignLeft:
        {
            [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
            [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignRight:
        {
            [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
            [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignCenter:
        {
            [_label autoAlignAxisToSuperviewAxis:ALAxisVertical];
            [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
            [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
            break;
        }
        default:
        {
            [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
            [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
    }

    __weak AIText *weakself = self;
    if (_maxWidth > 0)
    {
        
        [[NSLayoutConstraint autoCreateConstraintsWithoutInstalling:^
        {
            [weakself.label autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh+1;
        }] autoInstallConstraints];
        
        [_label autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:-_theme.margins relation:NSLayoutRelationLessThanOrEqual];
    }
    else if (_maxWidth < 0)
    {
        [self autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self.superview withMultiplier:(-1.0*_maxWidth)/100.0];
    }
//    else
//    {
//        [_label autoPinEdgesToSuperviewEdgesWithInsets:UIEdgeInsetsMake(_theme.margins, _theme.margins, _theme.margins, _theme.margins)];
//    }

    [NSLayoutConstraint autoSetPriority:UILayoutPriorityDefaultHigh forConstraints:^
    {
        [weakself.label autoSetContentCompressionResistancePriorityForAxis:ALAxisHorizontal];
    }];
    [self updateConstraints];
}

- (NSString *)value
{
    return @"";
}

- (void)enable:(BOOL)enabled
{
    _label.textColor = enabled ? [_theme colour:_colour] : [_theme grayColour];
}

- (void)setValue:(nonnull NSString *)value
{
    _label.text = value;
}

- (UIView *)view
{
    return self;
}

- (void)configure:(nonnull NSString *)config {}
- (void)setError:(nonnull NSString *)error {}
- (BOOL)validate { return YES; }

@end
