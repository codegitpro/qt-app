// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "NativeDirectoryView.hpp"  // my header
#include "Marshal.hpp"
#include "NativeDirectoriesDetail.hpp"
#include "NativeFileState.hpp"
#include "NativeFilesDetail.hpp"

namespace djinni_generated {

NativeDirectoryView::NativeDirectoryView() : ::djinni::JniInterface<::ai::DirectoryView, NativeDirectoryView>() {}

NativeDirectoryView::~NativeDirectoryView() = default;

NativeDirectoryView::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeDirectoryView::JavaProxy::~JavaProxy() = default;

void NativeDirectoryView::JavaProxy::refresh_list(const std::vector<int32_t> & c_folder_path, const ::ai::DirectoriesDetail & c_dirs, const ::ai::FilesDetail & c_files, const std::vector<std::string> & c_new_directory_names, const std::vector<std::string> & c_new_file_names) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeDirectoryView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_refreshList,
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_folder_path)),
                           ::djinni::get(::djinni_generated::NativeDirectoriesDetail::fromCpp(jniEnv, c_dirs)),
                           ::djinni::get(::djinni_generated::NativeFilesDetail::fromCpp(jniEnv, c_files)),
                           ::djinni::get(::djinni::List<::djinni::String>::fromCpp(jniEnv, c_new_directory_names)),
                           ::djinni::get(::djinni::List<::djinni::String>::fromCpp(jniEnv, c_new_file_names)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeDirectoryView::JavaProxy::file_states_changed(const std::vector<int32_t> & c_folder_path, const std::vector<int32_t> & c_file_ids, const std::vector<::ai::FileState> & c_states) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeDirectoryView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_fileStatesChanged,
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_folder_path)),
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_file_ids)),
                           ::djinni::get(::djinni::List<::djinni_generated::NativeFileState>::fromCpp(jniEnv, c_states)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeDirectoryView::JavaProxy::file_changed(const std::vector<int32_t> & c_folder_path, int32_t c_file_id, int32_t c_file_name_id, int64_t c_size, int32_t c_percentage, ::ai::FileState c_state, const std::vector<uint8_t> & c_thumbnail) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeDirectoryView>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_fileChanged,
                           ::djinni::get(::djinni::List<::djinni::I32>::fromCpp(jniEnv, c_folder_path)),
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_file_id)),
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_file_name_id)),
                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c_size)),
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_percentage)),
                           ::djinni::get(::djinni_generated::NativeFileState::fromCpp(jniEnv, c_state)),
                           ::djinni::get(::djinni::Binary::fromCpp(jniEnv, c_thumbnail)));
    ::djinni::jniExceptionCheck(jniEnv);
}

}  // namespace djinni_generated
