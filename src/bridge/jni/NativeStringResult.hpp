// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "string_result.hpp"

namespace djinni_generated {

class NativeStringResult final : ::djinni::JniInterface<::ai::StringResult, NativeStringResult> {
public:
    using CppType = std::shared_ptr<::ai::StringResult>;
    using CppOptType = std::shared_ptr<::ai::StringResult>;
    using JniType = jobject;

    using Boxed = NativeStringResult;

    ~NativeStringResult();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeStringResult>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeStringResult>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeStringResult();
    friend ::djinni::JniClass<NativeStringResult>;
    friend ::djinni::JniInterface<::ai::StringResult, NativeStringResult>;

};

}  // namespace djinni_generated
