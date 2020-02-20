// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "files_detail.hpp"

namespace djinni_generated {

class NativeFilesDetail final {
public:
    using CppType = ::ai::FilesDetail;
    using JniType = jobject;

    using Boxed = NativeFilesDetail;

    ~NativeFilesDetail();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeFilesDetail();
    friend ::djinni::JniClass<NativeFilesDetail>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("net/ai/FilesDetail") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Ljava/util/ArrayList;Ljava/util/ArrayList;Ljava/util/ArrayList;Ljava/util/ArrayList;Ljava/util/ArrayList;Ljava/util/ArrayList;)V") };
    const jfieldID field_mIds { ::djinni::jniGetFieldID(clazz.get(), "mIds", "Ljava/util/ArrayList;") };
    const jfieldID field_mTitles { ::djinni::jniGetFieldID(clazz.get(), "mTitles", "Ljava/util/ArrayList;") };
    const jfieldID field_mSizes { ::djinni::jniGetFieldID(clazz.get(), "mSizes", "Ljava/util/ArrayList;") };
    const jfieldID field_mPercentages { ::djinni::jniGetFieldID(clazz.get(), "mPercentages", "Ljava/util/ArrayList;") };
    const jfieldID field_mStates { ::djinni::jniGetFieldID(clazz.get(), "mStates", "Ljava/util/ArrayList;") };
    const jfieldID field_mThumbnails { ::djinni::jniGetFieldID(clazz.get(), "mThumbnails", "Ljava/util/ArrayList;") };
};

}  // namespace djinni_generated