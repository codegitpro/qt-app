#pragma once
#include "../viewmodel.h"
#include "../../directory.h"

namespace ai
{
    struct about_viewmodel : FormViewmodel, std::enable_shared_from_this<about_viewmodel>
    {
        about_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services)
        {
        }
        virtual ~about_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            cancel = 1
        };
        
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        
        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = center(medium
                                 (
                                  {
                                      fld(row++).label("Blomp offers the best free cloud file, photo, video and more storage.").w(-500),
                                      fld(row++).label("Version " + str(VERSION_TEXT)).w(-400),
                                      fld(row++).button(id_cancel, "Cancel").w(-300).opt()
                                  }
                                  ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("About");
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
