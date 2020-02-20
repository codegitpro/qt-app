// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "transaction_detail.hpp"

namespace djinni_generated {

class NativeTransactionDetail final {
public:
    using CppType = ::ai::TransactionDetail;
    using JniType = jobject;

    using Boxed = NativeTransactionDetail;

    ~NativeTransactionDetail();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeTransactionDetail();
    friend ::djinni::JniClass<NativeTransactionDetail>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("net/ai/TransactionDetail") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Lnet/ai/TransactionState;Ljava/lang/String;)V") };
    const jfieldID field_mState { ::djinni::jniGetFieldID(clazz.get(), "mState", "Lnet/ai/TransactionState;") };
    const jfieldID field_mErrorMessage { ::djinni::jniGetFieldID(clazz.get(), "mErrorMessage", "Ljava/lang/String;") };
};

}  // namespace djinni_generated