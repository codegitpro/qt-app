//
//  AIBaseForm.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIBaseForm.h"
#import "AIInputElement.h"
#import "PureLayout.h"
#import "AIBooleanInput.h"
#import "AITextInput.h"
#import "AIButton.h"
#import "AIText.h"
#import "AIPicker.h"
#import "AIMultilineTextInput.h"
#import "AIImage.h"
#import "AIVerticalSpace.h"
#import "AIHelpElement.h"

@interface AIBaseForm()
@property (nonatomic, readonly) NSMutableArray<id<AIInputElement>> *inputs;
@property (nonatomic, readonly) AITheme *theme;
@property (nonatomic, readonly, weak) AIFormViewmodel *vm;
@property (nonatomic, readonly) NSArray<AIReference *> *references;
@property (nonatomic, readonly) UIViewController *parentController;
@property (nonatomic, readonly) UIScrollView *scroller;
@property (nonatomic, readonly) OnChange onChange;
@end

@implementation AIBaseForm

- (instancetype)initWithTheme:(AITheme *)theme
                           vm:(AIFormViewmodel *)vm
                   references:(nonnull NSArray<AIReference *> *)references
             parentController:(UIViewController *)parentController
                     scroller:(UIScrollView *)scroller
                     onChange:(OnChange)onChange
{
    if (self = [super init])
    {
        _theme = theme;
        _vm = vm;
        _references = references;
        _parentController = parentController;
        _scroller = scroller;
        if (onChange)
            _onChange = onChange;
        else
        {
            __weak AIBaseForm *weakself = self;
            _onChange = ^(int inputId)
            {
                [weakself.vm action:inputId];
            };
        }
        
        _error = [UILabel newAutoLayoutView];
        _error.textColor = [theme colour:AIColourRed];
        _inputs = [NSMutableArray new];
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"dealloc AIBaseForm");
}

- (void)close
{
}

- (void)disableActions
{
}

- (BOOL)enable:(int32_t)elementId enabled:(BOOL)enabled
{
    if (-1 == elementId)
    {
        for (int i = 0; i < _inputs.count; ++i)
        {
            [_inputs[i] enable:enabled];
        }
        return YES;
    }
    
    {
        for (int i = 0; i < _inputs.count; ++i)
        {
            if (elementId == _inputs[i].inputId)
            {
                [_inputs[i] enable:enabled];
                return YES;
            }
        }
        
    }
    return NO;
}

- (void)error:(int32_t)elementId message:(nonnull NSString *)message
{
    if (-1 == elementId)
    {
        _error.text = message;
        return;
    }
    
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (elementId == _inputs[i].inputId)
        {
            [_inputs[i] setError:message];
            return;
        }
    }
}

- (void)inputConfigure:(int32_t)inputId config:(nonnull NSString *)config
{
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (inputId == _inputs[i].inputId)
        {
            [_inputs[i] configure:config];
            return;
        }
    }
}

- (nonnull NSString *)inputGet:(int32_t)inputId
{
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (inputId == _inputs[i].inputId)
        {
            return _inputs[i].value;
        }
    }
    return @"";
}

- (void)inputSet:(int32_t)inputId value:(nonnull NSString *)value
{
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (inputId == _inputs[i].inputId)
        {
            _inputs[i].value = value;
            return;
        }
    }
    
}

- (BOOL)validate
{
    _error.text = @"";
    BOOL foundError = NO;
    
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (![_inputs[i] validate])
            foundError = YES;
    }
    
    return !foundError;
}

- (void)reload:(int32_t)section row:(int32_t)row
{
}

- (void)layoutEdges:(NSArray<NSArray<NSMutableArray<UIView *> *> *> *)edges parent:(UIView *)parent
{
    NSArray<NSMutableArray<UIView *> *> *leadingEdge = edges[0];
    NSArray<NSMutableArray<UIView *> *> *trailingEdge = edges[1];
    NSArray<NSMutableArray<UIView *> *> *topEdge = edges[2];
    NSArray<NSMutableArray<UIView *> *> *bottomEdge = edges[3];
    
    {
        NSMutableArray<UIView *> *firstLeadingViews = [NSMutableArray new];
        for (int i = 0; i < topEdge.count; ++i)
        {
            UIView *spacer = [UIView newAutoLayoutView];
            [parent addSubview:spacer];
            [parent sendSubviewToBack:spacer];
            
            [firstLeadingViews addObject:spacer];
            [topEdge[i] addObject:spacer];
            [bottomEdge[i] addObject:spacer];
        }
        [_theme distributeVertically:firstLeadingViews margins:NO];
    }

    {
        NSMutableArray<UIView *> *firstTopViews = [NSMutableArray new];
        for (int i = 0; i < leadingEdge.count; ++i)
        {
            UIView *spacer = [UIView newAutoLayoutView];
            [parent addSubview:spacer];
            [parent sendSubviewToBack:spacer];
            
            [firstTopViews addObject:spacer];
            [leadingEdge[i] addObject:spacer];
            [trailingEdge[i] addObject:spacer];
        }
        [_theme distributeHorizontally:firstTopViews margins:NO];
    }
    
    for (int columnIndex = 0; columnIndex < leadingEdge.count; ++columnIndex)
    {
        NSArray<UIView *> *column = leadingEdge[columnIndex];
        if (column.count > 1)
            [column autoAlignViewsToEdge:ALEdgeLeading];
    }
    for (int columnIndex = 0; columnIndex < trailingEdge.count; ++columnIndex)
    {
        NSArray<UIView *> *column = trailingEdge[columnIndex];
        if (column.count > 1)
            [column autoAlignViewsToEdge:ALEdgeTrailing];
    }
    for (int rowIndex = 0; rowIndex < topEdge.count; ++rowIndex)
    {
        NSArray<UIView *> *row = topEdge[rowIndex];
        if (row.count > 1)
            [row autoAlignViewsToEdge:ALEdgeTop];
    }
    for (int rowIndex = 0; rowIndex < bottomEdge.count; ++rowIndex)
    {
        NSArray<UIView *> *row = bottomEdge[rowIndex];
        if (row.count > 1)
            [row autoAlignViewsToEdge:ALEdgeBottom];
    }
}

- (UIView *)addCenteredFields:(NSArray<AIField *> *)fields content:(UIView *)content prev:(nullable UIView *)prev isDialog:(BOOL)isDialog;
{
    UIView *parent = [UIView newAutoLayoutView];
    [content addSubview:parent];
    UIView *leftLane = [UIView newAutoLayoutView];
    [content addSubview:leftLane];
    UIView *rightLane = [UIView newAutoLayoutView];
    [content addSubview:rightLane];
    
//    leftLane.backgroundColor = [UIColor redColor];
//    rightLane.backgroundColor = [UIColor blueColor];
    
    [_theme distributeHorizontally:@[leftLane, parent, rightLane] margins:NO];
    [leftLane autoMatchDimension:ALDimensionWidth toDimension:ALDimensionWidth ofView:rightLane];

    UIView *topSpacer = [UIView newAutoLayoutView];
    [parent addSubview:topSpacer];
    prev = [_theme pinToBottom:topSpacer fromTop:prev margins:NO];

    prev = [self addFields:fields parent:parent prev:prev isDialog:isDialog multiline:YES];

    UIView *bottomSpacer = [UIView newAutoLayoutView];
    [parent addSubview:bottomSpacer];
    prev = [_theme pinToBottom:bottomSpacer fromTop:prev margins:NO];
    
//        topSpacer.backgroundColor = [UIColor blueColor];
//        bottomSpacer.backgroundColor = [UIColor redColor];
    
    [topSpacer autoMatchDimension:ALDimensionHeight toDimension:ALDimensionHeight ofView:bottomSpacer];
    return prev;
}

- (NSArray<NSArray<NSMutableArray<UIView *> *> *> *)edgesFromFields:(NSArray<AIField *> *)fields startIndex:(int)startIndex startRow:(int)startRow
{
    int maxCol = 0;
    int maxRow = 0;
    int minRow = INT_MAX;
    for (int i = startIndex; i < fields.count; ++i)
    {
        AIField *field = fields[i];
        if (AIFieldTypePanelBreak == field.fieldT)
            break;
        maxRow = MAX(maxRow, field.rect.rowEnd);
        minRow = MIN(minRow, field.rect.rowBeg);
        maxCol = MAX(maxCol, field.rect.colEnd);
    }
    NSAssert(minRow == startRow, @"minRow == startRow");
    
    NSMutableArray<NSMutableArray<UIView *> *> *leadingEdge = [NSMutableArray new];
    NSMutableArray<NSMutableArray<UIView *> *> *trailingEdge = [NSMutableArray new];
    NSMutableArray<NSMutableArray<UIView *> *> *topEdge = [NSMutableArray new];
    NSMutableArray<NSMutableArray<UIView *> *> *bottomEdge = [NSMutableArray new];
    
    for (int i = minRow; i < maxRow; ++i)
    {
        [topEdge addObject:[NSMutableArray new]];
        [bottomEdge addObject:[NSMutableArray new]];
    }
    for (int i = 0; i < maxCol; ++i)
    {
        [leadingEdge addObject:[NSMutableArray new]];
        [trailingEdge addObject:[NSMutableArray new]];
    }

    return @[leadingEdge, trailingEdge, topEdge, bottomEdge];
}

- (UIView *)addFields:(NSArray<AIField *> *)fields
               parent:(UIView *)parent
                 prev:(nullable UIView *)prev
             isDialog:(BOOL)isDialog
            multiline:(BOOL)multiline
{
    
    [parent addSubview:_error];
    prev = [_theme pinToBottom:_error fromTop:prev margins:YES];

    __weak AIBaseForm *weakself = self;

    
    NSMutableArray<UIView *> *cell = [NSMutableArray new];
//    NSMutableArray<UIView *> *baselineViews = [NSMutableArray new];
    AIAlign lastAlign = AIAlignNone;
    
    AIField *prevField = nil;
    {
        UIView *panel = [UIView newAutoLayoutView];
        [parent addSubview:panel];
        prev = [_theme pinToBottom:panel fromTop:prev margins:NO];
    }
    
    int startRow = 0;
    NSArray<NSArray<NSMutableArray<UIView *> *> *> *edges = [self edgesFromFields:fields startIndex:0 startRow:startRow];
    
    for (int i = 0; i < fields.count; ++i)
    {
        AIField *field = fields[i];
        UIView *control = nil;
//        UIView *baselineView = nil;
        switch (field.fieldT)
        {
            case AIFieldTypeHelp:
            {
                AIHelpElement *el = [[AIHelpElement alloc] initWithTheme:_theme label:field.title viewController:_parentController];
                control = el;
                break;
            }
            case AIFieldTypeChat:
                break;
            case AIFieldTypeError: break;
            case AIFieldTypeConsole: break;

            case AIFieldTypeVerticalSpace:
            {
                AIVerticalSpace *el = [[AIVerticalSpace alloc] initWithSize:[_theme pointsForSize:field.style.size]];
                control = el;
                break;
            }
            case AIFieldTypeLogo:
            {
//                AIImage *el = [[AIImage alloc] initWithImage:_theme.logo align:field.style.align];
//                control = el;
                break;
            }
            case AIFieldTypePanelBreak:
            {
                {
                    UIView *cellView = [_theme horizontalViews:cell toParent:prev align:lastAlign];
                    [cell removeAllObjects];
                    [edges[0][prevField.rect.colBeg] addObject:cellView];
                    [edges[1][prevField.rect.colEnd - 1] addObject:cellView];
                    [edges[2][prevField.rect.rowBeg - startRow] addObject:cellView];
                    [edges[3][prevField.rect.rowEnd - 1 - startRow] addObject:cellView];
                }

                [self layoutEdges:edges parent:prev];
                
                UIView *newPanel = [UIView newAutoLayoutView];
                [parent addSubview:newPanel];
                prev = [_theme pinToBottom:newPanel fromTop:prev margins:NO];
                prevField = nil;
                startRow = field.rect.rowEnd;
                edges = [self edgesFromFields:fields startIndex:i+1 startRow:startRow];
                break;
            }
            case AIFieldTypeLabel:
            {
                AIText *el = [[AIText alloc] initWithTheme:_theme
                                                   inputId:field.fieldId
                                                     label:field.title
                                                      size:field.style.size
                                                    weight:field.style.weight
                                                textColour:field.style.colour
                                                     align:field.style.align
                                                  isDialog:isDialog
                                                  maxWidth:-field.style.width
                              ];
                [_inputs addObject:el];
                control = el;
//                baselineView = el;
                break;
            }
            case AIFieldTypeLink: 
            case AIFieldTypeButton:
            {
                AIButton *el = [[AIButton alloc] initWithTheme:_theme
                                                       inputId:field.fieldId
                                                         label:field.title
                                                          size:field.style.size
                                                        weight:field.style.weight
                                                        colour:field.style.colour
                                                      isDialog:isDialog
                                                  mustValidate:field.required
                                                      maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                         align:field.style.align
                                                     hasBorder:field.fieldT == AIFieldTypeButton
                                                     multiline:multiline
                                                        action:^(int inputId, BOOL mustValidate) {
                                                            if (!weakself) return;
                                                            if (!mustValidate || [weakself validate])
                                                                weakself.onChange(inputId);
                                                        }];
                [_inputs addObject:el];
                control = el;
                break;
            }
            case AIFieldTypeBack:
            {
                AIButton *el = [[AIButton alloc] initWithTheme:_theme
                                                       inputId:field.fieldId
                                                         label:[NSString stringWithFormat:@"< %@", field.title]
                                                          size:field.style.size
                                                        weight:field.style.weight
                                                        colour:AIColourPurple
                                                      isDialog:isDialog
                                                  mustValidate:NO
                                                      maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                         align:AIAlignLeft
                                                     hasBorder:NO
                                                     multiline:multiline
                                                        action:^(int inputId, BOOL mustValidate) {
                                                            if (!weakself) return;
                                                            [weakself.vm back];
                                                        }];
                control = el;

                break;
            }
            case AIFieldTypeInput:
            {
                switch (field.dataT)
                {
                    case AIDataTypeChoice:
                    {
                        NSArray *items = [AIPicker optionsFromString:field.title];
                        AIPicker *el = [[AIPicker alloc] initWithTheme:_theme
                                                               inputId:field.fieldId
                                                               options:items
                                                                  size:field.style.size
                                                                weight:field.style.weight
                                                            textColour:field.style.colour
                                                              isDialog:isDialog
                                                            isRequired:field.required
                                                                 align:field.style.align
                                                              maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                  parentViewController:_parentController
                                                              onChange:^(int inputId)
                                        {
                                            if (!weakself) return;
                                            weakself.onChange(inputId);
                                        }
                                        ];
                        
                        [_inputs addObject:el];
                        
                        control = el;

                        break;
                    }
                    case AIDataTypeRating:
                    {
                        break;
                    }
                    case AIDataTypeBoolean:
                    {
                        AIBooleanInput *el = [[AIBooleanInput alloc] initWithTheme:_theme
                                                                           inputId:field.fieldId
                                                                             label:field.title
                                                                              size:field.style.size
                                                                            weight:field.style.weight
                                                                        textColour:field.style.colour
                                              
                                                                        onChange:^(int inputId)
                                                                        {
                                                                            if (!weakself) return;
                                                                            weakself.onChange(inputId);
                                                                        }
                                              ];
                        [_inputs addObject:el];
                        control = el;
                        break;
                    }
                    case AIDataTypeReference:
                    {
                        NSArray<NSString *> *options = nil;
                        for (int i = 0; i < _references.count; ++i)
                        {
                            AIReference *ref = _references[i];
                            if ([field.title isEqualToString:ref.name])
                            {
                                options = ref.items;

                                break;
                            }
                        }
                        
                        if (!options)
                            options = [field.title componentsSeparatedByString:@"|"];
                        if (!options)
                            break;
                        
                        AIPicker *el = [[AIPicker alloc] initWithTheme:_theme
                                                               inputId:field.fieldId
                                                               options:options
                                                                  size:field.style.size
                                                                weight:field.style.weight
                                                            textColour:field.style.colour
                                                              isDialog:isDialog
                                                            isRequired:field.required
                                                                 align:field.style.align
                                                              maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                  parentViewController:_parentController
                                                              onChange:^(int inputId)
                                        {
                                            if (!weakself) return;
                                            weakself.onChange(inputId);
                                        }
                                        ];
                        
                        [_inputs addObject:el];
                        control = el;

                        break;
                    }
                    case AIDataTypeMultilineText:
                    {
                        AIMultilineTextInput *el = [[AIMultilineTextInput alloc] initWithTheme:_theme
                                                                                       inputId:field.fieldId
                                                                                         label:field.title
                                                                                          size:field.style.size
                                                                                        weight:field.style.weight
                                                                                    textColour:field.style.colour
//                                                                                          type:AIElementTypeTextInput
                                                                                      isDialog:isDialog
                                                                                    isRequired:field.required
                                                                                         align:field.style.align
                                                                                      maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                                                         lines:0
                                                                                      scroller:_scroller
                                                                                      onChange:^(int inputId) {
                                                                                          if (!weakself) return;
                                                                                          weakself.onChange(inputId);
                                                                                      }
                                                    ];
                        [_inputs addObject:el];
                        control = el;

                        break;
                    }

                    default:
                    {
//                        AIElementType type = AIElementTypeTextInput;
//                        switch (field.dataT)
//                        {
//                            case AIDataTypeUrl: type = AIElementTypeUrlInput; break;
//                            case AIDataTypeEmail: type = AIElementTypeEmailInput; break;
//                            case AIDataTypePassword: type = AIElementTypePasswordInput; break;
//                            case AIDataTypeSkype: type = AIElementTypeSkypeInput; break;
//                            case AIDataTypePhone: type = AIElementTypePhoneInput; break;
//                            case AIDataTypeCurrency: type = AIElementTypeCurrencyInput; break;
//                            case AIDataTypeInteger: type = AIElementTypeIntegerInput; break;
//                            default: break;
//                        }
                        AITextInput *el = [[AITextInput alloc] initWithTheme:_theme
                                                                     inputId:field.fieldId
                                                                       label:field.title
                                                                        size:field.style.size
                                                                      weight:field.style.weight
                                                                  textColour:field.style.colour
                                                                        type:field.dataT
                                                                    isDialog:isDialog
                                                                  isRequired:field.required
                                                                       align:field.style.align
                                                                    maxWidth:field.style.width < 0 ? -field.style.width : 0
                                                                    scroller:_scroller
                                                              viewController:_parentController
                                                                    onChange:^(int inputId) {
                                                                        if (!weakself) return;
                                                                        weakself.onChange(inputId);
                                                                    }
                                           ];
                        [_inputs addObject:el];
                        control = el;
                    }
                }
                break;
            }
            default: break;
        }
        if (control)
        {
            if (prevField)
            {
                if (prevField.rect.rowBeg != field.rect.rowBeg || prevField.rect.colBeg != field.rect.colBeg)
                {
                    NSAssert(cell.count > 0, @"cell.count > 0");
                    UIView *cellView = [_theme horizontalViews:cell toParent:prev align:lastAlign];
//                    if (baselineViews.count > 1)
//                    {
//                        [baselineViews autoAlignViewsToAxis:ALAxisFirstBaseline];
//                    }
//                    [baselineViews removeAllObjects];
                    lastAlign = AIAlignNone;
                    [cell removeAllObjects];
                    [edges[0][prevField.rect.colBeg] addObject:cellView];
                    [edges[1][prevField.rect.colEnd - 1] addObject:cellView];
                    [edges[2][prevField.rect.rowBeg - startRow] addObject:cellView];
                    [edges[3][prevField.rect.rowEnd - 1 - startRow] addObject:cellView];
                }
                
            }
            [cell addObject:control];
//            if (baselineView)
//            {
//                [baselineViews addObject:baselineView];
//            }
            lastAlign = field.style.align;
            prevField = field;
        }
    }

    {
        UIView *cellView = [_theme horizontalViews:cell toParent:prev align:lastAlign];
        [cell removeAllObjects];
        [edges[0][prevField.rect.colBeg] addObject:cellView];
        [edges[1][prevField.rect.colEnd - 1] addObject:cellView];
        [edges[2][prevField.rect.rowBeg - startRow] addObject:cellView];
        [edges[3][prevField.rect.rowEnd - 1 - startRow] addObject:cellView];
    }
    
    [self layoutEdges:edges parent:prev];
    
//    prev.backgroundColor = [UIColor redColor];

    return prev;
}

- (void)showMessage:(nonnull NSString *)message
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    [_theme showMessage:message controller:_parentController];
}

@end
