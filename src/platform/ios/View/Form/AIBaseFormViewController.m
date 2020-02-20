//
//  AIBaseFormViewController.m
//  user
//
//  Created by Hans Malherbe on 2019/04/28.
//  Copyright © 2019 Hans Malherbe. All rights reserved.
//

#import "AIBaseFormViewController.h"
#import "AIInputElement.h"
#import "PureLayout.h"

@interface AIBaseFormViewController ()
@property (nonatomic, readonly) NSMutableArray<id<AIInputElement>> *inputs;
@property (nonatomic, readonly) AITheme *theme;
@end

@implementation AIBaseFormViewController

- (instancetype)initWithTheme:(AITheme *)theme
{
    if (self = [super init])
    {
        _theme = theme;
        
        _error = [UILabel newAutoLayoutView];
        _error.textColor = [theme colour:AIColourRed];
        _inputs = [NSMutableArray new];
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"dealloc AIBaseFormViewController");
}

- (void)addInput:(id<AIInputElement>)input
{
    [_inputs addObject:input];
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
    BOOL foundError = NO;
    
    for (int i = 0; i < _inputs.count; ++i)
    {
        if (![_inputs[i] validate])
            foundError = YES;
    }

    return !foundError;
}

- (void)reload:(int32_t)section row:(int32_t)row {}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)showMessage:(nonnull NSString *)message
{
    NSAssert([NSThread isMainThread], @"Must be called on main thread");
    [_theme showMessage:message controller:self];
}

@end
