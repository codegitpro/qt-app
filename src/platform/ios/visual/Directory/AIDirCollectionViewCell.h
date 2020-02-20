//
//  AIDirCollectionViewCell.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AIFileCollectionViewCell.h"
#import "AITheme.h"

@interface AIDirCollectionViewCell : UICollectionViewCell
@property (nonatomic) AITheme *theme;
@property (nonatomic, readonly) UILabel *textLabel;
@property (nonatomic) AIAccessoryType accessoryType;
@end

