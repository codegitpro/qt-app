// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "folder_view.hpp"

namespace djinni_generated {

class NativeFolderView final : ::djinni::JniInterface<::ai::FolderView, NativeFolderView> {
public:
    using CppType = std::shared_ptr<::ai::FolderView>;
    using CppOptType = std::shared_ptr<::ai::FolderView>;
    using JniType = jobject;

    using Boxed = NativeFolderView;

    ~NativeFolderView();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeFolderView>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeFolderView>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeFolderView();
    friend ::djinni::JniClass<NativeFolderView>;
    friend ::djinni::JniInterface<::ai::FolderView, NativeFolderView>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::ai::FolderView
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        void create_folder_view(const std::shared_ptr<::ai::FolderViewmodel> & folder_vm, const std::shared_ptr<::ai::FormViewmodel> & form_vm) override;
        void create_photo_preview(const std::shared_ptr<::ai::PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) override;
        void create_video_preview(const std::shared_ptr<::ai::PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) override;
        void create_text_preview(const std::shared_ptr<::ai::PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) override;
        void reload(int32_t section, int32_t row) override;
        void on_reset(const std::vector<::ai::FolderItemDetail> & folders, const std::vector<::ai::FileItemDetail> & files) override;
        void on_file_states_change(const std::vector<::ai::StateChange> & state_changes) override;
        void on_file_remove(int32_t row) override;
        void on_folder_remove(int32_t row) override;
        void on_file_detail_change(int32_t row, const std::string & detail) override;
        void on_clear_selection(const std::vector<int32_t> & folder_indexes, const std::vector<int32_t> & file_indexes) override;
        void on_file_change(int32_t row, const ::ai::FileItemDetail & item) override;
        void on_folder_change(int32_t row, const ::ai::FolderItemDetail & item) override;
        void on_select_all() override;
        void on_file_select(int32_t row, bool selected) override;
        void on_folder_select(int32_t row, bool selected) override;
        void on_file_added(int32_t row, const ::ai::FileItemDetail & item) override;
        void on_folder_added(int32_t row, const ::ai::FolderItemDetail & item) override;

    private:
        friend ::djinni::JniInterface<::ai::FolderView, ::djinni_generated::NativeFolderView>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("net/ai/FolderView") };
    const jmethodID method_createFolderView { ::djinni::jniGetMethodID(clazz.get(), "createFolderView", "(Lnet/ai/FolderViewmodel;Lnet/ai/FormViewmodel;)V") };
    const jmethodID method_createPhotoPreview { ::djinni::jniGetMethodID(clazz.get(), "createPhotoPreview", "(Lnet/ai/PreViewmodel;Ljava/lang/String;Ljava/lang/String;)V") };
    const jmethodID method_createVideoPreview { ::djinni::jniGetMethodID(clazz.get(), "createVideoPreview", "(Lnet/ai/PreViewmodel;Ljava/lang/String;Ljava/lang/String;)V") };
    const jmethodID method_createTextPreview { ::djinni::jniGetMethodID(clazz.get(), "createTextPreview", "(Lnet/ai/PreViewmodel;Ljava/lang/String;Ljava/lang/String;)V") };
    const jmethodID method_reload { ::djinni::jniGetMethodID(clazz.get(), "reload", "(II)V") };
    const jmethodID method_onReset { ::djinni::jniGetMethodID(clazz.get(), "onReset", "(Ljava/util/ArrayList;Ljava/util/ArrayList;)V") };
    const jmethodID method_onFileStatesChange { ::djinni::jniGetMethodID(clazz.get(), "onFileStatesChange", "(Ljava/util/ArrayList;)V") };
    const jmethodID method_onFileRemove { ::djinni::jniGetMethodID(clazz.get(), "onFileRemove", "(I)V") };
    const jmethodID method_onFolderRemove { ::djinni::jniGetMethodID(clazz.get(), "onFolderRemove", "(I)V") };
    const jmethodID method_onFileDetailChange { ::djinni::jniGetMethodID(clazz.get(), "onFileDetailChange", "(ILjava/lang/String;)V") };
    const jmethodID method_onClearSelection { ::djinni::jniGetMethodID(clazz.get(), "onClearSelection", "(Ljava/util/ArrayList;Ljava/util/ArrayList;)V") };
    const jmethodID method_onFileChange { ::djinni::jniGetMethodID(clazz.get(), "onFileChange", "(ILnet/ai/FileItemDetail;)V") };
    const jmethodID method_onFolderChange { ::djinni::jniGetMethodID(clazz.get(), "onFolderChange", "(ILnet/ai/FolderItemDetail;)V") };
    const jmethodID method_onSelectAll { ::djinni::jniGetMethodID(clazz.get(), "onSelectAll", "()V") };
    const jmethodID method_onFileSelect { ::djinni::jniGetMethodID(clazz.get(), "onFileSelect", "(IZ)V") };
    const jmethodID method_onFolderSelect { ::djinni::jniGetMethodID(clazz.get(), "onFolderSelect", "(IZ)V") };
    const jmethodID method_onFileAdded { ::djinni::jniGetMethodID(clazz.get(), "onFileAdded", "(ILnet/ai/FileItemDetail;)V") };
    const jmethodID method_onFolderAdded { ::djinni::jniGetMethodID(clazz.get(), "onFolderAdded", "(ILnet/ai/FolderItemDetail;)V") };
};

}  // namespace djinni_generated
