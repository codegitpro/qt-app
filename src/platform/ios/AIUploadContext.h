//
//  AIUploadContext.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AIHttpProgressResult.h"

@interface AIUploadContext : NSObject
@property (nonatomic) int64_t contentLength;
@property (nonatomic) AIHttpProgressResult* progressResult;
@end
