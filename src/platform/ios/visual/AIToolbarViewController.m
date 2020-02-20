#import "AIToolbarViewController.h"
#import "PureLayout.h"

@interface AIToolbarViewController ()
@property (nonatomic, readonly) UIViewController *controller;
@property (nonatomic, readonly) UIToolbar *toolbar;
@property (nonatomic) NSLayoutConstraint *toolbarBottomConstraint;
@end

@implementation AIToolbarViewController

- (instancetype)init
{
    if (self = [super init])
    {
        _toolbar = [[UIToolbar alloc] initForAutoLayout];
        _toolbar.opaque = YES;
        _toolbar.translucent = NO;
        
        _toolbarHidden = YES;
        _tabbarHidden = NO;
    }
    return self;
}

- (void)setViewController:(UIViewController *)controller
{
    _controller = controller;
}

- (void)setItems:(nullable NSArray<UIBarButtonItem *> *)items animated:(BOOL)animated
{
    [_toolbar setItems:items animated:animated];
}

- (void)setRightBarButtonItems:(NSArray<UIBarButtonItem *> *)rightBarButtonItems
{
    self.navigationItem.rightBarButtonItems = rightBarButtonItems;
}

- (void)setTintColor:(UIColor *)tintColor
{
    [_toolbar setTintColor:tintColor];
}

- (void)setBarTintColor:(UIColor *)barTintColor
{
    [_toolbar setBarTintColor:barTintColor];
}

- (void)setScreenName:(nonnull NSString *)title
{
    
}


- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    if (self.isMovingFromParentViewController)
    {
        NSLog(@"Back Button Pressed");
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self addChildViewController:_controller];
    [_controller.view configureForAutoLayout];
    [self.view addSubview:_controller.view];
    
    [_controller.view autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [_controller.view autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [_controller.view autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
    
    [self.view addSubview:_toolbar];
    [_toolbar autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:_controller.view];
    [_toolbar autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [_toolbar autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
    _toolbarBottomConstraint = [_toolbar autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    _toolbarBottomConstraint.identifier = @"toolbarBottom";
    
    [_toolbar autoSetDimension:ALDimensionHeight toSize:self.tabBarController.tabBar.frame.size.height];
    
    [_controller didMoveToParentViewController:self];
    
    [self applyTabbar];
}

- (void)setToolbarHidden:(BOOL)toolbarHidden
{
    _toolbarHidden = toolbarHidden;
    [self applyTabbar];
}

- (void)applyTabbar
{
    _toolbar.hidden = _toolbarHidden;
    _toolbarBottomConstraint.constant = _toolbarHidden && _tabbarHidden ? self.tabBarController.tabBar.frame.size.height : 0;
    self.tabBarController.tabBar.hidden = _tabbarHidden;
}

- (void)setTabbarHidden:(BOOL)tabbarHidden
{
    _tabbarHidden = tabbarHidden;
    [self applyTabbar];
}

@end
