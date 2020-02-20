#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct rename_folder_viewmodel : FormViewmodel, std::enable_shared_from_this<rename_folder_viewmodel>
    {
        rename_folder_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, n4 folder_id, str folder_name)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        folder_id_(folder_id),
        folder_name_(folder_name)
        {
        }
        virtual ~rename_folder_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        n4 folder_id_;
        str folder_name_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            rename = 1,
            cancel,
            folder_name
        };
        
        static constexpr n4 id_rename = static_cast<n4>(control::rename);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_folder_name = static_cast<n4>(control::folder_name);
        
        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = center(medium
                                 (
                                  {
                                      fld(row++).label("Change the name of the folder and tap the Rename button. Renaming a folder may take some time as all the files in the folder need to be moved to the new folder.").w(-500),
                                      fld(row++).input(id_folder_name, "Folder Name").w(-400),
                                      fld(row++).button(id_rename, "Rename").w(-300),
                                      fld(row++).button(id_cancel, "Cancel").w(-300).opt()
                                  }
                                  ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            form_ = form;
            form_->input_set(id_folder_name, folder_name_);
            view->set_screen_name("Rename Folder");
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
                case control::rename:
                {
                    auto name = form_->input_get(id_folder_name);
                    if (name == folder_name_)
                    {
                        form_->close();
                        return;
                    }
                    services_.folder_rename(folder_id_, name, [ui_cb = ui_cb_, wself = std::weak_ptr<rename_folder_viewmodel>(shared_from_this())](folder_rename_result result)
                    {
                        run_ui(ui_cb, [wself, result]()
                        {
                            auto sself = wself.lock();
                            if (!sself)
                                return;

                            switch (result)
                            {
                                case folder_rename_result::success: sself->form_->close(); return;
                                case folder_rename_result::not_found: sself->form_->error(id_rename, "No eligible files found to rename"); return;
                                default: sself->form_->error(id_rename, "Failed to rename folder"); return;
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
