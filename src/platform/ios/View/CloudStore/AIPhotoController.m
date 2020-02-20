//
//  AIPhotoPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIPhotoController.h"
#import "PureLayout.h"

@interface AIPhotoController ()
@property (nonatomic, readonly) AIPreViewmodel *vm;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *path;
@property (nonatomic, readonly) UIImageView *imgView;
@property (nonatomic, readonly) UIBarButtonItem *deleteBtn;
@property (nonatomic, readonly) UIBarButtonItem *saveBtn;
@end

@implementation AIPhotoController

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
        
        _imgView = [UIImageView newAutoLayoutView];
        _imgView.contentMode = UIViewContentModeScaleAspectFit;

        _deleteBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgDelete style:UIBarButtonItemStylePlain target:self action:@selector(deleteFile)];
        _saveBtn = [[UIBarButtonItem alloc] initWithImage:_theme.imgSave style:UIBarButtonItemStylePlain target:self action:@selector(saveFile)];

    }
    return self;
}

- (void)dealloc
{
    NSLog(@"AIPhotoController dealloc");
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    if (self.isMovingFromParentViewController)
    {
        _vm = nil;
    }
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
    
    [self.view addSubview:_imgView];
    [_imgView autoPinEdgesToSuperviewEdges];
    
    self.navigationItem.rightBarButtonItems = @[_deleteBtn];
    [self loadImage];
}

- (void)deleteFile
{
    [_vm fileDelete];
    [self.navigationController popViewControllerAnimated:NO];
    _vm = nil;
}

- (void)saveFile
{
    if (!_path)
        return;
    
    __weak AIPhotoController *weakself = self;
    
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

- (void)loadImage
{
    if (!_path)
        return;
    
    __weak AIPhotoController *weakself = self;
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
                   {
                       UIImage *img = [UIImage imageWithContentsOfFile:weakself.path];
                       CGFloat w = [UIScreen mainScreen].bounds.size.width;
                       CGFloat h = img.size.height*w/img.size.width;
                       UIImage *scaledImg = [AIPhotoController imageWithImage:img scaledToSize:CGSizeMake(w, h)];
                       dispatch_async( dispatch_get_main_queue(), ^
                                      {
                                          weakself.imgView.image = scaledImg;
                                      });
                   });
    self.navigationItem.rightBarButtonItems = @[_deleteBtn, _saveBtn];
}

@end
