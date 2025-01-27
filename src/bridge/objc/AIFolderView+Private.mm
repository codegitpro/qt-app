// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIFolderView+Private.h"
#import "AIFolderView.h"
#import "AIFileItemDetail+Private.h"
#import "AIFolderItemDetail+Private.h"
#import "AIFolderViewmodel+Private.h"
#import "AIFormViewmodel+Private.h"
#import "AIPreViewmodel+Private.h"
#import "AIStateChange+Private.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class FolderView::ObjcProxy final
: public ::ai::FolderView
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::FolderView;
public:
    using ObjcProxyBase::ObjcProxyBase;
    void create_folder_view(const std::shared_ptr<::ai::FolderViewmodel> & c_folder_vm, const std::shared_ptr<::ai::FormViewmodel> & c_form_vm) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() createFolderView:(::djinni_generated::FolderViewmodel::fromCpp(c_folder_vm))
                                                                formVm:(::djinni_generated::FormViewmodel::fromCpp(c_form_vm))];
        }
    }
    void create_photo_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() createPhotoPreview:(::djinni_generated::PreViewmodel::fromCpp(c_vm))
                                                                fileName:(::djinni::String::fromCpp(c_file_name))
                                                                filePath:(::djinni::String::fromCpp(c_file_path))];
        }
    }
    void create_video_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() createVideoPreview:(::djinni_generated::PreViewmodel::fromCpp(c_vm))
                                                                fileName:(::djinni::String::fromCpp(c_file_name))
                                                                filePath:(::djinni::String::fromCpp(c_file_path))];
        }
    }
    void create_text_preview(const std::shared_ptr<::ai::PreViewmodel> & c_vm, const std::string & c_file_name, const std::string & c_file_path) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() createTextPreview:(::djinni_generated::PreViewmodel::fromCpp(c_vm))
                                                               fileName:(::djinni::String::fromCpp(c_file_name))
                                                               filePath:(::djinni::String::fromCpp(c_file_path))];
        }
    }
    void reload(int32_t c_section, int32_t c_row) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() reload:(::djinni::I32::fromCpp(c_section))
                                                         row:(::djinni::I32::fromCpp(c_row))];
        }
    }
    void on_reset(const std::vector<::ai::FolderItemDetail> & c_folders, const std::vector<::ai::FileItemDetail> & c_files) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onReset:(::djinni::List<::djinni_generated::FolderItemDetail>::fromCpp(c_folders))
                                                        files:(::djinni::List<::djinni_generated::FileItemDetail>::fromCpp(c_files))];
        }
    }
    void on_file_states_change(const std::vector<::ai::StateChange> & c_state_changes) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileStatesChange:(::djinni::List<::djinni_generated::StateChange>::fromCpp(c_state_changes))];
        }
    }
    void on_file_remove(int32_t c_row) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileRemove:(::djinni::I32::fromCpp(c_row))];
        }
    }
    void on_folder_remove(int32_t c_row) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFolderRemove:(::djinni::I32::fromCpp(c_row))];
        }
    }
    void on_file_detail_change(int32_t c_row, const std::string & c_detail) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileDetailChange:(::djinni::I32::fromCpp(c_row))
                                                                  detail:(::djinni::String::fromCpp(c_detail))];
        }
    }
    void on_clear_selection(const std::vector<int32_t> & c_folder_indexes, const std::vector<int32_t> & c_file_indexes) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onClearSelection:(::djinni::List<::djinni::I32>::fromCpp(c_folder_indexes))
                                                           fileIndexes:(::djinni::List<::djinni::I32>::fromCpp(c_file_indexes))];
        }
    }
    void on_file_change(int32_t c_row, const ::ai::FileItemDetail & c_item) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileChange:(::djinni::I32::fromCpp(c_row))
                                                              item:(::djinni_generated::FileItemDetail::fromCpp(c_item))];
        }
    }
    void on_folder_change(int32_t c_row, const ::ai::FolderItemDetail & c_item) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFolderChange:(::djinni::I32::fromCpp(c_row))
                                                                item:(::djinni_generated::FolderItemDetail::fromCpp(c_item))];
        }
    }
    void on_select_all() override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onSelectAll];
        }
    }
    void on_file_select(int32_t c_row, bool c_selected) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileSelect:(::djinni::I32::fromCpp(c_row))
                                                          selected:(::djinni::Bool::fromCpp(c_selected))];
        }
    }
    void on_folder_select(int32_t c_row, bool c_selected) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFolderSelect:(::djinni::I32::fromCpp(c_row))
                                                            selected:(::djinni::Bool::fromCpp(c_selected))];
        }
    }
    void on_file_added(int32_t c_row, const ::ai::FileItemDetail & c_item) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFileAdded:(::djinni::I32::fromCpp(c_row))
                                                             item:(::djinni_generated::FileItemDetail::fromCpp(c_item))];
        }
    }
    void on_folder_added(int32_t c_row, const ::ai::FolderItemDetail & c_item) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onFolderAdded:(::djinni::I32::fromCpp(c_row))
                                                               item:(::djinni_generated::FolderItemDetail::fromCpp(c_item))];
        }
    }
};

}  // namespace djinni_generated

namespace djinni_generated {

auto FolderView::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto FolderView::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

}  // namespace djinni_generated
