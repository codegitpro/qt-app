//
//  AIHelp.m
//  tech
//
//  Created by Hans Malherbe on 2019/05/26.
//  Copyright © 2019 DITS. All rights reserved.
//

#import "AIHelp.h"

@interface AIHelp()
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly, weak) UIViewController *viewController;
@property (nonatomic, readonly) NSAttributedString *helpText;
@end
@implementation AIHelp

- (instancetype)initWithText:(NSString *)text
                       theme:(AITheme *)theme
              viewController:(UIViewController *)viewController
{
    if (self = [super initWithImage:[UIImage imageNamed:@"help"]])
    {
        _helpText = [[NSAttributedString alloc] initWithString:text attributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
        _theme = theme;
        _viewController = viewController;
        self.contentMode = UIViewContentModeCenter;
        [self setUserInteractionEnabled:YES];
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(helpTapped)];
        [self addGestureRecognizer:tap];
    }
    return self;
}

- (void)helpTapped
{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:@""
                                                                   message:@""
                                                            preferredStyle:UIAlertControllerStyleAlert];
    [alert setValue:_helpText forKey:@"attributedTitle"];
    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction * action) {}];
    
    [alert addAction:defaultAction];
    [_viewController presentViewController:alert animated:YES completion:nil];
    alert.view.tintColor = [_theme colour:AIColourPurple];
    
    UIView *subView = alert.view.subviews.firstObject;
    UIView *alertContentView = subView.subviews.firstObject;
    alertContentView.backgroundColor = _theme.formBackground;
    
    for (UIView *subSubView in alertContentView.subviews)
    {
        subSubView.backgroundColor = _theme.formBackground;
    }

}
@end
