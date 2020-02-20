//
//  AIRenameFile.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/03/24.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIRenameFile.h"

@interface AIRenameFile ()
@property (nonatomic, readonly) int fileId;
@property (nonatomic, readonly) AIPortal *portal;
@end

@implementation AIRenameFile

- (instancetype)initWithTheme:(AITheme *)theme fileId:(int)fileId fileName:(NSString *)fileName portal:(AIPortal *)portal
{
    if  (self = [super init])
    {
        _fileId = fileId;
        _portal = portal;
        
        NSMutableArray<AIElement *> *elements = [NSMutableArray new];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Change the name of the file and tap the Rename button. Changing the file extension may prevent previewing the file." value:@""]];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeTextInput inputId:-1 title:@"File name" value:fileName]];
        [elements addObject:[AIElement elementWithGroup:1 type:AIElementTypeButton inputId:1 title:@"Rename" value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:2 title:@"Cancel" value:@""]];

        AIFormViewController *form = [[AIFormViewController alloc] initForm:theme title:@"Rename File" width:400 height:0 action:self elements:elements];
        
        _controller = form;
    }
    return self;
}

- (void)onResult:(int32_t)value
{
    if (1 == value)
        [_portal renameFile:_fileId newName:_controller.inputElements.firstObject.value form:_controller];
    else
        [_controller.navigationController popViewControllerAnimated:YES];
}

@end
