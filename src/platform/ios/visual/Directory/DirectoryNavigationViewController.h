//
//  DirectoryNavigationViewController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/21.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIPortal.h"
#import "AINameStore.h"
#import "AITheme.h"
#import "AIOsIoImpl.h"

@interface DirectoryNavigationViewController : UINavigationController

- (instancetype) initWithPortal:(AIPortal*)portal
                          names:(id<AINameStore>)names
                          theme:(AITheme *)theme
                             io:(AIOsIoImpl *)io
;

@end
