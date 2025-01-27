// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIScreenType.h"
#import <Foundation/Foundation.h>
@class AIPortal;
@protocol AIDirectoryView;
@protocol AIOsForm;
@protocol AIOsGui;
@protocol AIOsIo;
@protocol AIWorker;


@interface AIPortal : NSObject

+ (nullable AIPortal *)create:(nullable id<AIWorker>)w
              appDataLocation:(nonnull NSString *)appDataLocation
                      thumbPx:(int32_t)thumbPx
                           io:(nullable id<AIOsIo>)io
                          gui:(nullable id<AIOsGui>)gui
                         type:(AIScreenType)type;

- (void)createFolder:(int32_t)parentFolderId
          folderName:(nonnull NSString *)folderName
                form:(nullable id<AIOsForm>)form;

- (void)renameFolder:(int32_t)folderId
             newName:(nonnull NSString *)newName
                form:(nullable id<AIOsForm>)form;

- (void)renameFile:(int32_t)fileId
           newName:(nonnull NSString *)newName
              form:(nullable id<AIOsForm>)form;

- (nonnull NSString *)ping;

- (void)watchDirectory:(int32_t)id
                  view:(nullable id<AIDirectoryView>)view;

- (void)unwatchDirectory:(int32_t)id;

- (void)downloadAll:(nonnull NSArray<NSNumber *> *)folderIds
            fileIds:(nonnull NSArray<NSNumber *> *)fileIds;

- (void)saveAll:(nonnull NSString *)targetFolder
      folderIds:(nonnull NSArray<NSNumber *> *)folderIds
        fileIds:(nonnull NSArray<NSNumber *> *)fileIds;

- (void)deleteAll:(nonnull NSArray<NSNumber *> *)folderIds
          fileIds:(nonnull NSArray<NSNumber *> *)fileIds;

- (void)uploadAll:(int32_t)targetFolderId
     commonFolder:(nonnull NSString *)commonFolder
relativeFilePaths:(nonnull NSArray<NSString *> *)relativeFilePaths;

- (void)uploadFilesOnly:(int32_t)targetFolderId
              filenames:(nonnull NSArray<NSString *> *)filenames
                  paths:(nonnull NSArray<NSString *> *)paths
             thumbnails:(nonnull NSArray<NSData *> *)thumbnails;

- (void)preview:(int32_t)fileId;

- (void)clearCache;

- (void)cancelNetworkOperations;

- (void)signOut;

- (void)about;

@end
