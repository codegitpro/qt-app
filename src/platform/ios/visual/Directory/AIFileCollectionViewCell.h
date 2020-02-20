//
//  AIFileCollectionViewCell.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AITheme.h"

typedef NS_ENUM(NSInteger, AIAccessoryType)
{
    AIAccessoryTypeNone,
    AIAccessoryTypeCheckmark,
    AIAccessoryTypeRightarrow
};

@interface AIFileCollectionViewCell : UICollectionViewCell
@property (nonatomic) AITheme *theme;
@property (nonatomic, readonly) UIImageView *imageView;
@property (nonatomic, readonly) UILabel *textLabel;
@property (nonatomic, readonly) UILabel *detailTextLabel;
@property (nonatomic) AIAccessoryType accessoryType;
@property (nonatomic) int fileId;
@end

