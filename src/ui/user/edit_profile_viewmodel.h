#pragma once
#include "../viewmodel.h"
#include "../../directory.h"

namespace ai
{
    struct edit_profile_viewmodel : FormViewmodel, std::enable_shared_from_this<edit_profile_viewmodel>
    {
        edit_profile_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, blomp_session::user_profile profile)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        profile_(profile)
        {
        }
        virtual ~edit_profile_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        blomp_session::user_profile profile_;
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            name = 1,
            email,
            address,
            phone,
            company,
//            forum_username,
            registration_date,
            save,
            cancel,
            loading
        };
        
        static constexpr n4 id_name = static_cast<n4>(control::name);
        static constexpr n4 id_email = static_cast<n4>(control::email);
        static constexpr n4 id_address = static_cast<n4>(control::address);
        static constexpr n4 id_phone = static_cast<n4>(control::phone);
        static constexpr n4 id_company = static_cast<n4>(control::company);
//        static constexpr n4 id_forum_username = static_cast<n4>(control::forum_username);
        static constexpr n4 id_registration_date = static_cast<n4>(control::registration_date);
        static constexpr n4 id_save = static_cast<n4>(control::save);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_loading = static_cast<n4>(control::loading);


        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = left(medium(
            {
                fld(row++).label("Loading...").idd(id_loading).center().large(),
                fld(row++).label(profile_.email).idd(id_email).w(-300),
                fld(row++).label(profile_.forum_username).w(-300),
                fld(row++).input(id_name, "Name").w(-300),
                fld(row++).multiline(id_address, "Address").w(-300),
                fld(row++).input(id_phone, "Phone").phone().w(-300),
                fld(row++).input(id_company, "Company").w(-300),
                fld(row++).label(profile_.registration_date).idd(id_registration_date).w(-300).small(),
                fld(row++).button(id_save, "Save").w(-300).opt(),
                fld(row++).button(id_cancel, "Cancel").w(-300).opt()
            }));
            return fields;
        }
        
        void populate()
        {
            form_->input_set(id_name, profile_.name);
            form_->input_set(id_address, profile_.address);
            form_->input_set(id_phone, profile_.phone);
            form_->input_set(id_company, profile_.company);
//            form_->input_set(id_forum_username, profile_.forum_username);
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("Edit Profile");

            populate();
            
            form_->enable(-1, false);
            services_.get_user_profile([ui_cb = ui_cb_, wself = std::weak_ptr<edit_profile_viewmodel>(shared_from_this())](auto&& status, auto&& profile)
            {
                run_ui(ui_cb, [wself, status, profile]()
                {
                    auto sself = wself.lock();
                    if (!sself) return;
                    auto& vm = *sself;
                    
                    switch (status)
                    {
                    case http_status::success:
                    {
                        vm.populate();
                        vm.profile_ = profile;
                        break;
                    }
                    case http_status::blomp_session: vm.form_->error(-1, "Not connected to Blomp"); break;
                    default: vm.form_->error(-1, "Error fetching your Blomp profile"); break;
                    }

                    vm.form_->input_set(id_loading, "");
                    vm.form_->enable(-1, true);
                });
            });
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::cancel:
                {
                    form_->close();
                    return;
                }
                case control::save:
                {
                    form_->enable(-1, false);
                    profile_.name = form_->input_get(id_name);
//                    profile_.forum_username = form_->input_get(id_forum_username);
                    profile_.address = form_->input_get(id_address);
                    profile_.phone = form_->input_get(id_phone);
                    profile_.company = form_->input_get(id_company);
                    services_.update_profile
                    (
                        profile_.name,
                        profile_.forum_username,
                        profile_.address,
                        profile_.phone,
                        profile_.company,
                        [ui_cb = ui_cb_, wself = std::weak_ptr<edit_profile_viewmodel>(shared_from_this())](auto status)
                        {
                            run_ui(ui_cb, [wself, status]()
                            {
                                auto sself = wself.lock();
                                if (!sself) return;
                                auto& vm = *sself;
                                
                                switch (status)
                                {
                                case http_status::success: vm.form_->close(); return;
                                case http_status::blomp_session: vm.form_->error(-1, "Not connected to Blomp"); break;
                                default: vm.form_->error(id_save, "Failed to save profile"); break;
                                }

                                vm.form_->enable(-1, true);
                            });
                        }
                    );
                    return;
                }
                default: break;
            }
        }
        
        void back() final
        {
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }

    };
}
