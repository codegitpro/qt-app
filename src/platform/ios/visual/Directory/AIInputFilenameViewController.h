//
//  AIInputFilenameViewController.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/28.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^ TextDelegate)(NSString *text);

@interface AIInputFilenameViewController : UIViewController

+ (AIInputFilenameViewController *)withTextDelegate:(TextDelegate)textDelegate;

- (instancetype)initWithTextDelegate:(TextDelegate)textDelegate;

@end
