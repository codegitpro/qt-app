#import "AIOsGui.h"
#import <UIKit/UIKit.h>
#import "AITheme.h"
#import "AIPortal.h"
#import "AINameStore.h"
#import "AIAccountActions.h"
#import "AIOsIoImpl.h"
#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMessageComposeViewController.h>

@interface AILaunchController : UITabBarController<AIOsGui, AIAccountActions, MFMessageComposeViewControllerDelegate>

- (instancetype)initWithTheme:(AITheme *)theme
                        names:(id<AINameStore>)names
                           io:(AIOsIoImpl *)io
;
- (void)setPortal:(AIPortal *)portal;
@end
