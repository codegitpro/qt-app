// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeCancellationToken.hpp"  // my header

namespace djinni_generated {

NativeCancellationToken::NativeCancellationToken() : ::djinni::JniInterface<::ai::CancellationToken, NativeCancellationToken>() {}

NativeCancellationToken::~NativeCancellationToken() = default;

NativeCancellationToken::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeCancellationToken::JavaProxy::~JavaProxy() = default;

void NativeCancellationToken::JavaProxy::cancel() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeCancellationToken>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_cancel);
    ::djinni::jniExceptionCheck(jniEnv);
}

}  // namespace djinni_generated
