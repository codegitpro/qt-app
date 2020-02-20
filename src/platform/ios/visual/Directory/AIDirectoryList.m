#import "AIDirectoryList.h"
#import "AIDirectoriesDetail.h"
#import "AIFilesDetailView.h"
#import "AIFileCollectionViewCell.h"
#import "AIDirCollectionViewCell.h"
#import "AIPhotoPreviewController.h"
#import "AITextPreviewController.h"
#import "AIFilePreviewController.h"
#import "AIInputFilenameViewController.h"
#import <Photos/PHAsset.h>
#import <Photos/PHImageManager.h>
#import "AIToolbarViewController.h"
#import "AICreateFolderScreen.h"
#import "AIRenameFile.h"
#import "AIRenameFolder.h"
#import <MediaPlayer/MPMediaEntity.h>
#import <MediaPlayer/MediaPlayerDefines.h>
#import <MediaPlayer/MPMediaItem.h>

@interface AIDirectoryList ()
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) AIOsIoImpl *io;
@property (nonatomic, readonly) id<AINameStore> names;

@property (nonatomic) AIPortal *portal;
@property (nonatomic) AIDirectoriesDetail *dirs;
@property (nonatomic, readonly) AIFilesDetailView *files;
@property (nonatomic) bool didLoad;
@property (atomic, readonly) int directoryId;
@property (nonatomic) NSMutableSet<NSNumber *> *fileSelections;
@property (nonatomic) NSMutableSet<NSNumber *> *folderSelections;

@property (nonatomic, readonly) UIBarButtonItem *cancelButton;
@property (nonatomic, readonly) UIBarButtonItem *downloadButton;
@property (nonatomic, readonly) UIBarButtonItem *deleteButton;
@property (nonatomic, readonly) UIBarButtonItem *renameButton;
@property (nonatomic, readonly) UIBarButtonItem *selectAllBtn;
@property (nonatomic, readonly) BOOL renameButtonSet;
@property (nonatomic, readonly) UIBarButtonItem *flex;

@property (nonatomic, readonly) id<AIToolbarViewControllerDelegate> parent;

@property (nonatomic, readonly) UICollectionViewFlowLayout *layout;

@property (nonatomic) int currentLaneWidth;

@end

@implementation AIDirectoryList

- (instancetype)initWithPortal:(AIPortal*)portal
                        parent:(id<AIToolbarViewControllerDelegate>)parent
                         title:(NSString*)title
                   directoryId:(int)directoryId
                         names:(id<AINameStore>)names
                         theme:(AITheme *)theme
                            io:(AIOsIoImpl *)io
{
    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    
    self = [super initWithCollectionViewLayout:layout];
    if (self)
    {
        _io = io;
        _layout = layout;
        _portal = portal;
        _parent = parent;
        _theme = theme;
        _didLoad = false;
        _directoryId = directoryId;
        self.title = title;
        _names = names;
        _fileSelections = [NSMutableSet new];
        _folderSelections = [NSMutableSet new];
        _files = [AIFilesDetailView new];
        _currentLaneWidth = 0;
        
        _cancelButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancelSelections)];
        _flex = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
        
        _downloadButton = [[UIBarButtonItem alloc] initWithImage:theme.imgDownload style:UIBarButtonItemStylePlain target:self action:@selector(downloadSelections)];
        
        _deleteButton = [[UIBarButtonItem alloc] initWithTitle:@"Delete" style:UIBarButtonItemStyleDone target:self action:@selector(deleteSelections)];
        _deleteButton.tintColor = theme.formErrorColour;
        
        _renameButton = [[UIBarButtonItem alloc] initWithTitle:@"Rename" style:UIBarButtonItemStyleDone target:self action:@selector(renameSelection)];

        _selectAllBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSelectAll style:UIBarButtonItemStylePlain target:self action:@selector(selectAllItems)];

        parent.tabbarHidden = YES;
        [parent setItems:@[_cancelButton, _deleteButton, _flex, _renameButton, _selectAllBtn, _downloadButton] animated:NO];
        _renameButtonSet = YES;
        
        [self.navigationController.navigationBar.titleTextAttributes setValue:theme.navbarTextColour forKey:NSForegroundColorAttributeName];
    }
    return self;
}

static NSString * const reuseDirId = @"DirCell";
static NSString * const resuseFileId = @"FileCell";

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section
{
    return _theme.listRowSpacing;
}

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section
{
    return _theme.listColumnSpacing;
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [self setItemSize:size.width];
}

- (void)setItemSize:(CGFloat)width
{
    CGFloat colSpace = _theme.listColumnSpacing;
    double laneCount = floor((width+colSpace)/(300.0+colSpace));
    double laneWidth = floor((width + (1-laneCount)*colSpace)/laneCount);
    if (_currentLaneWidth == (int)laneWidth)
        return;
    _currentLaneWidth = (int)laneWidth;
    _layout.itemSize = CGSizeMake(laneWidth, _theme.listRowHeight);
    
    [UIView performWithoutAnimation:^
    {
        [self.collectionView reloadData];
    }];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setItemSize:self.collectionView.bounds.size.width];
    
    self.collectionView.backgroundColor = _theme.listBackgroundColour;

    // Register cell classes
    [self.collectionView registerClass:[AIDirCollectionViewCell class] forCellWithReuseIdentifier:reuseDirId];
    [self.collectionView registerClass:[AIFileCollectionViewCell class] forCellWithReuseIdentifier:resuseFileId];

    NSLog(@"viewDidLoad %d", self.directoryId);
    _didLoad = true;
    self.clearsSelectionOnViewWillAppear = NO;
    
    UIBarButtonItem *photoBtn = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action:@selector(uploadPhoto)];
    UIBarButtonItem *uploadBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgUpload style:UIBarButtonItemStylePlain target:self action:@selector(uploadFile)];
    UIBarButtonItem *createFolderBtn = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(createFolder)];
    
    [_parent setRightBarButtonItems:@[createFolderBtn, uploadBtn, photoBtn]];
    
    {
        UILongPressGestureRecognizer *lpgr = [[UILongPressGestureRecognizer alloc]
                                              initWithTarget:self action:@selector(handleLongPress:)];
        lpgr.minimumPressDuration = 0.5; //seconds
        [self.collectionView addGestureRecognizer:lpgr];
    }
}

- (void)renameSelection
{
    if (1 == _fileSelections.count)
    {
        long pos = [_files findPositionWithId:_fileSelections.allObjects.firstObject.intValue];
        if (0 <= pos && pos < _files.size)
        {
            NSString *title = [_names.fileNames objectAtIndex:[_files titleAt:pos]];
            int fileId = [_files idAt:pos];
            AIRenameFile *screen = [[AIRenameFile alloc] initWithTheme:_theme fileId:fileId fileName:title portal:_portal];
            [_fileSelections removeAllObjects];
            [self.navigationController pushViewController:screen.controller animated:NO];
        }
    }
    else if (1 == _folderSelections.count)
    {
        int folderId = _folderSelections.allObjects.firstObject.intValue;
        NSArray<NSNumber *> *ids = _dirs.ids;
        for (int i = 0; i < ids.count; ++i)
        {
            if (folderId == ids[i].intValue)
            {
                NSString *title = [_names.directoryNames objectAtIndex:_dirs.titles[i].intValue];
                AIRenameFolder *screen = [[AIRenameFolder alloc] initWithTheme:_theme folderId:folderId folderName:title portal:_portal];
                [_folderSelections removeAllObjects];
                [self.navigationController pushViewController:screen.controller animated:NO];
                return;
            }
        }
    }
}

- (void)createFolder
{
    AICreateFolderScreen *screen = [[AICreateFolderScreen alloc] initWithTheme:_theme directoryId:_directoryId portal:_portal title:self.title];
    [self.navigationController pushViewController:screen.controller animated:NO];
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
    [self.portal uploadFilesOnly:_directoryId filenames:@[url.lastPathComponent] paths:@[url.path] thumbnails:@[[NSData new]]];
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
    [self.portal uploadFilesOnly:_directoryId filenames:filenames paths:paths thumbnails:data];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    NSLog(@"viewDidAppear %d", self.directoryId);
    [_portal watchDirectory:self.directoryId view:self];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self showOrHideToolbar];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    NSLog(@"viewWillDisappear %d", self.directoryId);
    [_portal unwatchDirectory:self.directoryId];
}

-(void)handleLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    if (gestureRecognizer.state == UIGestureRecognizerStateBegan)
    {
        CGPoint p = [gestureRecognizer locationInView:self.collectionView];
        NSIndexPath *indexPath = [self.collectionView indexPathForItemAtPoint:p];
        if (nil != indexPath)
        {
            [self selectRow:indexPath navigateAllowed:NO];
        }
    }
}

- (void)dealloc
{
    NSLog(@"dealloc %d", self.directoryId);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)cancelSelections
{
    _parent.toolbarHidden = YES;
    _parent.tabbarHidden = 0 != _directoryId;
    if (_folderSelections.count > 0 && _fileSelections.count > 0)
    {
        [_folderSelections removeAllObjects];
        [_fileSelections removeAllObjects];
        [UIView performWithoutAnimation:^{
            [self.collectionView reloadData];
        }];
    }
    else if (_folderSelections.count > 0)
    {
        [_folderSelections removeAllObjects];
        [UIView performWithoutAnimation:^{
            [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
        }];
    }
    else if (_fileSelections.count > 0)
    {
        [_fileSelections removeAllObjects];
        [UIView performWithoutAnimation:^{
            [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:1]];
        }];
    }
}

- (void)deleteSelections
{
    __weak AIDirectoryList *weakself = self;
    [_theme confirmWithTitle:@"Delete"
                     message:@"Are you sure you meant to delete the selection!"
                  controller:self
                    yesPress:^
    {
        [weakself.portal deleteAll:[weakself.folderSelections allObjects] fileIds:[weakself.fileSelections allObjects]];
        [weakself cancelSelections];
    }];
}

- (void)downloadSelections
{
    [_portal downloadAll:[_folderSelections allObjects] fileIds:[_fileSelections allObjects]];
    [self cancelSelections];
}

- (void)showOrHideToolbar
{
    if (_folderSelections.count > 0 || _fileSelections.count > 0)
    {
        _parent.toolbarHidden = NO;
        _parent.tabbarHidden = YES;
        
        if (_folderSelections.count + _fileSelections.count == 1)
        {
            if (!_renameButtonSet)
            {
                _renameButtonSet = YES;
                [_parent setItems:@[_cancelButton, _deleteButton, _flex, _renameButton, _selectAllBtn, _downloadButton] animated:NO];
            }
        }
        else
        {
            if (_renameButtonSet)
            {
                _renameButtonSet = NO;
                [_parent setItems:@[_cancelButton, _deleteButton, _flex, _selectAllBtn, _downloadButton] animated:NO];
            }
        }
    }
    else
    {
        _parent.toolbarHidden = YES;
        _parent.tabbarHidden = 0 != _directoryId;
    }
}

- (void)selectAllItems
{
    for (int i = 0; i < _dirs.ids.count; ++i)
    {
        int dirId = (int)[_dirs.ids objectAtIndex:i].integerValue;
        NSNumber *oDirId = [NSNumber numberWithInt:dirId];
        if (![_folderSelections containsObject:oDirId])
            [_folderSelections addObject:oDirId];
    }
    
    for (int i = 0; i < _files.ids.count; ++i)
    {
        int fileId = [_files idAt:i];
        NSNumber *oFileId = [NSNumber numberWithInt:fileId];
        if (![_fileSelections containsObject:oFileId])
            [_fileSelections addObject:oFileId];
    }
    
    [self.collectionView reloadData];
    [self showOrHideToolbar];
}

- (void)selectRow:(NSIndexPath *)indexPath navigateAllowed:(BOOL)navigateAllowed
{
    long i = indexPath.row;
    if (0 == indexPath.section)
    {
        NSAssert(0 <= i && i < _dirs.ids.count, @"Invalid directory index");
        
        int dirId = (int)[_dirs.ids objectAtIndex:i].integerValue;
        NSNumber *oDirId = [NSNumber numberWithInt:dirId];
        BOOL wasSelected = [_folderSelections containsObject:oDirId];
        if (wasSelected)
            [_folderSelections removeObject:oDirId];
        else
        {
            if (navigateAllowed && 0 == _fileSelections.count && 0 == _folderSelections.count)
            {
                int dirTitleId = (int)[_dirs.titles objectAtIndex:i].integerValue;
                NSString* dirTitle = [_names.directoryNames objectAtIndex:dirTitleId];
                
                AIToolbarViewController *toolbarController = [AIToolbarViewController new];
                UIViewController *controller = [[AIDirectoryList alloc] initWithPortal:_portal
                                                                                parent:toolbarController
                                                                                 title:dirTitle
                                                                           directoryId:dirId
                                                                                 names:_names
                                                                                 theme:_theme
                                                                                    io: _io
                                                ];
                toolbarController.title = dirTitle;
                [toolbarController setViewController:controller];
                [self.navigationController pushViewController:toolbarController animated:YES];
                return;
            }
            else
            {
                [_folderSelections addObject:oDirId];
            }
        }
    }
    else if (1 == indexPath.section)
    {
        NSAssert(0 <= i && i < _files.size, @"Invalid directory index");
        
        int fileId = [_files idAt:i];
        NSNumber *oFileId = [NSNumber numberWithInt:fileId];
        BOOL wasSelected = [_fileSelections containsObject:oFileId];
        if (wasSelected)
            [_fileSelections removeObject:oFileId];
        else
        {
            if (navigateAllowed && 0 == _fileSelections.count && 0 == _folderSelections.count)
            {
                AIFileState state = [_files stateAt:i];
                if (AIFileStateLocal == state)
                {
                    [_portal preview:fileId];
                    return;
                }
            }
            [_fileSelections addObject:oFileId];
        }
    }
    [self showOrHideToolbar];
    [self.collectionView deselectItemAtIndexPath:indexPath animated:NO];
    [UIView performWithoutAnimation:^
    {
        [self.collectionView reloadItemsAtIndexPaths:@[indexPath]];
    }];
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [self selectRow:indexPath navigateAllowed:YES];
}

+ (int)folderIdFromPath:(NSArray<NSNumber *> *)folderPath
{
    return 0 == folderPath.count ? 0 : folderPath.lastObject.intValue;
}

- (void)refreshList:(NSArray<NSNumber *> *)folderPath
               dirs:(AIDirectoriesDetail *)dirs
              files:(AIFilesDetail *)files
  newDirectoryNames:(NSArray<NSString *> *)newDirectoryNames
       newFileNames:(NSArray<NSString *> *)newFileNames
{
    id<AINameStore> names = _names;
    
    __weak AIDirectoryList* weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^
    {
        [names.directoryNames addObjectsFromArray:newDirectoryNames];
        [names.fileNames addObjectsFromArray:newFileNames];
        
        int folderId = [AIDirectoryList folderIdFromPath:folderPath];
        if (!weakSelf || weakSelf.directoryId != folderId)
            return;
        NSLog(@"refreshList %d", folderId);
        weakSelf.dirs = dirs;
        [weakSelf.files setFiles:files];
        
        {
            NSMutableSet<NSNumber *> *newSelections = [NSMutableSet new];
            NSSet<NSNumber *> *oldSelections = weakSelf.fileSelections;
            for (NSNumber *fileId in files.ids)
            {
                if ([oldSelections containsObject:fileId])
                    [newSelections addObject:fileId];
            }
            weakSelf.fileSelections = newSelections;
        }
        
        {
            NSMutableSet<NSNumber *> *newSelections = [NSMutableSet new];
            NSSet<NSNumber *> *oldSelections = weakSelf.folderSelections;
            for (NSNumber *fileId in dirs.ids)
            {
                if ([oldSelections containsObject:fileId])
                    [newSelections addObject:fileId];
            }
            weakSelf.folderSelections = newSelections;
        }
        
        if (weakSelf.didLoad)
            [UIView performWithoutAnimation:^
            {
                [weakSelf.collectionView reloadData];
            }];
    });
}

- (void)fileChanged:(nonnull NSArray<NSNumber *> *)folderPath
             fileId:(int32_t)fileId
         fileNameId:(int32_t)fileNameId
               size:(int64_t)size
         percentage:(int32_t)percentage
              state:(AIFileState)state
          thumbnail:(nonnull NSData *)thumbnail
{
    __weak AIDirectoryList* weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^
    {
        long position = [weakSelf.files findPositionWithId:fileId];
        if (-1 == position)
            return;
        NSNumber *oFileId = [NSNumber numberWithInt:fileId];
        if ([weakSelf.fileSelections containsObject:oFileId] && AIFileStateLocal != state && AIFileStateNormal != state)
            [weakSelf.fileSelections removeObject:oFileId];
        [weakSelf.files changeAt:position state:state size:size percentage:percentage thumbnail:thumbnail];
        [UIView performWithoutAnimation:^
        {
            [weakSelf.collectionView reloadItemsAtIndexPaths:@[[NSIndexPath indexPathForRow:position inSection:1]]];
        }];
    });
}

- (void)fileStatesChanged:(nonnull NSArray<NSNumber *> *)folderPath
                  fileIds:(nonnull NSArray<NSNumber *> *)fileIds
                   states:(nonnull NSArray<NSNumber *> *)states
{
    __weak AIDirectoryList* weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        int folderId = [AIDirectoryList folderIdFromPath:folderPath];
        if (!weakSelf || weakSelf.directoryId != folderId)
            return;
        NSLog(@"fileStatesChanged %d", folderId);
        NSMutableArray *newStates = [NSMutableArray arrayWithArray:states];
        [weakSelf.files setStates:newStates];
        if (weakSelf.didLoad)
            [UIView performWithoutAnimation:^
            {
                [weakSelf.collectionView reloadData];
            }];
    });
    
}

+ (void)setupFileCell:(AIFileCollectionViewCell*)cell
                theme:(AITheme *)theme
           percentage:(NSUInteger)percentage
                 size:(NSNumber*)fileSize
           isSelected:(BOOL)isSelected
                state:(AIFileState)state
            thumbnail:(NSData *)thumbnail
{
    
    switch (state)
    {
        case AIFileStateDeleting: cell.imageView.image = theme.imgDelete; break;
        case AIFileStateUploading:
            cell.imageView.image = theme.imgUpload;
            break;
        case AIFileStateDownloading: cell.imageView.image = theme.imgDownload; break;
        default:
            if (thumbnail && thumbnail.length > 0)
            {
                UIImage *image = [UIImage imageWithData:thumbnail];
                cell.imageView.image = image;
            }
            else if (AIFileStateLocal == state)
                cell.imageView.image = theme.imgCloudDone;
            else
                cell.imageView.image = nil;
            break;
    }
    
    if (isSelected)
        cell.accessoryType = AIAccessoryTypeCheckmark;
    else
        cell.accessoryType = AIAccessoryTypeNone;

    if (0 != percentage && 100 != percentage)
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%lu %%", (unsigned long)percentage];
    else
        cell.detailTextLabel.text = [AITheme formatSize:fileSize];
}

#pragma mark <UICollectionViewDataSource>

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 2;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    if (0 == section)
        return _dirs && _dirs.ids ? _dirs.ids.count : 0;
    if (1 == section)
        return _files.size;
    return 0;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (0 == indexPath.section)
    {
        if (!_dirs || !_dirs.ids || _dirs.ids.count <= indexPath.row)
            return nil;
        AIDirCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:reuseDirId forIndexPath:indexPath];
        if (!cell.theme)
            cell.theme = _theme;
        NSNumber *oDirId = _dirs.ids[indexPath.row];
        cell.accessoryType = [_folderSelections containsObject:oDirId] ? AIAccessoryTypeCheckmark : AIAccessoryTypeRightarrow;
        cell.textLabel.text = [_names.directoryNames objectAtIndex:[_dirs.titles objectAtIndex:indexPath.row].unsignedIntegerValue];
        return cell;
    }
    
    if (1 == indexPath.section)
    {
        if (_files.size <= indexPath.row)
            return nil;
        AIFileCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:resuseFileId forIndexPath:indexPath];
        if (!cell.theme)
            cell.theme = _theme;
        cell.fileId = [_files idAt:indexPath.row];
        NSString *title = [_names.fileNames objectAtIndex:[_files titleAt:indexPath.row]];
        cell.textLabel.text = title;
        NSUInteger percentage = [_files percentageAt:indexPath.row];
        AIFileState state = [_files stateAt:indexPath.row];
        long long fileSize = [_files sizeAt:indexPath.row];
        BOOL selected = [_fileSelections containsObject:[NSNumber numberWithInt:cell.fileId]];
        NSData *thumbnail = [_files thumbnailAt:indexPath.row];
        [AIDirectoryList setupFileCell:cell
                                              theme:_theme
                                         percentage:percentage
                                               size:[NSNumber numberWithLongLong:fileSize]
                                         isSelected:selected
                                              state:state
                                          thumbnail:thumbnail
         ];
        return cell;
    }
    return nil;
}

#pragma mark <UICollectionViewDelegate>

- (void)uploadPhoto
{
    IQMediaPickerController *controller = [[IQMediaPickerController alloc] initWithTheme:_theme];
    controller.delegate = self;
    
    [controller setMediaTypes:@[@(PHAssetMediaTypeAudio),@(PHAssetMediaTypeVideo),@(PHAssetMediaTypeImage)]];
    
    [self presentViewController:controller animated:YES completion:nil];
}

- (void)mediaPickerController:(IQMediaPickerController*_Nonnull)controller didFinishMedias:(IQMediaPickerSelection *_Nonnull)selection
{
    for (PHAsset *asset in selection.selectedAssets)
    {
        __weak AIDirectoryList *weakself = self;
        [_io convertAsset:asset assetDetail:^(NSString *filename, NSString *path, NSData *thumbnail)
        {
            [weakself.portal uploadFilesOnly:weakself.directoryId filenames:@[filename] paths:@[path] thumbnails:@[thumbnail]];
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

@end
