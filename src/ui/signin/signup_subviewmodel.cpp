#include "signup_subviewmodel.h"
#include "signup_success_vm.h"

namespace ai
{
    void signup_subviewmodel::action(n4 element_id) 
    {
        if (!form_)
            return;
        form_->input_set(id_busy, "");
        if (id_forum_name == element_id)
            form_->input_set(id_availibility, "");
        
        if (!validate(form_, fields_, element_id))
            return;
        
        switch (static_cast<control>(element_id))
        {
            case control::register_button:
            {
                str pwd = form_->input_get(id_password);
                str confirm_pwd = form_->input_get(id_confirm_password);
                if (pwd != confirm_pwd)
                {
                    form_->error(id_register_button, "Passwords must match");
                    return;
                }
                str terms = form_->input_get(id_terms);
                if ("true" != terms)
                {
                    form_->error(id_register_button, "Please accept the terms");
                    return;
                }
                str forum_name = form_->input_get(id_forum_name);
                str email = form_->input_get(id_email);
                str name = form_->input_get(id_name);
                form_->input_set(id_busy, "Contacting Blomp...");
                services_.signup(email, name, forum_name, pwd, [ui_cb = ui_cb_, wself = std::weak_ptr<signup_subviewmodel>(shared_from_this()), email](auto status, str message)
                {
                    run_ui(ui_cb, [wself, status, email, message]()
                    {
                        auto sself = wself.lock();
                        if (!sself) return;
                        auto& m = *sself;
                        
                        m.form_->input_set(id_busy, "");
                        switch (status)
                        {
                        case http_status::success:
                        {
//                            std::make_shared<signin_viewmodel>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_, email)->show();
                            auto vm = std::make_shared<signup_success_vm>(m.ui_cb_, m.screentype_, m.services_, m.log_, m.main_menu_, email);
                            m.current_view_->replace_form(vm);
                            break;
                        }
                        case http_status::not_found:
                        {
                            m.form_->error(id_register_button, "The email or name already exists");
                            break;
                        }
                        case http_status::bad_request:
                        {
                            m.form_->error(id_register_button, message.empty() ? "Registration failed" : message);
                            break;
                        }
                        default:
                        {
                            m.form_->error(id_register_button, "Registration failed");
                            break;
                        }
                        }
                    });
                });
                
                break;
            }
            case control::check_availability:
            {
                str forum_name = form_->input_get(id_forum_name);
                if (forum_name.empty())
                {
                    form_->error(id_forum_name, "Please enter a forum name");
                    return;
                }
                form_->input_set(id_availibility, "Checking availability...");
                services_.is_forum_name_availible(form_->input_get(id_forum_name), [ui_cb = ui_cb_, wself = std::weak_ptr<signup_subviewmodel>(shared_from_this())](auto status, str message)
                {
                    run_ui(ui_cb, [wself, message, status]()
                    {
                        auto sself = wself.lock();
                        if (!sself) return;
                        auto& m = *sself;

                        m.form_->input_set(id_availibility, message);
                    });
                });
                
                return;
            }
            default: break;
        }
    }
}
