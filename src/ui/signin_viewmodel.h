#pragma once
#include "viewmodel.h"
#include "signin/signup_subviewmodel.h"
#include "signin/forgot_viewmodel.h"

#include "cache/folder_subviewmodel.h"
#include "user_viewmodel.h"
#include "settings_viewmodel.h"
#include "plan_viewmodel.h"
#include "plan/store_proxy.h"

namespace ai
{
    struct signin_viewmodel : FormViewmodel, std::enable_shared_from_this<signin_viewmodel>
    {
        signin_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const sptr<View>& main_menu, str email)
            :
            ui_cb_(ui_cb),
            screentype_(type),
            services_(services),
            log_(log),
            main_menu_(main_menu),
            email_(email)
        {
        }
        virtual ~signin_viewmodel()
        {
            
        }
        
        std::vector<Field> fields() final
        {
            int row = 0;
            auto fields = center(medium(
            {
                fld(row++).label("Get Up to 200 GB for Free!").large().purple(),
                fld(row++).label("Sign Up Today").large().purple(),
                fld(row++).vspace(),
                fld(row++).input(id_email, "Email").email().w(-400),
                fld(row++).input(id_password, "Password").password().w(-400),
                fld(row++).check(id_remember_me, "Remember Me").w(-400),
                fld(row++).button(id_login, "Login").w(-300),
                fld(row++).button(id_sign_up, "Sign Up Now!").w(-300).opt(),
                fld(row++).button(id_forgot, "Forgot Password").w(-300).opt()
            }));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            form_view_ = view;
            form_->input_set(id_email, email_);
            view->set_screen_name("Blomp");
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
        
        sptr<FormView> form_view_;
        sptr<GenericForm> form_;
        sptr<folder_subviewmodel> root_folder_vm_;
        
        enum class control : n4
        {
            email = 1,
            password,
            remember_me,
            login,
            sign_up,
            forgot
        };
        
        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_password = static_cast<n4>(control::password);
        static constexpr n4 id_remember_me = static_cast<n4>(control::remember_me);
        static constexpr n4 id_login = static_cast<n4>(control::login);
        static constexpr n4 id_sign_up = static_cast<n4>(control::sign_up);
        static constexpr n4 id_forgot = static_cast<n4>(control::forgot);
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::sign_up:
                {
                    if (!form_view_)
                        return;
                    auto vm = std::make_shared<signup_subviewmodel>(ui_cb_, screentype_, services_, log_, form_view_, main_menu_);
                    vm->show();
                    break;
                }
                case control::login:
                {
                    email_ = form_->input_get(id_email);
                    str password = form_->input_get(id_password);
                    str remember_me = form_->input_get(id_remember_me);
                    form_->enable(-1, false);
                    services_.login(email_, password, remember_me == "true", [ui_cb = ui_cb_, email = email_, wself = std::weak_ptr<signin_viewmodel>(shared_from_this())](str err)
                    {
                        run_ui(ui_cb, [wself, email, err]()
                        {
                            auto sself = wself.lock();
                            if (!sself) return;
                            auto& m = *sself;

                            if (!err.empty())
                            {
                                m.form_->error(id_login, "Login failed. Please try again.");
                                m.form_->enable(-1, true);
                                return;
                            }
                            m.root_folder_vm_ = std::make_shared<folder_subviewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, 0);
                            m.root_folder_vm_->load_folder([wself]()
                            {
                                auto sself = wself.lock();
                                if (!sself) return;
                                auto& m = *sself;

                                auto user_vm = std::make_shared<user_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_);
                                auto plan_vm = std::make_shared<plan_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, std::make_shared<store_proxy>(m.services_, m.main_menu_));
                                auto settings_vm = std::make_shared<settings_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_);
                                m.main_menu_->create_main_menu(m.root_folder_vm_, m.root_folder_vm_, user_vm, plan_vm, settings_vm);
                                m.form_->close();
                            });
                        });
                    });

                    break;
                }
                case control::forgot:
                {
                    main_menu_->open_link_in_browser("https://dashboard.blomp.com/recovery/");
//                    if (!form_view_)
//                        return;
//                    std::make_shared<forgot_viewmodel>(ui_cb_, screentype_, services_, log_, form_view_, email_)->show();
                    break;
                }
                default: break;
            }
        }
        
        
        void back() final
        {
            
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return true; }

    };
}
