// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form.djinni

#include "NativeOsForm.hpp"  // my header
#include "Marshal.hpp"
#include "NativeMessage.hpp"
#include "NativeStatus.hpp"

namespace djinni_generated {

NativeOsForm::NativeOsForm() : ::djinni::JniInterface<::ai::OsForm, NativeOsForm>() {}

NativeOsForm::~NativeOsForm() = default;

NativeOsForm::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeOsForm::JavaProxy::~JavaProxy() = default;

void NativeOsForm::JavaProxy::action_result(::ai::Status c_outcome, const std::vector<::ai::Message> & c_lines) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeOsForm>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_actionResult,
                           ::djinni::get(::djinni_generated::NativeStatus::fromCpp(jniEnv, c_outcome)),
                           ::djinni::get(::djinni::List<::djinni_generated::NativeMessage>::fromCpp(jniEnv, c_lines)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeOsForm::JavaProxy::progress(int32_t c_position, int32_t c_total) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeOsForm>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_progress,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_position)),
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_total)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeOsForm::JavaProxy::enable(int32_t c_group) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeOsForm>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_enable,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_group)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeOsForm::JavaProxy::disable(int32_t c_group) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeOsForm>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_disable,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_group)));
    ::djinni::jniExceptionCheck(jniEnv);
}
std::unordered_map<int32_t, std::string> NativeOsForm::JavaProxy::input() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeOsForm>::get();
    auto jret = jniEnv->CallObjectMethod(Handle::get().get(), data.method_input);
    ::djinni::jniExceptionCheck(jniEnv);
    return ::djinni::Map<::djinni::I32, ::djinni::String>::toCpp(jniEnv, jret);
}

}  // namespace djinni_generated