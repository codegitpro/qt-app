#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct signup_subviewmodel : FormViewmodel, std::enable_shared_from_this<signup_subviewmodel>
    {
        signup_subviewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<FormView> & view, const std::shared_ptr<View>& main_menu)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        log_(log),
        view_(view),
        main_menu_(main_menu)
        {
            int row = 0;
            if (ScreenType::PHONE == screentype_)
            {
                fields_ = center
                (
                    medium
                    (
                        {
                            fld(row++).label("Welcome to Blomp! Please enter your details below to register.").purple(),
                            fld(row++).vspace(),
                            fld(row++).input(id_email, "Email").email().w(-400),
                            fld(row++).input(id_name, "Name").w(-400),
                            fld(row++).input(id_forum_name, "Prefered Forum Username").w(-400),
                            fld(row++).label("").idd(id_availibility).purple(),

                            fld(row++).button(id_check_availability, "Check Availability").w(-300).opt(),
                            fld(row++).input(id_password, "Password").password().w(-400),
                            fld(row++).input(id_confirm_password, "Repeat Password").password().w(-400),
                            fld(row++).check(id_terms, "I agree to the Terms").w(-400),
                            fld(row++).label("If you have a promo code, please enter it below.").w(-400),
                            fld(row++).input(id_promo, "Promo Code").w(-300).opt(),

                            fld(row++).label("").idd(id_busy).purple(),

                            fld(row++).button(id_register_button, "Register").w(-300),
                        }
                    )
                );
            }
            else
            {
                fields_ = left
                (
                    medium
                    (
                        {
                            fld(row++).label("Welcome to Blomp! Please enter your details below to register.").purple().w(-400),
                            fld(row++).vspace(),
                            fld(row).input(id_email, "Email").email().w(-200),
                            fld(row++).input(id_name, "Name").w(-200),
                            fld(row++).label("").idd(id_availibility).purple(),
                            fld(row).input(id_forum_name, "Prefered Forum Name").w(-200),
                            fld(row++).button(id_check_availability, "Check Availability").w(-200).opt(),

                            fld(row).input(id_password, "Password").password().w(-200),
                            fld(row++).input(id_confirm_password, "Repeat Password").password().w(-200),
                            fld(row++).check(id_terms, "I agree to the Terms").w(-400),
                            fld(row).label("If you have a promo code, please enter it here.").w(-200),
                            fld(row++).input(id_promo, "Promo Code").w(-200).opt(),

                            fld(row++).label("").idd(id_busy).purple(),

                            fld(row++).button(id_register_button, "Register").w(-300).center(),
                        }
                    )
                );
            }
        }

        virtual ~signup_subviewmodel()
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
        sptr<View> main_menu_;
        
        sptr<GenericForm> form_;
        std::shared_ptr<FormView> current_view_;
        std::vector<Field> fields_;
        
        enum class control : n4
        {
            email = 1,
            name,
            forum_name,
            check_availability,
            password,
            confirm_password,
            terms,
            register_button,
            availibility,
            busy,
            promo
        };

        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_name = static_cast<n4>(control::name);
        static constexpr n4 id_forum_name = static_cast<n4>(control::forum_name);
        static constexpr n4 id_check_availability = static_cast<n4>(control::check_availability);
        static constexpr n4 id_password = static_cast<n4>(control::password);
        static constexpr n4 id_confirm_password = static_cast<n4>(control::confirm_password);
        static constexpr n4 id_terms = static_cast<n4>(control::terms);
        static constexpr n4 id_register_button = static_cast<n4>(control::register_button);
        static constexpr n4 id_availibility = static_cast<n4>(control::availibility);
        static constexpr n4 id_busy = static_cast<n4>(control::busy);
        static constexpr n4 id_promo = static_cast<n4>(control::promo);

        std::vector<Field> fields() final
        {
            return fields_;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            current_view_ = view;
            
            view->set_screen_name("Sign Up");
        }

        void action(n4 element_id) final;
        void back() final {}
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }
    };
}
