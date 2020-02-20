#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "AIWeight.h"
#import "AISize.h"
#import "AIColour.h"
#import "AIAlign.h"

NS_ASSUME_NONNULL_BEGIN

@interface AITheme : NSObject

@property (nonatomic, readonly) UIColor *formBackground;

@property (nonatomic, readonly) UIImage *imgDownload;
@property (nonatomic, readonly) UIImage *imgUpload;
@property (nonatomic, readonly) UIImage *imgDelete;
@property (nonatomic, readonly) UIImage *imgCloudDone;
@property (nonatomic, readonly) UIImage *imgSave;
@property (nonatomic, readonly) UIImage *imgSelectAll;

@property (nonatomic, readonly) CGFloat margins;
@property (nonatomic, readonly) CGFloat textInputCorderRadius;
@property (nonatomic, readonly) UIColor *textInputBackground;
@property (nonatomic, readonly) UIFont *textInputFont;
@property (nonatomic, readonly) UIFont *textInputToplabelFont;

@property (nonatomic, readonly) UIColor *formErrorColour;

@property (nonatomic, readonly) UIColor *labelColour;
@property (nonatomic, readonly) UIFont *labelFont;

@property (nonatomic, readonly) UIColor *buttonColour;
@property (nonatomic, readonly) UIColor *buttonTextColour;
@property (nonatomic, readonly) UIFont *buttonFont;
@property (nonatomic, readonly) CGFloat buttonCornerRadius;

@property (nonatomic, readonly) int thumbnailSize;
@property (nonatomic, readonly) int thumbnailPx;

@property (nonatomic, readonly) UIColor *navbarTextColour;
@property (nonatomic, readonly) UIColor *navbarColour;
@property (nonatomic, readonly) UIColor *navbarActionColour;

@property (nonatomic, readonly) UIColor *switchColour;
@property (nonatomic, readonly) UIColor *switchLabelColour;
@property (nonatomic, readonly) UIFont *switchLabelFont;
@property (nonatomic, readonly) CGFloat switchInset;

@property (nonatomic, readonly) UIColor *tabBarBackgroundColour;
@property (nonatomic, readonly) UIColor *tabBarSelectionColour;

@property (nonatomic, readonly) UIColor *bodyTextColour;
@property (nonatomic, readonly) UIFont *bodyTextFont;

@property (nonatomic, readonly) UIFont *listTitleFont;
@property (nonatomic, readonly) UIFont *listDetailFont;
@property (nonatomic, readonly) UIColor *listTitleColour;
@property (nonatomic, readonly) UIColor *listDetailColour;
@property (nonatomic, readonly) UIColor *listBackgroundColour;
@property (nonatomic, readonly) CGFloat listRowSpacing;
@property (nonatomic, readonly) CGFloat listRowHeight;
@property (nonatomic, readonly) CGFloat listColumnSpacing;
@property (nonatomic, readonly) CGFloat listThumbnailSpacing;
@property (nonatomic, readonly) UIColor *listRuleColour;

+ (UIImage *)imageWithImage:(UIImage *)image scaledToSize:(CGSize)newSize;
- (CGSize) scaledThumbPtWidth:(CGFloat)width height:(CGFloat)height;
- (CGSize) scaledThumbPxWidth:(CGFloat)width height:(CGFloat)height;
- (UIImage *)thumbnailFromImage:(UIImage *)img;
- (void)showMessage:(NSString *)message controller:(UIViewController *)controller;
typedef void (^ YesPress)(void);
- (void)confirmWithTitle:(NSString *)title message:(NSString *)message controller:(UIViewController *)controller yesPress:(YesPress)yesPress;
+ (NSString *)formatSize:(NSNumber *)fileSize;

- (UIFont *)fontWithSize:(AISize)size weight:(AIWeight)weight;
- (UIColor *)colour:(AIColour)colour;
+ (NSTextAlignment) convertTextAlign:(AIAlign)align;
@property (nonatomic, readonly) UIColor *grayColour;
- (CGFloat)pointsForSize:(AISize)size;
+ (UIControlContentHorizontalAlignment) convertAlign:(AIAlign)align;
- (void)distributeHorizontally:(NSArray<UIView *> *)views margins:(BOOL)margins;
- (void)distributeVertically:(NSArray<UIView *> *)views margins:(BOOL)margins;
- (void)layoutHorizontal:(NSArray<UIView *> *)views margins:(BOOL)margins;
- (void)layoutVertical:(NSArray<UIView *> *)views margins:(BOOL)margins;
- (UIView *)pinToBottom:(UIView *)current fromTop:(nullable UIView *)prev margins:(BOOL)margins;
- (UIView *)pinLeftTop:(UIView *)current left:(UIView *)left top:(UIView *)top;
- (UIView *)horizontalViews:(NSArray<UIView *> *)views toParent:(UIView *)parent align:(AIAlign)align;
@end

NS_ASSUME_NONNULL_END
