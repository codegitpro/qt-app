// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIDirectoriesDetail.h"


@implementation AIDirectoriesDetail

- (nonnull instancetype)initWithIds:(nonnull NSArray<NSNumber *> *)ids
                             titles:(nonnull NSArray<NSNumber *> *)titles
{
    if (self = [super init]) {
        _ids = [ids copy];
        _titles = [titles copy];
    }
    return self;
}

+ (nonnull instancetype)directoriesDetailWithIds:(nonnull NSArray<NSNumber *> *)ids
                                          titles:(nonnull NSArray<NSNumber *> *)titles
{
    return [[self alloc] initWithIds:ids
                              titles:titles];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p ids:%@ titles:%@>", self.class, (void *)self, self.ids, self.titles];
}

@end