// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AITask+Private.h"
#import "AITask.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AITask ()

- (id)initWithCpp:(const std::shared_ptr<::ai::Task>&)cppRef;

@end

@implementation AITask {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::Task>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::Task>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)run {
    try {
        _cppRefHandle.get()->run();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto Task::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto Task::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AITask>(cpp);
}

}  // namespace djinni_generated

@end