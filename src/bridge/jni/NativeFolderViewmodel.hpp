// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "folder_viewmodel.hpp"

namespace djinni_generated {

class NativeFolderViewmodel final : ::djinni::JniInterface<::ai::FolderViewmodel, NativeFolderViewmodel> {
public:
    using CppType = std::shared_ptr<::ai::FolderViewmodel>;
    using CppOptType = std::shared_ptr<::ai::FolderViewmodel>;
    using JniType = jobject;

    using Boxed = NativeFolderViewmodel;

    ~NativeFolderViewmodel();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeFolderViewmodel>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeFolderViewmodel>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeFolderViewmodel();
    friend ::djinni::JniClass<NativeFolderViewmodel>;
    friend ::djinni::JniInterface<::ai::FolderViewmodel, NativeFolderViewmodel>;

};

}  // namespace djinni_generated