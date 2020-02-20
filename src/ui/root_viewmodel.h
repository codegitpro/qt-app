#pragma once

#include "../bridge/cpp/portal.hpp"
#include "../bridge/cpp/screen_type.hpp"
#include "../general.h"
#include "../platform/device_platform.h"
#include "../caching_engine.h"
#include "../bridge/cpp/worker.hpp"
#include "../directory.h"
#include "viewmodel.h"
#include "../bridge/cpp/root_viewmodel.hpp"
#include "../bridge/cpp/view.hpp"
#include "signin_viewmodel.h"
#include "loading/loading_subviewmodel.h"

namespace ai
{
    struct root_viewmodel : RootViewmodel, std::enable_shared_from_this<root_viewmodel>
    {
        root_viewmodel
        (
         const std::shared_ptr<Worker> & scheduler,
         const std::string & app_data_location,
         int32_t thumb_px,
         const std::shared_ptr<OsIo> & io,
         const std::shared_ptr<View> & view,
         ScreenType type,
         const sptr<UiCallback>& ui_callback,
         const sptr<Diagnostic>& log
         );
    private:
        sptr<Worker> worker_;
        std::shared_ptr<OsIo> io_;
        sptr<View> view_;
        ScreenType screentype_;
        sptr<UiCallback> ui_callback_;
        sptr<Diagnostic> log_;
        
        services services_;
        
        void create_main_menu();
    };
}
