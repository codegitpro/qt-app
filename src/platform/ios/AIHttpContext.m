//
//  AIHttpContext.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIHttpContext.h"

@interface AIHttpContext()
@property (nonatomic) AIHttpProgressResult *progressResult;
@end

@implementation AIHttpContext

+ (instancetype)httpUploadProgress:(AIHttpProgressResult *)progress filePath:(NSString *)filePath
{
    return [[AIHttpContext alloc] initWithProgress:progress upload:YES filePath:filePath task:nil];
}

+ (instancetype)httpDownloadProgress:(AIHttpProgressResult *)progress filePath:(NSString *)filePath task:(NSURLSessionDownloadTask *)task
{
    return [[AIHttpContext alloc] initWithProgress:progress upload:NO filePath:filePath task:task];
}

- (instancetype)initRestoredDownload:(NSURLSessionDownloadTask *)task
{
    if (self = [super init])
    {
        _isUpload = NO;
        _task = task;
    }
    return self;
}

- (instancetype)initWithProgress:(AIHttpProgressResult *)progress upload:(BOOL)isUpload filePath:(NSString *)filePath task:(NSURLSessionDownloadTask *)task
{
    if (self = [super init])
    {
        _progressResult = progress;
        _isUpload = isUpload;
        _filePath = filePath;
    }
    return self;
}


- (BOOL)onProgress:(nonnull AIProgress *)at
{
    if (!_progressResult)
    {
        return NO;
    }
    [_progressResult onProgress:at];
    return YES;
}

- (BOOL)onResult:(int32_t)statusCode
         headers:(NSArray<AIHeader *> *)headers
   contentLength:(int64_t)contentLength
{
    if (!_progressResult)
    {
        _resultHeaders = headers;
        _resultStatusCode = statusCode;
        _contentLength = contentLength;
        return NO;
    }
    if (_isUpload && _filePath)
        NSLog(@"Upload file %@ not deleted", _filePath);
    [_progressResult onResult:statusCode headers:headers contentLength:contentLength];
    return YES;
}

- (void)setupResumePath:(NSString *)path progress:(AIHttpProgressResult *)progress
{
    _filePath = path;
    _progressResult = progress;
}

- (BOOL)hasProgressResult
{
    return nil != _progressResult;
}

- (void)uploadFileWasDeleted
{
    _filePath = nil;
}

@end
