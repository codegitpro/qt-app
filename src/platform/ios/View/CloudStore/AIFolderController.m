//
//  AIFolderController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/06/09.
//  Copyright © 2019 AiNET. All rights reserved.
//

#define idCancelSelections  1
#define idSelectAll         2
#define idButtonBar         3
#define idButtonBarRename   4
#define idDownload          5
#define idDelete            6
#define idRename            7
#define idCreateFolder      8

#import "AIFolderController.h"
#import "AIDirCollectionViewCell.h"
#import "AIFileCollectionViewCell.h"
#import "AIToolbarController.h"
#import "AIVoidResult.h"
#import "AIPhoneFormController.h"
#import "AIPhotoController.h"
#import "AIVideoController.h"
#import "AITextController.h"

#import <MediaPlayer/MPMediaEntity.h>
#import <MediaPlayer/MediaPlayerDefines.h>
#import <MediaPlayer/MPMediaItem.h>
#import <Photos/PHAsset.h>
#import <Photos/PHImageManager.h>

//- (void)onReset:(nonnull NSArray<AIFolderItemDetail *> *)folders files:(nonnull NSArray<AIFileItemDetail *> *)files

@interface AIFolderController()
@property (nonatomic, readonly) UICollectionViewFlowLayout *layout;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly, weak) AIFormViewmodel *formVm;
@property (nonatomic, readonly, weak) AIFolderViewmodel *folderVm;
@property (nonatomic, readonly, weak) id<AIToolbarViewControllerDelegate> parent;
@property (nonatomic, readonly) int folderId;
@property (nonatomic, readonly) AIThumbnails *thumbnails;
@property (nonatomic, readonly) AIOsIoImpl *io;

@property (nonatomic, readonly) NSMutableArray<AIFolderItemDetail *> *folderItems;
@property (nonatomic, readonly) NSMutableArray<AIFileItemDetail *> *fileItems;

@property (nonatomic) int currentLaneWidth;
@property (nonatomic) int currentLaneCount;

@property (nonatomic, readonly) UIBarButtonItem *cancelButton;
@property (nonatomic, readonly) UIBarButtonItem *downloadButton;
@property (nonatomic, readonly) UIBarButtonItem *deleteButton;
@property (nonatomic, readonly) UIBarButtonItem *renameButton;
@property (nonatomic, readonly) UIBarButtonItem *selectAllBtn;
@property (nonatomic, readonly) BOOL renameButtonSet;
@property (nonatomic, readonly) UIBarButtonItem *flex;

@end

@implementation AIFolderController

- (instancetype)initWithTheme:(nonnull AITheme *)theme
                       formVm:(AIFormViewmodel *)formVm
                     folderVm:(AIFolderViewmodel *)folderVm
                       parent:(id<AIToolbarViewControllerDelegate>)parent
                   thumbnails:(AIThumbnails *)thumbnails
                           io:(AIOsIoImpl *)io
{
    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    if (self = [super initWithCollectionViewLayout:layout])
    {
        _theme = theme;
        _layout = layout;
        _formVm = formVm;
        _folderVm = folderVm;
        _parent = parent;
        _folderId = [folderVm folderId];
        _thumbnails = thumbnails;
        _io = io;
        
        _folderItems = [NSMutableArray new];
        _fileItems = [NSMutableArray new];
        
        _currentLaneCount = 0;
        _currentLaneWidth = 0;
        
        _cancelButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancelSelections)];
        _flex = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
        
        _downloadButton = [[UIBarButtonItem alloc] initWithImage:theme.imgDownload style:UIBarButtonItemStylePlain target:self action:@selector(downloadSelections)];
        
        _deleteButton = [[UIBarButtonItem alloc] initWithTitle:@"Delete" style:UIBarButtonItemStyleDone target:self action:@selector(deleteSelections)];
        _deleteButton.tintColor = theme.formErrorColour;
        
        _renameButton = [[UIBarButtonItem alloc] initWithTitle:@"Rename" style:UIBarButtonItemStyleDone target:self action:@selector(renameSelection)];
        
        _selectAllBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSelectAll style:UIBarButtonItemStylePlain target:self action:@selector(selectAllItems)];
        parent.tabbarHidden = 0 != _folderId;
        [parent setItems:@[_cancelButton, _deleteButton, _flex, _renameButton, _selectAllBtn, _downloadButton] animated:NO];
        _renameButtonSet = YES;

    }
    return self;
}

//- (void)viewWillDisappear:(BOOL)animated
//{
//    [super viewWillDisappear:animated];
//
//    if (self.isMovingFromParentViewController)
//    {
//        [_vm back];
//        _vm = nil;
//    }
//}

- (void)dealloc
{
    NSLog(@"AIFolderController dealloc");
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    _parent.tabbarHidden = 0 != _folderId;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.collectionView registerClass:[AIDirCollectionViewCell class] forCellWithReuseIdentifier:reuseDirId];
    [self.collectionView registerClass:[AIFileCollectionViewCell class] forCellWithReuseIdentifier:resuseFileId];

    self.collectionView.backgroundColor = _theme.listBackgroundColour;

    [self setItemSize:self.collectionView.bounds.size.width];
    self.clearsSelectionOnViewWillAppear = NO;
    
    UILongPressGestureRecognizer *lpgr = [[UILongPressGestureRecognizer alloc]
                                          initWithTarget:self action:@selector(handleLongPress:)];
    lpgr.minimumPressDuration = 0.5; //seconds
    [self.collectionView addGestureRecognizer:lpgr];
    [_folderVm onLoad:self];
    [_formVm onLoad:self form:self];
    
    UIBarButtonItem *photoBtn = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action:@selector(uploadPhoto)];
    UIBarButtonItem *uploadBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgUpload style:UIBarButtonItemStylePlain target:self action:@selector(uploadFile)];
    UIBarButtonItem *createFolderBtn = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(createFolder)];
    
    [_parent setRightBarButtonItems:@[createFolderBtn, uploadBtn, photoBtn]];

}

- (void)uploadFile
{
    UIDocumentPickerViewController* documentPicker = [[UIDocumentPickerViewController alloc] initWithDocumentTypes:@[@"public.data"] inMode:UIDocumentPickerModeImport];
    documentPicker.delegate = self;
    
    [self presentViewController:documentPicker animated:NO completion:^
     {
         if (@available(iOS 11.0, *))
         {
             documentPicker.allowsMultipleSelection = YES;
         }
         
     }];
}

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentAtURL:(NSURL *)url
{
    [_folderVm uploadFileOnly:url.lastPathComponent path:url.path thumbnail:[NSData new]];
}

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
    NSMutableArray *filenames = [NSMutableArray new];
    NSMutableArray *paths = [NSMutableArray new];
    NSMutableArray *data = [NSMutableArray new];
    for (int i = 0; i < urls.count; ++i)
    {
        NSURL *url = [urls objectAtIndex:i];
        [filenames addObject:url.lastPathComponent];
        [paths addObject:url.path];
        [data addObject:[NSData new]];
    }
    [_folderVm uploadFilesOnly:filenames paths:paths thumbnails:data];
}

- (void)uploadPhotoWithPermission
{
    IQMediaPickerController *controller = [[IQMediaPickerController alloc] initWithTheme:_theme];
    controller.delegate = self;
    
    [controller setMediaTypes:@[@(PHAssetMediaTypeAudio),@(PHAssetMediaTypeVideo),@(PHAssetMediaTypeImage)]];
    
    [self presentViewController:controller animated:YES completion:nil];
}

- (void)uploadPhoto
{
    switch ([PHPhotoLibrary authorizationStatus])
    {
    case PHAuthorizationStatusAuthorized: [self uploadPhotoWithPermission]; break;
    case PHAuthorizationStatusNotDetermined:
    {
        __weak AIFolderController *weakself = self;
        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status)
        {
            if (PHAuthorizationStatusAuthorized == status)
                [weakself uploadPhotoWithPermission];
        }];
        break;
    }
    default: break;
    }
}

- (void)mediaPickerController:(IQMediaPickerController*_Nonnull)controller didFinishMedias:(IQMediaPickerSelection *_Nonnull)selection
{
    for (PHAsset *asset in selection.selectedAssets)
    {
        __weak AIFolderController *weakself = self;
        [_io convertAsset:asset assetDetail:^(NSString *filename, NSString *path, NSData *thumbnail)
         {
             [weakself.folderVm uploadFileOnly:filename path:path thumbnail:thumbnail];
         }];
    }
    for (UIImage *img in selection.selectedImages)
    {
        NSLog(@"Image selected %f x %f", img.size.width, img.size.height);
        
    }
    for (NSURL *url in selection.selectedAssetsURL)
    {
        NSLog(@"URL selected %@", url.absoluteString);
    }
    for (MPMediaItem *audio in selection.selectedAudios)
    {
        NSLog(@"Audio selected %@", audio.title);
    }
}

- (void)mediaPickerControllerDidCancel:(IQMediaPickerController *_Nonnull)controller
{
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [self setItemSize:size.width];
}

-(void)handleLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    if (gestureRecognizer.state == UIGestureRecognizerStateBegan)
    {
        CGPoint p = [gestureRecognizer locationInView:self.collectionView];
        NSIndexPath *indexPath = [self.collectionView indexPathForItemAtPoint:p];
        if (nil != indexPath)
        {
            [_folderVm listAction:(int)indexPath.section row:(int)indexPath.row subIndex:1];
            [self.collectionView reloadItemsAtIndexPaths:@[indexPath]];
        }
    }
}

- (void)setItemSize:(CGFloat)width
{
    CGFloat colSpace = _theme.listColumnSpacing;
    double laneCount = floor((width+colSpace)/(300.0+colSpace));
    double laneWidth = floor((width + (1-laneCount)*colSpace)/laneCount);
    if (_currentLaneWidth == (int)laneWidth && _currentLaneCount == (int)laneCount)
        return;
    _currentLaneWidth = (int)laneWidth;
    _currentLaneCount = (int)laneCount;
    
    _layout.itemSize = CGSizeMake(laneWidth-1, _theme.listRowHeight);
    _layout.minimumLineSpacing = 0;
    _layout.minimumInteritemSpacing = 0;
    [self.view layoutIfNeeded];
    [UIView performWithoutAnimation:^
     {
         NSLog(@"setItemSize reload all");
         [self.collectionView reloadData];
     }];
}

- (void)close
{
    
}

- (void)disableActions
{
    
}

- (BOOL)enable:(int32_t)elementId enabled:(BOOL)enabled
{
    if (idButtonBar == elementId)
    {
        if (enabled)
        {
            if (_renameButtonSet)
            {
                _renameButtonSet = NO;
                [_parent setItems:@[_cancelButton, _deleteButton, _flex, _selectAllBtn, _downloadButton] animated:NO];
            }
            _parent.toolbarHidden = NO;
            _parent.tabbarHidden = YES;
        }
        else
        {
            _parent.toolbarHidden = YES;
            _parent.tabbarHidden = 0 != _folderId;
        }
        return YES;
    }
    
    if (idButtonBarRename == elementId)
    {
        if (enabled)
        {
            if (!_renameButtonSet)
            {
                _renameButtonSet = YES;
                [_parent setItems:@[_cancelButton, _deleteButton, _flex, _renameButton, _selectAllBtn, _downloadButton] animated:NO];
            }
            _parent.toolbarHidden = NO;
            _parent.tabbarHidden = YES;
        }
        else
        {
            _parent.toolbarHidden = YES;
            _parent.tabbarHidden = 0 != _folderId;
        }
    }
    return NO;
    
}

- (void)error:(int32_t)elementId message:(nonnull NSString *)message {}

- (void)inputConfigure:(int32_t)inputId config:(nonnull NSString *)config {}

- (nonnull NSString *)inputGet:(int32_t)inputId { return @""; }

- (void)inputSet:(int32_t)inputId value:(nonnull NSString *)value {}

- (void)reload:(int32_t)section row:(int32_t)row
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    if (-1 == section)
    {
        [self.collectionView reloadData];
        NSLog(@"reload all");
    }
    else if (-1 == row)
    {
        [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:section]];
        NSLog(@"reload section %d", section);
        
    }
    else
    {
        [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:section]]];
        NSLog(@"reload section %d row %d", section, row);
    }
}

- (BOOL)validate { return YES; }


- (void)cancelSelections
{
    [_formVm action:idCancelSelections];
}

- (void)selectAllItems
{
    [_formVm action:idSelectAll];
}

- (void)downloadSelections
{
    [_formVm action:idDownload];
}

- (void)deleteSelections
{
    [_formVm action:idDelete];
}

- (void)renameSelection
{
    [_formVm action:idRename];
}

- (void)createFolder
{
    [_formVm action:idCreateFolder];
}


#pragma mark <UICollectionViewDataSource>

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [_folderVm listAction:(int)indexPath.section row:(int)indexPath.row subIndex:0];
    NSLog(@"reload select");
    [self.collectionView reloadItemsAtIndexPaths:@[indexPath]];
}

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 2;
    //    return [_folderVm listNumberOfSections];
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return 0 == section ? _folderItems.count : _fileItems.count;
    //    return [_folderVm listNumberOfRowsInSection:(int)section];
}

static NSString * const reuseDirId = @"DirCell";
static NSString * const resuseFileId = @"FileCell";

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    int row = (int)indexPath.row;
    
    if (0 == indexPath.section)
    {
        AIDirCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:reuseDirId forIndexPath:indexPath];
        if (!cell.theme)
            cell.theme = _theme;
        
        //        AIFolderItemDetail *item = [_folderVm listFolderDetail:row];
        AIFolderItemDetail *item = _folderItems[row];
        cell.textLabel.text = item.title;
        cell.accessoryType = item.selected ? AIAccessoryTypeCheckmark : AIAccessoryTypeRightarrow;
        return cell;
    }
    
    if (1 == indexPath.section)
    {
        AIFileCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:resuseFileId forIndexPath:indexPath];
        if (!cell.theme)
            cell.theme = _theme;
        
        //        AIFileItemDetail *item = [_folderVm listFileDetail:row];
        AIFileItemDetail *item = _fileItems[row];
        cell.fileId = item.itemId;
        cell.textLabel.text = item.title;
        cell.accessoryType = item.selected ? AIAccessoryTypeCheckmark : AIAccessoryTypeNone;
        cell.detailTextLabel.text = item.detail;
        switch (item.state)
        {
            case AIFileStateDeleting:
                cell.imageView.image = _theme.imgDelete;
                break;
            case AIFileStateDownloading:
                cell.imageView.image = _theme.imgDownload;
                break;
            case AIFileStateUploading:
                cell.imageView.image = _theme.imgUpload;
                break;
            default:
            {
                NSData *thumbnailContent = [_folderVm listThumbnailContent:(int)indexPath.row];
                if (thumbnailContent.length > 0)
                {
                    UIImage *thumb = [_thumbnails cachedImageForId:item.itemId];
                    cell.imageView.image = thumb;
                    if (!thumb)
                    {
                        [_thumbnails createWithId:item.itemId data:thumbnailContent onImage:^(UIImage * _Nonnull image) {
                            if (cell.fileId == item.itemId)
                                cell.imageView.image = image;
                        }];
                    }
                }
                else if (AIFileStateLocal == item.state)
                    cell.imageView.image = _theme.imgCloudDone;
                else
                    cell.imageView.image = nil;
                
                break;
            }
        }
        return cell;
    }
    return nil;
}

- (void)createFolderView:(nullable AIFolderViewmodel *)folderVm formVm:(nullable AIFormViewmodel *)formVm
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    
    AIToolbarController *controller = [[AIToolbarController alloc] initWithFormVm:formVm folderVm:folderVm];
    AIFolderController *folderController = [[AIFolderController alloc] initWithTheme:_theme
                                                                              formVm:formVm
                                                                            folderVm:folderVm
                                                                              parent:controller
                                                                          thumbnails:_thumbnails
                                                                                  io:_io
                                            ];
    [controller setViewController:folderController];
    
    [self.navigationController pushViewController:controller animated:YES];
}

- (void)createPhotoPreview:(nullable AIPreViewmodel *)vm fileName:(nonnull NSString *)fileName filePath:(nonnull NSString *)filePath
{
    AIPhotoController *controller = [[AIPhotoController alloc] initWithTitle:fileName vm:vm theme:_theme path:filePath];
    [self.navigationController pushViewController:controller animated:YES];
}

- (void)createVideoPreview:(nullable AIPreViewmodel *)vm fileName:(nonnull NSString *)fileName filePath:(nonnull NSString *)filePath
{
    _parent.tabbarHidden = YES;
    AIVideoController *controller = [[AIVideoController alloc] initWithTitle:fileName vm:vm theme:_theme path:filePath];
    [self.navigationController pushViewController:controller animated:YES];
}

- (void)createTextPreview:(nullable AIPreViewmodel *)vm fileName:(nonnull NSString *)fileName filePath:(nonnull NSString *)filePath
{
    AITextController *controller = [[AITextController alloc] initWithTitle:fileName vm:vm theme:_theme path:filePath];
    [self.navigationController pushViewController:controller animated:YES];
}

- (void)onClearSelection:(nonnull NSArray<NSNumber *> *)folderIndexes fileIndexes:(nonnull NSArray<NSNumber *> *)fileIndexes
{
    NSMutableArray<NSIndexPath *> *indexPaths = [NSMutableArray new];
    for (NSNumber *index in folderIndexes)
    {
        [indexPaths addObject:[NSIndexPath indexPathForRow:index.intValue inSection:0]];
        AIFolderItemDetail *item = _folderItems[index.intValue];
        [_folderItems replaceObjectAtIndex:index.intValue withObject:[AIFolderItemDetail folderItemDetailWithItemId:item.itemId selected:NO title:item.title]];
    }
    for (NSNumber *index in fileIndexes)
    {
        [indexPaths addObject:[NSIndexPath indexPathForRow:index.intValue inSection:1]];
        AIFileItemDetail *item = _fileItems[index.intValue];
        [_fileItems replaceObjectAtIndex:index.intValue withObject:[AIFileItemDetail fileItemDetailWithItemId:item.itemId state:item.state selected:NO title:item.title detail:item.detail]];
    }
    if (indexPaths.count > 0)
        [self.collectionView reloadItemsAtIndexPaths:indexPaths];
}

- (void)onFileRemove:(int32_t)row
{
    [_fileItems removeObjectAtIndex:row];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:1]];
}

- (void)onFolderRemove:(int32_t)row
{
    [_folderItems removeObjectAtIndex:row];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
}

- (void)onReset:(nonnull NSArray<AIFolderItemDetail *> *)folders files:(nonnull NSArray<AIFileItemDetail *> *)files
{
    [_fileItems removeAllObjects];
    [_folderItems removeAllObjects];
    for (id item in folders)
    {
        [_folderItems addObject:item];
    }
    for (id item in files)
    {
        [_fileItems addObject:item];
    }
    [self.collectionView reloadData];
}

- (void)onFileDetailChange:(int32_t)row detail:(nonnull NSString *)detail
{
    AIFileItemDetail *item = _fileItems[row];
    [_fileItems replaceObjectAtIndex:row withObject:[AIFileItemDetail fileItemDetailWithItemId:item.itemId state:item.state selected:item.selected title:item.title detail:detail]];
    [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:1]]];
}

- (void)onFileStatesChange:(nonnull NSArray<AIStateChange *> *)stateChanges
{
    NSMutableArray<NSIndexPath *> *indexPaths = [NSMutableArray new];
    for (AIStateChange *stateChange in stateChanges)
    {
        [indexPaths addObject:[NSIndexPath indexPathForRow:stateChange.index inSection:1]];
        AIFileItemDetail *item = _fileItems[stateChange.index];
        [_fileItems replaceObjectAtIndex:stateChange.index withObject:[AIFileItemDetail fileItemDetailWithItemId:item.itemId state:stateChange.state selected:item.selected title:item.title detail:item.detail]];
    }
    [self.collectionView reloadItemsAtIndexPaths:indexPaths];
}

- (void)onSelectAll
{
    NSMutableArray<NSIndexPath *> *indexPaths = [NSMutableArray new];
    for (int i = 0; i < _folderItems.count; ++i)
    {
        if (_folderItems[i].selected)
            continue;
        [indexPaths addObject:[NSIndexPath indexPathForRow:i inSection:0]];
        AIFolderItemDetail *item = _folderItems[i];
        [_folderItems replaceObjectAtIndex:i withObject:[AIFolderItemDetail folderItemDetailWithItemId:item.itemId selected:YES title:item.title]];
    }
    for (int i = 0; i < _fileItems.count; ++i)
    {
        if (_fileItems[i].selected)
            continue;
        [indexPaths addObject:[NSIndexPath indexPathForRow:i inSection:1]];
        AIFileItemDetail *item = _fileItems[i];
        [_fileItems replaceObjectAtIndex:i withObject:[AIFileItemDetail fileItemDetailWithItemId:item.itemId state:item.state selected:YES title:item.title detail:item.detail]];
    }
    if (indexPaths.count > 0)
        [self.collectionView reloadItemsAtIndexPaths:indexPaths];
}

- (void)onFileChange:(int32_t)row item:(nonnull AIFileItemDetail *)item
{
    [_fileItems replaceObjectAtIndex:row withObject:item];
    [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:1]]];
}

- (void)onFileSelect:(int32_t)row selected:(BOOL)selected
{
    AIFileItemDetail *item = _fileItems[row];
    [_fileItems replaceObjectAtIndex:row
                          withObject:[AIFileItemDetail fileItemDetailWithItemId:item.itemId state:item.state selected:selected title:item.title detail:item.detail]];
    [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:1]]];
}


- (void)onFolderSelect:(int32_t)row selected:(BOOL)selected
{
    AIFolderItemDetail *item = _folderItems[row];
    [_folderItems replaceObjectAtIndex:row
                            withObject:[AIFolderItemDetail folderItemDetailWithItemId:item.itemId selected:selected title:item.title]];
    [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:0]]];
}

- (void)onFolderChange:(int32_t)row item:(nonnull AIFolderItemDetail *)item
{
    [_folderItems replaceObjectAtIndex:row withObject:item];
    [self.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:row inSection:0]]];
}

- (void)onFileAdded:(int32_t)row item:(nonnull AIFileItemDetail *)item
{
    [_fileItems insertObject:item atIndex:row];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:1]];
}

- (void)onFolderAdded:(int32_t)row item:(nonnull AIFolderItemDetail *)item
{
    [_folderItems insertObject:item atIndex:row];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
}

- (void)createForm:(nullable AIFormViewmodel *)vm
{
}

- (void)setScreenName:(nonnull NSString *)title
{
    [_parent setScreenName:title];
}

- (void)pushForm:(nullable AIFormViewmodel *)vm
{
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                  vm:vm
                                                                          references:@[]
                                                                            showMenu:YES
                                                                            isDialog:NO
                                                                            onChange:nil
                                         ];
    [self.navigationController pushViewController:controller animated:YES];
}


- (void)replaceForm:(nullable AIFormViewmodel *)vm
{
    AIPhoneFormController *controller = [[AIPhoneFormController alloc] initWithTheme:_theme
                                                                                  vm:vm
                                                                          references:@[]
                                                                            showMenu:YES
                                                                            isDialog:NO
                                                                            onChange:nil
                                         ];
//    self.navigationController.viewControllers = @[controller];
    UINavigationController *nav = self.navigationController;
    
    NSMutableArray<UIViewController *> *stack = [NSMutableArray new];
    for (int i = 0; i < nav.viewControllers.count - 1; ++i)
    {
        [stack addObject:nav.viewControllers[i]];
    }
    
    [stack addObject:controller];
    [nav setViewControllers:stack animated:NO];
}

- (void)guiThread:(nullable AIVoidResult *)cb
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [cb onResult];
    });
}

- (void)showMessage:(nonnull NSString *)message
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    [_theme showMessage:message controller:self];
}

@end
