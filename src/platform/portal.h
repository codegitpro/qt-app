#pragma once

#include "../bridge/cpp/portal.hpp"
#include "../bridge/cpp/screen_type.hpp"
#include "../general.h"
#include "device_platform.h"
#include "../caching_engine.h"
#include "lambda_scheduler.h"
#include "../bridge/cpp/worker.hpp"
#include "../directory.h"
#include "../ui/viewmodel.h"

namespace ai
{
	struct portal : Portal, cache_callback
	{
        portal(const sptr<Worker>&, const str&, n4 thumb_px, const sptr<OsIo>&, const sptr<OsGui>& gui, ScreenType type);
		virtual ~portal();

		void create_folder(n4 parent_folder_id, const str& folder_name, const sptr<OsForm>& form) final;
        void rename_folder(n4 folder_id, const str& new_name, const sptr<OsForm>& form) final;
        void rename_file(n4 file_id, const str& new_name, const sptr<OsForm>& form) final;
        

		str ping() final;
        void watch_directory(n4 id, const sptr<DirectoryView>&) final;
		void unwatch_directory(n4 id) final;
		void clear_cache() final;

		void download_all(const arr<n4>& folder_ids, const arr<n4>& file_ids) final;
		void save_all(const str& target_folder, const arr<n4>& folder_ids, const arr<n4>& file_ids) final;
		void delete_all(const arr<n4>& folder_ids, const arr<n4>& file_ids) final;
		void upload_all(n4 target_folder_id, const str& common_folder, const arr<str>& relative_file_paths) final;
        void upload_files_only(n4 target_folder_id, const arr<str>& filenames, const arr<str>& paths, const arr<arr<p1>>& thumbnails) final;
		void preview(int32_t file_id) final;
		void cancel_network_operations() final;
		void sign_out() final;
		void about() final;

		void reset() final;

        void folder_change(n4 folder_id, const directory&) final;
		void folder_state_changes(arr<n4>& folder_ids, const directory&) final;

        void file_change(n4 file_id, const directory&) final;
        void report(severity, const str&) final;
        void show_message(const str& message);
    private:
		sptr<OsGui> gui_;
        lambda_scheduler scheduler_;
        device_platform platform_;
        cache cache_;
		ScreenType screen_type_;

		hmap<n4, sptr<DirectoryView>> views_;
        sz filename_count_ = 0;
        sz dirname_count_ = 0;

		sptr<DirectoryView> find_view(n4 folder_id);
		arr<par<n4, sptr<DirectoryView>>> find_views(const arr<n4>& folder_ids);
		std::mutex mutex_;
    };
}
