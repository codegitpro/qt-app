// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeBinaryResult.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeBinaryResult::NativeBinaryResult() : ::djinni::JniInterface<::ai::BinaryResult, NativeBinaryResult>("net/ai/BinaryResult$CppProxy") {}

NativeBinaryResult::~NativeBinaryResult() = default;


CJNIEXPORT void JNICALL Java_net_ai_BinaryResult_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        delete reinterpret_cast<::djinni::CppProxyHandle<::ai::BinaryResult>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_net_ai_BinaryResult_00024CppProxy_native_1onResult(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jbyteArray j_data)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::ai::BinaryResult>(nativeRef);
        ref->on_result(::djinni::Binary::toCpp(jniEnv, j_data));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

}  // namespace djinni_generated
