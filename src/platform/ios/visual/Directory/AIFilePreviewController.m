//
//  AIFilePreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIFilePreviewController.h"
#import "PureLayout.h"

@interface AIFilePreviewController ()
@property (nonatomic) NSString* path;
@property (nonatomic) int64_t size;
@property (nonatomic, readonly) int fileId;
@property (nonatomic, readonly) AIPortal *portal;
@property (nonatomic, readonly) AITheme *theme;

@end

@implementation AIFilePreviewController

- (instancetype)initWithFilePath:(NSString *)path title:(NSString *)title size:(int64_t)size fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme
{
    self = [super init];
    if (self)
    {
        self.title = title;
        
        _fileId = fileId;
        _portal = portal;
        _theme = theme;
        
        _path = path;
        _size = size;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    UITextView* warning = [UITextView newAutoLayoutView];
    [self.view addSubview:warning];
    warning.text = @"This file cannot be previewed on your device";
    warning.font = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle2];
    [warning autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [warning autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [warning autoPinEdgeToSuperviewEdge:ALEdgeTrailing];

    UILabel* sizeLabel = [UILabel newAutoLayoutView];
    [self.view addSubview:sizeLabel];
    sizeLabel.text = [NSString stringWithFormat:@"Size: %lld", _size];
    [sizeLabel autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:warning];
    [sizeLabel autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [sizeLabel autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
    
    UITextView* pathText = [UITextView newAutoLayoutView];
    [self.view addSubview:pathText];
    pathText.text = _path;
    [pathText autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:sizeLabel];
    [pathText autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [pathText autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
    [pathText autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    [pathText autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:sizeLabel];
    [pathText autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:warning];
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithImage:_theme.imgDelete style:UIBarButtonItemStylePlain target:self action:@selector(deleteFile)];
}

- (void)deleteFile
{
    [_portal deleteAll:@[] fileIds:@[[NSNumber numberWithInt:_fileId]]];
    [self.navigationController popViewControllerAnimated:NO];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
