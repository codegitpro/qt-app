//
//  AITextInput.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AITextInput.h"
#import "PureLayout.h"
#import "AITextField.h"
#import "AIHelp.h"
@interface AITextInput()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UITextField *field;
@property (nonatomic, readonly) BOOL isRequired;
@property (nonatomic, readonly) OnChange onChange;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIAlign align;
@property (nonatomic, readonly) AIColour textColour;
@property (nonatomic, readonly) int maxWidth;
@property (nonatomic, readonly) UIScrollView *scroller;

@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) BOOL keyboardOn;
@property (nonatomic) CGSize scrollerContentSize;
@property (nonatomic) CGPoint scrollerContentOffset;
@property (nonatomic) UIDeviceOrientation scrollerOrientation;
@property (nonatomic) BOOL enabled;

@end

@implementation AITextInput

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                         type:(AIDataType)type
                     isDialog:(BOOL)isDialog
                   isRequired:(BOOL)isRequired
                        align:(AIAlign)align
                     maxWidth:(int)maxWidth
                     scroller:(UIScrollView *)scroller
               viewController:(UIViewController *)viewController
                     onChange:(OnChange)onChange
{
    if (self = [super initForAutoLayout])
    {
        _inputId = inputId;
        _onChange = onChange;
        _isRequired = isRequired;
        _maxWidth = maxWidth;
        _theme = theme;
        _align = align;
        _scroller = scroller;
        _textColour = textColour;
        
        _didSetupConstraints = NO;
        _keyboardOn = NO;
        _enabled = YES;
        
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.font = [theme fontWithSize:size weight:weight];
        _label.textColor = [theme colour:AIColourRed];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;

        _field = [AITextField newAutoLayoutView];
        [self addSubview:_field];
        
        if
        (
            AIDataTypeUrl == type
            || AIDataTypeEmail == type
            || AIDataTypePhone == type
            || AIDataTypeSkype == type
            || AIDataTypeInteger == type
            || AIDataTypePassword == type
        )
        {
            _field.autocorrectionType = UITextAutocorrectionTypeNo;
            _field.autocapitalizationType = UITextAutocapitalizationTypeNone;
            _field.spellCheckingType = UITextSpellCheckingTypeNo;
        }
        else
        {
            _field.autocorrectionType = UITextAutocorrectionTypeDefault;
            _field.autocapitalizationType = UITextAutocapitalizationTypeWords;
            _field.spellCheckingType = UITextSpellCheckingTypeDefault;
        }
        _field.secureTextEntry = AIDataTypePassword == type;
        //        _field.enablesReturnKeyAutomatically = YES;
        _field.backgroundColor = theme.textInputBackground;
        _field.font = [theme fontWithSize:size weight:weight];
        _field.layer.cornerRadius = theme.textInputCorderRadius;
        _field.placeholder = label;
        _field.textColor = [theme colour:AIColourNone == textColour ? AIColourDarkGrey : textColour];
//        _field.attributedPlaceholder = [[NSAttributedString alloc] initWithString:label attributes:@{NSForegroundColorAttributeName: [theme colour:AIColourLightGrey]}];
        switch (type)
        {
            case AIDataTypeEmail:
            {
                _field.keyboardType = UIKeyboardTypeEmailAddress;
                
                break;
            }
            case AIDataTypeInteger: _field.keyboardType = UIKeyboardTypeNumberPad; break;
            case AIDataTypeCurrency: _field.keyboardType = UIKeyboardTypeDecimalPad; break;
            case AIDataTypePhone:
            {
                _field.keyboardType = UIKeyboardTypePhonePad;
                break;
            }
            case AIDataTypeUrl:
            {
                _field.keyboardType = UIKeyboardTypeURL;
                break;
            }
            case AIDataTypeSkype:
            {
                break;
            }
            default: break;
        }
        _field.delegate = self;
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

//    NSMutableArray *line = [NSMutableArray new];
//    UIView *container = [UIView newAutoLayoutView];
//    [self addSubview:container];
//    [container addSubview:_field];
//    [line addObject:_field];

//    [_theme distributeHorizontally:line margins:NO];
    [_field autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
    [_field autoPinEdgeToSuperviewEdge:ALEdgeBottom withInset:_theme.margins];

    if (_maxWidth > 0)
    {
        __weak AITextInput *weakself = self;
        [[NSLayoutConstraint autoCreateConstraintsWithoutInstalling:^{
            [weakself.label autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
            [weakself.field autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
        }] autoInstallConstraints];
        
//        [_label autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:_field withOffset:0 relation:NSLayoutRelationLessThanOrEqual];
//        [_field autoSetDimension:ALDimensionWidth toSize:_maxWidth relation:NSLayoutRelationEqual];

    }
    switch (_align)
    {
        case AIAlignLeft:
        {
            [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
            [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignRight:
        {
            [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
            [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        case AIAlignCenter:
        {
            [_field autoAlignAxisToSuperviewAxis:ALAxisVertical];
            [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
        default:
        {
            [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
            break;
        }
    }

//    [container autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
//    [container autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];

    [_label autoPinEdge:ALEdgeLeading toEdge:ALEdgeLeading ofView:_field];
    [_label autoPinEdge:ALEdgeTrailing toEdge:ALEdgeTrailing ofView:_field];
    [super updateConstraints];
}

- (void)enable:(BOOL)enabled
{
    _enabled = enabled;
    _field.textColor = _enabled ? [_theme colour:AIColourNone == _textColour ? AIColourDarkGrey : _textColour] : _theme.grayColour;
    [_field setEnabled:enabled];
}

- (void)setValue:(nonnull NSString *)value
{
    _field.text = value;
}

- (void)configure:(nonnull NSString *)config
{
    
}

- (void)setError:(nonnull NSString *)error
{
    _label.text = error;
}

- (BOOL)validate
{
    if (!_isRequired || 0 != _field.text.length)
        return true;
    [self setError:@"Please enter a value"];
    return false;
}

- (nonnull NSString *)value
{
    return _field.text;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return NO;
}

- (UIView *)view
{
    return self;
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    return YES;
}

- (void)adjustScrollForKeyboard
{
    _scrollerContentOffset = _scroller.contentOffset;
    UIView *origin = _field.superview;
    CGPoint childStartPoint = [origin convertPoint:_field.frame.origin toView:_scroller];
    
    _scroller.contentSize = CGSizeMake(_scrollerContentSize.width, _scrollerContentSize.height + 250);
    [_scroller scrollRectToVisible:CGRectMake(0, childStartPoint.y, 44, 250) animated:YES];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    
    if (_keyboardOn)
    {
        if (UIDevice.currentDevice.orientation != _scrollerOrientation)
        {
            _scrollerContentSize = _scroller.contentSize;
            _scrollerOrientation = UIDevice.currentDevice.orientation;
        }
        [self adjustScrollForKeyboard];
    }
}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    _keyboardOn = YES;
    _scrollerContentSize = _scroller.contentSize;
    _scrollerOrientation = UIDevice.currentDevice.orientation;
    [self adjustScrollForKeyboard];

    [textField becomeFirstResponder];
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    _keyboardOn = NO;
    _scroller.contentSize = _scrollerContentSize;
    _scroller.contentOffset = _scrollerContentOffset;
    
    [textField resignFirstResponder];
    _onChange(_inputId);
}

@end
