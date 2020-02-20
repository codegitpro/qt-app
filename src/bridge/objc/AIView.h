// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import <Foundation/Foundation.h>
@class AIFolderViewmodel;
@class AIFormViewmodel;
@class AIStoreViewmodel;


@protocol AIView

- (void)createMainMenu:(nullable AIFormViewmodel *)rootFormVm
          rootFolderVm:(nullable AIFolderViewmodel *)rootFolderVm
                userVm:(nullable AIFormViewmodel *)userVm
                planVm:(nullable AIFormViewmodel *)planVm
            settingsVm:(nullable AIFormViewmodel *)settingsVm;

- (void)selectMenu:(int32_t)index;

- (void)showMenu:(BOOL)visible;

- (void)sendInvite;

- (void)pushForm:(nullable AIFormViewmodel *)vm;

- (void)replaceForm:(nullable AIFormViewmodel *)vm;

- (void)copyToClipboard:(nonnull NSString *)content;

- (void)openLinkInBrowser:(nonnull NSString *)url;

- (void)registerStoreVm:(nullable AIStoreViewmodel *)vm;

- (BOOL)isAuthorizedForPayments;

- (void)fetchProducts:(nonnull NSArray<NSString *> *)productIds;

- (void)subscriptionPurchase:(nonnull NSString *)email
                   productId:(nonnull NSString *)productId;

- (void)subscriptionRestore:(nonnull NSString *)email;

- (void)subscriptionCancel:(nonnull NSString *)productId;

- (nonnull NSString *)subscriptionReceipt:(nonnull NSString *)productId;

- (void)subscriptionManage;

@end