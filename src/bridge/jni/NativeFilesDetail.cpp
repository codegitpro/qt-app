// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeFilesDetail.hpp"  // my header
#include "Marshal.hpp"
#include "NativeFileState.hpp"

namespace djinni_generated {

NativeFilesDetail::NativeFilesDetail() = default;

NativeFilesDetail::~NativeFilesDetail() = default;

auto NativeFilesDetail::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeFilesDetail>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c.ids)),
                                                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c.titles)),
                                                           ::djinni::get(::djinni::List<::djinni::I64>::fromCpp(jniEnv, c.sizes)),
                                                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c.percentages)),
                                                           ::djinni::get(::djinni::List<::djinni_generated::NativeFileState>::fromCpp(jniEnv, c.states)),
                                                           ::djinni::get(::djinni::List<::djinni::Binary>::fromCpp(jniEnv, c.thumbnails)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeFilesDetail::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 7);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeFilesDetail>::get();
    return {::djinni::List<::djinni::I32>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mIds)),
            ::djinni::List<::djinni::I32>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mTitles)),
            ::djinni::List<::djinni::I64>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mSizes)),
            ::djinni::List<::djinni::I32>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mPercentages)),
            ::djinni::List<::djinni_generated::NativeFileState>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mStates)),
            ::djinni::List<::djinni::Binary>::toCpp(jniEnv, jniEnv->GetObjectField(j, data.field_mThumbnails))};
}

}  // namespace djinni_generated
