#pragma once
#include "viewmodel.h"

namespace ai
{
    struct user_viewmodel : FormViewmodel, std::enable_shared_from_this<user_viewmodel>
    {
        user_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<View>& main_menu)
            :
            ui_cb_(ui_cb),
            screentype_(type),
            services_(services),
            log_(log),
            main_menu_(main_menu)
        {
        }
        void show()
        {
            
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        sptr<View> main_menu_;
        
        sptr<GenericForm> form_;
        sptr<FormView> view_;
        blomp_session::user_profile profile_;
        
        enum class control : n4
        {
            sign_out = 1,
            send_invite,
            name,
            email,
            address,
            phone,
            company,
            forum_username,
            registration_date,
            edit_profile,
            change_password,
            change_forumname
        };
        
        static constexpr n4 id_sign_out = static_cast<n4>(control::sign_out);
        static constexpr n4 id_send_invite = static_cast<n4>(control::send_invite);
        
        static constexpr n4 id_name = static_cast<n4>(control::name);
        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_address = static_cast<n4>(control::address);
        static constexpr n4 id_phone = static_cast<n4>(control::phone);
        static constexpr n4 id_company = static_cast<n4>(control::company);
        static constexpr n4 id_forum_username = static_cast<n4>(control::forum_username);
        static constexpr n4 id_registration_date = static_cast<n4>(control::registration_date);
        static constexpr n4 id_edit_profile = static_cast<n4>(control::edit_profile);
        static constexpr n4 id_change_password = static_cast<n4>(control::change_password);
        static constexpr n4 id_change_forumname = static_cast<n4>(control::change_forumname);
/*
 str email;
 str name;
 str address;
 str phone;
 str company;
 str forum_username;
 str registration_date;
 bool confirmed;
*/
        std::vector<Field> fields() final
        {
            int row = 0;
            auto r = center(medium
            (
             {
                 fld(row++).label("").idd(id_email).w(-300),
                 fld(row++).label("").idd(id_name).w(-300).purple(),
                 fld(row++).label("").idd(id_address).w(-300),
                 fld(row++).label("").idd(id_phone).w(-300),
                 fld(row++).label("").idd(id_company).w(-300),
                 fld(row++).label("").idd(id_forum_username).w(-300).purple(),
                 fld(row++).label("").idd(id_registration_date).w(-300).small(),
                 fld(row++).button(id_edit_profile, "Edit Profile").w(-200),
                 fld(row++).button(id_change_password, "Change Password").w(-200),
                 fld(row++).button(id_change_forumname, "Change Forum Name").w(-200),

                 fld(row++).label("Let your friends know about Blomp. Click the Send Invite button now!").purple().w(-400),
                 fld(row++).button(id_send_invite, "Send Invite").w(-200),
                 fld(row++).label("Signing out will take you the login screen and remove your password from memory and secure storage.").purple().w(-500),
                 fld(row++).button(id_sign_out, "Sign Out").w(-200),
             }
             ));

            return r;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view_ = view;
            view->set_screen_name("My Profile");
            
            services_.get_user_profile([ui_cb = ui_cb_, wself = std::weak_ptr<user_viewmodel>(shared_from_this())](auto status, auto profile)
            {
                run_ui(ui_cb, [wself, profile, status]()
                {
                    auto sself = wself.lock();
                    if (!sself) return;
                    auto& vm = *sself;
                    
                    switch (status)
                    {
                    case http_status::success:
                    {
                        vm.profile_ = profile;
                        vm.form_->input_set(id_name, profile.name);
                        vm.form_->input_set(id_email, profile.email);
                        vm.form_->input_set(id_address, profile.address);
                        vm.form_->input_set(id_phone, profile.phone);
                        vm.form_->input_set(id_company, profile.company);
                        vm.form_->input_set(id_forum_username, profile.forum_username);
                        vm.form_->input_set(id_registration_date, "Registered " + profile.registration_date);
                        break;
                    }
                    case http_status::blomp_session: vm.form_->error(-1, "Not connected to Blomp"); break;
                    default: vm.form_->error(-1, "Error fetching profile"); break;
                    }
                });
            });
        }

        void action(n4 element_id) final;
        
        void back() final
        {
            
        }
        
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return true; }
    };
}
