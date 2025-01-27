// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeFolderView.hpp"  // my header
#include "Marshal.hpp"
#include "NativeFileItemDetail.hpp"
#include "NativeFolderItemDetail.hpp"
#include "NativeFolderViewmodel.hpp"
#include "NativeFormViewmodel.hpp"
#include "NativePreViewmodel.hpp"
#include "NativeStateChange.hpp"

namespace djinni_generated {

NativeFolderView::NativeFolderView() : ::djinni::JniInterface<::ai::FolderView, NativeFolderView>() {}

NativeFolderView::~NativeFolderView() = default;

NativeFolderView::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeFolderView::JavaProxy::~JavaProxy() = default;

void NativeFolderView::JavaProxy::create_folder_view(const std::shared_ptr<::ai::FolderViewmodel> & c_folder_vm, const std::shared_ptr<::ai::FormViewmodel> & c_form_vm) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_createFolderView,
                           ::djinni::get(::djinni_generated::NativeFolderViewmodel::fromCpp(jniEnv, c_folder_vm)),
                           ::djinni::get(::djinni_generated::NativeFormViewmodel::fromCpp(jniEnv, c_form_vm)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::create_photo_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_createPhotoPreview,
                           ::djinni::get(::djinni_generated::NativePreViewmodel::fromCpp(jniEnv, c_vm)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_name)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_path)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::create_video_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_createVideoPreview,
                           ::djinni::get(::djinni_generated::NativePreViewmodel::fromCpp(jniEnv, c_vm)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_name)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_path)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::create_text_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_createTextPreview,
                           ::djinni::get(::djinni_generated::NativePreViewmodel::fromCpp(jniEnv, c_vm)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_name)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_file_path)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::reload(int32_t c_section, int32_t c_row) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_reload,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_section)),
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_reset(const std::vector<::ai::FolderItemDetail> & c_folders, const std::vector<::ai::FileItemDetail> & c_files) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onReset,
                           ::djinni::get(::djinni::List<::djinni_generated::NativeFolderItemDetail>::fromCpp(jniEnv, c_folders)),
                           ::djinni::get(::djinni::List<::djinni_generated::NativeFileItemDetail>::fromCpp(jniEnv, c_files)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_states_change(const std::vector<::ai::StateChange> & c_state_changes) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileStatesChange,
                           ::djinni::get(::djinni::List<::djinni_generated::NativeStateChange>::fromCpp(jniEnv, c_state_changes)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_remove(int32_t c_row) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileRemove,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_folder_remove(int32_t c_row) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFolderRemove,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_detail_change(int32_t c_row, const std::string & c_detail) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileDetailChange,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c_detail)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_clear_selection(const std::vector<int32_t> & c_folder_indexes, const std::vector<int32_t> & c_file_indexes) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onClearSelection,
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_folder_indexes)),
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_file_indexes)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_change(int32_t c_row, const ::ai::FileItemDetail & c_item) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileChange,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni_generated::NativeFileItemDetail::fromCpp(jniEnv, c_item)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_folder_change(int32_t c_row, const ::ai::FolderItemDetail & c_item) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFolderChange,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni_generated::NativeFolderItemDetail::fromCpp(jniEnv, c_item)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_select_all() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onSelectAll);
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_select(int32_t c_row, bool c_selected) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileSelect,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni::Bool::fromCpp(jniEnv, c_selected)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_folder_select(int32_t c_row, bool c_selected) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFolderSelect,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni::Bool::fromCpp(jniEnv, c_selected)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_file_added(int32_t c_row, const ::ai::FileItemDetail & c_item) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFileAdded,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni_generated::NativeFileItemDetail::fromCpp(jniEnv, c_item)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeFolderView::JavaProxy::on_folder_added(int32_t c_row, const ::ai::FolderItemDetail & c_item) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeFolderView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onFolderAdded,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_row)),
                           ::djinni::get(::djinni_generated::NativeFolderItemDetail::fromCpp(jniEnv, c_item)));
    ::djinni::jniExceptionCheck(jniEnv);
}

}  // namespace djinni_generated
