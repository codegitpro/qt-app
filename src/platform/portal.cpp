#include "portal.h"
#include <iostream>
#include "../bridge/cpp/directory_view.hpp"
#include "../bridge/cpp/file_type.hpp"
#include "../bridge/cpp/directories_detail.hpp"
#include "../bridge/cpp/files_detail.hpp"
#include "../bridge/cpp/file_state.hpp"
#include "../bridge/cpp/os_form.hpp"
#include "../bridge/cpp/status.hpp"
#include "../bridge/cpp/message.hpp"
#include "../gui/about_form.h"

using namespace std;

namespace ai
{
	sptr<Portal> Portal::create(const sptr<Worker> & w, const str& app_data_location, n4 thumb_px, const sptr<OsIo>& os, const sptr<OsGui>& gui, ScreenType type)
	{
        return make_shared<portal>(w, app_data_location, thumb_px, os, gui, type);
	}

    portal::portal(const sptr<Worker>& w, const str& s, n4 thumb_px, const sptr<OsIo>& os, const sptr<OsGui>& gui, ScreenType type) 
		: 
		gui_(gui), 
		scheduler_(w), 
		platform_(s, os),
		cache_(thumb_px, scheduler_, platform_, gui),
		screen_type_(type)
	{
		cache_.register_callback(*this);
	}

	portal::~portal() {}

	void portal::create_folder(n4 parent_folder_id, const str& folder_name, const sptr<OsForm>& form)
	{
		cache_.create_folder(parent_folder_id, folder_name, form);
	}

    void portal::rename_folder(n4 folder_id, const str& new_name, const sptr<OsForm>& form)
    {
        cache_.rename_folder(folder_id, new_name, form);
    }
    
    void portal::rename_file(n4 file_id, const str& new_name, const sptr<OsForm>& form)
    {
        cache_.rename_file(file_id, new_name, form);
    }

	void portal::download_all(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		cache_.download_all(folder_ids, file_ids);
	}

	void portal::save_all(const str& target_folder, const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		cache_.save_all(target_folder, folder_ids, file_ids);
	}

	void portal::delete_all(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		cache_.delete_all(folder_ids, file_ids);
	}

	void portal::upload_all(n4 target_folder_id, const str& common_folder, const arr<str>& relative_file_paths)
	{
		cache_.upload_all(target_folder_id, common_folder, relative_file_paths);
	}

    void portal::upload_files_only(n4 target_folder_id, const arr<str>& filenames, const arr<str>& paths, const arr<arr<p1>>& thumbnails)
    {
        cache_.upload_files_only(target_folder_id, filenames, paths, thumbnails);
    }
    
	void portal::preview(n4 file_id)
	{
		cache_.preview(file_id);
	}

	void portal::cancel_network_operations()
	{
		cache_.cancel_network_operations();
	}

	void portal::sign_out()
	{
		cache_.sign_out();
	}

	void portal::about()
	{
		about_form::show(gui_, cache_.version_text());
	}

	sptr<DirectoryView> portal::find_view(n4 folder_id)
	{
		gate lck(mutex_);
		auto it = views_.find(folder_id);
		return views_.end() != it ? it->second : nullptr;
	}

	arr<par<n4, sptr<DirectoryView>>> portal::find_views(const arr<n4>& folder_ids)
	{
		auto r = arr<par<n4, sptr<DirectoryView>>>();
		gate lck(mutex_);
		for_each(folder_ids.cbegin(), folder_ids.cend(), [this, &r](auto folder_id)
		{
			auto it = views_.find(folder_id);
			if (views_.end() != it)
				r.push_back(make_pair(folder_id, it->second));
		});
		return r;
	}

	void portal::folder_state_changes(arr<n4>& folder_ids, const directory& dir)
	{
		auto views = find_views(folder_ids);
		for_each(views.cbegin(), views.cend(), [&dir](auto&& p)
		{
			n4 folder_id = p.first;
			auto view = p.second;

			auto files = dir.list_file_columns(folder_id);

			view->file_states_changed(dir.path_from_folder(folder_id), files.ids, files.states);
		});
	}

	void portal::reset()
	{
		views_.clear();

		filename_count_ = 0;
		dirname_count_ = 0;
	}

	void portal::folder_change(n4 folder_id, const directory& dir)
	{
		auto fun = [this, &dir](n4 folder_id, const sptr<DirectoryView>& view)
		{
			auto new_dirnames = arr<str>();
			auto& dirnames = dir.folder_segments();
			if (dirnames.size() > dirname_count_)
			{
				copy(dirnames.cbegin() + dirname_count_, dirnames.cend(), back_inserter(new_dirnames));
				dirname_count_ = dirnames.size();
			}

			auto new_filenames = arr<str>();
			auto& filenames = dir.file_segments();
			if (filenames.size() > filename_count_)
			{
				copy(filenames.cbegin() + filename_count_, filenames.cend(), back_inserter(new_filenames));
				filename_count_ = filenames.size();
			}

			auto dirs = dir.list_directory_columns(folder_id);
			auto files = dir.list_file_columns(folder_id);

			auto dirs_detail = DirectoriesDetail(dirs.ids, dirs.titles);
			auto files_detail = FilesDetail(files.ids, files.titles, files.sizes, files.percentages, files.states, files.thumbnails);
			view->refresh_list(dir.path_from_folder(folder_id), dirs_detail, files_detail, new_dirnames, new_filenames);
		};

		if (folder_id >= 0)
		{
			auto view = find_view(folder_id);
			if (!view)
				return;
			{
				gate lck(mutex_);
				fun(folder_id, view);
			}
			return;
		}

		{
			gate lck(mutex_);
			for_each(views_.cbegin(), views_.cend(), [fun](auto&& view_detail)
			{
				fun(view_detail.first, view_detail.second);
			});
		}
	}

	void portal::file_change(n4 file_id, const directory& dir)
	{
		auto folder_id = dir.file_parent(file_id);

		auto view = find_view(folder_id);
		if (!view)
			return;

		auto item = dir.file(file_id);
		view->file_changed
		(
			dir.path_from_folder(folder_id), 
			file_id, 
			dir.file_name_id(file_id), 
			item.size, 
			item.percentage, 
			item.state,
			item.thumbnail
		);
	}

	void portal::report(severity sev, const str& msg)
	{
		plat::log_debug(to_str(sev) + " " + msg);
	}

	str portal::ping()
	{
		return "hallo";
	}

	void portal::watch_directory(n4 id, const sptr<DirectoryView>& view)
	{
		if (!view)
			return;
		{
			gate lck(mutex_);
			views_[id] = view;
		}

		cache_.list(id);
	}

	void portal::unwatch_directory(n4 id)
	{
		gate lck(mutex_);
		views_.erase(id);
	}

	void portal::clear_cache()
	{
	}
    
    void portal::show_message(const str& message)
    {
        gui_->show_message(message);
    }
}
