//
//  AIPhoneFormController.m
//  user
//
//  Created by Hans Malherbe on 2019/05/04.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIPhoneFormController.h"
#import "PureLayout.h"
#import "AIBaseForm.h"

@interface AIPhoneFormController ()
@property (nonatomic, readonly) AIBaseForm *form;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) UIView *contentView;
@property (nonatomic, readonly) UIScrollView *scroller;
@property (nonatomic, readonly) BOOL showMenu;
@property (nonatomic, readonly) BOOL isDialog;

@property (nonatomic) BOOL isKeyboardVisible;
@property (nonatomic) BOOL didSetupConstraints;
@property (nonatomic) UIView *bottomElement;
@property (nonatomic) AIFormViewmodel *vm;
@property (nonatomic) NSLayoutConstraint *heightConstraint;
@end

@implementation AIPhoneFormController

- (instancetype)initWithTheme:(nonnull AITheme *)theme
                            vm:(AIFormViewmodel *)vm
                    references:(nonnull NSArray<AIReference *> *)references
                      showMenu:(BOOL)showMenu
                      isDialog:(BOOL)isDialog
                      onChange:(nullable OnChange)onChange
{
    if (self = [super init])
    {
        _theme = theme;
        _vm = vm;
        
        _didSetupConstraints = NO;
        _contentView = [UIView newAutoLayoutView];
        _scroller = [UIScrollView newAutoLayoutView];
        _showMenu = showMenu;
        _isDialog = isDialog;
        _form = [[AIBaseForm alloc] initWithTheme:theme
                                               vm:vm
                                       references:references
                                 parentController:self
                                         scroller:_scroller
                                         onChange:onChange
                 ];

        _isKeyboardVisible = NO;
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(didShow) name:UIKeyboardDidShowNotification object:nil];
        [center addObserver:self selector:@selector(didHide) name:UIKeyboardWillHideNotification object:nil];

    }
    return self;
}

- (void)dealloc
{
    NSLog(@"dealloc AIPhoneFormController");
}

- (void)updateViewConstraints
{
    if (_didSetupConstraints)
    {
        [super updateViewConstraints];
        return;
    }
    _didSetupConstraints = YES;
    
    [_scroller autoPinEdgesToSuperviewEdges];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [_contentView autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:_scroller];
    _heightConstraint = [_contentView autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:self.view withOffset:0 relation:NSLayoutRelationGreaterThanOrEqual];

    [_bottomElement autoPinEdgeToSuperviewSafeArea:ALEdgeBottom];
    [super updateViewConstraints];
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
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
    [super viewDidLayoutSubviews];
    
    CGFloat h = 0;
    if (!self.navigationController.navigationBarHidden)
        h -= self.navigationController.navigationBar.frame.size.height;
    if (!self.tabBarController.tabBar.hidden)
        h -= self.tabBarController.tabBar.frame.size.height;
//    UIInterfaceOrientation orient = [[UIApplication sharedApplication] statusBarOrientation];
//    if (UIInterfaceOrientationIsPortrait(orient))
//    {
    h -= [UIApplication sharedApplication].statusBarFrame.size.height;
//    }
//    else if (UIInterfaceOrientationIsLandscape(orient))
//    {
//        h -= [UIApplication sharedApplication].statusBarFrame.size.width;
//    }
    
    _heightConstraint.constant = h;
    NSLog(@"extra h %f", h);
    
    [_contentView setNeedsLayout];
    [_contentView layoutIfNeeded];

    if (!_isKeyboardVisible)
        _scroller.contentSize = _contentView.frame.size;
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    if (self.isMovingFromParentViewController)
    {
        [_vm back];
        if (![_vm holdonToVm])
        {
            _vm = nil;
            _form = nil;
        }
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    self.navigationController.navigationBarHidden = [_vm hideNavigation];
    
    [_vm onLoad:self form:self];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.view.backgroundColor = _theme.formBackground;
    
    [self.view addSubview:_scroller];
    //    [_scroller addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tap:)]];
    
    [_scroller addSubview:_contentView];
    
    
    self.navigationController.navigationBarHidden = !_showMenu;
    _bottomElement = [_form addCenteredFields:[_vm fields] content:_contentView prev:nil isDialog:NO];
//    [_vm onLoad:self form:self];
}

- (void)menuItemSelected
{
}

- (void)close
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    UINavigationController *nav = self.navigationController;
    if (nav.viewControllers.count > 1)
        [nav popViewControllerAnimated:NO];
    else
        nav.viewControllers = @[];
    _vm = nil;
    _form = nil;
}

- (void)disableActions
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    [_form disableActions];
}

- (BOOL)enable:(int32_t)elementId enabled:(BOOL)enabled
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    return [_form enable:elementId enabled:enabled];
}

- (void)error:(int32_t)elementId message:(nonnull NSString *)message
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    [_form error:elementId message:message];
}

- (void)inputConfigure:(int32_t)inputId config:(nonnull NSString *)config
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    [_form inputConfigure:inputId config:config];
}

- (nonnull NSString *)inputGet:(int32_t)inputId
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    return [_form inputGet:inputId];
}

- (void)inputSet:(int32_t)inputId value:(nonnull NSString *)value
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    [_form inputSet:inputId value:value];
}

- (BOOL)validate
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");

    return [_form validate];
}

- (void)showMessage:(nonnull NSString *)message
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    [_theme showMessage:message controller:self];
}

- (void)reload:(int32_t)section row:(int32_t)row
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    
}

- (void)setScreenName:(nonnull NSString *)title
{
    self.navigationItem.title = title;
}

- (void)pushForm:(nullable AIFormViewmodel *)vm {
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                  vm:vm
                                                                          references:@[]
                                                                            showMenu:YES
                                                                            isDialog:NO
                                                                            onChange:nil
                                         ];
    [self.navigationController pushViewController:controller animated:YES];
    [self.navigationController setNavigationBarHidden:NO animated:NO];
    
}


- (void)replaceForm:(nullable AIFormViewmodel *)vm
{
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                  vm:vm
                                                                          references:@[]
                                                                            showMenu:_showMenu
                                                                            isDialog:_isDialog
                                                                            onChange:nil
                                         ];
    UINavigationController *nav = self.navigationController;

//    nav.viewControllers = @[controller];
    
    NSMutableArray<UIViewController *> *stack = [NSMutableArray new];
    for (int i = 0; i < nav.viewControllers.count - 1; ++i)
    {
        [stack addObject:nav.viewControllers[i]];
    }
    
    [stack addObject:controller];
    [nav setViewControllers:stack animated:NO];
}


@end
