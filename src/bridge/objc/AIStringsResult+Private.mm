// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIStringsResult+Private.h"
#import "AIStringsResult.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIStringsResult ()

- (id)initWithCpp:(const std::shared_ptr<::ai::StringsResult>&)cppRef;

@end

@implementation AIStringsResult {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::StringsResult>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::StringsResult>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)onResult:(nonnull NSArray<NSString *> *)content {
    try {
        _cppRefHandle.get()->on_result(::djinni::List<::djinni::String>::toCpp(content));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto StringsResult::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto StringsResult::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIStringsResult>(cpp);
}

}  // namespace djinni_generated

@end