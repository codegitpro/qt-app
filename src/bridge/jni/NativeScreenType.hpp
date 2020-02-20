// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "screen_type.hpp"

namespace djinni_generated {

class NativeScreenType final : ::djinni::JniEnum {
public:
    using CppType = ::ai::ScreenType;
    using JniType = jobject;

    using Boxed = NativeScreenType;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeScreenType>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeScreenType>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeScreenType() : JniEnum("net/ai/ScreenType") {}
    friend ::djinni::JniClass<NativeScreenType>;
};

}  // namespace djinni_generated