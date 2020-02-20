#pragma once
#include "../viewmodel.h"
#include "../cache/folder_subviewmodel.h"
#include "../user_viewmodel.h"
#include "../settings_viewmodel.h"
#include "../plan_viewmodel.h"
#include "../plan/store_proxy.h"

namespace ai
{
    struct loading_subviewmodel : FormViewmodel, std::enable_shared_from_this<loading_subviewmodel>
    {
        loading_subviewmodel
        (
         const sptr<UiCallback>& ui_cb,
         ScreenType type,
         services& services,
         Diagnostic& log,
         const sptr<View>& main_menu,
         str email,
         str password,
         funk<void(bool)> success_fun
        )
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        log_(log),
        main_menu_(main_menu),
        email_(email),
        password_(password),
        success_fun_(success_fun)
        {
            
        }
        virtual ~loading_subviewmodel()
        {
            
        }
        
        arr<Field> fields() final
        {
            int row = 0;
            
            auto fields = center(medium
            (
             {
                 fld(row++).label("Logging in").large().purple().idd(id_banner),
                 fld(row++).label("").idd(id_email),
                 fld(row++).label("").idd(id_network).red().w(-500),
                 fld(row++).button(id_login, "").w(-300),
             }
             ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const sptr<GenericForm>& form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            form_->input_set(id_email, email_);
            view->set_screen_name("Blomp");
            
            services_.login(email_, password_, true, [ui_cb = ui_cb_, email = email_, wself = std::weak_ptr<loading_subviewmodel>(shared_from_this())](str err)
            {
                run_ui(ui_cb, [wself, email, err]()
                {
                    auto sself = wself.lock();
                    if (!sself) return;
                    auto& m = *sself;

                    if (!err.empty())
                    {
                        m.form_->input_set(id_network, "Failed to sign in to Blomp. Please make sure that you are connected to the Internet and that you can browse the Web.");
                        m.form_->input_set(id_login, "Login Again");
                        return;
                    }
                    m.form_->input_set(id_banner, "Loading");
                    m.form_->input_set(id_email, "Directory");
                    m.root_folder_vm_ = std::make_shared<folder_subviewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, 0);
                    m.root_folder_vm_->load_folder([wself]()
                    {
                        auto sself = wself.lock();
                        if (!sself) return;
                        auto& m = *sself;

                        m.success_fun_(true);
                        auto user_vm = std::make_shared<user_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_);
                        auto plan_vm = std::make_shared<plan_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, std::make_shared<store_proxy>(m.services_, m.main_menu_));
                        auto settings_vm = std::make_shared<settings_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_);
                        m.main_menu_->create_main_menu(m.root_folder_vm_, m.root_folder_vm_, user_vm, plan_vm, settings_vm);
                        m.form_->close();
                    });
                });
            });
        }
        
        void show()
        {
            main_menu_->replace_form(shared_from_this());
        }
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        sptr<View> main_menu_;
        str email_;
        str password_;
        funk<void(bool)> success_fun_;
        
        sptr<GenericForm> form_;
        sptr<folder_subviewmodel> root_folder_vm_;
        
        enum class control : n4
        {
            email = 1,
            err,
            network,
            login,
            banner
        };
        
        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_err = static_cast<n4>(control::err);
        static constexpr n4 id_network = static_cast<n4>(control::network);
        static constexpr n4 id_login = static_cast<n4>(control::login);
        static constexpr n4 id_banner = static_cast<n4>(control::banner);

        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::login:
                {
                    auto fun = success_fun_;
                    form_->close();
                    fun(false);
                    break;
                }
                default: break;
            }
        }
        
        void back() final
        {
        }
        
        bool hide_navigation() final { return true; }
        bool holdon_to_vm() final { return false; }

    };
}
