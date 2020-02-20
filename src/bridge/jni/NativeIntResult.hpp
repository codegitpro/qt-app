// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "int_result.hpp"

namespace djinni_generated {

class NativeIntResult final : ::djinni::JniInterface<::ai::IntResult, NativeIntResult> {
public:
    using CppType = std::shared_ptr<::ai::IntResult>;
    using CppOptType = std::shared_ptr<::ai::IntResult>;
    using JniType = jobject;

    using Boxed = NativeIntResult;

    ~NativeIntResult();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeIntResult>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeIntResult>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeIntResult();
    friend ::djinni::JniClass<NativeIntResult>;
    friend ::djinni::JniInterface<::ai::IntResult, NativeIntResult>;

};

}  // namespace djinni_generated