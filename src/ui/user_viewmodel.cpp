#include "user_viewmodel.h"
#include "signin_viewmodel.h"
#include "user/edit_profile_viewmodel.h"
#include "user/change_password_viewmodel.h"
#include "user/change_forumname_viewmodel.h"

namespace ai
{
    void user_viewmodel::action(n4 element_id)
    {
        if (!form_)
            return;
        form_->error(element_id, "");
        switch (static_cast<control>(element_id))
        {
            case control::sign_out:
            {
                if (!view_)
                    return;
                //                    auto vm = std::make_shared<about_viewmodel>(ui_cb_, screentype_, services_);
                //                    view_->create_form(vm);
                services_.sign_out([ui_cb = ui_cb_, wself = std::weak_ptr<user_viewmodel>(shared_from_this())]()
                {
                    run_ui(ui_cb, [wself]()
                    {
                        auto sself = wself.lock();
                        if (!sself)
                            return;

                        std::make_shared<signin_viewmodel>(sself->ui_cb_, sself->screentype_, sself->services_, sself->log_, sself->main_menu_, "")->show();
                    });

                });
                return;
            }
            case control::send_invite:
            {
                main_menu_->send_invite();
                break;
            }
            case control::edit_profile:
            {
                auto vm = std::make_shared<edit_profile_viewmodel>(ui_cb_, screentype_, services_, profile_);
                view_->push_form(vm);
                break;
            }
            case control::change_password:
            {
                view_->push_form(std::make_shared<change_password_viewmodel>(ui_cb_, screentype_, services_));
                break;
            }
            case control::change_forumname:
            {
                view_->push_form(std::make_shared<change_forumname_viewmodel>(ui_cb_, screentype_, services_));
                break;
            }
            default: break;
        }
        
    }

}
