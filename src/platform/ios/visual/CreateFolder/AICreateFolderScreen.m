//
//  AICreateFolderScreen.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2019/02/03.
//  Copyright © 2019 AiNET. All rights reserved.
//

#import "AICreateFolderScreen.h"

@interface AICreateFolderScreen ()
@property (nonatomic, readonly) int directoryId;
@property (nonatomic, readonly) AIPortal *portal;
@end

@implementation AICreateFolderScreen

- (instancetype)initWithTheme:(AITheme *)theme directoryId:(int)directoryId portal:(AIPortal *)portal title:(NSString *)title
{
    if  (self = [super init])
    {
        _directoryId = directoryId;
        _portal = portal;
        
        NSMutableArray<AIElement *> *elements = [NSMutableArray new];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeText inputId:-1 title:@"Enter the name of the folder you want to create" value:@""]];
        [elements addObject:[AIElement elementWithGroup:-1 type:AIElementTypeTextInput inputId:-1 title:@"Folder name" value:@""]];
        [elements addObject:[AIElement elementWithGroup:0 type:AIElementTypeButton inputId:1 title:@"Create" value:@""]];
        
        AIFormViewController *form = [[AIFormViewController alloc] initForm:theme title:title width:400 height:0 action:self elements:elements];
        
        _controller = form;
    }
    return self;
}

- (void)onResult:(int32_t)value
{
    [_portal createFolder:_directoryId folderName:_controller.inputElements.firstObject.value form:_controller];
}

@end
