// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AITransactionDetail.h"


@implementation AITransactionDetail

- (nonnull instancetype)initWithState:(AITransactionState)state
                         errorMessage:(nonnull NSString *)errorMessage
{
    if (self = [super init]) {
        _state = state;
        _errorMessage = [errorMessage copy];
    }
    return self;
}

+ (nonnull instancetype)transactionDetailWithState:(AITransactionState)state
                                      errorMessage:(nonnull NSString *)errorMessage
{
    return [[self alloc] initWithState:state
                          errorMessage:errorMessage];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p state:%@ errorMessage:%@>", self.class, (void *)self, @(self.state), self.errorMessage];
}

@end
