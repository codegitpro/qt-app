// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include "djinni_support.hpp"
#include "view.hpp"

namespace djinni_generated {

class NativeView final : ::djinni::JniInterface<::ai::View, NativeView> {
public:
    using CppType = std::shared_ptr<::ai::View>;
    using CppOptType = std::shared_ptr<::ai::View>;
    using JniType = jobject;

    using Boxed = NativeView;

    ~NativeView();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeView>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeView>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeView();
    friend ::djinni::JniClass<NativeView>;
    friend ::djinni::JniInterface<::ai::View, NativeView>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::ai::View
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        void create_main_menu(const std::shared_ptr<::ai::FormViewmodel> & root_form_vm, const std::shared_ptr<::ai::FolderViewmodel> & root_folder_vm, const std::shared_ptr<::ai::FormViewmodel> & user_vm, const std::shared_ptr<::ai::FormViewmodel> & plan_vm, const std::shared_ptr<::ai::FormViewmodel> & settings_vm) override;
        void select_menu(int32_t index) override;
        void show_menu(bool visible) override;
        void send_invite() override;
        void push_form(const std::shared_ptr<::ai::FormViewmodel> & vm) override;
        void replace_form(const std::shared_ptr<::ai::FormViewmodel> & vm) override;
        void copy_to_clipboard(const std::string & content) override;
        void open_link_in_browser(const std::string & url) override;
        void register_store_vm(const std::shared_ptr<::ai::StoreViewmodel> & vm) override;
        bool is_authorized_for_payments() override;
        void fetch_products(const std::vector<std::string> & product_ids) override;
        void subscription_purchase(const std::string & email, const std::string & product_id) override;
        void subscription_restore(const std::string & email) override;
        void subscription_cancel(const std::string & product_id) override;
        std::string subscription_receipt(const std::string & product_id) override;
        void subscription_manage() override;

    private:
        friend ::djinni::JniInterface<::ai::View, ::djinni_generated::NativeView>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("net/ai/View") };
    const jmethodID method_createMainMenu { ::djinni::jniGetMethodID(clazz.get(), "createMainMenu", "(Lnet/ai/FormViewmodel;Lnet/ai/FolderViewmodel;Lnet/ai/FormViewmodel;Lnet/ai/FormViewmodel;Lnet/ai/FormViewmodel;)V") };
    const jmethodID method_selectMenu { ::djinni::jniGetMethodID(clazz.get(), "selectMenu", "(I)V") };
    const jmethodID method_showMenu { ::djinni::jniGetMethodID(clazz.get(), "showMenu", "(Z)V") };
    const jmethodID method_sendInvite { ::djinni::jniGetMethodID(clazz.get(), "sendInvite", "()V") };
    const jmethodID method_pushForm { ::djinni::jniGetMethodID(clazz.get(), "pushForm", "(Lnet/ai/FormViewmodel;)V") };
    const jmethodID method_replaceForm { ::djinni::jniGetMethodID(clazz.get(), "replaceForm", "(Lnet/ai/FormViewmodel;)V") };
    const jmethodID method_copyToClipboard { ::djinni::jniGetMethodID(clazz.get(), "copyToClipboard", "(Ljava/lang/String;)V") };
    const jmethodID method_openLinkInBrowser { ::djinni::jniGetMethodID(clazz.get(), "openLinkInBrowser", "(Ljava/lang/String;)V") };
    const jmethodID method_registerStoreVm { ::djinni::jniGetMethodID(clazz.get(), "registerStoreVm", "(Lnet/ai/StoreViewmodel;)V") };
    const jmethodID method_isAuthorizedForPayments { ::djinni::jniGetMethodID(clazz.get(), "isAuthorizedForPayments", "()Z") };
    const jmethodID method_fetchProducts { ::djinni::jniGetMethodID(clazz.get(), "fetchProducts", "(Ljava/util/ArrayList;)V") };
    const jmethodID method_subscriptionPurchase { ::djinni::jniGetMethodID(clazz.get(), "subscriptionPurchase", "(Ljava/lang/String;Ljava/lang/String;)V") };
    const jmethodID method_subscriptionRestore { ::djinni::jniGetMethodID(clazz.get(), "subscriptionRestore", "(Ljava/lang/String;)V") };
    const jmethodID method_subscriptionCancel { ::djinni::jniGetMethodID(clazz.get(), "subscriptionCancel", "(Ljava/lang/String;)V") };
    const jmethodID method_subscriptionReceipt { ::djinni::jniGetMethodID(clazz.get(), "subscriptionReceipt", "(Ljava/lang/String;)Ljava/lang/String;") };
    const jmethodID method_subscriptionManage { ::djinni::jniGetMethodID(clazz.get(), "subscriptionManage", "()V") };
};

}  // namespace djinni_generated
