//
//  DirectoryNavigationViewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/21.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "DirectoryNavigationViewController.h"
#import "PureLayout.h"
#import "AIToolbarViewController.h"
#import "AIDirectoryList.h"

@interface DirectoryNavigationViewController ()
@property (nonatomic, readonly) AITheme *theme;
@end

@implementation DirectoryNavigationViewController

- (instancetype) initWithPortal:(AIPortal*)portal
                          names:(id<AINameStore>)names
                          theme:(AITheme *)theme
                             io:(AIOsIoImpl *)io
{
    self = [super init];
    if (self)
    {
        _theme = theme;
        self.navigationBar.translucent = NO;
        
        [self.navigationBar.titleTextAttributes setValue:theme.navbarTextColour forKey:NSForegroundColorAttributeName];
        AIToolbarViewController *toolbarController = [AIToolbarViewController new];
        UIViewController *controller = [[AIDirectoryList alloc] initWithPortal:portal
                                                                        parent:toolbarController
                                                                         title:@"Directory"
                                                                   directoryId:0
                                                                         names:names
                                                                         theme:theme
                                                                            io:io
                                        ];
        toolbarController.title = @"Directory";
        [toolbarController setViewController:controller];
        
        [self pushViewController:toolbarController animated:NO];
    }
    return self;
}

- (UIBarPosition)positionForBar:(id<UIBarPositioning>)bar
{
    return UIBarPositionBottom;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.navigationBar setTitleTextAttributes:@{NSForegroundColorAttributeName : _theme.navbarTextColour}];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
