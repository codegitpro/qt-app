#define UIColorFromRGB(rgbValue) \
[UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 \
green:((float)((rgbValue & 0x00FF00) >>  8))/255.0 \
blue:((float)((rgbValue & 0x0000FF) >>  0))/255.0 \
alpha:1.0]

#import "AITheme.h"
#import "PureLayout.h"

@interface AITheme()
@property (nonatomic) BOOL messageActive;

@property (nonatomic, readonly) UIFont *smallFont;
@property (nonatomic, readonly) UIFont *mediumFont;
@property (nonatomic, readonly) UIFont *largeFont;

@property (nonatomic, readonly) UIFont *smallBoldFont;
@property (nonatomic, readonly) UIFont *mediumBoldFont;
@property (nonatomic, readonly) UIFont *largeBoldFont;

@property (nonatomic, readonly) UIColor *lightBlueColour;
@property (nonatomic, readonly) UIColor *purpleColour;
@property (nonatomic, readonly) UIColor *dirtyBlueColour;
@property (nonatomic, readonly) UIColor *darkGreyColour;
@property (nonatomic, readonly) UIColor *lightGreyColour;

@end

@implementation AITheme

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _smallFont = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption1];
        _mediumFont = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        _largeFont = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle2];
        
        _smallBoldFont = [UIFont fontWithDescriptor:[[UIFontDescriptor preferredFontDescriptorWithTextStyle:UIFontTextStyleCaption1] fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitBold] size:0];
        _mediumBoldFont = [UIFont fontWithDescriptor:[[UIFontDescriptor preferredFontDescriptorWithTextStyle:UIFontTextStyleBody] fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitBold] size:0];
        _largeBoldFont = [UIFont fontWithDescriptor:[[UIFontDescriptor preferredFontDescriptorWithTextStyle:UIFontTextStyleTitle2] fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitBold] size:0];


        _messageActive = NO;

        _formBackground = [UIColor whiteColor];
        _imgDownload = [UIImage imageNamed:@"download"];
        _imgDelete = [UIImage imageNamed:@"delete"];
        _imgCloudDone = [UIImage imageNamed:@"cloud_done"];
        _imgUpload = [UIImage imageNamed:@"upload"];
        _imgSave = [UIImage imageNamed:@"save"];
        _imgSelectAll = [UIImage imageNamed:@"select_all"];
        
        _purpleColour = UIColorFromRGB(0x9843D2);
        _lightBlueColour = UIColorFromRGB(0xD7EDFF);
        _dirtyBlueColour = UIColorFromRGB(0x7CABC9);
        _darkGreyColour = UIColorFromRGB(0x333333);
        _lightGreyColour = UIColorFromRGB(0xe0e0e0);
        
        _margins = 5;
        _textInputCorderRadius = 20.0;
        _textInputFont = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle3];
        _textInputBackground = _lightBlueColour;
        _textInputToplabelFont = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        
        _formErrorColour = [UIColor redColor];
        _labelColour = _purpleColour;
        _labelFont = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle2];
        
        _buttonColour = _dirtyBlueColour;
        _buttonTextColour = [UIColor whiteColor];
        _buttonFont = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle3];
        _buttonCornerRadius = 20.0;
        _thumbnailSize = 48;
        _thumbnailPx = (int)[UIScreen mainScreen].scale*_thumbnailSize;

        
        _navbarColour = _lightBlueColour;
        _navbarTextColour = _purpleColour;
        _navbarActionColour = _dirtyBlueColour;
        
        _switchColour = _dirtyBlueColour;
        _switchLabelColour = _dirtyBlueColour;
        _switchLabelFont = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle3];
        _switchInset = 38;

        _tabBarSelectionColour = _purpleColour;
        _tabBarBackgroundColour = _lightBlueColour;

        _bodyTextColour = _darkGreyColour;
        _bodyTextFont = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        
        _listTitleFont = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        _listTitleColour = _darkGreyColour;
        _listDetailFont = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption2];
        _listDetailColour = _darkGreyColour;
        _listColumnSpacing = 1;
        _listRowSpacing = 0;
        _listRowHeight = 48;
        _listThumbnailSpacing = 5;
        _listBackgroundColour = [UIColor whiteColor];
        _listRuleColour = _lightGreyColour;
    }
    return self;
}

- (UIColor *)grayColour
{
    return _lightGreyColour;
}

+ (UIImage *)imageWithImage:(UIImage *)image scaledToSize:(CGSize)newSize
{
    UIGraphicsBeginImageContext(newSize);
    [image drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}

+ (CGSize) scaledSizeMaxLength:(int)len w:(CGFloat)w h:(CGFloat)h
{
    CGFloat length = (CGFloat)len;
    return w < h
        ? CGSizeMake(length*w/h, length)
        : CGSizeMake(length, length*h/w);
}

- (CGSize) scaledThumbPxWidth:(CGFloat)width height:(CGFloat)height
{
    return [AITheme scaledSizeMaxLength:_thumbnailPx w:width h:height];
}

- (CGSize) scaledThumbPtWidth:(CGFloat)width height:(CGFloat)height
{
    return [AITheme scaledSizeMaxLength:_thumbnailSize w:width h:height];
}

- (UIImage *)thumbnailFromImage:(UIImage *)img
{
    return [AITheme imageWithImage:img scaledToSize:[self scaledThumbPxWidth:img.size.width height:img.size.height]];
}

- (void)showMessage:(NSString *)message controller:(UIViewController *)controller
{
    if (_messageActive)
        return;
    _messageActive = YES;
    
    UIAlertController * alert=[UIAlertController alertControllerWithTitle:nil
                                                                  message:@""
                                                           preferredStyle:UIAlertControllerStyleAlert];
    NSMutableAttributedString *AS = [[NSMutableAttributedString alloc] initWithString:message];
    [alert setValue:AS forKey:@"attributedTitle"];
    [controller presentViewController:alert animated:YES completion:nil];
    __weak AITheme *weakself = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^
    {
        [alert dismissViewControllerAnimated:YES completion:^{}];
        weakself.messageActive = NO;
    });
}

- (void)confirmWithTitle:(NSString *)title message:(NSString *)message controller:(UIViewController *)controller yesPress:(YesPress)yesPress
{
    UIAlertController * alert = [UIAlertController alertControllerWithTitle:title
                                                                    message:message
                                                             preferredStyle:UIAlertControllerStyleAlert];
    alert.view.tintColor = _buttonColour;
    UIAlertAction* yesButton = [UIAlertAction actionWithTitle:@"Yes"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction * action)
                                {
                                    yesPress();
                                }];
    
    UIAlertAction* noButton = [UIAlertAction actionWithTitle:@"No"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction * action) {}];
    
    [alert addAction:yesButton];
    [alert addAction:noButton];
    
    [controller presentViewController:alert animated:YES completion:nil];
}

+ (NSString *)formatSize:(NSNumber *)fileSize
{
    double size = fileSize.doubleValue;
    if (size >= 1000000000000)
        return [NSString stringWithFormat:@"%0.1f TB", size/1000000000000.0];
    if (size >= 10000000000)
        return [NSString stringWithFormat:@"%d GB", (int)round(size/1000000000.0)];
    if (size >= 1000000000)
        return [NSString stringWithFormat:@"%0.1f GB", size/1000000000.0];
    if (size >= 10000000)
        return [NSString stringWithFormat:@"%d MB", (int)round(size/1000000.0)];
    if (size >= 1000000)
        return [NSString stringWithFormat:@"%0.1f MB", size/1000000.0];
    if (size >= 10000)
        return [NSString stringWithFormat:@"%d KB", (int)round(size/1000.0)];
    if (size >= 1000)
        return [NSString stringWithFormat:@"%0.1f KB", size/1000.0];
    
    return [NSString stringWithFormat:@"%lu", fileSize.unsignedLongValue];
}

- (UIFont *)fontWithSize:(AISize)size weight:(AIWeight)weight
{
    switch (size)
    {
        case AISizeSmall: return AIWeightNormal == weight ? _smallFont : _smallBoldFont;
        case AISizeMedium: return AIWeightNormal == weight ? _mediumFont : _mediumBoldFont;
        case AISizeLarge: return AIWeightNormal == weight ? _largeFont : _largeBoldFont;
        default: return AIWeightNormal == weight ? _smallFont : _smallBoldFont;
    }
}

- (UIColor *)colour:(AIColour)colour
{
    switch (colour)
    {
        case AIColourLightGrey: return _lightGreyColour;
        case AIColourDarkGrey: return _darkGreyColour;
        case AIColourLightBlue: return _lightBlueColour;
        case AIColourDirtyBlue: return _dirtyBlueColour;
        case AIColourRed: return [UIColor redColor];
        case AIColourPurple: return _purpleColour;
        case AIColourWhite: return [UIColor whiteColor];
        default: return _darkGreyColour;
    }
    return nil;
}

+ (NSTextAlignment) convertTextAlign:(AIAlign)align
{
    switch (align)
    {
        case AIAlignLeft: return NSTextAlignmentLeft;
        case AIAlignRight: return NSTextAlignmentRight;
        case AIAlignCenter: return NSTextAlignmentCenter;
        default: return NSTextAlignmentLeft;
    }
}

- (CGFloat)pointsForSize:(AISize)size
{
    UIFont *font = [self fontWithSize:size weight:AIWeightNormal];
    return font.pointSize;
}

+ (UIControlContentHorizontalAlignment) convertAlign:(AIAlign)align
{
    switch (align)
    {
        case AIAlignLeft: return UIControlContentHorizontalAlignmentLeft;
        case AIAlignRight: return UIControlContentHorizontalAlignmentRight;
        case AIAlignCenter: return UIControlContentHorizontalAlignmentCenter;
        default: return UIControlContentHorizontalAlignmentCenter;
    }
}

- (void)layoutVertical:(NSArray<UIView *> *)views margins:(BOOL)margins
{
    UIView *parent = views[0].superview;
    
    UIView *topSpacer = [UIView newAutoLayoutView];
    [parent addSubview:topSpacer];
    UIView *bottomSpacer = [UIView newAutoLayoutView];
    [parent addSubview:bottomSpacer];
    
    UIView *prev = [self pinToBottom:topSpacer fromTop:nil margins:margins];
    for (int i = 0; i < views.count; ++i)
    {
        UIView *view = views[i];
        prev = [self pinToBottom:view fromTop:prev margins:margins];
    }
    prev = [self pinToBottom:bottomSpacer fromTop:prev margins:margins];
    [prev autoPinEdgeToSuperviewSafeArea:ALEdgeBottom];
    
    [topSpacer autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:bottomSpacer];
}

- (UIView *)pinToBottom:(UIView *)current fromTop:(nullable UIView *)prev margins:(BOOL)margins
{
    if (prev)
        [current autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:prev withOffset:(margins ? _margins : 0.0f)];
    else
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTop withInset:(margins ? _margins : 0.0f)];
    if (margins)
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeLeading withInset:_margins];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTrailing withInset:_margins];
    }
    else
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeLeading];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTrailing];
    }
    return current;
}

- (UIView *)pinLeftTop:(UIView *)current left:(UIView *)left top:(UIView *)top
{
    if (left)
        [current autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:left];
    else
        [current autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    if (top)
        [current autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:top];
    else
        [current autoPinEdgeToSuperviewEdge:ALEdgeTop];
    return current;
}


- (void)layoutHorizontal:(NSArray<UIView *> *)views margins:(BOOL)margins
{
    UIView *parent = views[0].superview;
    
    UIView *leftSpacer = [UIView newAutoLayoutView];
    [parent addSubview:leftSpacer];
    UIView *rightSpacer = [UIView newAutoLayoutView];
    [parent addSubview:rightSpacer];
    
    UIView *prev = [self pinToRight:leftSpacer fromLeft:nil margins:margins];
    for (int i = 0; i < views.count; ++i)
    {
        UIView *view = views[i];
        prev = [self pinToRight:view fromLeft:prev margins:margins];
    }
    prev = [self pinToRight:rightSpacer fromLeft:prev margins:margins];
    [prev autoPinEdgeToSuperviewSafeArea:ALEdgeTrailing];
    
    [leftSpacer autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:rightSpacer];
}

- (UIView *)pinToRight:(UIView *)current fromLeft:(nullable UIView *)prev margins:(BOOL)margins
{
    if (prev)
        [current autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:prev withOffset:(margins ? _margins : 0.0f)];
    else
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeLeading withInset:(margins ? _margins : 0.0f)];
    if (margins)
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTop withInset:_margins];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeBottom withInset:_margins];
    }
    else
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTop];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeBottom];
    }
    return current;
}

- (void)distributeHorizontally:(NSArray<UIView *> *)views margins:(BOOL)margins
{
    UIView *prev = nil;
    for (int i = 0; i < views.count; ++i)
    {
        prev = [self pinToRight:views[i] fromLeft:prev margins:margins];
    }
    [prev autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
}

- (void)distributeVertically:(NSArray<UIView *> *)views margins:(BOOL)margins
{
    UIView *prev = nil;
    for (int i = 0; i < views.count; ++i)
    {
        prev = [self pinToBottom:views[i] fromTop:prev margins:NO];
    }
    [prev autoPinEdgeToSuperviewEdge:ALEdgeBottom];
}

- (UIView *)horizontalViews:(NSArray<UIView *> *)views toParent:(UIView *)parent align:(AIAlign)align
{
    if (views.count == 1)
    {
        [parent addSubview:views[0]];
        return views[0];
    }
    
    UIView *container = [UIView newAutoLayoutView];
    [parent addSubview:container];
    //    views[0].backgroundColor = [UIColor redColor];
    //    views[1].backgroundColor = [UIColor blueColor];
    
    switch (align)
    {
        case AIAlignLeft:
        {
            NSMutableArray<UIView *> *extraViews = [NSMutableArray arrayWithArray:views];
            if (!extraViews)
                break;
            [extraViews addObject:[UIView newAutoLayoutView]];
            for (int i = 0; i < extraViews.count; ++i)
            {
                [container addSubview:extraViews[i]];
            }
            [self distributeHorizontally:extraViews margins:NO];
            break;
        }
        case AIAlignCenter:
        {
            NSMutableArray<UIView *> *extraViews = [NSMutableArray arrayWithArray:views];
            if (!extraViews)
                break;
            UIView *first = [UIView newAutoLayoutView];
            [extraViews insertObject:first atIndex:0];
            UIView *last = [UIView newAutoLayoutView];
            [extraViews addObject:last];
            for (int i = 0; i < extraViews.count; ++i)
            {
                [container addSubview:extraViews[i]];
            }
            [self distributeHorizontally:extraViews margins:NO];
            [first autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:last];
            break;
        }
        case AIAlignRight:
        {
            NSMutableArray<UIView *> *extraViews = [NSMutableArray arrayWithArray:views];
            [extraViews insertObject:[UIView newAutoLayoutView] atIndex:0];
            for (int i = 0; i < extraViews.count; ++i)
            {
                [container addSubview:extraViews[i]];
            }
            [self distributeHorizontally:extraViews margins:NO];
            break;
        }
        case AIAlignNone:
        {
            for (int i = 0; i < views.count; ++i)
            {
                [container addSubview:views[i]];
            }
            [self distributeHorizontally:views margins:NO];
            break;
        }
    }
    return container;
}

@end
