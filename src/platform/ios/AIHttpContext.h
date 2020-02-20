//
//  AIHttpContext.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AIHttpProgressResult.h"

@interface AIHttpContext : NSObject
@property (nonatomic, readonly) BOOL isUpload;
@property (nonatomic, readonly) BOOL hasProgressResult;

@property (nonatomic) NSString * _Nullable filePath;
@property (nonatomic) NSURLSessionDownloadTask * _Nullable task;
@property (nonatomic) int64_t contentLength;

@property (nonatomic) NSError * _Nullable error;
@property (nonatomic, readonly) NSArray<AIHeader *> * _Nullable resultHeaders;
@property (nonatomic, readonly) int resultStatusCode;
@property (nonatomic) NSString * _Nullable temporaryDownloadedFilePath;

- (instancetype _Nonnull )initRestoredDownload:(NSURLSessionDownloadTask *_Nonnull)task;
+ (instancetype _Nonnull )httpDownloadProgress:(AIHttpProgressResult *_Nonnull)progress filePath:(NSString*_Nonnull) filePath task:(NSURLSessionDownloadTask *_Nonnull)task;
+ (instancetype _Nonnull )httpUploadProgress:(AIHttpProgressResult *_Nonnull)progress filePath:(NSString *_Nonnull)filePath;

- (BOOL)onProgress:(nonnull AIProgress *)at;

- (BOOL)onResult:(int32_t)statusCode
         headers:(NSArray<AIHeader *> *_Nonnull)headers
   contentLength:(int64_t)contentLength;

- (void)setupResumePath:(NSString *_Nonnull)path progress:(AIHttpProgressResult *_Nonnull)progress;
- (void)uploadFileWasDeleted;
@end
