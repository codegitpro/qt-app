// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeDirectoriesDetail.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeDirectoriesDetail::NativeDirectoriesDetail() = default;

NativeDirectoriesDetail::~NativeDirectoriesDetail() = default;

auto NativeDirectoriesDetail::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeDirectoriesDetail>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c.ids)),
                                                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c.titles)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeDirectoriesDetail::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeDirectoriesDetail>::get();
    return {::djinni::List<::djinni::I32>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mIds)),
            ::djinni::List<::djinni::I32>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mTitles))};
}

}  // namespace djinni_generated
