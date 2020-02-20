//
//  AIRootController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/06.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIRootController.h"

@interface AIRootController ()
@property (nonatomic, readonly) int menuIndex;
@property (nonatomic, readonly) AIRootViewmodel *vm;
@end

@implementation AIRootController

- (instancetype)initWithTheme:(AITheme *)theme
                           vm:(AIRootViewmodel *)vm
                    menuIndex:(int)index
                        title:(NSString *)title
                         icon:(NSString *)icon
{
    if (self = [super initWithTheme:theme])
    {
        _vm = vm;
        _menuIndex = index;
        
        self.title = title;
        self.tabBarItem.image = [[UIImage imageNamed:icon] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

@end
