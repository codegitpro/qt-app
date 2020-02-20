//
//  ASRatingView.m
//  AppShike
//
//  Created by yanguango on 12/19/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "AIStarRatingView.h"
#import "PureLayout.h"

@interface AIStarRatingView()
@property (nonatomic, readonly) AIAlign align;
@property (nonatomic, readonly) AITheme *theme;

@property (nonatomic) BOOL didUpdateConstraints;
@end

@implementation AIStarRatingView
@synthesize notSelectedStar = _notSelectedStar;
@synthesize selectedStar = _selectedStar;
@synthesize halfSelectedStar = _halfSelectedStar;
@synthesize canEdit = _canEdit;
@synthesize maxRating = _maxRating;
@synthesize midMargin = _midrMargin;
@synthesize leftMargin = _leftMargin;
@synthesize rightMargin = _rightMargin;
@synthesize minStarSize = _minStarSize;
@synthesize rating = _rating;
@synthesize minAllowedRating = _minAllowedRating;
@synthesize maxAllowedRating = _maxAllowedRating;

- (void)refreshStars {
    for(int i = 0; i < _starViews.count; ++i) {
        UIImageView *imageView = [_starViews objectAtIndex:i];
        if (_rating >= i+1) {
            imageView.image = _selectedStar;
        } else if (_rating > i) {
            imageView.image = _halfSelectedStar;
        } else {
            imageView.image = _notSelectedStar;
        }
    }
}

- (void)setupView {
    for(int i = 0; i < _maxRating; ++i) {
        UIImageView *imageView = [UIImageView newAutoLayoutView];
        imageView.contentMode = UIViewContentModeScaleAspectFit;
        [_starViews addObject:imageView];
        [self addSubview:imageView];
    }
    [self refreshStars];
}

- (void)baseInit {
    _notSelectedStar = [UIImage imageNamed:@"not_selected_star"];
    _selectedStar = [UIImage imageNamed:@"selected_star"];
    _halfSelectedStar = [UIImage imageNamed:@"half_selected_star"];
    _starViews = [NSMutableArray array];
    _maxRating = kDefaultMaxRating;
    _midMargin = kDefaultMidMargin;
    _leftMargin = kDefaultLeftMargin;
    _rightMargin = kDefaultRightMargin;
    _minStarSize = kDefaultMinStarSize;
    _minAllowedRating = kDefaultMinAllowedRating;
    _maxAllowedRating = kDefaultMaxAllowedRating;
    _rating = _minAllowedRating;
    _canEdit = YES;
    [self setupView];
}

- (instancetype)initWithTheme:(AITheme *)theme align:(AIAlign)align
{
    if (self = [super init])
    {
        _theme = theme;
        _align = align;
        
        self.translatesAutoresizingMaskIntoConstraints = NO;

        _didUpdateConstraints = NO;
        [self baseInit];
    }
    return self;
}

- (void)updateConstraints
{
    if (_didUpdateConstraints)
    {
        [super updateConstraints];
        return;
    }
    
    _didUpdateConstraints = YES;

    UIView *leftView = [UIView newAutoLayoutView];
    [self addSubview:leftView];
    [leftView autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [leftView autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    [leftView autoPinEdgeToSuperviewEdge:ALEdgeLeading];

    UIView *prev = leftView;
    for (int i = 0; i < _starViews.count; ++i)
    {
        UIImageView *imageView = _starViews[i];
        [imageView autoPinEdgeToSuperviewEdge:ALEdgeTop];
        [imageView autoPinEdgeToSuperviewEdge:ALEdgeBottom];
        if (prev)
            [imageView autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:prev];
        else
            [imageView autoPinEdgeToSuperviewEdge:ALEdgeLeading];
        prev = imageView;
    }

    UIView *rightView = [UIView newAutoLayoutView];
    [self addSubview:rightView];
    [rightView autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:prev];
    [rightView autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [rightView autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    [rightView autoPinEdgeToSuperviewEdge:ALEdgeTrailing];

    switch (_align)
    {
        case AIAlignLeft: [rightView autoSetDimension:ALDimensionWidth toSize:1 relation:NSLayoutRelationGreaterThanOrEqual]; break;
        case AIAlignRight: [leftView autoSetDimension:ALDimensionWidth toSize:1 relation:NSLayoutRelationGreaterThanOrEqual]; break;
        case AIAlignCenter: [leftView autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:rightView withOffset:0 relation:NSLayoutRelationEqual]; break;
        default: [rightView autoSetDimension:ALDimensionWidth toSize:1 relation:NSLayoutRelationGreaterThanOrEqual]; break;
    }
    [super updateConstraints];
}

- (void)setMaxRating:(int)maxRating {
    if (_maxAllowedRating == _maxRating) {
        _maxAllowedRating = maxRating;
    }
    _maxRating = maxRating;
    
    
    // Remove old image views
    for(int i = 0; i < _starViews.count; ++i) {
        UIImageView *imageView = (UIImageView *) [_starViews objectAtIndex:i];
        [imageView removeFromSuperview];
    }
    [_starViews removeAllObjects];
    
    // Add new image views
    [self setupView];    
    // Relayout and refresh
    [self setNeedsLayout];
    [self refreshStars];
}

- (void)setRating:(float)rating {
    _rating = rating;
    [self refreshStars];
}

#pragma mark - Touch Detection

- (void)handleTouchAtLocation:(CGPoint)touchLocation {
    if (!_canEdit) return;
    
    _rating = 0;
    for(long i = _starViews.count - 1; i >= 0; i--) {
        UIImageView *imageView = [_starViews objectAtIndex:i];        
        if (touchLocation.x > imageView.frame.origin.x) {
            _rating = i+1;
            break;
        }
    }
    _rating = MAX(_minAllowedRating, _rating);
    _rating = MIN(_maxAllowedRating, _rating);
    [self refreshStars];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint touchLocation = [touch locationInView:self];
    [self handleTouchAtLocation:touchLocation];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint touchLocation = [touch locationInView:self];
    [self handleTouchAtLocation:touchLocation];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    
}


@end
