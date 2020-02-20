#pragma once
#include "../viewmodel.h"
#include "../../directory.h"

namespace ai
{
    struct change_password_viewmodel : FormViewmodel, std::enable_shared_from_this<change_password_viewmodel>
    {
        change_password_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services)
        {
        }
        virtual ~change_password_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        
        sptr<GenericForm> form_;
        bool is_dirty_ = false;
        
        enum class control : n4
        {
            old_pwd = 1,
            new_pwd,
            confirm_pwd,
            change,
            cancel,
            busy
        };
        
        static constexpr n4 id_old_pwd = static_cast<n4>(control::old_pwd);
        static constexpr n4 id_new_pwd = static_cast<n4>(control::new_pwd);
        static constexpr n4 id_confirm_pwd = static_cast<n4>(control::confirm_pwd);
        static constexpr n4 id_change = static_cast<n4>(control::change);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_busy = static_cast<n4>(control::busy);

        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = left(medium(
            {
                fld(row++).label("").idd(id_busy).light_grey(),
                fld(row++).label("Current Password"),
                fld(row++).input(id_old_pwd, "Current Password").password().w(-300),
                fld(row++).label("New Password"),
                fld(row++).input(id_new_pwd, "New Password").password().w(-300),
                fld(row++).label("Confirm password"),
                fld(row++).input(id_confirm_pwd, "Confirm Password").password().w(-300),
                fld(row++).button(id_change, "Change").w(-300),
                fld(row++).button(id_cancel, "Cancel").w(-300).opt()
            }
            ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("Change Password");
            form_->enable(id_change, is_dirty_);
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::old_pwd:
                case control::new_pwd:
                case control::confirm_pwd:
                {
                    is_dirty_ = true;
                    break;
                }
                case control::cancel:
                {
                    form_->close();
                    return;
                }
                case control::change:
                {
                    str old_pwd = form_->input_get(id_old_pwd);
                    str new_pwd = form_->input_get(id_new_pwd);
                    str confirm_pwd = form_->input_get(id_confirm_pwd);
                    if (new_pwd != confirm_pwd)
                    {
                        form_->error(id_confirm_pwd, "Passwords must match");
                        return;
                    }
                    if (old_pwd == new_pwd)
                    {
                        form_->error(id_new_pwd, "Password is unchanged");
                        return;
                    }
                    form_->input_set(id_busy, "Changing Password...");
                    form_->enable(id_change, false);
                    services_.change_password(old_pwd, new_pwd, [ui_cb = ui_cb_, wself = std::weak_ptr<change_password_viewmodel>(shared_from_this())](auto&& status)
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
                            default: vm.form_->error(-1, "Failed to change password"); break;
                            }
                            vm.form_->input_set(id_busy, "");
                            vm.form_->enable(id_change, true);
                        });
                    });
                            

                    return;
                }
                default: break;
            }
            form_->enable(id_change, is_dirty_);
        }
        
        void back() final
        {
        }
        
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }

    };
}
