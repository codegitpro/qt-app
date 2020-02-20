#import <Foundation/Foundation.h>
#import "AICancellationToken.h"

@interface AIUrlSessionCancellationToken : NSObject<AICancellationToken>
@property (nonatomic) NSURLSessionTask *task;
- (instancetype)initWithUrlSession:(NSURLSessionTask *)task;
@end
