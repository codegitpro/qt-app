//
//  AIPicker.m
//  user
//
//  Created by Hans Malherbe on 2019/05/01.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIPicker.h"
#import "PureLayout.h"
#import "AIPickerController.h"

@interface AIPicker()
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UIButton *button;

@property (nonatomic, readonly) BOOL isRequired;
@property (nonatomic, readonly) OnChange onChange;
@property (nonatomic, readonly) NSArray<NSString *> *options;
@property (nonatomic, readonly) UIColor *color;

@property (nonatomic, readonly) UIPickerView *picker;
@property (nonatomic, readonly) AIPickerController *pickerController;
@property (nonatomic, readonly, weak) UIViewController *parentController;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIAlign align;
@property (nonatomic, readonly) int maxWidth;

@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) int currentIndex;
@end

@implementation AIPicker

- (instancetype)initWithTheme:(AITheme *)theme
                      inputId:(int)inputId
                      options:(NSArray<NSString *> *)options
                         size:(AISize)size
                       weight:(AIWeight)weight
                   textColour:(AIColour)textColour
                     isDialog:(BOOL)isDialog
                   isRequired:(BOOL)isRequired
                        align:(AIAlign)align
                     maxWidth:(int)maxWidth
         parentViewController:(UIViewController *)parentViewController
                     onChange:(OnChange)onChange
{
    if (self = [super initForAutoLayout])
    {
        _isRequired = isRequired;
        _options = options;
        _inputId = inputId;
        _parentController = parentViewController;
        _onChange = onChange;
        _theme = theme;
        _align = AIAlignNone == align ? AIAlignLeft : align;
        _maxWidth = maxWidth;
        
        _didSetupConstraints = NO;
        _currentIndex = 0;
        _color = [theme colour:textColour];
        
        _label = [UILabel newAutoLayoutView];
        [self addSubview:_label];
        _label.font = [theme fontWithSize:size weight:weight];
        _label.textColor = [theme colour:AIColourRed];
        _label.textAlignment = [AITheme convertTextAlign:_align];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;

        _button = [UIButton buttonWithType:UIButtonTypeSystem];
        [self addSubview:_button];
        [_button configureForAutoLayout];
        
        _button.backgroundColor = isDialog ? theme.formBackground : theme.listBackgroundColour;
        _button.tintColor = [theme colour:textColour];
        _button.titleLabel.font = [theme fontWithSize:size weight:weight];
        _button.contentHorizontalAlignment = [AITheme convertAlign:_align];
        _button.contentEdgeInsets = UIEdgeInsetsMake(10, 0, 10, 0);
        [_button addTarget:self action:@selector(click) forControlEvents:UIControlEventTouchUpInside];
        if (_options.count > 0)
            [_button setTitle:_options[_currentIndex] forState:UIControlStateNormal];
        else
            _button.enabled = NO;
        
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
    
    [_button autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_label];
    [_button autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    if (_maxWidth > 0)
    {
        __weak AIPicker *weakself = self;
        [[NSLayoutConstraint autoCreateConstraintsWithoutInstalling:^{
            [weakself.button autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
            [weakself.label autoSetDimension:ALDimensionWidth toSize:weakself.maxWidth].priority = UILayoutPriorityDefaultHigh;
        }] autoInstallConstraints];
        
        [_button autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:0 relation:NSLayoutRelationLessThanOrEqual];
        [_label autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:self withOffset:0 relation:NSLayoutRelationLessThanOrEqual];
        
        switch (_align)
        {
            case AIAlignLeft:
            {
                [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                break;
            }
            case AIAlignRight:
            {
                [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                break;
            }
            case AIAlignCenter:
            {
                [_button autoAlignAxisToSuperviewAxis:ALAxisVertical];
                [_label autoAlignAxisToSuperviewAxis:ALAxisVertical];
                [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
                [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                break;
            }
            default:
            {
                [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
                [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins relation:NSLayoutRelationGreaterThanOrEqual];
                break;
            }
        }
    }
    else
    {
        [_button autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
        [_button autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
        [_label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
    }
    [_picker autoPinEdgesToSuperviewEdges];

    [super updateConstraints];
}

- (void)click
{
    if (_options.count == 0)
        return;
    
    if (_pickerController)
    {
        [_pickerController dismissViewControllerAnimated:NO completion:nil];
    }
    _picker = [UIPickerView newAutoLayoutView];
    _picker.showsSelectionIndicator = YES;
    _picker.delegate = self;
    _picker.dataSource = self;
    _picker.backgroundColor = _theme.formBackground;
    _pickerController = [AIPickerController new];
    [_pickerController.view addSubview:_picker];
    _pickerController.preferredContentSize = CGSizeMake(320, 216);
    
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        _pickerController.modalPresentationStyle = UIModalPresentationPopover;
        UIPopoverPresentationController *ppc = _pickerController.popoverPresentationController;
        ppc.permittedArrowDirections = UIPopoverArrowDirectionAny;
        ppc.sourceView = _button;
    }
    else
    {
        _pickerController.modalPresentationStyle = UIModalPresentationFormSheet;
    }

    [_parentController presentViewController:_pickerController animated:NO completion:nil];
}

- (void)configure:(nonnull NSString *)config
{
    _options = [AIPicker optionsFromString:config];
    if (_options.count > 0)
    {
        _button.enabled = YES;
        _currentIndex = 0;
        [_button setTitle:_options[_currentIndex] forState:UIControlStateNormal];
    }
    else
    {
        _button.enabled = NO;
        [_button setTitle:@"" forState:UIControlStateNormal];
    }
    _button.enabled = _options.count > 0;
}

- (void)enable:(BOOL)enabled
{
    [_button setEnabled:_options.count > 0 && enabled];
}

- (void)setValue:(NSString *)value
{
    _currentIndex = 0 == value.length ? 0 : [value intValue];
    [_button setTitle:_options[_currentIndex] forState:UIControlStateNormal];
}
- (NSString *)value
{
    return 0 == _currentIndex ? @"" : [NSString stringWithFormat:@"%d", _currentIndex];
}

- (void)setError:(nonnull NSString *)error
{
    _label.text = error;
}

- (BOOL)validate
{
    if (!_isRequired || 0 != _currentIndex)
        return true;
    [self setError:@"Please enter a value"];
    return false;
}

- (nonnull UIView *)view
{
    return self;
}

- (NSInteger)numberOfComponentsInPickerView:(nonnull UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(nonnull UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return _options.count;
}

- (NSAttributedString *)pickerView:(UIPickerView *)pickerView attributedTitleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [[NSAttributedString alloc] initWithString:_options[row] attributes:@{NSForegroundColorAttributeName: _color}];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    _currentIndex = (int)[_picker selectedRowInComponent:component];
    [_pickerController dismissViewControllerAnimated:NO completion:nil];
    _pickerController = nil;
    _picker = nil;
    [_button setTitle:_options[_currentIndex] forState:UIControlStateNormal];
    _onChange(_inputId);
}

+ (NSArray<NSString *> *)optionsFromString:(NSString *)pipeDelimitedOptions
{
    NSMutableArray<NSString *> *r = [NSMutableArray new];
    NSArray *items = [pipeDelimitedOptions componentsSeparatedByString:@"|"];
    for (int i = 0; i < items.count; ++i)
    {
        NSString *item = items[i];
        if (item.length > 0)
            [r addObject:item];
    }
    return r;
}

@end
