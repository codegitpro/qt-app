// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "status.hpp"

namespace djinni_generated {

class NativeStatus final : ::djinni::JniEnum {
public:
    using CppType = ::ai::Status;
    using JniType = jobject;

    using Boxed = NativeStatus;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeStatus>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeStatus>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeStatus() : JniEnum("net/ai/Status") {}
    friend ::djinni::JniClass<NativeStatus>;
};

}  // namespace djinni_generated
