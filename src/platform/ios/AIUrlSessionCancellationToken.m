#import "AIUrlSessionCancellationToken.h"

@interface AIUrlSessionCancellationToken()
@property (nonatomic) BOOL cancelWasCalled;
@end
@implementation AIUrlSessionCancellationToken

- (void)cancel
{
    [_task cancel];
    _cancelWasCalled = YES;
}

- (void)setTask:(NSURLSessionTask *)task
{
    _task = task;
    if (_cancelWasCalled)
        [task cancel];
}

- (instancetype)initWithUrlSession:(NSURLSessionTask *)task
{
    if (self = [super init])
    {
        _task = task;
        _cancelWasCalled = NO;
    }
    return self;
}

@end
