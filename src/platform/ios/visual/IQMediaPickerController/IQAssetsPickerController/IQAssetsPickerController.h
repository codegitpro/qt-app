//
//  IQAssetsPickerController.h
//  https://github.com/hackiftekhar/IQMediaPickerController
//  Copyright (c) 2013-17 Iftekhar Qurashi.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.


#import <UIKit/UITableViewController.h>

#import "IQAlbumAssetsViewController.h"

@class PHAsset, PHCollectionList;

@protocol IQAssetsPickerControllerDelegate;

@interface IQAssetsPickerController : UITableViewController <UITableViewDataSource, UITableViewDelegate>
- (instancetype _Nullable )initWithTheme:(AITheme *_Nonnull)theme;
@property(nonatomic, strong, nullable) PHCollectionList *collectionList;

@property(nullable, weak) id<IQAssetsPickerControllerDelegate> delegate;
@property NSUInteger maximumItemCount;
@property(nonatomic, nullable) NSArray <NSNumber *> * mediaTypes;
@property(nullable) NSMutableArray<PHAsset*> *selectedItems;

//For internal use only
-(void)sendFinalSelectedAssets;

@end


@protocol IQAssetsPickerControllerDelegate <NSObject>

- (void)assetsPickerController:(IQAssetsPickerController*_Nonnull)controller didPickAssets:(NSArray<PHAsset*> *_Nonnull)assets;
- (void)assetsPickerControllerDidCancel:(IQAssetsPickerController *_Nonnull)controller;

@end
