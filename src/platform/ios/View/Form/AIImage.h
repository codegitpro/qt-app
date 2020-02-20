//
//  AIImage.h
//  user
//
//  Created by Hans Malherbe on 2019/05/18.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIAlign.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIImage : UIImageView

- (instancetype)initWithImage:(UIImage *)image align:(AIAlign)align;
@end

NS_ASSUME_NONNULL_END
