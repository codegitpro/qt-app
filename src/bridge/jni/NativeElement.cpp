// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form.djinni

#include "NativeElement.hpp"  // my header
#include "Marshal.hpp"
#include "NativeElementType.hpp"

namespace djinni_generated {

NativeElement::NativeElement() = default;

NativeElement::~NativeElement() = default;

auto NativeElement::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeElement>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c.group)),
                                                           ::djinni::get(::djinni_generated::NativeElementType::fromCpp(jniEnv, c.type)),
                                                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c.input_id)),
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.title)),
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.value)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeElement::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 6);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeElement>::get();
    return {::djinni::I32::toCpp(jniEnv, jniEnv->GetIntField(j, data.field_mGroup)),
            ::djinni_generated::NativeElementType::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mType)),
            ::djinni::I32::toCpp(jniEnv, jniEnv->GetIntField(j, data.field_mInputId)),
            ::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_mTitle)),
            ::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_mValue))};
}

}  // namespace djinni_generated