#import "AIBooleanInputx.h"
#import "PureLayout.h"
#import "AIFonts.h"
#import "AITextField.h"

@interface AIBooleanInputx()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UISwitch *field;
@end

@implementation AIBooleanInputx

- (instancetype)initWithTheme:(AITheme *)theme label:(NSString *)label element:(AIElement *)element
{
    if (self = [super initForAutoLayout])
    {
        _group = element.group;
        _inputId = element.inputId;
        
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeTop withInset:theme.margins];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:theme.switchInset];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        _label.font = theme.switchLabelFont;
        _label.textColor = theme.switchLabelColour;
        [_label setLineBreakMode:NSLineBreakByTruncatingTail];
        [_label setNumberOfLines:1];
        _label.text = label;
        
        _field = [UISwitch newAutoLayoutView];
        [self addSubview:_field];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeTop withInset:theme.margins];
        [_field autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:_label];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:theme.margins];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeBottom];

        _field.tintColor = theme.switchColour;
        _field.onTintColor = theme.switchColour;
    }
    return self;
}

- (void)enable:(BOOL)enabled
{
    [_field setEnabled:enabled];
}

- (void)setValue:(NSString *)value
{
    [_field setOn:[value isEqualToString:@"true"]];
}

- (NSString *)value
{
    return _field.isOn ? @"true" : @"false";
}

- (void)setError:(nonnull NSString *)error
{
}

@end
