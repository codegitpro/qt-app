#pragma once
#include "general.h"
#include "platform/task_scheduler.h"
#include "platform/plat.h"
#include "directory.h"
#include "openstack/openstack.h"
#include <condition_variable>
#include "bridge/cpp/os_gui.hpp"
#include "bridge/cpp/os_form.hpp"
#include "bridge/cpp/log_type.hpp"

#include <deque>

namespace ai
{
	struct file
	{
		str name;
		str size;
		str last_modified;
	};

	struct dir
	{
		str name;
	};

	enum class freshness
	{
		no_cache,
		cache
	};

	enum class severity
	{
		message,
		error,
		critical
	};

	static str to_str(severity s)
	{
		switch (s)
		{
		case severity::error: return "Error";
		case severity::critical: return "Critical";
		default: return "";
		}
	}

	struct cache_callback
	{
		virtual ~cache_callback() {}
		virtual void reset() = 0;
		virtual void folder_change(n4 folder_id, const directory&) = 0;
		virtual void folder_state_changes(arr<n4>& folder_ids, const directory&) = 0;
		virtual void file_change(n4 file_id, const directory&) = 0;
		virtual void report(severity, const str&) = 0;
	};

	struct cache
	{
		enum class status
		{
			success,
			error
		};
		void login(const str& email, const str& pwd, bool remember_me, funk<void(str)> fun);
		void create_folder(n4 parent_folder_id, const str& folder_name, const sptr<OsForm>& form);
		void register_callback(cache_callback&);
		void refresh_folder(n4);

        void rename_file(n4, str, const sptr<OsForm>& form);
        void rename_folder(n4, str, const sptr<OsForm>& form);
        
		void download_all(const arr<n4>& folder_ids, const arr<n4>& file_ids);
		void save_all(const str& target_folder, const arr<n4>& folder_ids, const arr<n4>& file_ids);
		void delete_all(const arr<n4>& folder_ids, const arr<n4>& file_ids);
		void upload_all(n4 target_folder_id, const str& common_folder, const arr<str>& relative_file_paths);
        void upload_files_only(n4 target_folder_id, const arr<str>& filenames, const arr<str>& paths, arr<arr<p1>> thumbnails);
		void preview(n4 file_id);
		void list(n4);
		void list_directory(const str& path, freshness, funk<void(status, const arr<file>&, const arr<dir>&)>);
		cache(n4 thumb_px, task_scheduler& scheduler, plat& platform, sptr<OsGui> gui);
		~cache();
		void finish_and_wait()
		{
			platform_.finish_io_and_wait();
		}
		void cancel_network_operations();
		void sign_out();
		str version_text() const;
	private:
		uptr<directory> dir_;
        n4 thumb_px_;
		plat& platform_;
		sptr<OsGui> gui_;
		uptr<openstack::session> session_;
		int directory_version_ = 0;
		bool saving_ = false;
		str email_;
		std::deque<funk<void(directory*)>> tasks_;
		n8 netid_ = 0;
		std::mutex net_mutex_;
		hmap<n8, sptr<CancellationToken>> netops_;
		std::mutex task_mutex_;
		std::condition_variable task_cv_;
		bool stopping_ = false;
        
		int counter_ = 0;
		cache_callback* callback_ = nullptr;
		str content_path_;

		void add_task(funk<void(directory*)> fun);
		void report_message(severity s, const str& msg);
		void report_folder_change(n4 id);
		void load_fresh_folder(n4, funk<void(directory* dir)>);
		void save_cache(directory* dir);

		void report_thumbnail(n4 file_id, p16 md5, n4 percentage, FileState state, directory* dir, bool fallback_only);
		void save();
		void direct_download(const arr<n4>& file_ids, directory* dir);
		void direct_delete(const arr<n4>& file_ids, directory* dir);

		void net_add(const sptr<CancellationToken>&);
		void net_remove(n8);
		sptr<CancellationToken> net_cancellation(n8);
		n8 net_nextid();
		void read_safely(const str&, funk<bool(arr<p1>)>);
        void direct_upload(directory* dir, n4 folder_id, const str& filename, const str& path, arr<p1> thumbnail);
        str hash_path(p16, const str& filename_or_path);
        void log(const char* file, int line, const str&, LogType type = LogType::LDEBUG);
        static bool is_image_ext(const str& ext);
        static FileType type_from_path(const str&);
        sz direct_cancel_network_operations(directory& dir);
        void direct_rename_file(directory& dir, n4 old_file_id, n4 new_file_id);
	};
}
