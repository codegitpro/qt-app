//
//  AIOsIoImpl.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/25.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AIOsIo.h"
#import "AITheme.h"
#import <Photos/PHAsset.h>
#import "AIDiagnostic.h"

typedef void (^ AssetDetail)(NSString *fileName, NSString *path, NSData *thumbnail);

@interface AIOsIoImpl : NSObject<AIOsIo, AIDiagnostic, NSURLSessionDownloadDelegate, NSURLSessionTaskDelegate>
@property (nonatomic, copy) void (^savedCompletionHandler)(void);
- (instancetype)initWithTheme:(AITheme *)theme appDataLocation:(NSString *)appDataLocation;
- (void)convertAssetUrl:(NSURL *)url assetDetail:(AssetDetail)assetDetail;
- (void)convertAsset:(PHAsset *)asset assetDetail:(AssetDetail)assetDetail;
@end
