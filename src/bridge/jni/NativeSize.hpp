// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form2.djinni

#pragma once

#include "djinni_support.hpp"
#include "size.hpp"

namespace djinni_generated {

class NativeSize final : ::djinni::JniEnum {
public:
    using CppType = ::ai::Size;
    using JniType = jobject;

    using Boxed = NativeSize;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeSize>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeSize>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeSize() : JniEnum("net/ai/Size") {}
    friend ::djinni::JniClass<NativeSize>;
};

}  // namespace djinni_generated
