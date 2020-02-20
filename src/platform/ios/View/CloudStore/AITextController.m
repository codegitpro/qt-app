//
//  AITextPreviewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/27.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AITextController.h"
#import "PureLayout.h"

@interface AITextController ()
@property (nonatomic, readonly) AIPreViewmodel *vm;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSString *path;
@property (nonatomic, readonly) UIBarButtonItem *deleteBtn;

@property (nonatomic) NSString* content;
@end

@implementation AITextController

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
        
        _content = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"AITextController dealloc");
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    if (self.isMovingFromParentViewController)
    {
        _vm = nil;
    }
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
    [_vm fileDelete];
    [self.navigationController popViewControllerAnimated:NO];
    _vm = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
