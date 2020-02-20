#pragma once
#include "../viewmodel.h"
#include "../../directory.h"

namespace ai
{
    struct viewlog_viewmodel : FormViewmodel, std::enable_shared_from_this<viewlog_viewmodel>
    {
        viewlog_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, const std::shared_ptr<View> & mainview)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        mainview_(mainview)
        {
        }
        virtual ~viewlog_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        std::shared_ptr<View> mainview_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            copy_to_clipboard = 1,
            log_content
        };
        
        static constexpr n4 id_copy_to_clipboard = static_cast<n4>(control::copy_to_clipboard);
        static constexpr n4 id_log_content = static_cast<n4>(control::log_content);

        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = left(small(
            {
                fld(row++).button(id_copy_to_clipboard, "Copy to Clipboard").center().medium(),
                fld(row++).label("").idd(id_log_content)
            }));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            form_ = form;
            view->set_screen_name("Log");
            auto content = services_.log_content();
            form->enable(id_copy_to_clipboard, content.empty() ? "false" : "true");
            form->input_set(id_log_content, content);
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::copy_to_clipboard:
                {
                    mainview_->copy_to_clipboard(services_.log_content());
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

