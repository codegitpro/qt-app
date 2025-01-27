// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIView+Private.h"
#import "AIView.h"
#import "AIFolderViewmodel+Private.h"
#import "AIFormViewmodel+Private.h"
#import "AIStoreViewmodel+Private.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class View::ObjcProxy final
: public ::ai::View
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::View;
public:
    using ObjcProxyBase::ObjcProxyBase;
    void create_main_menu(const std::shared_ptr<::ai::FormViewmodel> & c_root_form_vm, const std::shared_ptr<::ai::FolderViewmodel> & c_root_folder_vm, const std::shared_ptr<::ai::FormViewmodel> & c_user_vm, const std::shared_ptr<::ai::FormViewmodel> & c_plan_vm, const std::shared_ptr<::ai::FormViewmodel> & c_settings_vm) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() createMainMenu:(::djinni_generated::FormViewmodel::fromCpp(c_root_form_vm))
                                                        rootFolderVm:(::djinni_generated::FolderViewmodel::fromCpp(c_root_folder_vm))
                                                              userVm:(::djinni_generated::FormViewmodel::fromCpp(c_user_vm))
                                                              planVm:(::djinni_generated::FormViewmodel::fromCpp(c_plan_vm))
                                                          settingsVm:(::djinni_generated::FormViewmodel::fromCpp(c_settings_vm))];
        }
    }
    void select_menu(int32_t c_index) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() selectMenu:(::djinni::I32::fromCpp(c_index))];
        }
    }
    void show_menu(bool c_visible) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() showMenu:(::djinni::Bool::fromCpp(c_visible))];
        }
    }
    void send_invite() override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() sendInvite];
        }
    }
    void push_form(const std::shared_ptr<::ai::FormViewmodel> & c_vm) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() pushForm:(::djinni_generated::FormViewmodel::fromCpp(c_vm))];
        }
    }
    void replace_form(const std::shared_ptr<::ai::FormViewmodel> & c_vm) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() replaceForm:(::djinni_generated::FormViewmodel::fromCpp(c_vm))];
        }
    }
    void copy_to_clipboard(const std::string & c_content) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() copyToClipboard:(::djinni::String::fromCpp(c_content))];
        }
    }
    void open_link_in_browser(const std::string & c_url) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() openLinkInBrowser:(::djinni::String::fromCpp(c_url))];
        }
    }
    void register_store_vm(const std::shared_ptr<::ai::StoreViewmodel> & c_vm) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() registerStoreVm:(::djinni_generated::StoreViewmodel::fromCpp(c_vm))];
        }
    }
    bool is_authorized_for_payments() override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() isAuthorizedForPayments];
            return ::djinni::Bool::toCpp(objcpp_result_);
        }
    }
    void fetch_products(const std::vector<std::string> & c_product_ids) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() fetchProducts:(::djinni::List<::djinni::String>::fromCpp(c_product_ids))];
        }
    }
    void subscription_purchase(const std::string & c_email, const std::string & c_product_id) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() subscriptionPurchase:(::djinni::String::fromCpp(c_email))
                                                                 productId:(::djinni::String::fromCpp(c_product_id))];
        }
    }
    void subscription_restore(const std::string & c_email) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() subscriptionRestore:(::djinni::String::fromCpp(c_email))];
        }
    }
    void subscription_cancel(const std::string & c_product_id) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() subscriptionCancel:(::djinni::String::fromCpp(c_product_id))];
        }
    }
    std::string subscription_receipt(const std::string & c_product_id) override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() subscriptionReceipt:(::djinni::String::fromCpp(c_product_id))];
            return ::djinni::String::toCpp(objcpp_result_);
        }
    }
    void subscription_manage() override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() subscriptionManage];
        }
    }
};

}  // namespace djinni_generated

namespace djinni_generated {

auto View::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto View::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

}  // namespace djinni_generated
