// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "http_verb.hpp"

namespace djinni_generated {

class NativeHttpVerb final : ::djinni::JniEnum {
public:
    using CppType = ::ai::HttpVerb;
    using JniType = jobject;

    using Boxed = NativeHttpVerb;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeHttpVerb>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeHttpVerb>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeHttpVerb() : JniEnum("net/ai/HttpVerb") {}
    friend ::djinni::JniClass<NativeHttpVerb>;
};

}  // namespace djinni_generated
