#include "root_viewmodel.h"
#include "user_viewmodel.h"
#include "settings_viewmodel.h"
#include "plan_viewmodel.h"
#include "signin_viewmodel.h"
#include "cache/folder_subviewmodel.h"

namespace ai
{
    std::shared_ptr<RootViewmodel> RootViewmodel::create
    (
        const std::shared_ptr<Worker> & scheduler,
        const std::string & app_data_location,
        int32_t thumb_px,
        const std::shared_ptr<OsIo> & io,
        const std::shared_ptr<View> & view,
     ScreenType type,
     const sptr<UiCallback>& ui_callback,
     const sptr<Diagnostic>& log
    )
    {
        return std::make_shared<root_viewmodel>(scheduler, app_data_location, thumb_px, io, view, type, ui_callback, log);
    }

    root_viewmodel::root_viewmodel
    (
     const std::shared_ptr<Worker> & scheduler,
     const std::string & app_data_location,
     int32_t thumb_px,
     const std::shared_ptr<OsIo> & io,
     const std::shared_ptr<View> & view,
     ScreenType type,
     const sptr<UiCallback>& ui_callback,
     const sptr<Diagnostic>& log
    )
        :
        worker_(scheduler),
        io_(io),
        view_(view),
        screentype_(type),
        ui_callback_(ui_callback),
        services_(scheduler, app_data_location, io, ui_callback),
        log_(log)
    {
        services_.read_credentials([this](str email, str password)
        {
            io_->log(LogType::LDEBUG, __LINE__, __FILE__, "email pwd -> " + email + ":" + password);
            run_ui(ui_callback_, [this, email, password]()
            {
                if (email.empty() || password.empty())
                {
                    std::make_shared<signin_viewmodel>(ui_callback_, screentype_, services_, *log_, view_, email)->show();
                    return;
                }
                
                std::make_shared<loading_subviewmodel>(ui_callback_, screentype_, services_, *log_, view_, email, password, [this, email](bool success)
                {
                    if (!success)
                        std::make_shared<signin_viewmodel>(ui_callback_, screentype_, services_, *log_, view_, email)->show();

                })->show();

//                services_.login(email, password, false, [this, email](str err)
//                {
//                    io_->log(LogType::LDEBUG, __LINE__, __FILE__, err.empty() ? "login success" : ("login fail " + err));
//
//                    run_ui(view_, [this, email, err]()
//                    {
//                        if (!err.empty())
//                        {
//                            active_viewmodel_ = std::make_shared<signin_viewmodel>(view_, screentype_, services_, email);
//                            active_viewmodel_->show();
//                            return;
//                        }
//
////                        services_.read_or_create_local_cache([this](bool exist)
////                        {
////                            run_ui(view_, [this, exist]()
////                            {
////
////                            //                            view_->init(shared_from_this(), {});
////                            //                            active_viewmodel_ = viewmodels_[0];
////                            //                            active_viewmodel_->show();
////                            });
////                        });
//                    });
//
//                });
            });

        });
        
    }
    
}
