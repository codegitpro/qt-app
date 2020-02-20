//
//  AIMultilineTextInput.m
//  user
//
//  Created by Hans Malherbe on 2019/05/04.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIMultilineTextInput.h"
#import "PureLayout.h"
#import "AITextField.h"

@interface AIMultilineTextInput()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UITextView *field;
@property (nonatomic, readonly) BOOL isRequired;
@property (nonatomic, readonly) OnChange onChange;
//@property (nonatomic, readonly) NSLayoutConstraint *textHeightConstraint;
@property (nonatomic, readonly) NSString *placeholder;
@property (nonatomic, readonly) AIColour textColour;
@property (nonatomic, readonly) UIScrollView *scroller;
@property (nonatomic, readonly) int maxWidth;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIAlign align;

@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) BOOL keyboardOn;

@property (nonatomic) CGSize scrollerContentSize;
@property (nonatomic) CGPoint scrollerContentOffset;
@property (nonatomic) UIDeviceOrientation scrollerOrientation;

@property (nonatomic) int newlineCount;
@end

@implementation AIMultilineTextInput

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                        label:(NSString *)label
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                     isDialog:(BOOL)isDialog
                   isRequired:(BOOL)isRequired
                        align:(AIAlign)align
                     maxWidth:(int)maxWidth
                        lines:(int)lines
                     scroller:(UIScrollView *)scroller
                     onChange:(OnChange)onChange
{
    if (self = [super initForAutoLayout])
    {
        _theme = theme;
        _textColour = textColour;
        _inputId = inputId;
        _onChange = onChange;
        _isRequired = isRequired;
        _placeholder = label;
        _scroller = scroller;
        _maxWidth = maxWidth;
        _align = align;
        
        _didSetupConstraints = NO;
        _keyboardOn = NO;
        _newlineCount = 0;
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.font = [theme fontWithSize:size weight:weight];
        _label.textColor = [theme colour:AIColourRed];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;

        _field = [UITextView newAutoLayoutView];
        [self addSubview:_field];

//        if
//            (
//             AIElementTypeUrlInput == type
//             || AIElementTypeEmailInput == type
//             || AIElementTypePhoneInput == type
//             || AIElementTypeSkypeInput == type
//             || AIElementTypeIntegerInput == type
//             || AIElementTypePasswordInput == type
//             )
//        {
//            _field.autocorrectionType = UITextAutocorrectionTypeNo;
//            _field.autocapitalizationType = UITextAutocapitalizationTypeNone;
//            _field.spellCheckingType = UITextSpellCheckingTypeNo;
//        }
//        else
//        {
//            _field.autocorrectionType = UITextAutocorrectionTypeDefault;
//            _field.autocapitalizationType = UITextAutocapitalizationTypeWords;
//            _field.spellCheckingType = UITextSpellCheckingTypeDefault;
//        }
//        _field.secureTextEntry = AIElementTypePasswordInput == type;
        _field.backgroundColor = theme.textInputBackground;

        _field.font = [theme fontWithSize:size weight:weight];
        _field.layer.cornerRadius = theme.textInputCorderRadius;
//        _field.placeholder = label;
        _field.text = label;
        _field.textColor = theme.grayColour;
//        _field.attributedPlaceholder = [[NSAttributedString alloc] initWithString:label attributes:@{NSForegroundColorAttributeName: [theme colour:textColour]}];
//        switch (type)
//        {
//            case AIElementTypeEmailInput: _field.keyboardType = UIKeyboardTypeEmailAddress; break;
//            case AIElementTypeIntegerInput: _field.keyboardType = UIKeyboardTypeNumberPad; break;
//            case AIElementTypeCurrencyInput: _field.keyboardType = UIKeyboardTypeDecimalPad; break;
//            case AIElementTypePhoneInput: _field.keyboardType = UIKeyboardTypePhonePad; break;
//            case AIElementTypeUrlInput: _field.keyboardType = UIKeyboardTypeURL; break;
//            default: break;
//        }
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

    [_label autoPinEdgeToSuperviewEdge:ALEdgeTop];

    [_field autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
    [_field autoPinEdgeToSuperviewEdge:ALEdgeBottom];

    __weak AIMultilineTextInput *weakself = self;
    if (_maxWidth > 0)
    {
        [[NSLayoutConstraint autoCreateConstraintsWithoutInstalling:^{
            [weakself.field autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
            [weakself.label autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
        }] autoInstallConstraints];
        
        [_field autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:0 relation:NSLayoutRelationLessThanOrEqual];
        [_label autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:0 relation:NSLayoutRelationLessThanOrEqual];
        
        switch (_align)
        {
            case AIAlignLeft:
            {
                [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                break;
            }
            case AIAlignRight:
            {
                [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                break;
            }
            case AIAlignCenter:
            {
                [_field autoAlignAxisToSuperviewAxis:ALAxisVertical];
                [_label autoAlignAxisToSuperviewAxis:ALAxisVertical];
                [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                break;
            }
            default:
            {
                [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                break;
            }
        }
    }
    else
    {
        [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
    }

    [NSLayoutConstraint autoSetPriority:UILayoutPriorityRequired forConstraints:^{
        [weakself.label autoSetContentCompressionResistancePriorityForAxis:ALAxisHorizontal];
    }];
//    _textHeightConstraint = [_field autoSetDimension:ALDimensionHeight toSize:MAX(44, _field.contentSize.height) relation:NSLayoutRelationGreaterThanOrEqual];
    [_field autoSetDimension:ALDimensionHeight toSize:120];
    [_field autoSetDimension:ALDimensionWidth toSize:100 relation:NSLayoutRelationGreaterThanOrEqual];
    [super updateConstraints];
}

- (void)enable:(BOOL)enabled
{
    _field.editable = enabled;
}

- (void)setValue:(nonnull NSString *)value
{
    if (value.length == 0)
    {
        _field.text = _placeholder;
        _field.textColor = _theme.grayColour;
    }
    else
    {
        _field.text = value;
        _field.textColor = [_theme colour:_textColour];
    }
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
    if (!_isRequired || (0 != _field.text.length && ![_field.text isEqualToString:_placeholder]))
        return true;
    [self setError:@"Please enter a value"];
    return false;
}

- (nonnull NSString *)value
{
    return [_field.text isEqualToString:_placeholder] ? @"" : _field.text;
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
    _onChange(_inputId);
    return YES;
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

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    if([text isEqualToString:@"\n"])
    {
        if (_newlineCount++ > 0)
        {
            _newlineCount = 0;
            [textView resignFirstResponder];
            return NO;
        }
//        [_textHeightConstraint setConstant:MAX(44, _field.contentSize.height)];
    }
    else
        _newlineCount = 0;
    
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

- (void)textViewDidBeginEditing:(UITextView *)textView
{
    _keyboardOn = YES;
    _scrollerContentSize = _scroller.contentSize;
    _scrollerOrientation = UIDevice.currentDevice.orientation;
    [self adjustScrollForKeyboard];
    
    if ([textView.text isEqualToString:_placeholder])
    {
        textView.text = @"";
        textView.textColor = [_theme colour:_textColour];
    }
    [textView becomeFirstResponder];
}

- (void)textViewDidEndEditing:(UITextView *)textView
{
    _keyboardOn = NO;
    _scroller.contentSize = _scrollerContentSize;
    _scroller.contentOffset = _scrollerContentOffset;
    
    if ([textView.text isEqualToString:@""])
    {
        textView.text = _placeholder;
        textView.textColor = _theme.grayColour;
    }
    [textView resignFirstResponder];
    _onChange(_inputId);
}

@end
