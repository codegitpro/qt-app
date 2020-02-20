//
//  AIToolbarController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/12.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIToolbarViewControllerDelegate.h"
#import "AIFormViewmodel.h"
#import "AIFolderViewmodel.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIToolbarController : UIViewController<AIToolbarViewControllerDelegate>
- (instancetype)initWithFormVm:(AIFormViewmodel *)formVm
                      folderVm:(AIFolderViewmodel *)folderVm
;
@property (nonatomic) BOOL tabbarHidden;
@property (nonatomic) BOOL toolbarHidden;
- (void)setViewController:(UIViewController *)controller;
@end

NS_ASSUME_NONNULL_END
