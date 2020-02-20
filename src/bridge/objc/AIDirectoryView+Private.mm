// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIDirectoryView+Private.h"
#import "AIDirectoryView.h"
#import "AIDirectoriesDetail+Private.h"
#import "AIFileState+Private.h"
#import "AIFilesDetail+Private.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class DirectoryView::ObjcProxy final
: public ::ai::DirectoryView
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::DirectoryView;
public:
    using ObjcProxyBase::ObjcProxyBase;
    void refresh_list(const std::vector<int32_t> & c_folder_path, const ::ai::DirectoriesDetail & c_dirs, const ::ai::FilesDetail & c_files, const std::vector<std::string> & c_new_directory_names, const std::vector<std::string> & c_new_file_names) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() refreshList:(::djinni::List<::djinni::I32>::fromCpp(c_folder_path))
                                                             dirs:(::djinni_generated::DirectoriesDetail::fromCpp(c_dirs))
                                                            files:(::djinni_generated::FilesDetail::fromCpp(c_files))
                                                newDirectoryNames:(::djinni::List<::djinni::String>::fromCpp(c_new_directory_names))
                                                     newFileNames:(::djinni::List<::djinni::String>::fromCpp(c_new_file_names))];
        }
    }
    void file_states_changed(const std::vector<int32_t> & c_folder_path, const std::vector<int32_t> & c_file_ids, const std::vector<::ai::FileState> & c_states) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() fileStatesChanged:(::djinni::List<::djinni::I32>::fromCpp(c_folder_path))
                                                                fileIds:(::djinni::List<::djinni::I32>::fromCpp(c_file_ids))
                                                                 states:(::djinni::List<::djinni::Enum<::ai::FileState, AIFileState>>::fromCpp(c_states))];
        }
    }
    void file_changed(const std::vector<int32_t> & c_folder_path, int32_t c_file_id, int32_t c_file_name_id, int64_t c_size, int32_t c_percentage, ::ai::FileState c_state, const std::vector<uint8_t> & c_thumbnail) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() fileChanged:(::djinni::List<::djinni::I32>::fromCpp(c_folder_path))
                                                           fileId:(::djinni::I32::fromCpp(c_file_id))
                                                       fileNameId:(::djinni::I32::fromCpp(c_file_name_id))
                                                             size:(::djinni::I64::fromCpp(c_size))
                                                       percentage:(::djinni::I32::fromCpp(c_percentage))
                                                            state:(::djinni::Enum<::ai::FileState, AIFileState>::fromCpp(c_state))
                                                        thumbnail:(::djinni::Binary::fromCpp(c_thumbnail))];
        }
    }
};

}  // namespace djinni_generated

namespace djinni_generated {

auto DirectoryView::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto DirectoryView::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

}  // namespace djinni_generated