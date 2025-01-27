// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeHttpResult.hpp"  // my header
#include "Marshal.hpp"
#include "NativeHeader.hpp"

namespace djinni_generated {

NativeHttpResult::NativeHttpResult() : ::djinni::JniInterface<::ai::HttpResult, NativeHttpResult>("net/ai/HttpResult$CppProxy") {}

NativeHttpResult::~NativeHttpResult() = default;


CJNIEXPORT void JNICALL Java_net_ai_HttpResult_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        delete reinterpret_cast<::djinni::CppProxyHandle<::ai::HttpResult>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_net_ai_HttpResult_00024CppProxy_native_1onResult(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jint j_statusCode, jobject j_headers, jstring j_data)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::ai::HttpResult>(nativeRef);
        ref->on_result(::djinni::I32::toCpp(jniEnv, j_statusCode),
                       ::djinni::List<::djinni_generated::NativeHeader>::toCpp(jniEnv, j_headers),
                       ::djinni::String::toCpp(jniEnv, j_data));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

}  // namespace djinni_generated
