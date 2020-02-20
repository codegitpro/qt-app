#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct forgot_viewmodel : FormViewmodel, std::enable_shared_from_this<forgot_viewmodel>
    {
        forgot_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<FormView> & view, str email)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        log_(log),
        view_(view),
        email_(email)
        {
            
        }
        virtual ~forgot_viewmodel()
        {
            
        }
        void show()
        {
            view_->push_form(shared_from_this());
            //            view_->create_form(shared_from_this());
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        sptr<FormView> view_;
        str email_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            email = 1,
            submit
        };
        
        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_submit = static_cast<n4>(control::submit);
        
        std::vector<Field> fields() final
        {
            int row = 0;
            auto fields = center(medium(
                                        {
                                            fld(row++).label("Password Recovery").purple().large(),
                                            fld(row++).vspace(),
                                            fld(row++).label("To recover your password start with submitting your email below:").w(-400),
                                            fld(row++).input(id_email, "Email").email().w(-300),
                                            fld(row++).button(id_submit, "Submit").w(-300),
                                        }));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("Recovery");
            form_->input_set(id_email, email_);
        }
        
        void action(n4 element_id) final
        {
            
        }
        
        
        void back() final
        {
            
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }

    };
}

