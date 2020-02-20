//
//  ActionResult.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIIntResult.h"

typedef void (^ IntResult)(int32_t value);

NS_ASSUME_NONNULL_BEGIN

@interface ActionResult : AIIntResult
- (instancetype)initWithResult:(IntResult)result;
@end

NS_ASSUME_NONNULL_END
