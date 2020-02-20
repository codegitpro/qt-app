//
//  AIPhotoPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIPhotoPreviewController.h"
#import "PureLayout.h"

@interface AIPhotoPreviewController ()
@property (nonatomic, readonly) int fileId;
@property (nonatomic, readonly) AIPortal *portal;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *path;
@end

@implementation AIPhotoPreviewController

- (instancetype)initWithFilePath:(NSString *)path title:(NSString *)title fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme
{
    self = [super init];
    if (self)
    {
        self.title = title;
        
        _fileId = fileId;
        _portal = portal;
        _theme = theme;
        _path = path;
    }
    return self;
}

+ (UIImage *)imageWithImage:(UIImage *)image scaledToSize:(CGSize)newSize
{
    UIGraphicsBeginImageContextWithOptions(newSize, NO, 0.0);
    [image drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    UIImageView *imgView = [UIImageView newAutoLayoutView];
    [self.view addSubview:imgView];
    imgView.contentMode = UIViewContentModeScaleAspectFit;
    [imgView autoPinEdgesToSuperviewEdges];

    __weak AIPhotoPreviewController *weakself = self;
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        UIImage *img = [UIImage imageWithContentsOfFile:weakself.path];
        CGFloat w = [UIScreen mainScreen].bounds.size.width;
        CGFloat h = img.size.height*w/img.size.width;
        UIImage *scaledImg = [AIPhotoPreviewController imageWithImage:img scaledToSize:CGSizeMake(w, h)];
        dispatch_async( dispatch_get_main_queue(), ^
        {
            imgView.image = scaledImg;
        });
    });
    
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
    __weak AIPhotoPreviewController *weakself = self;

    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        UIImage *img = [UIImage imageWithContentsOfFile:weakself.path];
        UIImageWriteToSavedPhotosAlbum(img, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
    });
}

- (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if (error)
        [_theme showMessage:@"Failed to save image" controller:self];
    else
        [_theme showMessage:[NSString stringWithFormat:@"%@ saved to library", self.title] controller:self];
}

@end
