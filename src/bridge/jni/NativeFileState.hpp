// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "file_state.hpp"

namespace djinni_generated {

class NativeFileState final : ::djinni::JniEnum {
public:
    using CppType = ::ai::FileState;
    using JniType = jobject;

    using Boxed = NativeFileState;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeFileState>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeFileState>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeFileState() : JniEnum("net/ai/FileState") {}
    friend ::djinni::JniClass<NativeFileState>;
};

}  // namespace djinni_generated