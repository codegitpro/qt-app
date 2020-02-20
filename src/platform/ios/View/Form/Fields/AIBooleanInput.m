//
//  AIBooleanInput.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIBooleanInput.h"
#import "PureLayout.h"

@interface AIBooleanInput()
@property (nonatomic) NSString *localValue;
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UISwitch *field;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) OnChange onChange;

@property (nonatomic) BOOL didSetupConstraints;
@end

@implementation AIBooleanInput

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                     onChange:(OnChange)onChange
{
    if (self = [super initForAutoLayout])
    {
        _inputId = inputId;
        _theme = theme;
        _onChange = onChange;
        
        _didSetupConstraints = NO;
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.font = [theme fontWithSize:size weight:weight];
        UIColor *uic = [theme colour:AIColourNone == textColour ? AIColourDirtyBlue : textColour];
        _label.textColor = uic;
        [_label setLineBreakMode:NSLineBreakByTruncatingTail];
        [_label setNumberOfLines:1];
        _label.text = label;
        
        _field = [UISwitch newAutoLayoutView];
        [self addSubview:_field];
        
        _field.tintColor = uic;
        _field.onTintColor = uic;
        [self setNeedsUpdateConstraints];
        
        [_field addTarget:self action:@selector(changeSwitch) forControlEvents:UIControlEventValueChanged];
    }
    return self;
}

- (void)changeSwitch
{
    _onChange(_inputId);
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
    [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.switchInset];
    [_label autoPinEdgeToSuperviewEdge:ALEdgeBottom];

    [_field autoPinEdgeToSuperviewEdge:ALEdgeTop withInset:_theme.margins];
    [_field autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:_label];
    [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
    [_field autoPinEdgeToSuperviewEdge:ALEdgeBottom];

    [super updateConstraints];
}

- (void)enable:(BOOL)enabled
{
    [_field setEnabled:enabled];
}

- (void)setValue:(nonnull NSString *)value
{
    [_field setOn:[value isEqualToString:@"true"]];
}

- (void)configure:(nonnull NSString *)config
{
    
}

- (void)setError:(nonnull NSString *)error
{
}

- (BOOL)validate
{
    return YES;
}

- (nonnull NSString *)value
{
    return _field.isOn ? @"true" : @"false";
}

- (UIView *)view
{
    return self;
}

@end
