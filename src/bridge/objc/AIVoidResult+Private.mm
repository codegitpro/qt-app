// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIVoidResult+Private.h"
#import "AIVoidResult.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIVoidResult ()

- (id)initWithCpp:(const std::shared_ptr<::ai::VoidResult>&)cppRef;

@end

@implementation AIVoidResult {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::VoidResult>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::VoidResult>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)onResult {
    try {
        _cppRefHandle.get()->on_result();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto VoidResult::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto VoidResult::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIVoidResult>(cpp);
}

}  // namespace djinni_generated

@end
