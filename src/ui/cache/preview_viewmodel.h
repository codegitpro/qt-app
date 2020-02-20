#pragma once
#include "../viewmodel.h"

namespace ai
{
    struct preview_viewmodel : PreViewmodel, std::enable_shared_from_this<preview_viewmodel>
    {
        preview_viewmodel(const sptr<UiCallback>& view, ScreenType type, services& services, n4 file_id, str file_name, const sptr<FolderView>& folder_view)
        :
        view_(view),
        screentype_(type),
        services_(services),
        file_id_(file_id),
        file_name_(file_name),
        folder_view_(folder_view)
        {
        }
        virtual ~preview_viewmodel()
        {
        }
        void show(funk<void()> fun)
        {
            services_.file_path(file_id_, [wself = std::weak_ptr<preview_viewmodel>(shared_from_this()), fun](FileType type, str path)
            {
                auto sself = wself.lock();
                if (!sself)
                    return;
                switch (type)
                {
                    case FileType::IMAGE: sself->folder_view_->create_photo_preview(sself, sself->file_name_, path); break;
                    case FileType::VIDEO: sself->folder_view_->create_video_preview(sself, sself->file_name_, path); break;
                    case FileType::TEXT: sself->folder_view_->create_text_preview(sself, sself->file_name_, path); break;
                    default: return;
                }
                fun();
            });
        }

    private:
        sptr<UiCallback> view_;
        ScreenType screentype_;
        services& services_;
        n4 file_id_;
        str file_name_;
        sptr<FolderView> folder_view_;

        void file_delete() final
        {
            services_.schedule_batch_delete({}, {file_id_});
        }
    };
}
