#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct create_folder_viewmodel : FormViewmodel, std::enable_shared_from_this<create_folder_viewmodel>
    {
        create_folder_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, n4 folder_id)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        folder_id_(folder_id)
        {
        }
        virtual ~create_folder_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        n4 folder_id_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            folder_name = 1,
            cancel,
            create
        };
        
        static constexpr n4 id_folder_name = static_cast<n4>(control::folder_name);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_create = static_cast<n4>(control::create);
        
        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = center(medium
                                 (
                                  {
                                      fld(row++).label("Enter the name of the folder you want to create").w(-500),
                                      fld(row++).input(id_folder_name, "Folder Name").w(-400),
                                      fld(row++).button(id_create, "Create").w(-300),
                                      fld(row++).button(id_cancel, "Cancel").w(-300).opt()
                                  }
                                  ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            form_ = form;
            view->set_screen_name("Create Folder");
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
                case control::create:
                {
                    auto folder_name = form_->input_get(id_folder_name);
                    services_.folder_create(folder_id_, folder_name, [ui_cb = ui_cb_, wself = std::weak_ptr<create_folder_viewmodel>(shared_from_this())](folder_create_result result)
                    {
                        run_ui(ui_cb, [wself, result]()
                        {
                            auto sself = wself.lock();
                            if (!sself)
                                return;

                            switch (result)
                            {
                                case folder_create_result::success: sself->form_->close(); return;
                                case folder_create_result::contain_alpha: sself->form_->error(id_folder_name, "Enter a name containing an alphabetic character"); return;
                                default: sself->form_->error(id_create, "Failed to create folder"); return;
                            }

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
        bool holdon_to_vm() final { return false; }
    };
}
