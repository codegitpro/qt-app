#import "AIFilesDetailView.h"

@interface AIFilesDetailView()
@property (nonatomic, readonly, nonnull) NSArray<NSNumber *> *titles;
@property (nonatomic, readonly, nonnull) NSMutableArray<NSNumber *> *sizes;
@property (nonatomic, readonly, nonnull) NSMutableArray<NSNumber *> *percentages;
@property (nonatomic, readonly, nonnull) NSMutableArray<NSData *> *thumbnails;
@end

@implementation AIFilesDetailView
{
    int* _map;
    NSUInteger _allocatedSize;
}

- (void)changeAt:(long)position state:(AIFileState)state size:(long long)size percentage:(NSUInteger)percentage thumbnail:(NSData *)thumbnail
{
    if (!(position < _size))
        return;
    int i = _map[position];
    [_states setObject:[NSNumber numberWithInt:state] atIndexedSubscript:i];
    [_sizes setObject:[NSNumber numberWithLongLong:size] atIndexedSubscript:i];
    [_percentages setObject:[NSNumber numberWithUnsignedInteger:percentage] atIndexedSubscript:i];
    [_thumbnails setObject:thumbnail atIndexedSubscript:i];
}

- (void)setFiles:(AIFilesDetail *)files
{
    _ids = nil;
    _titles = nil;
    _sizes = nil;
    _percentages = nil;
    _states = nil;
    _thumbnails = nil;
    if (!files || !files.ids || 0 == files.ids.count)
    {
        _size = 0;
        return;
    }

    _ids = files.ids;
    _titles = files.titles;
    _sizes = [NSMutableArray arrayWithArray:files.sizes];
    _percentages = [NSMutableArray arrayWithArray:files.percentages];
    _states = [NSMutableArray arrayWithArray:files.states];
    _thumbnails = [NSMutableArray arrayWithArray:files.thumbnails];
    
    if (!_map || _allocatedSize < files.ids.count)
    {
        if (_map)
            free(_map);
        _allocatedSize = files.ids.count;
        _map = calloc(files.ids.count, sizeof(int));
    }
    NSUInteger position = 0;
    NSArray<NSNumber*>* states = files.states;
    for (int i = 0; i < states.count; ++i)
    {
        AIFileState state = [states objectAtIndex:i].integerValue;
        if (AIFileStateDeleted == state)
            continue;
        _map[position++] = i;
    }
    _size = position;
}

- (void)dealloc
{
    if (_map)
        free(_map);
}

- (int)idAt:(long)position
{
    return [_ids objectAtIndex:_map[position]].intValue;
}

- (long)findPositionWithId:(int)idd
{
    NSUInteger position = 0;
    for (long i = 0; i < _states.count; ++i)
    {
        AIFileState state = [_states objectAtIndex:i].integerValue;
        if (AIFileStateDeleted == state)
            continue;
        if (_ids[i].intValue == idd)
            return position;
        ++position;
    }
    return -1;
}

- (AIFileState)stateAt:(long)position
{
    return [_states objectAtIndex:_map[position]].intValue;
}

- (long long)sizeAt:(long)position
{
    return [_sizes objectAtIndex:_map[position]].longLongValue;
}

- (int)titleAt:(long)position
{
    return [_titles objectAtIndex:_map[position]].intValue;
}

- (NSUInteger)percentageAt:(long)position
{
    return [_percentages objectAtIndex:_map[position]].unsignedIntegerValue;
}

- (NSData *)thumbnailAt:(long)position
{
    return [_thumbnails objectAtIndex:_map[position]];
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _allocatedSize = 0;
        _size = 0;
    }
    return self;
}

@end
