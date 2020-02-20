//
//  AIAccountActions.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/28.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AIAccountActions <NSObject>
- (void)accountSignOut;
- (void)accountAbortNetworkTasks;
- (void)accountAbout;
- (void)sendInvite;
@end

NS_ASSUME_NONNULL_END
