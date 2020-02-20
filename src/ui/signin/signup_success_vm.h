#pragma once
#include "../viewmodel.h"
#include "../signin_viewmodel.h"

namespace ai
{
    struct signup_success_vm : FormViewmodel, std::enable_shared_from_this<signup_success_vm>
    {
        signup_success_vm(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<View>& main_menu, str email)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        log_(log),
        main_menu_(main_menu),
        email_(email)
        {
            
        }
        virtual ~signup_success_vm()
        {
            
        }
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        std::shared_ptr<View> main_menu_;
        str email_;
        
        sptr<GenericForm> form_;
        std::shared_ptr<FormView> current_view_;
        
        enum class control : n4
        {
            submit
        };
        
        static constexpr n4 id_submit = static_cast<n4>(control::submit);
        
        std::vector<Field> fields() final
        {
            int row = 0;
            auto fields = center(medium(
            {
                fld(row++).label("Sign up was successful!").purple().large(),
                fld(row++).vspace(),
                fld(row++).label("We have sent you an email with an activation link. Please open the email and follow the steps to activate your account. Once your account is activated, return here and sign in.").w(-600),
                fld(row++).button(id_submit, "Continue").w(-300),
            }));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            current_view_ = view;
            view->set_screen_name("Activate Account");
        }
        
        void action(n4 element_id) final
        {
            auto vm = std::make_shared<signin_viewmodel>(ui_cb_, screentype_, services_, log_, main_menu_, email_);
            vm->show();
        }
        
        
        void back() final
        {
            
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }
        
    };
}


