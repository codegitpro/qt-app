#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AIInteractiveElement <NSObject>
- (void)enable:(BOOL)enabled;
@property (nonatomic, readonly) int group;
@property (nonatomic, readonly) int inputId;
- (void)setError:(NSString *)error;
@end

NS_ASSUME_NONNULL_END
