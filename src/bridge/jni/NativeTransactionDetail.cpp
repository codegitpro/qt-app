// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeTransactionDetail.hpp"  // my header
#include "Marshal.hpp"
#include "NativeTransactionState.hpp"

namespace djinni_generated {

NativeTransactionDetail::NativeTransactionDetail() = default;

NativeTransactionDetail::~NativeTransactionDetail() = default;

auto NativeTransactionDetail::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeTransactionDetail>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni_generated::NativeTransactionState::fromCpp(jniEnv, c.state)),
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.error_message)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeTransactionDetail::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeTransactionDetail>::get();
    return {::djinni_generated::NativeTransactionState::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mState)),
            ::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_mErrorMessage))};
}

}  // namespace djinni_generated
