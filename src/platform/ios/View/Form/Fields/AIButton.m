//
//  AIButton.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIButton.h"
#import "PureLayout.h"
#import "AIMultilineButton.h"

@interface AIButton()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UIButton *button;
@property (nonatomic, readonly) OnClick action;
@property (nonatomic, readonly) BOOL mustValidate;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIAlign align;
@property (nonatomic, readonly) int maxWidth;
@property (nonatomic, readonly) BOOL hasBorder;
@property (nonatomic, readonly) AIColour colour;

@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) BOOL enabled;
@end

@implementation AIButton

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                       colour:(AIColour)colour
                     isDialog:(BOOL)isDialog
                 mustValidate:(BOOL)mustValidate
                     maxWidth:(int)maxWidth
                        align:(AIAlign)align
                    hasBorder:(BOOL)hasBorder
                    multiline:(BOOL)multiline
                       action:(OnClick)action
{
    if (self = [super initForAutoLayout])
    {
        _mustValidate = mustValidate;
        _inputId = inputId;
        _action = action;
        _theme = theme;
        _align = align;
        _maxWidth = maxWidth;
        _hasBorder = hasBorder;
        _colour = colour;
        
        _didSetupConstraints = NO;
        _enabled = YES;
        
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.font = [theme fontWithSize:size weight:weight];
        _label.textColor = [theme colour:AIColourRed];
//        _label.textAlignment = [AITheme convertTextAlign:align];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;
        
        _button = multiline ? [AIMultilineButton buttonWithType:UIButtonTypeSystem] : [UIButton buttonWithType:UIButtonTypeSystem];
//        _button = [UIButton buttonWithType:UIButtonTypeSystem];
        [_button configureForAutoLayout];
        [self addSubview:_button];
        
        _button.backgroundColor = isDialog ? theme.formBackground : theme.listBackgroundColour;
        _button.titleLabel.font = [theme fontWithSize:size weight:weight];
//        _button.contentHorizontalAlignment = [AITheme convertAlign:align];
        _button.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
        [self setupButton];
        if (label.length > 0)
            [_button setTitle:label forState:UIControlStateNormal];
        else
            _button.hidden = YES;
        _button.contentEdgeInsets = UIEdgeInsetsMake(10, 15, 10, 10);
        [_button addTarget:self action:@selector(click) forControlEvents:UIControlEventTouchUpInside];
        
        [self setNeedsUpdateConstraints];
    }
    return self;
}

- (void)setupButton
{
    if (_hasBorder)
    {
        _button.layer.cornerRadius = _theme.buttonCornerRadius;
        _button.layer.borderWidth = 1;

        UIColor *uic = _enabled ? [_theme colour:AIColourDirtyBlue] : _theme.grayColour;
        _button.layer.borderColor = uic.CGColor;
        _button.layer.masksToBounds = YES;
        _button.backgroundColor = uic;
        [_button setTitleColor:[_theme colour:AIColourNone == _colour ? AIColourWhite : _colour] forState:UIControlStateNormal];
    }
    else
    {
        _button.backgroundColor = _theme.formBackground;
        [_button setTitleColor:(_enabled ? [_theme colour:AIColourNone == _colour ? AIColourPurple : _colour] : _theme.grayColour) forState:UIControlStateNormal];
    }

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

    [_button autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
    [_button autoPinEdgeToSuperviewEdge:ALEdgeBottom withInset:_theme.margins];

    __weak AIButton *weakself = self;
    switch (_align)
    {
        case AIAlignLeft:
        {
            [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
            [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignRight:
        {
            [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
            [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignCenter:
        {
            [_button autoAlignAxisToSuperviewAxis:ALAxisVertical];
            [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        default:
        {
            [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
    }
    if (_maxWidth > 0)
    {
//        [[NSLayoutConstraint autoCreateConstraintsWithoutInstalling:^{
//            [weakself.button autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth relation:NSLayoutRelationLessThanOrEqual].priority = UILayoutPriorityDefaultHigh;
//        }] autoInstallConstraints];
        
//        [_button autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:-_theme.margins relation:NSLayoutRelationLessThanOrEqual];
        [_button autoSetDimension:ALDimensionWidth toSize:_maxWidth relation:NSLayoutRelationEqual];

    }
//    else
//    {
//        [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
//        [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
//    }

    [_label autoPinEdge:ALEdgeLeading toEdge:ALEdgeLeading ofView:_button];
    [_label autoPinEdge:ALEdgeTrailing toEdge:ALEdgeTrailing ofView:_button];
    
    [NSLayoutConstraint autoSetPriority:UILayoutPriorityRequired forConstraints:^{
        [weakself.label autoSetContentCompressionResistancePriorityForAxis:ALAxisHorizontal];
        [weakself.button autoSetContentCompressionResistancePriorityForAxis:ALAxisHorizontal];
    }];
    [super updateConstraints];
}

- (void)enable:(BOOL)enabled
{
    if (enabled == _enabled)
        return;
    
    _enabled = enabled;
    [_button setEnabled:enabled];
    [self setupButton];
}

- (NSString *)value { return @""; }

- (void)setValue:(nonnull NSString *)value
{
    if (value.length > 0)
    {
        [_button setTitle:value forState:UIControlStateNormal];
        _button.hidden = NO;
    }
    else
    {
        _button.hidden = YES;
    }
}

- (void)click
{
    _action(_inputId, _mustValidate);
}

- (UIView *)view
{
    return self;
}

- (void)configure:(nonnull NSString *)config {}

- (void)setError:(nonnull NSString *)error
{
    _label.text = error;
}

- (BOOL)validate
{
    return YES;
}

@end
