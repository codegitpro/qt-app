// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIPreViewmodel+Private.h"
#import "AIPreViewmodel.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIPreViewmodel ()

- (id)initWithCpp:(const std::shared_ptr<::ai::PreViewmodel>&)cppRef;

@end

@implementation AIPreViewmodel {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::PreViewmodel>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::PreViewmodel>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)fileDelete {
    try {
        _cppRefHandle.get()->file_delete();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto PreViewmodel::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto PreViewmodel::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIPreViewmodel>(cpp);
}

}  // namespace djinni_generated

@end