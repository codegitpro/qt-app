//
//  AIToolbarViewControllerDelegate.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/20.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AIToolbarViewControllerDelegate <NSObject>
- (void)setScreenName:(NSString *)title;
- (void)setItems:(nullable NSArray<UIBarButtonItem *> *)items animated:(BOOL)animated;
- (void)setRightBarButtonItems:(NSArray<UIBarButtonItem *> *)rightBarButtonItems;
- (void)setTintColor:(UIColor *)tintColor;
- (void)setBarTintColor:(UIColor *)barTintColor;
@property (nonatomic) BOOL tabbarHidden;
@property (nonatomic) BOOL toolbarHidden;
@end

NS_ASSUME_NONNULL_END
