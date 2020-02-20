//
//  IQMediaPickerController.m
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


#import "IQMediaPickerController.h"
#import "IQAssetsPickerController.h"
#import "IQAudioPickerController.h"

@interface IQMediaPickerController ()<IQAssetsPickerControllerDelegate,IQAudioPickerControllerDelegate,UITabBarControllerDelegate>
@property (nonatomic, readonly) AITheme *theme;

@property BOOL isFirstTimeAppearing;

@end

@implementation IQMediaPickerController

@dynamic delegate;

-(void)dealloc
{
    
}

- (instancetype)initWithTheme:(AITheme *)theme
{
    self = [super init];
    if (self)
    {
        _theme = theme;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    self.isFirstTimeAppearing = YES;
}

-(void)setMediaTypes:(NSArray<NSNumber *> *)mediaTypes
{
    _mediaTypes = [[NSMutableOrderedSet orderedSetWithArray:mediaTypes] array];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if (self.isFirstTimeAppearing)
    {
        self.isFirstTimeAppearing = NO;

        if (self.mediaTypes.count == 0)
        {
            self.mediaTypes = @[@(PHAssetMediaTypeImage)];
        }
        
        if ([self.mediaTypes containsObject:@(PHAssetMediaTypeImage)] ||
            [self.mediaTypes containsObject:@(PHAssetMediaTypeVideo)])
        {
            IQAssetsPickerController *controller = [[IQAssetsPickerController alloc] initWithTheme:_theme];
            controller.maximumItemCount = self.maximumItemCount;
            controller.delegate = self;
            controller.mediaTypes = self.mediaTypes;
            self.viewControllers = @[controller];
        }
        else if ([self.mediaTypes containsObject:@(PHAssetMediaTypeAudio)])
        {
            IQAudioPickerController *controller = [[IQAudioPickerController alloc] init];
            controller.maximumItemCount = self.maximumItemCount;
            controller.delegate = self;
            self.viewControllers = @[controller];
        }
    }
}


#pragma mark - Class methods

#pragma mark - Orientation

-(BOOL)shouldAutorotate
{
    UIViewController *topController = [self topViewController];
    
    if (topController)
    {
        return [topController shouldAutorotate];
    }
    else
    {
        return NO;
    }
}

-(UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    UIViewController *topController = [self topViewController];
    
    if (topController)
    {
        return [topController supportedInterfaceOrientations];
    }
    else
    {
        return UIInterfaceOrientationMaskAllButUpsideDown;
    }
}

#pragma mark - IQAssetsPickerControllerDelegate
-(void)assetsPickerController:(IQAssetsPickerController *)controller didPickAssets:(NSArray<PHAsset *> *)assets
{
    if ([self.delegate respondsToSelector:@selector(mediaPickerController:didFinishMedias:)])
    {
        IQMediaPickerSelection *selection = [[IQMediaPickerSelection alloc] init];
        [selection addAssets:assets];
        
        [self.delegate mediaPickerController:self didFinishMedias:selection];
    }
}

- (void)assetsPickerControllerDidCancel:(IQAssetsPickerController *)controller
{
    if ([self.delegate respondsToSelector:@selector(mediaPickerControllerDidCancel:)])
    {
        [self.delegate mediaPickerControllerDidCancel:self];
    }
}

#pragma mark - IQAudioPickerControllerDelegate
- (void)audioPickerController:(IQAudioPickerController *)mediaPicker didPickMediaItems:(NSArray<MPMediaItem*>*)mediaItems
{
    if ([self.delegate respondsToSelector:@selector(mediaPickerController:didFinishMedias:)])
    {
        IQMediaPickerSelection *selection = [[IQMediaPickerSelection alloc] init];
        [selection addAudios:mediaItems];

        [self.delegate mediaPickerController:self didFinishMedias:selection];
    }
}

- (void)audioPickerControllerDidCancel:(IQAudioPickerController *)mediaPicker
{
    if ([self.delegate respondsToSelector:@selector(mediaPickerControllerDidCancel:)])
    {
        [self.delegate mediaPickerControllerDidCancel:self];
    }
}

@end

