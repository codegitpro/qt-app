//
//  AIDirCollectionViewCell.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIDirCollectionViewCell.h"
#import "PureLayout.h"

@interface AIDirCollectionViewCell()
@property (nonatomic, readonly) UIImageView *accessoryImageView;
@property (nonatomic) BOOL didUpdateConstraints;
@property (nonatomic, readonly) UIView *hr;
@end

@implementation AIDirCollectionViewCell

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        _didUpdateConstraints = NO;
        _textLabel = [UILabel newAutoLayoutView];
        [self.contentView addSubview:_textLabel];
        
        _accessoryImageView = [UIImageView newAutoLayoutView];
        [self.contentView addSubview:_accessoryImageView];
        _accessoryImageView.contentMode = UIViewContentModeCenter;
        
        _hr = [UIView newAutoLayoutView];
        [self.contentView addSubview:_hr];
    }
    return self;
}

+ (BOOL)requiresConstraintBasedLayout
{
    return YES;
}

- (void)updateConstraints
{
    if (!_didUpdateConstraints)
    {
        _didUpdateConstraints = YES;
        
        [_hr autoPinEdgeToSuperviewEdge:ALEdgeLeading];
        [_hr autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
        [_hr autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        [_hr autoSetDimension:ALDimensionHeight toSize:1];

        [_textLabel autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_accessoryImageView autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_textLabel autoPinEdge:ALEdgeBottom toEdge:ALEdgeTop ofView:_hr];
        [_accessoryImageView autoPinEdge:ALEdgeBottom toEdge:ALEdgeTop ofView:_hr];
        
        [_textLabel autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:_theme.margins];
        [_textLabel autoPinEdge:ALEdgeTrailing toEdge:ALEdgeLeading ofView:_accessoryImageView];
        [_accessoryImageView autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];
        
        [_accessoryImageView autoSetDimension:ALDimensionWidth toSize:24 relation:NSLayoutRelationLessThanOrEqual];

    }
    [super updateConstraints];
}

- (void)setTheme:(AITheme *)theme
{
    _theme = theme;
    _textLabel.font = theme.listTitleFont;
    _textLabel.textColor = theme.listTitleColour;
    _hr.backgroundColor = theme.listRuleColour;
}

- (void)setAccessoryType:(AIAccessoryType)accessoryType
{
    _accessoryType = accessoryType;
    if (AIAccessoryTypeCheckmark == accessoryType)
        _accessoryImageView.image = [UIImage imageNamed:@"checkmark"];
    else
        _accessoryImageView.image = [UIImage imageNamed:@"chevron-right"];
}

@end
