#import <UIKit/UIKit.h>
#import "AIElement.h"
#import "AITheme.h"
#import "AIInteractiveElement.h"
#import "AIInputElement.h"

@interface AITextInputx : UIView<UITextFieldDelegate, AIInteractiveElement, AIInputElement>
@property (nonatomic) NSString *value;
@property (nonatomic, readonly) int inputId;
@property (nonatomic, readonly) int group;
- (instancetype)initWithTheme:(AITheme *)theme label:(NSString *)label element:(AIElement *)element;
@end
