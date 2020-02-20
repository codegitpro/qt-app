// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeBoolResult.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeBoolResult::NativeBoolResult() : ::djinni::JniInterface<::ai::BoolResult, NativeBoolResult>("net/ai/BoolResult$CppProxy") {}

NativeBoolResult::~NativeBoolResult() = default;


CJNIEXPORT void JNICALL Java_net_ai_BoolResult_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        delete reinterpret_cast<::djinni::CppProxyHandle<::ai::BoolResult>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_net_ai_BoolResult_00024CppProxy_native_1onResult(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jboolean j_success)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::ai::BoolResult>(nativeRef);
        ref->on_result(::djinni::Bool::toCpp(jniEnv, j_success));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

}  // namespace djinni_generated
