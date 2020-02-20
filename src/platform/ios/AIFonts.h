#import <UIKit/UIKit.h>

@interface AIFonts : NSObject

+ (AIFonts *)instance;

@property (nonatomic, readonly) UIFont *body;
@property (nonatomic, readonly) UIFont *title1;
@property (nonatomic, readonly) UIFont *title2;
@property (nonatomic, readonly) UIFont *title3;
@property (nonatomic, readonly) UIFont *callout;
@property (nonatomic, readonly) UIFont *caption1;
@property (nonatomic, readonly) UIFont *caption2;
@property (nonatomic, readonly) UIFont *footnote;
@property (nonatomic, readonly) UIFont *headline;
@property (nonatomic, readonly) UIFont *subheadline;

@end
