//
//  AITextPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AITextPreviewController.h"
#import "PureLayout.h"

@interface AITextPreviewController ()
@property (nonatomic) NSString* content;
@property (nonatomic, readonly) int fileId;
@property (nonatomic, readonly) AIPortal *portal;
@property (nonatomic, readonly) AITheme *theme;

@end

@implementation AITextPreviewController

- (instancetype)initWithFilePath:(NSString *)path title:(NSString *)title fileId:(int)fileId portal:(AIPortal *)portal theme:(AITheme *)theme
{
    self = [super init];
    if (self)
    {
        self.title = title;

        _fileId = fileId;
        _portal = portal;
        _theme = theme;

        _content = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    UITextView *text = [UITextView newAutoLayoutView];
    text.text = _content;
    [self.view addSubview:text];
    [text autoPinEdgesToSuperviewEdges];
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
