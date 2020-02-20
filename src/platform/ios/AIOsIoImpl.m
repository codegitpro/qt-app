#import "AIOsIoImpl.h"
#import "AIHttpResult.h"
#import "AIBoolResult.h"
#import "AILongResult.h"
#import "AIHttpProgressResult.h"
#import "AIProgress.h"
#import <Photos/PHImageManager.h>
#import <Photos/PHAssetResource.h>
#import <Photos/PHAssetResourceManager.h>
#import "AIHttpContext.h"
#import "AIStringResult.h"
#import "AIBinaryResult.h"
#import "AIVoidResult.h"
#import "AIUrlSessionCancellationToken.h"
#import "AIStringsResult.h"
#import "AIFileType.h"
#import "BCCKeychain.h"
//#import "AICertCheck.h"

@interface AIOsIoImpl()

@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *contentPath;
@property (nonatomic, readonly) NSString *appDataLocation;
@property (nonatomic, readonly) NSURLSession *shortSession;
@property (nonatomic, readonly) NSFileManager *fileManager;
@property (nonatomic, readonly) NSMutableDictionary<NSNumber *, AIHttpContext *> *httpSessions;
@property (nonatomic, readonly) NSOperationQueue *fileQueue;

@property (nonatomic, readonly) NSURLSession *backgroundSession;
@property (nonatomic, readonly) NSMutableDictionary<NSString *, AIHttpContext *> *restoredDownloads;
@property (nonatomic) int uploadingFileIndex;
@end

@implementation AIOsIoImpl

- (instancetype)initWithTheme:(AITheme *)theme appDataLocation:(NSString *)appDataLocation
{
    self = [super init];
    if (self)
    {
        _theme = theme;
        _appDataLocation = appDataLocation;
        _contentPath = [appDataLocation stringByAppendingString:@"/cache/content"];
        _shortSession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration ephemeralSessionConfiguration] delegate:nil delegateQueue:nil];
//        _shortSession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration ephemeralSessionConfiguration] delegate:[AICertCheck new] delegateQueue:nil];
        _fileManager = [NSFileManager new];
        _httpSessions = [NSMutableDictionary new];
        _fileQueue = [NSOperationQueue new];
        _fileQueue.maxConcurrentOperationCount = 1;
        _restoredDownloads = [NSMutableDictionary new];
        _uploadingFileIndex = 0;
        
        {
            NSURLSessionConfiguration * config = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:@"com.blomp.backgroundsession"];
            _backgroundSession = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil];
            __weak AIOsIoImpl *weakself = self;
            [_backgroundSession getTasksWithCompletionHandler:^(NSArray<NSURLSessionDataTask *> * _Nonnull dataTasks, NSArray<NSURLSessionUploadTask *> * _Nonnull uploadTasks, NSArray<NSURLSessionDownloadTask *> * _Nonnull downloadTasks)
            {
                for (NSURLSessionDownloadTask *task in downloadTasks)
                {
                    AIHttpContext *context = [[AIHttpContext alloc] initRestoredDownload:task];
                    NSNumber *taskId = [NSNumber numberWithUnsignedInteger:task.taskIdentifier];
                    weakself.httpSessions[taskId] = context;
                    if (task.originalRequest)
                    {
                        NSString *url = task.originalRequest.URL.absoluteString;
                        NSLog(@"--> restored download %@", url);
                        weakself.restoredDownloads[url] = context;
                    }
                    [task resume];
                }
            }];
        }
    }
    return self;
}

- (void)log:(AILogType)type line:(int32_t)line file:(NSString *)file message:(NSString *)message
{
    NSString *prefix = nil;
    switch (type)
    {
        case AILogTypeLverbose: prefix = @"v"; break;
        case AILogTypeLdebug: prefix = @"d"; break;
        case AILogTypeLinfo: prefix = @"i"; break;
        case AILogTypeLwarning: prefix = @"w"; break;
        default: prefix = @"e"; break;
    }
    NSLog(@"%@ *%@*%@(%d)", message, prefix, file, line);
}

- (void)logLines:(AILogType)type line:(int32_t)line file:(nonnull NSString *)file messages:(nonnull NSArray<NSString *> *)messages
{
    NSString *prefix = nil;
    switch (type)
    {
        case AILogTypeLverbose: prefix = @"v"; break;
        case AILogTypeLdebug: prefix = @"d"; break;
        case AILogTypeLinfo: prefix = @"i"; break;
        case AILogTypeLwarning: prefix = @"w"; break;
        default: prefix = @"e"; break;
    }
    NSLog(@"%@ *%@*%@(%d)", [messages componentsJoinedByString:@"\n"], prefix, file, line);

}


+ (NSArray<AIHeader *> *)convert:(NSDictionary *)allHeaderFields
{
    NSMutableArray<AIHeader *>* hh = [NSMutableArray arrayWithCapacity:allHeaderFields.count];
    for (id name in allHeaderFields)
    {
        [hh addObject:[AIHeader headerWithName:name value:allHeaderFields[name]]];
    }
    return hh;
}

+ (NSString*) verbToString:(AIHttpVerb)verb
{
    switch (verb) {
        case AIHttpVerbGet: return @"GET";
        case AIHttpVerbPut: return @"PUT";
        case AIHttpVerbPost: return @"POST";
        case AIHttpVerbPatch: return @"PATCH";
        case AIHttpVerbRemove: return @"DELETE";
        case AIHttpVerbOptions: return @"OPTIONS";
    }
    return nil;
}

- (NSData *)createThumbnail:(NSData *)data
{
    CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef)data, nil);
    
    {
        CFDictionaryRef options = (__bridge CFDictionaryRef)[NSDictionary dictionaryWithObjectsAndKeys:
                                                             (id)kCFBooleanTrue, (id)kCGImageSourceCreateThumbnailWithTransform,
                                                             (id)kCFBooleanTrue, (id)kCGImageSourceCreateThumbnailFromImageIfAbsent,
                                                             (id)@(_theme.thumbnailPx), (id)kCGImageSourceThumbnailMaxPixelSize,
                                                             nil];
        CGImageRef imgRef = CGImageSourceCreateThumbnailAtIndex(imageSource, 0, options);
        UIImage *thumb = [UIImage imageWithCGImage:imgRef];
        NSData *r = UIImageJPEGRepresentation(thumb, 1);
        return r;
    }
}

- (BOOL)fileThumbnail:(NSString *)path type:(AIFileType)type result:(AIBinaryResult *)result
{
    switch (type)
    {
        case AIFileTypeImage:
        {
            [_fileQueue addOperationWithBlock:^{
                NSData *data = [NSData dataWithContentsOfFile: path];
                NSData *thumbData = [self createThumbnail:data];
                if (!thumbData || 0 == thumbData.length)
                    NSLog(@"fileThumbnail from image failed for %@", path);
                [result onResult:thumbData ? thumbData : [NSData new]];
            }];
            return YES;
        }
        case AIFileTypeVideo:
        {
            __weak AIOsIoImpl *weakself = self;
            [_fileQueue addOperationWithBlock:^{
                NSURL *contentUrl = [NSURL fileURLWithPath:path];
                AVAsset *asset = [AVURLAsset assetWithURL:contentUrl];
                [asset loadValuesAsynchronouslyForKeys:@[@"playable",@"tracks"] completionHandler:^
                 {
                     AVAssetImageGenerator *generator = [[AVAssetImageGenerator alloc] initWithAsset:asset];
                     generator.appliesPreferredTrackTransform = YES;
                     NSError *err = NULL;
                     AVKeyValueStatus status = [asset statusOfValueForKey:@"tracks" error:&err];
                     if (err)
                     {
                         NSLog(@"fileThumbnail from 1 video %@",[err localizedDescription]);
                         [result onResult:[NSData new]];
                         return;
                     }
                     if (status == AVKeyValueStatusLoaded)
                     {
                         CMTime time = CMTimeMake(1, 60);
                         CGImageRef imgRef = [generator copyCGImageAtTime:time actualTime:NULL error:&err];
                         if (err)
                         {
                             NSLog(@"fileThumbnail from 2 video %@",[err localizedDescription]);
                             [result onResult:[NSData new]];
                             return;
                         }
                         UIImage *frame = [UIImage imageWithCGImage:imgRef];
                         UIImage *thumb = [weakself.theme thumbnailFromImage:frame];
                         NSData *thumbData = UIImageJPEGRepresentation(thumb, 1);
                         [result onResult:thumbData ? thumbData : [NSData new]];
                         return;
                     }
                     NSLog(@"fileThumbnail from 4 video not loaded in time");
                     [result onResult:[NSData new]];
                 }];
            }];

            return YES;
        }
        default:
        {
            NSLog(@"fileThumbnail unsupported type %@", path);
            return NO;
        }
    };
}

- (void)                URLSession:(NSURLSession *)session
                      downloadTask:(NSURLSessionDownloadTask *)task
         didFinishDownloadingToURL:(NSURL *)location
{
    AIHttpContext* context = [_httpSessions objectForKey:[NSNumber numberWithUnsignedInteger:task.taskIdentifier]];
    if (!context)
    {
        return;
    }
    
    NSAssert(!context.isUpload, @"http context must be download");
    
    if (!task.response)
    {
        NSLog(@"[%lu] downloadTask continue", (unsigned long)task.taskIdentifier);
        return;
    }

    NSHTTPURLResponse *res = (NSHTTPURLResponse *)task.response;

    if (context.hasProgressResult)
    {
        NSError* error = nil;
        [_fileManager moveItemAtPath:[location path] toPath:context.filePath error:&error];
        if (error)
        {
            NSLog(@"[%lu] downloadTask move error %@", (unsigned long)task.taskIdentifier, [error localizedDescription]);
            [context onResult:0 headers:@[] contentLength:0];
            return;
        }

        [context onResult:(int)res.statusCode headers:[AIOsIoImpl convert:res.allHeaderFields] contentLength:context.contentLength];
        [_httpSessions removeObjectForKey:[NSNumber numberWithUnsignedInteger:task.taskIdentifier]];
        return;
    }
    
    [context onResult:(int)res.statusCode headers:[AIOsIoImpl convert:res.allHeaderFields] contentLength:context.contentLength];
    context.temporaryDownloadedFilePath = [location path];
}


- (void)URLSession:(NSURLSession *)session
      downloadTask:(NSURLSessionDownloadTask *)downloadTask
 didResumeAtOffset:(int64_t)fileOffset
expectedTotalBytes:(int64_t)expectedTotalBytes
{
}


- (void)                URLSession:(NSURLSession *)session
                      downloadTask:(NSURLSessionDownloadTask *)downloadTask
                      didWriteData:(int64_t)bytesWritten
                 totalBytesWritten:(int64_t)totalBytesWritten
         totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
    AIHttpContext* context = [_httpSessions objectForKey:[NSNumber numberWithUnsignedInteger:downloadTask.taskIdentifier]];
    if (!context)
        return;
    
    NSAssert(!context.isUpload, @"context must be download");
    
    context.contentLength = totalBytesExpectedToWrite;
    
    [context onProgress:[AIProgress progressWithCount:totalBytesWritten total:totalBytesExpectedToWrite]];
}

- (nullable id<AICancellationToken>)httpDownloadFile:(nonnull NSString *)url
                                            filePath:(nonnull NSString *)filePath
                                             headers:(nonnull NSArray<AIHeader *> *)headers
                                                size:(int64_t)size
                                                 md5:(nonnull NSString *)md5
                                              result:(nullable AIHttpProgressResult *)result
{
    AIUrlSessionCancellationToken *cancellation = [AIUrlSessionCancellationToken new];
    __weak AIOsIoImpl *weakself = self;
    
    [self scheduleFileSize:filePath result:^(unsigned long long contentLength) {
        if (contentLength > 0 && contentLength == size)
        {
            [result onResult:200 headers:@[] contentLength:size];
            return;
        }
        
        {
            AIHttpContext *context = [weakself.restoredDownloads objectForKey:url];
            if (context)
            {
                int statusCode = context.resultStatusCode;
                NSString *tmpPath = context.temporaryDownloadedFilePath;
                int64_t len = context.contentLength;
                NSUInteger taskId = context.task.taskIdentifier;
                NSError *error = context.error;
                NSArray<AIHeader *> *headers = context.resultHeaders;
                if (headers || error)
                {
                    if (headers)
                    {
                        if (tmpPath)
                        {
                            [weakself scheduleMoveFileFrom:tmpPath to:filePath result:^(NSError *error) {
                                if (error)
                                {
                                    [context onResult:0 headers:@[] contentLength:0];
                                }
                                [result onResult:statusCode headers:headers contentLength:len];
                            }];
                            return;
                        }
                        
                        [result onResult:statusCode headers:headers contentLength:len];
                    }
                    else
                        [result onResult:0 headers:@[] contentLength:0];
                    
                    [weakself.httpSessions removeObjectForKey:[NSNumber numberWithUnsignedInteger:taskId]];
                    return;
                }
                [context setupResumePath:filePath progress:result];
                cancellation.task = context.task;
                return;
            }
        }
        
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
        [headers enumerateObjectsUsingBlock:^(AIHeader * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [request setValue:obj.value forHTTPHeaderField:obj.name];
        }];
        
        NSURLSessionDownloadTask* task = [weakself.backgroundSession downloadTaskWithRequest:request];
        weakself.httpSessions[[NSNumber numberWithUnsignedInteger:task.taskIdentifier]] = [AIHttpContext httpDownloadProgress:result filePath:filePath task:task];
        [task resume];
        cancellation.task = task;
    }];
    return cancellation;
}


- (void)        URLSession:(NSURLSession *)session
                      task:(NSURLSessionTask *)task
           didSendBodyData:(int64_t)bytesSent
            totalBytesSent:(int64_t)totalBytesSent
  totalBytesExpectedToSend:(int64_t)totalBytesExpectedToSend
{
    AIHttpContext* context = [_httpSessions objectForKey:[NSNumber numberWithUnsignedInteger:task.taskIdentifier]];
    if (!context)
    {
        return;
    }
    if (!context.isUpload)
        return;
    context.contentLength = totalBytesExpectedToSend;
    [context onProgress:[AIProgress progressWithCount:totalBytesSent total:totalBytesExpectedToSend]];
}

+ (NSString *)extensionForPath:(NSString *)path
{
    return [[path substringFromIndex:[path rangeOfString:@"." options:NSBackwardsSearch].location] uppercaseString];
}

- (void)            URLSession:(NSURLSession *)session
                          task:(NSURLSessionTask *)task
          didCompleteWithError:(nullable NSError *)error
{
    NSNumber *contextId = [NSNumber numberWithUnsignedInteger:task.taskIdentifier];
    AIHttpContext* context = [_httpSessions objectForKey:contextId];
    if (!context)
    {
        return;
    }
    if (error)
    {
        NSLog(@"[%lu] didCompleteWithError error %@", (unsigned long)task.taskIdentifier, [error localizedDescription]);
        NSData *resumeData = error.userInfo[NSURLSessionDownloadTaskResumeData];
        if (resumeData)
        {
            NSURLSessionDownloadTask *newTask = [_backgroundSession downloadTaskWithResumeData:resumeData];
            [_httpSessions removeObjectForKey:contextId];
            context.task = newTask;
            if (!context.hasProgressResult)
            {
                _restoredDownloads[newTask.originalRequest.URL.absoluteString] = context;
            }
            _httpSessions[[NSNumber numberWithUnsignedInteger:newTask.taskIdentifier]] = context;
            [newTask resume];
            NSLog(@"[%lu] didCompleteWithError resumed after error new task id %lu", (unsigned long)task.taskIdentifier, (unsigned long)newTask.taskIdentifier);
            return;
        }
        if (context.hasProgressResult)
        {
            NSLog(@"[%lu] didCompleteWithError progress called after error",  (unsigned long)task.taskIdentifier);
            [context onResult:0 headers:@[] contentLength:0];
            return;
        }
        context.error = error;
        return;
    }
    
    NSAssert(task.response, @"task has no response");
    
    NSHTTPURLResponse *res = (NSHTTPURLResponse *)task.response;
    NSArray<AIHeader *> *hh = [AIOsIoImpl convert:res.allHeaderFields];
    if (context.hasProgressResult)
    {
        if (200 <= res.statusCode && res.statusCode < 300)
        {
            NSString *etag = nil;
            for (int i = 0; i < hh.count; ++i)
            {
                AIHeader *h = [hh objectAtIndex:i];
                if ([h.name isEqualToString:@"Etag"])
                {
                    etag = h.value;
                    break;
                }
            }
            if (etag)
            {
                NSString *ext = [AIOsIoImpl extensionForPath:context.filePath];
                if (etag.length > 0)
                {
                    NSString *contentPath = [_contentPath stringByAppendingPathComponent:[etag stringByAppendingString:ext]];
                    
                    __weak AIOsIoImpl *weakself;
                    [self scheduleMoveFileFrom:context.filePath to:contentPath result:^(NSError *error) {
                       if (error)
                       {
                           NSLog(@"[%lu] didCompleteWithError move fail %@ error %@", (unsigned long)task.taskIdentifier, contentPath, [error localizedDescription]);
                           [weakself immediateDeleteFile:context.filePath];
                       }
                        [context uploadFileWasDeleted];
                        [context onResult:(error ? 298 : 299) headers:hh contentLength:context.contentLength];
                        [weakself.httpSessions removeObjectForKey:contextId];
                    }];
                    return;

                }

            }
        }
        
        [context onResult:(int)res.statusCode headers:hh contentLength:context.contentLength];
        [_httpSessions removeObjectForKey:contextId];
        return;
    }
    [context onResult:(int)res.statusCode headers:hh contentLength:context.contentLength];
}

+ (NSString *)changeExtensionForFilename:(NSString *)fileName to:(NSString *)extension
{
    NSRange dotRange = [fileName rangeOfString:@"." options:NSBackwardsSearch];
    NSString *newFilename = [fileName stringByReplacingCharactersInRange:NSMakeRange(dotRange.location, fileName.length - dotRange.location) withString:extension];
    return newFilename;
}

- (BOOL)immediateDeleteFile:(NSString *)path
{
    NSError *error = nil;
    [_fileManager removeItemAtPath:path error:&error];
    if (error)
        NSLog(@"delete file fail %@ error %@", path, [error localizedDescription]);
    return nil == error;
}

- (BOOL)immediateCopyFile:(nonnull NSString *)currentPath
                 newPath:(nonnull NSString *)newPath
{
    if ([currentPath hasPrefix:_contentPath] && [newPath hasPrefix:_contentPath])
    {
        NSError *error = nil;
        [_fileManager moveItemAtPath:currentPath toPath:newPath error:&error];
        if (error && NSFileWriteFileExistsError != error.code)
            NSLog(@"copyFile move %@", [error localizedDescription]);
        return nil == error || NSFileWriteFileExistsError == error.code;
    }
    
    {
        NSError *error = nil;
        [_fileManager copyItemAtPath:currentPath toPath:newPath error:&error];
        if (error && NSFileWriteFileExistsError != error.code)
            NSLog(@"copyFile copy %@", [error localizedDescription]);
        else
            NSLog(@"copyFile copy  success %@ to %@", currentPath, newPath);
        return nil == error || NSFileWriteFileExistsError == error.code;
    }
}

- (void)scheduleCopyFile:(nonnull NSString *)currentPath
                 newPath:(nonnull NSString *)newPath
                  result:(void(^)(BOOL success))result
{
    __weak AIOsIoImpl *weakself = self;
    
    [_fileQueue addOperationWithBlock:^{
        result([weakself immediateCopyFile:currentPath newPath:newPath]);
    }];
}

- (void) scheduleMoveFileFrom:(NSString *)from to:(NSString *)to result:(void (^)(NSError *error))result
{
    __weak AIOsIoImpl *weakself = self;
    [_fileQueue addOperationWithBlock:^{
        NSError* error = nil;
        [weakself.fileManager moveItemAtPath:from toPath:to error:&error];
        if (error && NSFileWriteFileExistsError == error.code)
        {
            error = nil;
            [weakself.fileManager removeItemAtPath:from error:&error];
            if (error)
                NSLog(@"move file exist delete from %@ to %@ error %@", from, to, [error localizedDescription]);
            result(nil);
            return;
        }
        result(error);
    }];
}

- (void)copyFile:(nonnull NSString *)currentPath
         newPath:(nonnull NSString *)newPath
          result:(nullable AIBoolResult *)result
{
    [self scheduleCopyFile:currentPath newPath:newPath result:^(BOOL success) {
        [result onResult:success];
    }];
}

- (void)convertAssetUrl:(NSURL *)url assetDetail:(AssetDetail)assetDetail
{
    PHFetchResult<PHAsset *> *assets = [PHAsset fetchAssetsWithALAssetURLs:@[url] options:nil];
    PHAsset *asset = [assets firstObject];
    if (!asset)
    {
        NSLog(@"convertAssetUrl asset not found %@", url);
        return;
    }
}

- (void)convertAsset:(PHAsset *)asset assetDetail:(AssetDetail)assetDetail
{
    __weak AIOsIoImpl *weakself = self;

    PHImageManager *manager = [PHImageManager defaultManager];
    if (PHAssetMediaTypeImage == asset.mediaType && PHAssetMediaSubtypePhotoLive == asset.mediaSubtypes)
    {
        NSString *filename = [asset valueForKey:@"filename"];
        NSArray *resourcesArray = [PHAssetResource assetResourcesForAsset:asset];
        if (2 == resourcesArray.count)
        {
            PHAssetResource *videoAssetRes = resourcesArray[1];
            NSString *tmpPath = [NSString stringWithFormat:@"%@/%@.MP4", weakself.contentPath, [[NSUUID UUID] UUIDString]];
            PHAssetResourceRequestOptions *options = [PHAssetResourceRequestOptions new];
            options.networkAccessAllowed = YES;
            NSURL *fileURL = [NSURL fileURLWithPath:tmpPath];
            [PHAssetResourceManager.defaultManager writeDataForAssetResource:videoAssetRes toFile:fileURL options:options completionHandler:^(NSError * _Nullable error)
            {
                if (error)
                {
                    NSLog(@"Error writing live phone %@", error.localizedDescription);
                    return;
                }
                NSString *newFilename = [AIOsIoImpl changeExtensionForFilename:filename to:@".MP4"];

                assetDetail(newFilename, tmpPath, [NSData new]);

            }];
        }
    }
    else if (PHAssetMediaTypeImage == asset.mediaType)
    {
        PHImageRequestOptions *options = [[PHImageRequestOptions alloc] init];
        options.resizeMode = PHImageRequestOptionsResizeModeExact;
        options.deliveryMode = PHImageRequestOptionsDeliveryModeHighQualityFormat;
        
        [manager requestImageForAsset:asset
                           targetSize:CGSizeMake(asset.pixelWidth, asset.pixelHeight)
                          contentMode:PHImageContentModeDefault
                              options:options
                        resultHandler:^(UIImage * _Nullable result, NSDictionary * _Nullable info)
        {
            CGSize originalSize = result.size;
            UIImage *redrawnOriginal = [AITheme imageWithImage:result scaledToSize:originalSize];
            
            NSData *imageData = UIImageJPEGRepresentation(redrawnOriginal, 1);
            if (!imageData)
            {
                NSLog(@"convertAssetUrl jpeg generate fail");
                return;
            }

            NSString *tmpPath = [NSString stringWithFormat:@"%@/%@.JPG", weakself.contentPath, [[NSUUID UUID] UUIDString]];
            
            NSError *error = nil;
            if (![imageData writeToFile:tmpPath options:NSDataWritingAtomic error:&error])
            {
                NSLog(@"convertAssetUrl image write to tmp fail");
                return;
            }

            [manager requestImageForAsset:asset
                               targetSize:[weakself.theme scaledThumbPxWidth:asset.pixelWidth height:asset.pixelHeight]
                              contentMode:PHImageContentModeAspectFill
                                  options:options
                            resultHandler:^(UIImage * _Nullable thumb, NSDictionary * _Nullable info)
             {
                 NSString *newFilename = [AIOsIoImpl changeExtensionForFilename:[asset valueForKey:@"filename"] to:@".JPG"];
                 
                 UIImage *redrawnThumb = [AITheme imageWithImage:thumb scaledToSize:thumb.size];
                 NSData *thumbData = UIImageJPEGRepresentation(redrawnThumb, 1);
                 assetDetail(newFilename, tmpPath, thumbData ? thumbData : [NSData new]);
             }];
        }];
    }
    else if (PHAssetMediaTypeVideo == asset.mediaType)
    {
        PHVideoRequestOptions *options = [[PHVideoRequestOptions alloc] init];
        options.deliveryMode = PHVideoRequestOptionsDeliveryModeHighQualityFormat;
        options.version = PHVideoRequestOptionsVersionOriginal;
        [manager requestAVAssetForVideo:asset options:options resultHandler:^(AVAsset * _Nullable avasset, AVAudioMix * _Nullable audioMix, NSDictionary * _Nullable info)
        {
            if (!avasset)
            {
                NSLog(@"convertAssetUrl requestAVAssetForVideo fail");
                return;
            }

            AVAssetExportSession *session = [AVAssetExportSession exportSessionWithAsset:avasset presetName:AVAssetExportPresetPassthrough];
            if (!session)
            {
                NSLog(@"convertAssetUrl video export session fail");
                return;
            }

            NSString *tmpPath = [NSString stringWithFormat:@"%@/%@.MP4", weakself.contentPath, [[NSUUID UUID] UUIDString]];

            session.outputURL = [NSURL fileURLWithPath:tmpPath];
            NSLog(@"%@", [session supportedFileTypes]);
            session.outputFileType = AVFileTypeMPEG4;
            [session exportAsynchronouslyWithCompletionHandler:^
            {
                switch (session.status)
                {
                    case AVAssetExportSessionStatusCompleted:
                    {
                        NSData *thumbData = nil;
                        {
                            AVAssetImageGenerator *generator = [[AVAssetImageGenerator alloc] initWithAsset:avasset];
                            generator.appliesPreferredTrackTransform = YES;
                            NSError *err = NULL;
                            CMTime time = CMTimeMake(0, 60);
                            CGImageRef imgRef = [generator copyCGImageAtTime:time actualTime:NULL error:&err];
                            if (!err)
                            {
                                UIImage *snapshot = [[UIImage alloc] initWithCGImage:imgRef];
                                UIImage *thumb = [weakself.theme thumbnailFromImage:snapshot];
                                thumbData = UIImageJPEGRepresentation(thumb, 1);
                            }
                            else
                            {
                                NSLog(@"convertAssetUrl video thumbnail failed with error %@", [err localizedDescription]);
                            }
                        }
                        
                        NSURL *fileURL = ((AVURLAsset *)avasset).URL;
                        NSString *newFilename = [AIOsIoImpl changeExtensionForFilename:[fileURL lastPathComponent] to:@".MP4"];
                        assetDetail(newFilename, tmpPath, thumbData ? thumbData : [NSData new]);
                        break;
                    }
                    case AVAssetExportSessionStatusFailed:
                        NSLog(@"convertAssetUrl video export failed");
                        break;
                    case AVAssetExportSessionStatusCancelled:
                        NSLog(@"convertAssetUrl video export cancelled");
                        break;
                    default:
                        NSLog(@"convertAssetUrl video export unexpected status");
                        break;
                }
            }];
        }];
    }
}

- (nullable id<AICancellationToken>)httpUploadFile:(AIHttpVerb)verb
                                               url:(nonnull NSString *)url
                                          filePath:(nonnull NSString *)filePath
                                           headers:(nonnull NSArray<AIHeader *> *)headers
                                            result:(nullable AIHttpProgressResult *)result
{
    AIUrlSessionCancellationToken *cancellation = [[AIUrlSessionCancellationToken alloc] initWithUrlSession:nil];

    __weak AIOsIoImpl *weakself = self;
    
    [self scheduleFileSize:filePath
                    result:^(unsigned long long contentLength)
                    {
                        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
                        request.HTTPMethod = [AIOsIoImpl verbToString:verb];
                        if (0 == contentLength)
                        {
                            NSLog(@"err httpUploadFile content length 0 %@", filePath);
                            [result onResult:0 headers:@[] contentLength:0];
                            return;
                        }
                        
                        NSString *ext = [AIOsIoImpl extensionForPath:filePath];
                        NSString *tmpPath = [NSString stringWithFormat:@"%@/up%d%@", weakself.contentPath, weakself.uploadingFileIndex++, ext];
                        if (![weakself immediateCopyFile:filePath newPath:tmpPath])
                        {
                            NSLog(@"err httpUploadFile copy tmp fail %@", filePath);
                            [result onResult:0 headers:@[] contentLength:0];
                            return;
                        }
                        
                        [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)contentLength] forHTTPHeaderField:@"Content-Length"];
                        
                        [headers enumerateObjectsUsingBlock:^(AIHeader * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
                            [request setValue:obj.value forHTTPHeaderField:obj.name];
                        }];
                        
                        NSURLSessionUploadTask *uploadTask = [weakself.backgroundSession uploadTaskWithRequest:request fromFile:[NSURL fileURLWithPath:tmpPath]];
                        cancellation.task = uploadTask;
                        weakself.httpSessions[[NSNumber numberWithUnsignedInteger:uploadTask.taskIdentifier]] = [AIHttpContext httpUploadProgress:result filePath:tmpPath];
                        [uploadTask resume];
                    }];

    return cancellation;
}

- (void)scheduleMakePath:(nonnull NSString *)path result:(void(^)(BOOL success))result
{
    __weak AIOsIoImpl *weakself = self;
    [_fileQueue addOperationWithBlock:^
    {
        NSError* error = nil;
        [weakself.fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
        result(error ? NO : YES);
    }];
}

- (void)makePath:(nonnull NSString *)path result:(nullable AIBoolResult *)result
{
    [self scheduleMakePath:path result:^(BOOL success)
    {
        [result onResult:success];
    }];
}

- (void)scheduleRenameFile:(nonnull NSString *)currentPath newPath:(nonnull NSString *)newPath result:(void(^)(BOOL success))result
{
    __weak AIOsIoImpl *weakself = self;
    [_fileQueue addOperationWithBlock:^
    {
        NSError *error = nil;
        [weakself.fileManager moveItemAtPath:currentPath toPath:newPath error:&error];
        result(nil == error);
    }];
}

- (void)renameFile:(nonnull NSString *)currentPath newPath:(nonnull NSString *)newPath result:(nullable AIBoolResult *)result
{
    [self scheduleRenameFile:currentPath newPath:newPath result:^(BOOL success)
    {
        [result onResult:success];
    }];
}

- (void)scheduleFileSize:(NSString *)path result:(void(^)(unsigned long long size))result
{
    __weak AIOsIoImpl *weakself = self;
    [_fileQueue addOperationWithBlock:^
    {
        NSError* error = nil;
        NSDictionary<NSFileAttributeKey, id> *attrs = [weakself.fileManager attributesOfItemAtPath:path error:&error];
        result(error ? 0 : [attrs fileSize]);
    }];
}

- (void)fileSize:(NSString *)path result:(AILongResult *)result
{
    [self scheduleFileSize:path result:^(unsigned long long size)
    {
        [result onResult:size];
    }];
}

- (nullable id<AICancellationToken>)httpRequest:(AIHttpVerb)verb
                                            url:(nonnull NSString *)url
                                        headers:(nonnull NSArray<AIHeader *> *)headers
                                           body:(nonnull NSString *)body
                                         result:(nullable AIHttpResult *)result
{
    NSString* v = [AIOsIoImpl verbToString:verb];
    NSLog(@"http %@ %@", v, url);
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = v;
    [headers enumerateObjectsUsingBlock:^(AIHeader * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop)
    {
        [request setValue:obj.value forHTTPHeaderField:obj.name];
    }];
    if (body.length > 0)
        [request setHTTPBody:[body dataUsingEncoding:NSUTF8StringEncoding]];
    
    NSURLSessionTask *task = [_shortSession dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error)
    {
        NSHTTPURLResponse *res = (NSHTTPURLResponse*)response;
        if (error)
        {
            NSLog(@"http error %@ %@ %@ %ld %@", v, url, error.domain, (long)error.code, error.localizedDescription);
            [result onResult:0 headers:@[] data:@""];
            return;
        }
        
        NSString* resBody = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        if (nil == resBody)
            resBody = @"";
        [result onResult:(int)res.statusCode headers:[AIOsIoImpl convert:res.allHeaderFields] data:resBody];
    }];
    [task resume];
    return [[AIUrlSessionCancellationToken alloc] initWithUrlSession:task];
}

- (void)scheduleFileReadall:(nonnull NSString *)path
                     result:(void(^)(NSData *data))result
{
    [_fileQueue addOperationWithBlock:^
    {
        NSError *error = nil;
        NSData *data = [NSData dataWithContentsOfFile:path options:NSDataReadingUncached error:&error];
        result(error ? [NSData new] : data);
    }];
}

- (void)fileReadall:(nonnull NSString *)path
             result:(nullable AIBinaryResult *)result
{
    [self scheduleFileReadall:path result:^(NSData *data)
    {
        [result onResult:data];
    }];
}

- (void)scheduleFileWriteall:(nonnull NSString *)path
             content:(nonnull NSData *)content
              result:(void(^)(BOOL success))result
{
    [_fileQueue addOperationWithBlock:^
    {
        result([content writeToFile:path atomically:YES]);
    }];
}

- (void)fileWriteall:(nonnull NSString *)path
             content:(nonnull NSData *)content
              result:(nullable AIBoolResult *)result
{
    [self scheduleFileWriteall:path content:content result:^(BOOL success)
    {
        [result onResult:success];
    }];
}

- (void)wait:(int32_t)millis result:(nullable AIVoidResult *)result
{
//    [NSTimer scheduledTimerWithTimeInterval:(millis*1.0)/1000.0 repeats:NO block:^(NSTimer * _Nonnull timer) {
//        [result onResult];
//    }];
}

- (void)fileCopyHierarchy:(nonnull NSString *)destRootPath
        destRelativePaths:(nonnull NSArray<NSString *> *)destRelativePaths
                 srcPaths:(nonnull NSArray<NSString *> *)srcPaths
{
}


- (void)fileExpandDirectories:(nonnull NSArray<NSString *> *)paths
                       result:(nullable AIStringsResult *)result
{
    [_fileQueue addOperationWithBlock:^
    {
        [result onResult:paths];
    }];
}


- (void)fileWriteallSafely:(nonnull NSString *)path
                   content:(nonnull NSData *)content
                    result:(nullable AIBoolResult *)result
{
    __weak AIOsIoImpl *weakself = self;
    [_fileQueue addOperationWithBlock:^
    {
        NSString *backupPath = [path stringByAppendingString:@".bak"];
        BOOL success = [content writeToFile:backupPath atomically:YES];
        if (!success)
        {
            [result onResult:NO];
            return;
        }
        
        NSError *error = nil;
        [weakself.fileManager removeItemAtPath:path error:&error];

        error = nil;
        [weakself.fileManager moveItemAtPath:backupPath toPath:path error:&error];
        [result onResult:nil == error];
    }];
}

- (void)fileReadPassword:(nonnull NSString *)username result:(nullable AIStringResult *)result
{
    [_fileQueue addOperationWithBlock:^
     {
         NSError *error = nil;
         NSString *data = [BCCKeychain getPasswordStringForUsername:username andServiceName:@"blomp" error:&error];
         [result onResult:nil == error && nil != data ? data : @""];
     }];

}


- (void)fileWritePassword:(nonnull NSString *)username password:(nonnull NSString *)password result:(nullable AIBoolResult *)result
{
    [_fileQueue addOperationWithBlock:^
     {
         NSError *error = nil;
         BOOL success = [BCCKeychain storeUsername:username andPasswordString:password forServiceName:@"blomp" updateExisting:YES error:&error];
         [result onResult:success];
     }];
}

- (void)fileClearCache:(nonnull NSString *)username result:(nullable AIBoolResult *)result
{
    __weak AIOsIoImpl *weakself = self;

    [_fileQueue addOperationWithBlock:^
    {
        NSError *error = nil;
        [BCCKeychain storeUsername:username andPasswordString:@"" forServiceName:@"blomp" updateExisting:YES error:&error];
        
        NSString *cachePath = weakself.contentPath;
        NSFileManager *fileManager = weakself.fileManager;
        NSMutableArray *files = [NSMutableArray arrayWithArray:[fileManager subpathsAtPath:cachePath]];
        [files addObject:[NSString stringWithFormat:@"../../%@.bin", username]];
        [files addObject:[NSString stringWithFormat:@"../../config.bin"]];
        for (NSString *p in files)
        {
            NSString *path = [cachePath stringByAppendingPathComponent:p];
            if ([fileManager fileExistsAtPath:path] && [[NSFileManager defaultManager] isDeletableFileAtPath:path])
            {
                if (![fileManager removeItemAtPath:path error:&error]) {
                    NSLog(@"Error trying to delete %@: %@", path, error);
                }
            } else {
                NSLog(@"Can't delete %@", path);
            }
        }
        
//        [weakself makePath:weakself.contentPath result:result];
        [result onResult:YES];
    }];

}



- (void)URLSessionDidFinishEventsForBackgroundURLSession:(NSURLSession *)session
{
    if (_savedCompletionHandler)
    {
        _savedCompletionHandler();
        _savedCompletionHandler = nil;
    }
}

@end
