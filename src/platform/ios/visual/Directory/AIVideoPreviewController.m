//
//  AIVideoPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIVideoPreviewController.h"

@interface AIVideoPreviewController()
@property (nonatomic, readonly) int fileId;
@property (nonatomic, readonly) AIPortal *portal;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *path;
@end

@implementation AIVideoPreviewController

- (instancetype)initWithFilePath:(NSString*)path title:(NSString *)title fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme
{
    if (self = [super init])
    {
        self.title = title;
        
        _fileId = fileId;
        _portal = portal;
        _theme = theme;
        _path = path;

        AVPlayerItem* playerItem = [AVPlayerItem playerItemWithURL:[NSURL fileURLWithPath:path]];
        self.player = [[AVPlayer alloc] initWithPlayerItem:playerItem];
        self.player.volume = 1.0;
        [self.player play];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    UIBarButtonItem *deleteBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgDelete style:UIBarButtonItemStylePlain target:self action:@selector(deleteFile)];
    UIBarButtonItem *saveBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSave style:UIBarButtonItemStylePlain target:self action:@selector(saveFile)];
    self.navigationItem.rightBarButtonItems = @[deleteBtn, saveBtn];
}

- (void)deleteFile
{
    if (_fileId > 0)
    {
        [_portal deleteAll:@[] fileIds:@[[NSNumber numberWithInt:_fileId]]];
        [self.navigationController popViewControllerAnimated:NO];
    }
}

- (void)saveFile
{
    __weak AIVideoPreviewController *weakself = self;
    
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        UISaveVideoAtPathToSavedPhotosAlbum(weakself.path, self, @selector(video:didFinishSavingWithError:contextInfo:), nil);
    });
}

- (void)               video: (NSString *) videoPath
    didFinishSavingWithError: (NSError *) error
                 contextInfo: (void *) contextInfo
{
    if (error)
        [_theme showMessage:@"Failed to save video" controller:self];
    else
        [_theme showMessage:[NSString stringWithFormat:@"%@ saved to library", self.title] controller:self];
}

@end
