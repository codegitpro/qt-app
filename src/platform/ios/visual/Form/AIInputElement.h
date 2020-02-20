//
//  AIInputElement.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/11/04.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AIInputElement <NSObject>
@property (nonatomic, readonly) int group;
@property (nonatomic, readonly) int inputId;
@property (nonatomic) NSString *value;
@end

NS_ASSUME_NONNULL_END
