// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIHttpProgressResult+Private.h"
#import "AIHttpProgressResult.h"
#import "AIHeader+Private.h"
#import "AIProgress+Private.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIHttpProgressResult ()

- (id)initWithCpp:(const std::shared_ptr<::ai::HttpProgressResult>&)cppRef;

@end

@implementation AIHttpProgressResult {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::HttpProgressResult>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::HttpProgressResult>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)onProgress:(nonnull AIProgress *)at {
    try {
        _cppRefHandle.get()->on_progress(::djinni_generated::Progress::toCpp(at));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)onResult:(int32_t)statusCode
         headers:(nonnull NSArray<AIHeader *> *)headers
   contentLength:(int64_t)contentLength {
    try {
        _cppRefHandle.get()->on_result(::djinni::I32::toCpp(statusCode),
                                       ::djinni::List<::djinni_generated::Header>::toCpp(headers),
                                       ::djinni::I64::toCpp(contentLength));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto HttpProgressResult::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto HttpProgressResult::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIHttpProgressResult>(cpp);
}

}  // namespace djinni_generated

@end