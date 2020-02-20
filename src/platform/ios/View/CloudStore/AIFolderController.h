//
//  AIFolderController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/09.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIField.h"
#import "AITheme.h"
#import "AIFolderViewmodel.h"
#import "AIFormViewmodel.h"

#import "AIGenericForm.h"
#import "AIReference.h"
#import "AIInputElement.h"
#import "AIToolbarViewControllerDelegate.h"
#import "AIFolderView.h"
#import "AIFormView.h"
#import "AIThumbnails.h"
#import "IQMediaPickerController.h"
#import "AIOsIoImpl.h"


NS_ASSUME_NONNULL_BEGIN

@interface AIFolderController : UICollectionViewController<AIGenericForm, AIFolderView, AIFormView, IQMediaPickerControllerDelegate, UINavigationControllerDelegate, UIDocumentPickerDelegate>
- (instancetype)initWithTheme:(nonnull AITheme *)theme
                       formVm:(AIFormViewmodel *)formVm
                     folderVm:(AIFolderViewmodel *)folderVm
                       parent:(id<AIToolbarViewControllerDelegate>)parent
                   thumbnails:(AIThumbnails *)thumbnails
                           io:(AIOsIoImpl *)io
;
@end

NS_ASSUME_NONNULL_END
