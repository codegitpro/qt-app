// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIFileItemDetail.h"


@implementation AIFileItemDetail

- (nonnull instancetype)initWithItemId:(int32_t)itemId
                                 state:(AIFileState)state
                              selected:(BOOL)selected
                                 title:(nonnull NSString *)title
                                detail:(nonnull NSString *)detail
{
    if (self = [super init]) {
        _itemId = itemId;
        _state = state;
        _selected = selected;
        _title = [title copy];
        _detail = [detail copy];
    }
    return self;
}

+ (nonnull instancetype)fileItemDetailWithItemId:(int32_t)itemId
                                           state:(AIFileState)state
                                        selected:(BOOL)selected
                                           title:(nonnull NSString *)title
                                          detail:(nonnull NSString *)detail
{
    return [[self alloc] initWithItemId:itemId
                                  state:state
                               selected:selected
                                  title:title
                                 detail:detail];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p itemId:%@ state:%@ selected:%@ title:%@ detail:%@>", self.class, (void *)self, @(self.itemId), @(self.state), @(self.selected), self.title, self.detail];
}

@end
