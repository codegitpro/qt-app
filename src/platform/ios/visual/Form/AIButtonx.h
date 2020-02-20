#import <UIKit/UIKit.h>
#import "AIElement.h"
#import "AITheme.h"
#import "AIInteractiveElement.h"
#import "AIIntResult.h"

NS_ASSUME_NONNULL_BEGIN

typedef void (^ OnClick)(int);

@interface AIButtonx : UIView<AIInteractiveElement>
@property (nonatomic, readonly) int group;
@property (nonatomic, readonly) int inputId;
- (instancetype)initWithTheme:(AITheme *)theme element:(AIElement *)element action:(OnClick)action;
@end

NS_ASSUME_NONNULL_END
