//
//  AIHelpElement.m
//  tech
//
//  Created by Hans Malherbe on 2019/05/26.
//  Copyright © 2019 DITS. All rights reserved.
//

#import "AIHelpElement.h"
#import "PureLayout.h"
#import "AIHelp.h"

@interface AIHelpElement()
@property (nonatomic) AIHelp *help;
@property (nonatomic) BOOL didSetupConstraints;
@end

@implementation AIHelpElement

- (instancetype)initWithTheme:(AITheme *)theme
                        label:(NSString *)label
               viewController:(UIViewController *)viewController
{
    if (self = [super initForAutoLayout])
    {
        _didSetupConstraints = NO;
        _help = [[AIHelp alloc] initWithText:label theme:theme viewController:viewController];
        [self addSubview:_help];
    }
    return self;
}

- (void)updateConstraints
{
    if (_didSetupConstraints)
    {
        [super updateConstraints];
        return;
    }
    _didSetupConstraints = YES;

    [_help autoSetDimension:ALDimensionWidth toSize:44];
    [_help autoPinEdgesToSuperviewEdges];
    
    [super updateConstraints];
}

@end
