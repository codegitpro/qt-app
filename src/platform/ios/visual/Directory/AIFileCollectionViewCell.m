//
//  AIFileCollectionViewCell.m
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/12/29.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import "AIFileCollectionViewCell.h"
#import "PureLayout.h"

@interface AIFileCollectionViewCell()
@property (nonatomic, readonly) UIImageView *accessoryImageView;
@property (nonatomic, readonly) UIView *textContainer;
@property (nonatomic, readonly) UIView *hr;

@property (nonatomic) BOOL didUpdateConstraints;
@end

@implementation AIFileCollectionViewCell

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        _didUpdateConstraints = NO;
        _hr = [UIView newAutoLayoutView];
        [self.contentView addSubview:_hr];

        _imageView = [UIImageView newAutoLayoutView];
        [self.contentView addSubview:_imageView];
        _imageView.contentMode = UIViewContentModeScaleAspectFill;
        
        _textContainer = [UIView newAutoLayoutView];
        [self.contentView addSubview:_textContainer];
        
        _textLabel = [UILabel newAutoLayoutView];
        [_textContainer addSubview:_textLabel];
        _textLabel.opaque = YES;

        _detailTextLabel = [UILabel newAutoLayoutView];
        [_textContainer addSubview:_detailTextLabel];
        _detailTextLabel.opaque = YES;

        _accessoryImageView = [UIImageView newAutoLayoutView];
        [self.contentView addSubview:_accessoryImageView];
        _accessoryImageView.contentMode = UIViewContentModeCenter;
        
        _imageView.clipsToBounds = YES;
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
        
        [_imageView autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_imageView autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        [_imageView autoPinEdgeToSuperviewEdge:ALEdgeLeading];
        [_imageView autoPinEdge:ALEdgeTrailing toEdge:ALEdgeLeading ofView:_textContainer withOffset:-_theme.listThumbnailSpacing];

        [_textContainer autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_accessoryImageView autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_textContainer autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        [_accessoryImageView autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        
        [_textContainer autoPinEdge:ALEdgeTrailing toEdge:ALEdgeLeading ofView:_accessoryImageView];
        [_accessoryImageView autoPinEdgeToSuperviewEdge:ALEdgeTrailing withInset:_theme.margins];

        [_imageView autoSetDimension:ALDimensionWidth toSize:_theme.thumbnailSize];
        [_accessoryImageView autoSetDimension:ALDimensionWidth toSize:24 relation:NSLayoutRelationLessThanOrEqual];
        
        [_textLabel autoPinEdgeToSuperviewEdge:ALEdgeLeading];
        [_textLabel autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
        [_detailTextLabel autoPinEdgeToSuperviewEdge:ALEdgeLeading];
        [_detailTextLabel autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
        
        [_textLabel autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [_textLabel autoPinEdge:ALEdgeBottom toEdge:ALEdgeTop ofView:_detailTextLabel];
        [_detailTextLabel autoPinEdgeToSuperviewEdge:ALEdgeBottom];

    }
    [super updateConstraints];
}

- (void)setTheme:(AITheme *)theme
{
    _theme = theme;
    _textLabel.font = theme.listTitleFont;
    _textLabel.textColor = theme.listTitleColour;
    _detailTextLabel.font = theme.listDetailFont;
    _detailTextLabel.textColor = theme.listDetailColour;
    _hr.backgroundColor = theme.listRuleColour;
}

- (void)setAccessoryType:(AIAccessoryType)accessoryType
{
    _accessoryType = accessoryType;
    if (AIAccessoryTypeCheckmark == accessoryType)
        _accessoryImageView.image = [UIImage imageNamed:@"checkmark"];
    else
        _accessoryImageView.image = nil;
}
@end
