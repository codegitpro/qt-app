#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct rename_file_subviewmodel : FormViewmodel, std::enable_shared_from_this<rename_file_subviewmodel>
    {
        rename_file_subviewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, n4 file_id, str file_name)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        file_id_(file_id),
        file_name_(file_name)
        {
        }
        virtual ~rename_file_subviewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        n4 file_id_;
        str file_name_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            rename = 1,
            cancel,
            file_name
        };
        
        static constexpr n4 id_rename = static_cast<n4>(control::rename);
        static constexpr n4 id_cancel = static_cast<n4>(control::cancel);
        static constexpr n4 id_file_name = static_cast<n4>(control::file_name);

        std::vector<Field> fields() final
        {
            int row = 0;
            
            auto fields = center(medium
            (
             {
                 fld(row++).label("Change the name of the file and tap the Rename button. Changing the file extension may prevent previewing the file.").w(-500),
                 fld(row++).input(id_file_name, "File Name").w(-400),
                 fld(row++).button(id_rename, "Rename").w(-300),
                 fld(row++).button(id_cancel, "Cancel").w(-300).opt()
             }
             ));
            return fields;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            form_ = form;
            form_->input_set(id_file_name, file_name_);
            view->set_screen_name("Rename File");
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
                    auto filename = form_->input_get(id_file_name);
                    if (filename == file_name_)
                    {
                        form_->close();
                        return;
                    }
                    services_.file_rename(file_id_, filename, [ui_cb = ui_cb_, wself = std::weak_ptr<rename_file_subviewmodel>(shared_from_this())](file_rename_result result)
                    {
                        run_ui(ui_cb, [wself, result]()
                        {
                            auto sself = wself.lock();
                            if (!sself)
                                return;

                            switch (result)
                            {
                                case file_rename_result::success: sself->form_->close(); return;
                                case file_rename_result::exist: sself->form_->error(id_rename, "File exists"); return;
                                default: sself->form_->error(id_rename, "Failed to rename file"); return;
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
