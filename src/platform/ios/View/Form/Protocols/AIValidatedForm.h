//
//  AIValidatedForm.h
//  user
//
//  Created by Hans Malherbe on 2019/05/01.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AIValidatedForm <NSObject>
- (BOOL)validate;
@end

NS_ASSUME_NONNULL_END
