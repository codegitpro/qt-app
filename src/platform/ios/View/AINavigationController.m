//
//  AINavigationController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/02.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AINavigationController.h"

@interface AINavigationController ()

@end

@implementation AINavigationController

- (instancetype)initWithTheme:(AITheme *)theme
{
    if (self = [super init])
    {
        [self.navigationBar setTitleTextAttributes:@{NSForegroundColorAttributeName : theme.navbarTextColour}];
        self.navigationBar.opaque = YES;
        self.navigationBar.tintColor = theme.navbarTextColour;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

@end
