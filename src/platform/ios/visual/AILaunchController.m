#import "AILaunchController.h"
#import "AIVoidResult.h"
#import "AIFormViewController.h"
#import "PureLayout.h"
#import "DirectoryNavigationViewController.h"
#import "AIPhotoPreviewController.h"
#import "AIAccountScreen.h"
#import "AIVideoPreviewController.h"

@interface AILaunchController ()
@property (nonatomic, readonly) id<AINameStore> names;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIOsIoImpl *io;
@property (nonatomic) AIPortal *portal;
@property (nonatomic) UINavigationController *accountController;
@property (nonatomic) DirectoryNavigationViewController *directoryController;
@property (nonatomic, weak) UIViewController *formController;
@property (nonatomic) MFMessageComposeViewController *messageController;
@end

@implementation AILaunchController

- (instancetype)initWithTheme:(AITheme *)theme
                        names:(id<AINameStore>)names
                           io:(AIOsIoImpl *)io
{
    if (self = [super init])
    {
        _names = names;
        _theme = theme;
        _io = io;
        
        self.tabBar.tintColor = theme.tabBarSelectionColour;
        self.tabBar.barTintColor = theme.tabBarBackgroundColour;
    }
    return self;
}

- (void)accountAbortNetworkTasks
{
    [_portal cancelNetworkOperations];
    self.selectedIndex = 0;
}

- (void)accountAbout
{
    [_portal about];
}

- (void)accountSignOut
{
    [_names.fileNames removeAllObjects];
    [_names.directoryNames removeAllObjects];
    _accountController = nil;
    _directoryController = nil;
    [_portal signOut];
}

- (void)sendInvite
{
    _messageController = [[MFMessageComposeViewController alloc] init];
    _messageController.body = @"Try Blomp for free cloud storage! Click https://sms.blomp.com";
    _messageController.messageComposeDelegate = self;
    [self presentViewController:_messageController animated:YES completion:nil];

}


- (void)setPortal:(AIPortal *)portal
{
    _portal = portal;
}

- (void)showCloudstore
{
    __weak AILaunchController *weakself = self;
    
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       DirectoryNavigationViewController *dirController = [[DirectoryNavigationViewController alloc] initWithPortal:weakself.portal
                                                                                                                              names:weakself.names
                                                                                                                              theme:weakself.theme
                                                                                                                                 io:weakself.io
                                                                           ];
                       weakself.directoryController = dirController;
                       
                       dirController.tabBarItem = [[UITabBarItem alloc] initWithTitle:@"Files" image:[UIImage imageNamed:@"tree"] tag:1];
                       
                       AIAccountScreen *accountScreen = [[AIAccountScreen alloc] initWithTheme:weakself.theme actions:self];
                       accountScreen.controller.bottomSpace = weakself.tabBar.frame.size.height;
                       UIViewController *accountController = accountScreen.controller;
                       
                       UINavigationController *accountNavController = [[UINavigationController alloc] initWithRootViewController:accountController];
                       [accountNavController.navigationBar setTitleTextAttributes:@{NSForegroundColorAttributeName : weakself.theme.navbarTextColour}];
                       weakself.accountController = accountNavController;
                       accountNavController.tabBarItem = [[UITabBarItem alloc] initWithTitle:@"Account" image:[UIImage imageNamed:@"misc"] selectedImage:[UIImage imageNamed:@"misc_heavy"]];
                       
                       weakself.viewControllers = @[dirController, accountNavController];
                   });
}

- (void)guiThread:(nullable AIVoidResult *)cb
{
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       [cb onResult];
                   });
}

- (void)back
{
}

- (nullable id<AIOsForm>)createForm:(nonnull NSString *)title
                     startHierarchy:(BOOL)startHierarchy
                              width:(int32_t)width
                             height:(int32_t)height
                             action:(nullable AIIntResult *)action
                           elements:(nonnull NSArray<AIElement *> *)elements
{
    NSAssert([NSThread isMainThread], @"AILaunchController.createForm called off main thread");
    
    AIFormViewController* controller = [[AIFormViewController alloc] initForm:_theme
                                                                        title:title
                                                                        width:width
                                                                       height:height
                                                                       action:action
                                                                     elements:elements
                                        ];
    _formController = controller;
    
    if (!_accountController)
    {
        UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
        self.viewControllers = @[navController];
        [navController.navigationBar setTitleTextAttributes:@{NSForegroundColorAttributeName : _theme.navbarTextColour}];
        self.tabBar.hidden = YES;
    }
    else
    {
        [_accountController pushViewController:controller animated:NO];
        controller.bottomSpace = 0;
        self.tabBar.hidden = NO;
        controller.bottomSpace = self.tabBar.frame.size.height;
    }
    
    return controller;
}


- (void)openLinkInBrowser:(nonnull NSString *)url
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
}

- (void)preview:(AIFileType)type
           path:(NSString *)path
         fileId:(int32_t)fileId
       fileName:(NSString *)fileName
{
    __weak AILaunchController *weakself = self;
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       if (!weakself.directoryController)
                           return;
                       switch (type)
                       {
                           case AIFileTypeText: break;
                           case AIFileTypeImage:
                           {
                               AIPhotoPreviewController *controller = [[AIPhotoPreviewController alloc] initWithFilePath:path
                                                                                                                   title:fileName
                                                                                                                  fileId:fileId
                                                                                                                  portal:weakself.portal
                                                                                                                   theme:weakself.theme];
                               [weakself.directoryController pushViewController:controller animated:NO];
                               break;
                           }
                           case AIFileTypeVideo:
                           {
                               AIVideoPreviewController *controller = [[AIVideoPreviewController alloc] initWithFilePath:path
                                                                                                                   title:fileName
                                                                                                                  fileId:fileId
                                                                                                                  portal:weakself.portal
                                                                                                                   theme:weakself.theme];
                               [weakself.directoryController pushViewController:controller animated:NO];
                               break;
                           }
                           default: break;
                       }
                       
                   });
}


- (void)reset
{
}

- (void)showMessage:(nonnull NSString *)message
{
    __weak AILaunchController *weakself = self;
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       UIViewController *controller = weakself.formController;
                       if (!controller)
                           controller = weakself.directoryController;
                       if (!controller)
                           return;
                       [weakself.theme showMessage:message controller:controller];
                   });
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
}

- (void)messageComposeViewController:(nonnull MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result
{
    [_messageController dismissViewControllerAnimated:NO completion:nil];
}

@end
