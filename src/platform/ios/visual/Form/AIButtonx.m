#import "AIButtonx.h"
#import "PureLayout.h"

@interface AIButtonx()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UIButton *button;
@property (nonatomic, readonly) OnClick action;
@end

@implementation AIButtonx
- (instancetype)initWithTheme:(AITheme *)theme element:(AIElement *)element action:(OnClick)action
{
    if (self = [super init])
    {
        _inputId = element.inputId;
        _group = element.group;
        _action = action;

        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:theme.margins];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:theme.margins];
        [_label autoSetDimension:ALDimensionHeight toSize:22];
        _label.font = theme.textInputToplabelFont;
        _label.textColor = theme.formErrorColour;
        [_label setLineBreakMode:NSLineBreakByTruncatingTail];
        [_label setNumberOfLines:1];

        _button = [UIButton buttonWithType:UIButtonTypeSystem];
        [self addSubview:_button];
        [_button autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
        [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:theme.margins];
        [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:theme.margins];
        [_button autoPinEdgeToSuperviewEdge:ALEdgeBottom];

        [_button configureForAutoLayout];

        _button.backgroundColor = theme.buttonColour;
        _button.tintColor = theme.buttonTextColour;
        _button.titleLabel.font = theme.buttonFont;
        _button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
        _button.layer.cornerRadius = theme.buttonCornerRadius;
        _button.layer.masksToBounds = YES;
        [_button setTitle:element.title forState:UIControlStateNormal];
        _button.contentEdgeInsets = UIEdgeInsetsMake(10, 15, 10, 10);
        [_button addTarget:self action:@selector(click) forControlEvents:UIControlEventTouchUpInside];
    }
    return self;
}

- (void)enable:(BOOL)enabled
{
    [_button setEnabled:enabled];
}

- (void)click
{
    _action(_inputId);
}

- (void)setError:(NSString *)error
{
    _label.text = error;
}
@end

