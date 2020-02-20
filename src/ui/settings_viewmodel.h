#pragma once
#include "viewmodel.h"
#include "settings/about_viewmodel.h"
#include "settings/viewlog_viewmodel.h"

namespace ai
{
    struct settings_viewmodel : FormViewmodel, std::enable_shared_from_this<settings_viewmodel>
    {
        settings_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<View> & mainview)
            :
            ui_cb_(ui_cb),
            screentype_(type),
            services_(services),
            log_(log),
            mainview_(mainview)
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
        std::shared_ptr<View> mainview_;

        sptr<GenericForm> form_;
        sptr<FormView> view_;
        
        enum class control : n4
        {
            about = 1,
            cancel_network,
            cancel_result,
            logging,
            view_log
        };
        
        static constexpr n4 id_about = static_cast<n4>(control::about);
        static constexpr n4 id_cancel_network = static_cast<n4>(control::cancel_network);
        static constexpr n4 id_cancel_result = static_cast<n4>(control::cancel_result);
        static constexpr n4 id_logging = static_cast<n4>(control::logging);
        static constexpr n4 id_view_log = static_cast<n4>(control::view_log);

        std::vector<Field> fields() final
        {
            int row = 0;
            auto r = center(medium
            (
                {
                    fld(row++).label("Cancel downloads, uploads and delete operations currently in progress.").w(-500),
                    fld(row++).button(id_cancel_network, "Cancel All").w(-200),
                    fld(row++).label("We may ask you to help us identify issues with your app by enabling logging.").w(-500),
                    fld(row++).check(id_logging, "Enable Logging"),
                    fld(row++).button(id_view_log, "View Log").w(-200),
                    fld(row++).button(id_about, "About").w(-200)
                }
            ));
            
            return r;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view_ = view;
            view->set_screen_name("Settings");
            if (services_.logging_enabled())
            {
                form->input_set(id_logging, "true");
                form->enable(id_view_log, true);
            }
            else
            {
                form->input_set(id_logging, "false");
                form->enable(id_view_log, false);
            }
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::view_log:
                {
                    view_->push_form(std::make_shared<viewlog_viewmodel>(ui_cb_, screentype_, services_, mainview_));
                    return;
                }
                case control::logging:
                {
                    bool enable = form_->input_get(id_logging) == "true";
                    services_.enable_logging(enable);
                    form_->enable(id_view_log, enable);
                    return;
                }
                case control::about:
                {
                    if (!view_)
                        return;
                    auto vm = std::make_shared<about_viewmodel>(ui_cb_, screentype_, services_);
                    view_->push_form(vm);
                    return;
                }
                case control::cancel_network:
                {
                    services_.cancel_networkoperations([ui_cb = ui_cb_, wself = std::weak_ptr<settings_viewmodel>(shared_from_this())](sz count)
                    {
                        run_ui(ui_cb, [wself, count]()
                        {
                            auto sself = wself.lock();
                            if (!sself)
                                return;
                            
                            auto msg = [count]()
                            {
                                str r;
                                switch (count)
                                {
                                    case 0: r = "Not a single active task was found"; break;
                                    case 1: r = "One task was cancelled"; break;
                                    default: r = std::to_string(count) + " tasks were cancelled"; break;
                                }
                                return r;
                            }();

                            sself->form_->show_message(msg);
                        });

                    });
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
