//
//  AIFilesDetailView.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/09/22.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AIFileState.h"
#import "AIFilesDetail.h"

@interface AIFilesDetailView : NSObject
@property (nonatomic, readonly, nonnull) NSArray<NSNumber *> *ids;
@property (nonatomic, readonly) NSUInteger size;
@property (nonatomic, nonnull) NSMutableArray<NSNumber *> *states;
- (long)findPositionWithId:(int)idd;
- (long long)sizeAt:(long)position;
- (AIFileState)stateAt:(long)position;
- (int)idAt:(long)position;
- (int)titleAt:(long)position;
- (NSUInteger)percentageAt:(long)position;
- (NSData *_Nonnull)thumbnailAt:(long)position;
- (void)setFiles:(AIFilesDetail *_Nullable)files;
- (void)changeAt:(long)position state:(AIFileState)state size:(long long)size percentage:(NSUInteger)percentage thumbnail:(NSData *_Nonnull)thumbnail;
@end
