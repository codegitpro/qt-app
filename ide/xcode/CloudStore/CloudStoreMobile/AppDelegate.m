//
//  AppDelegate.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/05.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AppDelegate.h"
#import "AIPortal.h"
#import "AIGCDWorker.h"
#import "AIOsIoImpl.h"
#import "DirectoryNavigationViewController.h"
#import "AITheme.h"
#import "AILaunchController.h"
#import <StoreKit/StoreKit.h>
#import "AITransactionObserver.h"

#import "AIRootViewmodel.h"
#import "AIViewController.h"
#import "AIUIThreadRunner.h"

@interface AppDelegate ()
@property (nonatomic) AIPortal* cppApi;
@property (nonatomic) UIActivityIndicatorView* activity;
@property (nonatomic) AIOsIoImpl *io;
@property (nonatomic) id <SKPaymentTransactionObserver> observer;

@property (nonatomic) AIRootViewmodel *vm;
@end

@implementation AppDelegate

@synthesize directoryNames;

@synthesize fileNames;

#define UIColorFromRGB(rgbValue) \
[UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 \
green:((float)((rgbValue & 0x00FF00) >>  8))/255.0 \
blue:((float)((rgbValue & 0x0000FF) >>  0))/255.0 \
alpha:1.0]

- (UIViewController *)mainControllerWithTheme:(AITheme *)theme old:(BOOL)old appDataLocation:(NSString *)appDataLocation
{
    if (old)
    {
        AILaunchController *controller = [[AILaunchController alloc] initWithTheme:theme names:self io:_io];
        {
            _cppApi = [AIPortal create:[AIGCDWorker new]
                       appDataLocation:appDataLocation
                               thumbPx:theme.thumbnailPx
                                    io:_io gui:controller
                                  type:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? AIScreenTypeTablet : AIScreenTypePhone
                       ];
            [controller setPortal:_cppApi];
            [_cppApi ping];
        }
        return controller;
    }
    
    AIViewController *mainView = [[AIViewController alloc] initWithTheme:theme io:_io];
    _observer = mainView;
    _vm = [AIRootViewmodel create:[AIGCDWorker new]
                  appDataLocation:appDataLocation
                          thumbPx:theme.thumbnailPx
                               io:_io
                             view:mainView
                             type:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? AIScreenTypeTablet : AIScreenTypePhone
                         callback:[AIUIThreadRunner new]
                           logger:_io
           ];
    mainView.vm = _vm;
    return mainView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.directoryNames = [NSMutableArray new];
    self.fileNames = [NSMutableArray new];

    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *appDataLocation = paths.firstObject;
    AITheme *theme = [AITheme new];
    _io = [[AIOsIoImpl alloc] initWithTheme:theme appDataLocation:appDataLocation];

    

    [UINavigationBar appearance].tintColor = theme.navbarActionColour;
    [UINavigationBar appearance].barTintColor = theme.navbarColour;
    [UIToolbar appearance].tintColor = theme.navbarActionColour;
    
    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _window.rootViewController = [self mainControllerWithTheme:theme old:NO appDataLocation:appDataLocation];
    [_window makeKeyAndVisible];
    _activity = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    _activity.center=_window.center;
    _activity.color = [UIColor redColor];
    _activity.hidesWhenStopped = YES;
    [_window addSubview:self.activity];

    [[SKPaymentQueue defaultQueue] addTransactionObserver:_observer];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {}
- (void)applicationDidEnterBackground:(UIApplication *)application {}
- (void)applicationWillEnterForeground:(UIApplication *)application {}
- (void)applicationDidBecomeActive:(UIApplication *)application {}
- (void)applicationWillTerminate:(UIApplication *)application
{
    if (_observer)
        [[SKPaymentQueue defaultQueue] removeTransactionObserver:_observer];
}

- (void)application:(UIApplication *)application handleEventsForBackgroundURLSession:(NSString *)identifier completionHandler:(void (^)(void))completionHandler
{
    _io.savedCompletionHandler = completionHandler;
}

@end
