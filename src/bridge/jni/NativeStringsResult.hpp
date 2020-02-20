// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "strings_result.hpp"

namespace djinni_generated {

class NativeStringsResult final : ::djinni::JniInterface<::ai::StringsResult, NativeStringsResult> {
public:
    using CppType = std::shared_ptr<::ai::StringsResult>;
    using CppOptType = std::shared_ptr<::ai::StringsResult>;
    using JniType = jobject;

    using Boxed = NativeStringsResult;

    ~NativeStringsResult();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeStringsResult>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeStringsResult>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeStringsResult();
    friend ::djinni::JniClass<NativeStringsResult>;
    friend ::djinni::JniInterface<::ai::StringsResult, NativeStringsResult>;

};

}  // namespace djinni_generated
