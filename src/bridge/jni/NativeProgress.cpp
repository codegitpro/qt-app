// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeProgress.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeProgress::NativeProgress() = default;

NativeProgress::~NativeProgress() = default;

auto NativeProgress::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeProgress>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c.count)),
                                                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c.total)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeProgress::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeProgress>::get();
    return {::djinni::I64::toCpp(jniEnv, jniEnv->GetLongField(j, data.field_mCount)),
            ::djinni::I64::toCpp(jniEnv, jniEnv->GetLongField(j, data.field_mTotal))};
}

}  // namespace djinni_generated
