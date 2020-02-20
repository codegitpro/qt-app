#import <UIKit/UIKit.h>
#import "AIToolbarViewControllerDelegate.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIToolbarViewController : UIViewController<AIToolbarViewControllerDelegate>
@property (nonatomic) BOOL tabbarHidden;
@property (nonatomic) BOOL toolbarHidden;
- (void)setViewController:(UIViewController *)controller;
@end

NS_ASSUME_NONNULL_END
