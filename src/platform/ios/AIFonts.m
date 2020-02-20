#import "AIFonts.h"

@implementation AIFonts

+ (AIFonts *)instance
{
    static AIFonts *instance = nil;
    static dispatch_once_t token;
    dispatch_once(&token, ^
    {
        instance = [AIFonts new];
    });
    return instance;
}

- (instancetype)init
{
    if (self = [super init])
    {
        _body = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        _title1 = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle1];
        _title2 = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle2];
        _title3 = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle3];
        _callout = [UIFont preferredFontForTextStyle:UIFontTextStyleCallout];
        _caption1 = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption1];
        _caption2 = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption2];
        _footnote = [UIFont preferredFontForTextStyle:UIFontTextStyleFootnote];
        _headline = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
        _subheadline = [UIFont preferredFontForTextStyle:UIFontTextStyleSubheadline];
    }
    return self;
}
@end
