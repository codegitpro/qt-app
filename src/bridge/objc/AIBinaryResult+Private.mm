// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIBinaryResult+Private.h"
#import "AIBinaryResult.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIBinaryResult ()

- (id)initWithCpp:(const std::shared_ptr<::ai::BinaryResult>&)cppRef;

@end

@implementation AIBinaryResult {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::BinaryResult>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::BinaryResult>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)onResult:(nonnull NSData *)data {
    try {
        _cppRefHandle.get()->on_result(::djinni::Binary::toCpp(data));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto BinaryResult::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto BinaryResult::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIBinaryResult>(cpp);
}

}  // namespace djinni_generated

@end
