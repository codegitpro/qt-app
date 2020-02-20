#import <UIKit/UIKit.h>
#import "AIOsForm.h"
#import "AIIntResult.h"
#import "AIElement.h"
#import "AITheme.h"
#import "AIInputElement.h"

@interface AIFormViewController : UIViewController<AIOsForm>

@property (nonatomic) CGFloat bottomSpace;

@property (nonatomic, readonly) NSArray<id<AIInputElement>> *inputElements;
- (instancetype)initForm:(AITheme *)theme
                   title:(NSString *)title
                   width:(int32_t)width
                  height:(int32_t)height
                  action:(AIIntResult *)action
                elements:(NSArray<AIElement *> *)elements
;
@end
