//
//  AIConfirmController.m
//  user
//
//  Created by Hans Malherbe on 2019/05/12.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIConfirmController.h"
#import "PureLayout.h"

@interface AIConfirmController ()
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIFormViewmodel *vm;
@property (nonatomic, readonly) UIView *buttonContainer;
@property (nonatomic, readonly) UIButton *yesButton;
@property (nonatomic, readonly) UIButton *noButton;
@property (nonatomic, readonly) UILabel *label;
@property (nonatomic, readonly) UIView *contentView;
@property (nonatomic, readonly) UIScrollView *scroller;

@property (nonatomic) BOOL isKeyboardVisible;
@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) int action;
@end

@implementation AIConfirmController

- (instancetype)initWithTitle:(nonnull NSString *)message
                          yes:(nonnull NSString *)yes
                           no:(nonnull NSString *)no
                        theme:(nonnull AITheme *)theme
                           vm:(AIFormViewmodel *)vm
{
    if (self = [super init])
    {
        _vm = vm;
        _theme = theme;
        
        _isKeyboardVisible = NO;
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(didShow) name:UIKeyboardDidShowNotification object:nil];
        [center addObserver:self selector:@selector(didHide) name:UIKeyboardWillHideNotification object:nil];

        _action = -1;
        _didSetupConstraints = NO;
        
        _contentView = [UIView newAutoLayoutView];
        _scroller = [UIScrollView newAutoLayoutView];

        _label = [UILabel newAutoLayoutView];
        [_contentView addSubview:_label];

        _label.text = message;
        _label.font = [theme fontWithSize:AISizeLarge weight:AIWeightNormal];
        _label.textColor = [theme colour:AIColourWhite];
        _label.textAlignment = [AITheme convertTextAlign:AIAlignCenter];
        _label.lineBreakMode = NSLineBreakByWordWrapping;
        _label.numberOfLines = 0;

        _buttonContainer = [UIView newAutoLayoutView];
        [_contentView addSubview:_buttonContainer];
        
        _yesButton = [self setupButtonWithTitle:yes];
        [_buttonContainer addSubview:_yesButton];
        [_yesButton addTarget:self action:@selector(yesClick) forControlEvents:UIControlEventTouchUpInside];

        _noButton = [self setupButtonWithTitle:no];
        [_buttonContainer addSubview:_noButton];
        [_noButton addTarget:self action:@selector(noClick) forControlEvents:UIControlEventTouchUpInside];
    }
    return self;
}

- (void)yesClick
{
    _action = 1;
    [self.navigationController popViewControllerAnimated:NO];
}

- (void)noClick
{
    _action = 2;
    [self.navigationController popViewControllerAnimated:NO];
}

- (UIButton *)setupButtonWithTitle:(NSString *)title
{
    UIButton *button = [UIButton newAutoLayoutView];
    button.backgroundColor = _theme.formBackground;
    button.tintColor = [_theme colour:AIColourWhite];
    button.titleLabel.font = [_theme fontWithSize:AISizeLarge weight:AIWeightNormal];
    button.contentHorizontalAlignment = [AITheme convertAlign:AIAlignCenter];
    button.layer.cornerRadius = _theme.buttonCornerRadius;
    button.layer.borderWidth = 1;
    button.layer.borderColor = [_theme colour:AIColourWhite].CGColor;
    button.layer.masksToBounds = YES;
    [button setTitleColor:[_theme colour:AIColourWhite] forState:UIControlStateNormal];
    button.contentEdgeInsets = UIEdgeInsetsMake(10, 15, 10, 10);
    [button setTitle:title forState:UIControlStateNormal];
    return button;
}

- (void)viewWillDisappear:(BOOL)animated
{
    [_vm action:_action];
}

- (void)updateViewConstraints
{
    if (_didSetupConstraints)
    {
        [super updateViewConstraints];
        return;
    }
  
    [_scroller autoPinEdgesToSuperviewEdges];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [_contentView autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:_scroller];
    [_contentView autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:self.view withOffset:0 relation:NSLayoutRelationGreaterThanOrEqual];

    [_theme layoutHorizontal:@[_yesButton, _noButton] margins:NO];
    [_theme layoutVertical:@[_label, _buttonContainer] margins:NO];
    
  
    _didSetupConstraints = YES;
    [super updateViewConstraints];
}

- (void)didShow
{
    _isKeyboardVisible = YES;
}

- (void)didHide
{
    _isKeyboardVisible = NO;
}

- (void)viewDidLayoutSubviews
{
    [_contentView setNeedsLayout];
    [_contentView layoutIfNeeded];
    if (!_isKeyboardVisible)
        _scroller.contentSize = _contentView.frame.size;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.view addSubview:_scroller];
    [_scroller addSubview:_contentView];
    [self.view setNeedsUpdateConstraints];
}

@end
