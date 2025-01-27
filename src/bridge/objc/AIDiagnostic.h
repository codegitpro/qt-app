// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AILogType.h"
#import <Foundation/Foundation.h>


@protocol AIDiagnostic

- (void)log:(AILogType)type
       line:(int32_t)line
       file:(nonnull NSString *)file
    message:(nonnull NSString *)message;

- (void)logLines:(AILogType)type
            line:(int32_t)line
            file:(nonnull NSString *)file
        messages:(nonnull NSArray<NSString *> *)messages;

@end
