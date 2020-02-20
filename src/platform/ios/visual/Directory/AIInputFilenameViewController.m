//
//  AIInputFilenameViewController.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/28.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIInputFilenameViewController.h"
#import "PureLayout.h"

@interface AIInputFilenameViewController ()
@property (nonatomic) TextDelegate textDelegate;
@property (nonatomic) UITextField *nameField;
@end

@implementation AIInputFilenameViewController

+ (AIInputFilenameViewController *)withTextDelegate:(TextDelegate)textDelegate
{
    return [[AIInputFilenameViewController alloc] initWithTextDelegate:textDelegate];
}

- (instancetype)initWithTextDelegate:(TextDelegate)textDelegate
{
    self = [super init];
    if (self)
    {
        _textDelegate = textDelegate;
        _nameField = [UITextField newAutoLayoutView];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor greenColor];
    [self.view addSubview:_nameField];
    _nameField.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
    _nameField.backgroundColor = [UIColor redColor];
    [_nameField autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:5];
    [_nameField autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:5];
    [_nameField autoAlignAxisToSuperviewAxis:ALAxisHorizontal];

    UILabel *label = [UILabel newAutoLayoutView];
    [self.view addSubview:label];
    label.text = @"File name:";
    [label autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:5];
    [label autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:5];
    [label autoPinEdge:ALEdgeBottom toEdge:ALEdgeTop ofView:_nameField withOffset:-10];

    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Upload" style:UIBarButtonItemStyleDone target:self action:@selector(upload)];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(cancel)];
     
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)upload
{
    _textDelegate(_nameField.text);
    [self.navigationController popViewControllerAnimated:NO];
}

- (void)cancel
{
    _textDelegate(nil);
    [self.navigationController popViewControllerAnimated:NO];
}

@end
