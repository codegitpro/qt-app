#pragma once
#include "../viewmodel.h"
#include "../../directory.h"

namespace ai
{
    struct change_forumname_viewmodel : FormViewmodel, std::enable_shared_from_this<change_forumname_viewmodel>
    {
        change_forumname_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services)
        {
        }
        virtual ~change_forumname_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            name = 1,
            change,
            cancel,
            busy,
            current_forumname,
            current_forumname_label
        };
        
        static constexpr n4 id_name = static_cast<n4>(control::name);
        static constexpr n4 id_change = static_cast<n4>(control::change);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_busy = static_cast<n4>(control::busy);
        static constexpr n4 id_current_forumname = static_cast<n4>(control::current_forumname);
        static constexpr n4 id_current_forumname_label = static_cast<n4>(control::current_forumname_label);

        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = left(medium(
            {
                fld(row++).label("").idd(id_busy).light_grey(),
                fld(row++).label("").light_grey().idd(id_current_forumname_label),
                fld(row++).label("").idd(id_current_forumname).purple(),
                fld(row++).input(id_name, "Forum Name").w(-300),
                fld(row++).button(id_change, "Change").w(-300),
                fld(row++).button(id_cancel, "Cancel").w(-300).opt()
            }
            ));
            return fields;
        }
        
        void refresh_forum_name()
        {
            if (!form_)
                return;
            
            form_->input_set(id_current_forumname_label, "Fetching forum name...");
            services_.get_user_profile([ui_cb = ui_cb_, wself = wptr<change_forumname_viewmodel>(shared_from_this())](auto status, auto profile)
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
                        if (profile.forum_username.empty())
                            vm.form_->input_set(id_current_forumname_label, "You do not currently have a forum name");
                        else
                        {
                            vm.form_->input_set(id_current_forumname, profile.forum_username);
                            vm.form_->input_set(id_current_forumname_label, "");
                        }
                        return;
                    }
                    case http_status::blomp_session: vm.form_->error(-1, "Not connected to Blomp"); break;
                    default: vm.form_->error(-1, "Error fetching your Blomp profile"); break;
                    }
                    vm.form_->input_set(id_current_forumname_label, "Failed to fetch forum name");
                });
            });
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("Forum Name");
            
            refresh_forum_name();
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::name:
                {
                    break;
                }
                case control::cancel:
                {
                    form_->close();
                    return;
                }
                case control::change:
                {
                    str name = form_->input_get(id_name);
                    form_->input_set(id_busy, "Changing Forum Name...");
                    form_->enable(id_change, false);
                    services_.change_forumname(name, [name, ui_cb = ui_cb_, wself = std::weak_ptr<change_forumname_viewmodel>(shared_from_this())](auto&& status)
                    {
                        run_ui(ui_cb, [wself, status, name]()
                        {
                            auto sself = wself.lock();
                            if (!sself) return;
                            auto& m = *sself;

                            m.form_->enable(id_change, true);
                            m.form_->input_set(id_busy, "");
                            switch (status)
                            {
                            case http_status::success: m.refresh_forum_name(); break;
                            case http_status::blomp_session: m.form_->error(-1, "Not connected to Blomp"); break;
                            default: m.form_->error(-1, "Failed to change forum name"); break;
                            }
                        });
                    });
                    

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

