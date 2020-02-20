//
//  AIDirectoryList.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIDirectoryView.h"
#import "AIPortal.h"
#import "AINameStore.h"
#import "AITheme.h"
#import "AIToolbarViewControllerDelegate.h"
#import "AIOsIoImpl.h"
#import "IQMediaPickerController.h"

NS_ASSUME_NONNULL_BEGIN

@interface AIDirectoryList : UICollectionViewController<AIDirectoryView, IQMediaPickerControllerDelegate/*, UIImagePickerControllerDelegate*/, UINavigationControllerDelegate, UICollectionViewDelegateFlowLayout, UIDocumentPickerDelegate>

- (instancetype)initWithPortal:(AIPortal*)portal
                        parent:(id<AIToolbarViewControllerDelegate>)parent
                         title:(NSString*)title
                   directoryId:(int)directoryId
                         names:(id<AINameStore>)names
                         theme:(AITheme *)theme
                            io:(AIOsIoImpl *)io
;

@end

NS_ASSUME_NONNULL_END
