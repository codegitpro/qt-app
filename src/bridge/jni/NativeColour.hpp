// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form2.djinni

#pragma once

#include "colour.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeColour final : ::djinni::JniEnum {
public:
    using CppType = ::ai::Colour;
    using JniType = jobject;

    using Boxed = NativeColour;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeColour>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeColour>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeColour() : JniEnum("net/ai/Colour") {}
    friend ::djinni::JniClass<NativeColour>;
};

}  // namespace djinni_generated
