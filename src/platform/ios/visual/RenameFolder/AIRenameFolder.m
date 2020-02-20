//
//  AIRenameFolder.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/03/25.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AIRenameFolder.h"

@interface AIRenameFolder()
@property (nonatomic, readonly) int folderId;
@property (nonatomic, readonly) AIPortal *portal;
@end

@implementation AIRenameFolder

- (instancetype)initWithTheme:(AITheme *)theme folderId:(int)folderId folderName:(NSString *)folderName portal:(AIPortal *)portal
{
    if  (self = [super init])
    {
        _folderId = folderId;
        _portal = portal;
        
        NSMutableArray<AIElement *> *elements = [NSMutableArray new];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Change the name of the folder and tap the Rename button." value:@""]];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeTextInput inputId:-1 title:@"Folder name" value:folderName]];
        [elements addObject:[AIElement elementWithGroup:1 type:AIElementTypeButton inputId:1 title:@"Rename" value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:2 title:@"Cancel" value:@""]];
        
        AIFormViewController *form = [[AIFormViewController alloc] initForm:theme title:@"Rename Folder" width:400 height:0 action:self elements:elements];
        
        _controller = form;
    }
    return self;
}

- (void)onResult:(int32_t)value
{
    if (1 == value)
        [_portal renameFolder:_folderId newName:_controller.inputElements.firstObject.value form:_controller];
    else
        [_controller.navigationController popViewControllerAnimated:YES];
}

@end
