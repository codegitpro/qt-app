//
//  AIVideoPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIVideoController.h"

@interface AIVideoController()
@property (nonatomic, readonly) AIPreViewmodel *vm;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *path;
@property (nonatomic, readonly) UIBarButtonItem *deleteBtn;
@property (nonatomic, readonly) UIBarButtonItem *saveBtn;
@end

@implementation AIVideoController

- (instancetype)initWithTitle:(NSString *)title
                           vm:(AIPreViewmodel *)vm
                        theme:(AITheme *)theme
                         path:(NSString *)path
{
    self = [super init];
    if (self)
    {
        self.title = title;
        
        _vm = vm;
        _theme = theme;
        _path = path;
        
        _deleteBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgDelete style:UIBarButtonItemStylePlain target:self action:@selector(deleteFile)];
        _saveBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSave style:UIBarButtonItemStylePlain target:self action:@selector(saveFile)];
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"AIVideoController dealloc");
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    if (self.isMovingFromParentViewController)
    {
        _vm = nil;
    }
    [self.player pause];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    UIBarButtonItem *deleteBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgDelete style:UIBarButtonItemStylePlain target:self action:@selector(deleteFile)];
    UIBarButtonItem *saveBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSave style:UIBarButtonItemStylePlain target:self action:@selector(saveFile)];
    self.navigationItem.rightBarButtonItems = @[deleteBtn, saveBtn];
    
    __weak AIVideoController *weakself = self;
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        AVPlayerItem* playerItem = [AVPlayerItem playerItemWithURL:[NSURL fileURLWithPath:weakself.path]];
        dispatch_async( dispatch_get_main_queue(), ^
        {
            weakself.player = [[AVPlayer alloc] initWithPlayerItem:playerItem];
            weakself.showsPlaybackControls = YES;
            weakself.player.volume = 1.0;
            [weakself.player play];
        });
    });
}

- (void)deleteFile
{
    [_vm fileDelete];
    [self.navigationController popViewControllerAnimated:NO];
    _vm = nil;
    [self.player pause];
}

- (void)saveFile
{
    __weak AIVideoController *weakself = self;
    
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
