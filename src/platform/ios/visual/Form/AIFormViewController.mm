#import "AIFormViewController.h"
#import "PureLayout.h"
#import "AIFonts.h"
#import "AITextInputx.h"
#import "AIButtonx.h"
#import "AIBooleanInputx.h"

@interface AIFormViewController ()
@property (nonatomic, readonly) NSArray<AIElement *> *elements;
@property (nonatomic, readonly) AIIntResult *action;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly) NSArray<id<AIInteractiveElement>> *interactiveElements;
@property (nonatomic, readonly) UIView *contentView;
@property (nonatomic, readonly) UIScrollView *scroller;
@property (nonatomic, readonly) UIView *matchBottomView;
@property (nonatomic, readonly) UIView *matchTopView;
@property (nonatomic, readonly) int32_t maxWidth;


@property (nonatomic) NSLayoutConstraint *bottomSpacerHeightConstraint;
@end

@implementation AIFormViewController

- (instancetype)initForm:(AITheme *)theme
                   title:(NSString *)title
                   width:(int32_t)width
                  height:(int32_t)height
                  action:(AIIntResult *)action
                elements:(NSArray<AIElement *> *)elements
{
    if (self = [super init])
    {
        self.title = title;
        _theme = theme;
        _action = action;
        _elements = elements;
        _contentView = [UIView newAutoLayoutView];
        _scroller = [UIScrollView newAutoLayoutView];
        _bottomSpace = 0;
        _matchBottomView = [UIView newAutoLayoutView];
        _matchTopView = [UIView newAutoLayoutView];
        _maxWidth = width;
    }
    return self;
}

- (void)pin:(UIView *)current prev:(UIView *)prev margins:(BOOL)margins
{
    if (prev)
        [current autoPinEdge:ALEdgeTop toEdge:ALEdgeBottom ofView:prev withOffset:(margins ? _theme.margins : 0.0f)];
    else
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTop withInset:(margins ? _theme.margins : 0.0f)];
    if (margins)
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeLeading withInset:_theme.margins];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTrailing withInset:_theme.margins];
    }
    else
    {
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeLeading];
        [current autoPinEdgeToSuperviewSafeArea:ALEdgeTrailing];
    }
}

- (void)viewDidLayoutSubviews
{
    [_contentView setNeedsLayout];
    [_contentView layoutIfNeeded];
    CGSize size = _contentView.frame.size;
    _scroller.contentSize = size;
}

+ (void)constrainChild:(UIView *)child withParent:(UIView *)parent toWidth:(CGFloat)width
{
    [parent addSubview:child];
    
    UIView *leftSpacer = [UIView newAutoLayoutView];
    UIView *rightSpacer = [UIView newAutoLayoutView];
    
    [parent addSubview:leftSpacer];
    [parent addSubview:rightSpacer];
    
    [leftSpacer autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [leftSpacer autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    [child autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [child autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    [rightSpacer autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [rightSpacer autoPinEdgeToSuperviewEdge:ALEdgeBottom];
    
    [leftSpacer autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [leftSpacer autoPinEdge:ALEdgeTrailing toEdge:ALEdgeLeading ofView:child];
    [rightSpacer autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:child];
    [rightSpacer autoPinEdgeToSuperviewEdge:ALEdgeTrailing];
    
    [leftSpacer autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:rightSpacer];
    [child autoSetDimension:ALDimensionWidth toSize:width relation:NSLayoutRelationLessThanOrEqual];
    NSLayoutConstraint *minwidth = [NSLayoutConstraint constraintWithItem:child attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationGreaterThanOrEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1 constant:width];
    minwidth.priority = UILayoutPriorityDefaultHigh;
    minwidth.active = YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    UIView *prev = nil;
    
    NSMutableArray<id<AIInteractiveElement>> *interactives = [NSMutableArray new];
    NSMutableArray<id<AIInputElement>> *inputs = [NSMutableArray new];

    self.navigationController.navigationBarHidden = NO;
    self.navigationController.navigationBar.translucent = NO;
    self.navigationController.navigationBar.opaque = YES;
    
    if (_maxWidth <= 300)
    {
        [self.view addSubview:_scroller];
        [_scroller autoPinEdgesToSuperviewEdges];
    }
    else
    {
        [AIFormViewController constrainChild:_scroller withParent:self.view toWidth:_maxWidth];
    }

    [_scroller addSubview:_contentView];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeTop];
    [_contentView autoPinEdgeToSuperviewEdge:ALEdgeLeading];
    [_contentView autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:_scroller];
    [_contentView autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:self.view withOffset:0 relation:NSLayoutRelationGreaterThanOrEqual];
    
    UIView *formView = _contentView;
    
    [formView addSubview:_matchTopView];
    [self pin:_matchTopView prev:prev margins:NO];
    prev = _matchTopView;
    
    for (int i = 0; i < _elements.count; ++i)
    {
        auto el = [_elements objectAtIndex:i];
        switch (el.type)
        {
            case AIElementTypeLabel:
            {
                auto label = [UILabel newAutoLayoutView];
                [formView addSubview:label];
                [self pin:label prev:prev margins:YES];
                
                label.text = el.title;
                label.font = _theme.labelFont;
                label.textColor = _theme.labelColour;
                label.textAlignment = NSTextAlignmentCenter;
                
                prev = label;
                break;
            }
            case AIElementTypeBoolInput:
            {
                auto input = [[AIBooleanInputx alloc] initWithTheme:_theme label:el.title element:el];
                [formView addSubview:input];
                [input setValue:el.value];
                
                [self pin:input prev:prev margins:NO];
                
                [interactives addObject:input];
                [inputs addObject:input];
                prev = input;

                break;
            }
            case AIElementTypeButton:
            {
                auto buttonContainer = [UIView newAutoLayoutView];
                [formView addSubview:buttonContainer];
                [self pin:buttonContainer prev:prev margins:NO];
                prev = buttonContainer;

                __weak AIFormViewController *weakself = self;
                auto button = [[AIButtonx alloc] initWithTheme:_theme element:el action:^(int inputId)
                               {
                                   [weakself clearErrors];
                                   AIIntResult *act = weakself.action;
                                   [act onResult:inputId];
                               }];
                [button configureForAutoLayout];
                [AIFormViewController constrainChild:button withParent:buttonContainer toWidth:300];
                
                [interactives addObject:button];
                prev = button;
                break;
            }
            case AIElementTypeText:
            {
                auto textBlock = [UITextView newAutoLayoutView];
                [formView addSubview:textBlock];
                [self pin:textBlock prev:prev margins:YES];
                textBlock.text = el.title;
                textBlock.font = _theme.bodyTextFont;
                textBlock.textColor = _theme.bodyTextColour;
                
                CGFloat maxw = _maxWidth <= 300 ? FLT_MAX : _maxWidth;
                CGFloat w = MIN([UIScreen mainScreen].bounds.size.width, maxw) - _theme.margins*2;
                CGFloat h = [textBlock sizeThatFits:CGSizeMake(w, FLT_MAX)].height;
                [textBlock autoSetDimension:ALDimensionHeight toSize:h];
                
                prev = textBlock;
                break;
            }
            default:
                if (AIElementTypeTextInput == el.type || AIElementTypePasswordInput == el.type || AIElementTypeEmailInput == el.type)
                {
                    auto input = [[AITextInputx alloc] initWithTheme:_theme label:el.title element:el];
                    [formView addSubview:input];
                    [input setValue:el.value];

                    [self pin:input prev:prev margins:NO];
                    
                    [interactives addObject:input];
                    [inputs addObject:input];
                    prev = input;
                }
                break;
        }
    }
    
    {
        [formView addSubview:_matchBottomView];
        [self pin:_matchBottomView prev:prev margins:NO];
        prev = _matchBottomView;
        
        [_matchTopView autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:_matchBottomView];
        
        UIView *bottomSpacer = [UIView newAutoLayoutView];
        [formView addSubview:bottomSpacer];
        [self pin:bottomSpacer prev:prev margins:NO];
        [bottomSpacer autoPinEdgeToSuperviewSafeArea:ALEdgeBottom];
        _bottomSpacerHeightConstraint = [bottomSpacer autoSetDimension:ALDimensionHeight toSize:_bottomSpace];
    }
    _interactiveElements = interactives;
    _inputElements = inputs;
}

- (void)setBottomSpace:(CGFloat)bottomSpace
{
    _bottomSpace = bottomSpace;
    _bottomSpacerHeightConstraint.constant = bottomSpace;
}

- (void)clearErrors
{
    for (int i = 0; i < _interactiveElements.count; ++i)
    {
        id<AIInteractiveElement> el = [_interactiveElements objectAtIndex:i];
        [el setError:@""];
    }
}

- (void)actionResult:(AIStatus)outcome
               lines:(nonnull NSArray<AIMessage *> *)lines
{
    __weak AIFormViewController *weakself = self;
    dispatch_async(dispatch_get_main_queue(), ^
    {
        if (AIStatusSuccess == outcome)
        {
            [self.navigationController popViewControllerAnimated:YES];
            return;
        }
        if (AIStatusFail == outcome)
        {
            for (int i = 0; i < lines.count; ++i)
            {
                AIMessage *msg = [lines objectAtIndex:i];
                for (int el_index = 0; el_index < weakself.interactiveElements.count; ++ el_index)
                {
                    id<AIInteractiveElement> element = [weakself.interactiveElements objectAtIndex:el_index];
                    if (element.inputId == msg.field)
                    {
                        [element setError:msg.content];
                        break;
                    }
                }
                
            }
            return;
        }

    });
}

- (void)progress:(int32_t)position
           total:(int32_t)total
{
    
}

- (void)enable:(int32_t)group
{
    __weak AIFormViewController *weakself = self;
    dispatch_async(dispatch_get_main_queue(), ^
    {
        for (int i = 0; i < weakself.interactiveElements.count; ++i)
        {
            id<AIInteractiveElement> el = [weakself.interactiveElements objectAtIndex:i];
            [el enable:YES];
        }
    });
}

- (void)disable:(int32_t)group
{
    __weak AIFormViewController *weakself = self;
    dispatch_async(dispatch_get_main_queue(), ^
    {
        for (int i = 0; i < weakself.interactiveElements.count; ++i)
        {
            id<AIInteractiveElement> el = [weakself.interactiveElements objectAtIndex:i];
            [el enable:NO];
        }
    });
}

- (nonnull NSDictionary<NSNumber *, NSString *> *)input
{
    NSMutableDictionary<NSNumber *, NSString *> *r = [NSMutableDictionary new];
    for (int i = 0; i < _inputElements.count; ++i)
    {
        auto el = [_inputElements objectAtIndex:i];
        [r setObject:el.value forKey:[NSNumber numberWithInteger:el.inputId]];
    }
    return r;
}

@end
