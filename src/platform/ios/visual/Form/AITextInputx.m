#import "AITextInputx.h"
#import "PureLayout.h"
#import "AIFonts.h"
#import "AITextField.h"

@interface AITextInputx()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UITextField *field;
@end

@implementation AITextInputx    

- (instancetype)initWithTheme:(AITheme *)theme label:(NSString *)label element:(AIElement *)element
{
    if (self = [super initForAutoLayout])
    {
        _group = element.group;
        _inputId = element.inputId;

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

        _field = [AITextField newAutoLayoutView];
        [self addSubview:_field];
        [_field autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:theme.margins];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:theme.margins];
        [_field autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        
        if (AIElementTypeEmailInput == element.type || AIElementTypePasswordInput == element.type)
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
        _field.secureTextEntry = AIElementTypePasswordInput == element.type;
//        _field.enablesReturnKeyAutomatically = YES;
        _field.backgroundColor = theme.textInputBackground;
        _field.font = theme.textInputFont;
        _field.layer.cornerRadius = theme.textInputCorderRadius;
        _field.placeholder = label;
        if (AIElementTypeEmailInput == element.type)
            _field.keyboardType = UIKeyboardTypeEmailAddress;
        _field.delegate = self;

    }
    return self;
}

- (void)enable:(BOOL)enabled
{
    [_field setEnabled:enabled];
}

- (void)setValue:(NSString *)value
{
    _field.text = value;
}

- (NSString *)value
{
    return _field.text;
}

- (void)setError:(nonnull NSString *)error
{
    _label.text = error;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return NO;
}

@end
