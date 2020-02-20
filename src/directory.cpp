#include "directory.h"

using namespace std;

namespace ai
{
	directory::directory()
	{
		t_dir_segment_map_.add_if_not_exists("");
		parent_child_segment_to_dir_id_[0] = 0;

		f_dir_child_counts_.push_back(0);
		f_dir_file_counts_.push_back(0);
		f_dir_hidden_.push_back(0);

		f_dir_name_ids_.push_back(0);
		f_dir_parent_ids_.push_back(0);
	}

	sz directory::size_on_disk() const
	{
		auto r = f_dir_child_counts_.size_on_disk();
		r += f_dir_file_counts_.size_on_disk();
		r += f_dir_hidden_.size_on_disk();

		r += f_dir_parent_ids_.size_on_disk();
		r += f_dir_name_ids_.size_on_disk();
		r += t_dir_segment_map_.size_on_disk();
		r += size_on_disk_hmap(parent_child_segment_to_dir_id_);

		r += f_file_dir_ids_.size_on_disk();
		r += f_file_sizes_.size_on_disk();
		r += f_file_name_ids_.size_on_disk();
		r += f_file_states_.size_on_disk();
		r += f_file_percentages_.size_on_disk();
		r += t_file_segment_map_.size_on_disk();
		
		r += size_on_disk_arr(f_file_md5s_);
		r += size_on_disk_hmap(parent_child_segment_to_file_id_);

		r += size_on_disk_arr_arr(f_file_thumbnails_);
		return r;
	}

	sz directory::deserialise(p8 version, const p1* data)
	{
		auto r = f_dir_child_counts_.deserialise(data);
		r += f_dir_file_counts_.deserialise(data + r);
		if (version > 0)
			r += f_dir_hidden_.deserialise(data + r);
		else
		{
			for (sz i = 0; i < f_dir_child_counts_.size(); ++i)
				f_dir_hidden_.push_back(0);
		}

		r += f_dir_parent_ids_.deserialise(data + r);
		r += f_dir_name_ids_.deserialise(data + r);
		r += t_dir_segment_map_.deserialise(data + r);
		r += deserialise_hmap(data + r, parent_child_segment_to_dir_id_);

		r += f_file_dir_ids_.deserialise(data + r);
		r += f_file_sizes_.deserialise(data + r);
		r += f_file_name_ids_.deserialise(data + r);
		r += f_file_states_.deserialise(data + r);
		r += f_file_percentages_.deserialise(data + r);
		r += t_file_segment_map_.deserialise(data + r);

		r += deserialise_arr<p16>(data + r, f_file_md5s_);
		r += deserialise_hmap(data + r, parent_child_segment_to_file_id_);

		r += deserialise_arr_arr(data + r, f_file_thumbnails_);
		return r;
	}

	sz directory::serialise(p1* data) const
	{
		auto r = f_dir_child_counts_.serialise(data);
		r += f_dir_file_counts_.serialise(data + r);
		r += f_dir_hidden_.serialise(data + r);

		r += f_dir_parent_ids_.serialise(data + r);
		r += f_dir_name_ids_.serialise(data + r);
		r += t_dir_segment_map_.serialise(data + r);
		r += serialise_hmap(data + r, parent_child_segment_to_dir_id_);

		r += f_file_dir_ids_.serialise(data + r);
		r += f_file_sizes_.serialise(data + r);
		r += f_file_name_ids_.serialise(data + r);
		r += f_file_states_.serialise(data + r);
		r += f_file_percentages_.serialise(data + r);
		r += t_file_segment_map_.serialise(data + r);

		r += serialise_arr<p16>(data + r, f_file_md5s_);
		r += serialise_hmap(data + r, parent_child_segment_to_file_id_);

		r += serialise_arr_arr(data + r, f_file_thumbnails_);

		return r;
	}

	files_and_folders directory::expand(const arr<n4>& folder_ids, const arr<n4>& file_ids, funk<bool(n4)> file_is_valid_fun)
	{
		auto root_folder_ids = arr<n4>();
		for_each(folder_ids.cbegin(), folder_ids.cend(), [this, &folder_ids, &root_folder_ids](n4 folder_id)
		{
			if (folder_id)
			{
				for (auto parent_id = f_dir_parent_ids_[folder_id]; parent_id != 0; parent_id = f_dir_parent_ids_[parent_id])
				{
					if (find(folder_ids.cbegin(), folder_ids.cend(), parent_id) != folder_ids.cend())
						return;
				}
			}
			root_folder_ids.push_back(folder_id);
		});

		sort(root_folder_ids.begin(), root_folder_ids.end());
		root_folder_ids.erase(unique(root_folder_ids.begin(), root_folder_ids.end()), root_folder_ids.end());

		build_dir_index();

		{
			auto folders_to_expand = arr<n4>(root_folder_ids);
			while (!folders_to_expand.empty())
			{
				auto new_folders = arr<n4>();
				for_each(folders_to_expand.cbegin(), folders_to_expand.cend(), [this, &new_folders](auto parent_id)
				{
					for_each(id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id], id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id + 1], [this, &new_folders](auto folder_id)
					{
						if (f_dir_hidden_[folder_id])
							return;
						new_folders.push_back(folder_id);
					});
				});
				sort(new_folders.begin(), new_folders.end());
				folders_to_expand = new_folders;
				auto len = root_folder_ids.size();
				root_folder_ids.reserve(len + new_folders.size());
				copy(new_folders.cbegin(), new_folders.cend(), back_inserter(root_folder_ids));
				inplace_merge(root_folder_ids.begin(), root_folder_ids.begin() + len, root_folder_ids.end());
			}
		}

		build_file_index();

		auto r = arr<n4>();

        for_each(file_ids.cbegin(), file_ids.cend(), [&r, file_is_valid_fun](auto file_id)
		{
			if (file_is_valid_fun(file_id))
				r.push_back(file_id);
		});

		sort(r.begin(), r.end());
		r.erase(unique(r.begin(), r.end()), r.end());

		{
			for_each(root_folder_ids.cbegin(), root_folder_ids.cend(), [this, &r, file_is_valid_fun](auto parent_id)
			{
				auto new_file_ids = arr<n4>();
                for_each(id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id], id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id + 1], [&new_file_ids, file_is_valid_fun](auto file_id)
				{
					if (file_is_valid_fun(file_id))
						new_file_ids.push_back(file_id);
				});
				auto len = r.size();
				r.reserve(len + new_file_ids.size());
				copy(new_file_ids.cbegin(), new_file_ids.cend(), back_inserter(r));
				inplace_merge(r.begin(), r.begin() + len, r.end());
			});
		}

		auto ff = files_and_folders();
		if (r.empty())
			return ff;

		++version_;

		{
			auto specified_file_parent_ids = arr<n4>();
			specified_file_parent_ids.reserve(file_ids.size());
			transform(file_ids.cbegin(), file_ids.cend(), back_inserter(specified_file_parent_ids), [this](auto file_id)
			{
				return f_file_dir_ids_[file_id];
			});
			sort(specified_file_parent_ids.begin(), specified_file_parent_ids.end());
			specified_file_parent_ids.erase(unique(specified_file_parent_ids.begin(), specified_file_parent_ids.end()), specified_file_parent_ids.end());
            
			auto len = root_folder_ids.size();
			copy(specified_file_parent_ids.cbegin(), specified_file_parent_ids.cend(), back_inserter(root_folder_ids));
			inplace_merge(root_folder_ids.begin(), root_folder_ids.begin() + len, root_folder_ids.end());
			root_folder_ids.erase(unique(root_folder_ids.begin(), root_folder_ids.end()), root_folder_ids.end());
		}
		ff.file_ids = r;
		ff.folder_ids = root_folder_ids;
		return ff;

	}

	files_and_folders directory::expand_and_mark(const arr<n4>& folder_ids, const arr<n4>& file_ids, FileState mark, funk<bool(n4)> file_is_valid_fun)
	{
		auto ff = expand(folder_ids, file_ids, file_is_valid_fun);
		for_each(ff.file_ids.cbegin(), ff.file_ids.cend(), [this, mark](auto file_id)
		{
			f_file_states_.at(file_id) = static_cast<n4>(mark);
		});
		return ff;
	}

	files_and_folders directory::mark_for_download(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		auto file_is_valid = [this](n4 file_id)
		{
			auto state = static_cast<FileState>(f_file_states_[file_id]);
			return FileState::NORMAL == state && f_file_sizes_[file_id] && f_file_md5s_[file_id].first != 0;
		};

		return expand_and_mark(folder_ids, file_ids, FileState::DOWNLOADING, file_is_valid);
	}

	files_and_folders directory::mark_for_delete(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		auto file_is_valid = [this](n4 file_id)
		{
			auto state = static_cast<FileState>(f_file_states_[file_id]);
			return FileState::NORMAL == state || FileState::LOCAL == state;
		};

		return expand_and_mark(folder_ids, file_ids, FileState::DELETING, file_is_valid);
	}

	bool directory::empty()
	{
		return f_dir_name_ids_.size() < 2 && f_file_name_ids_.empty();
	}

    dir_item directory::dir(n4 folder_id) const
    {
        dir_item x;
        x.id = folder_id;
        x.name = t_dir_segment_map_[f_dir_name_ids_[folder_id]];
        return x;
    }
    
    file_item directory::file(n4 id) const
    {
        file_item x;
        x.id = id;
        x.name = t_file_segment_map_[f_file_name_ids_[id]];
        x.size = f_file_sizes_[id];
        x.md5 = f_file_md5s_[id];
        x.percentage = f_file_percentages_[id];
        x.state = static_cast<FileState>(f_file_states_[id]);
		x.thumbnail = f_file_thumbnails_[id];
        return x;
    }
    
	arr<file_item> directory::list_files(n4 parent_id) const
	{
		if (!(0 <= parent_id && parent_id <= f_dir_name_ids_.size()))
			return {};

		build_file_index();

		auto r = arr<file_item>();
		for_each(id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id], id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id + 1], [this, &r](auto id)
		{
			auto state = static_cast<FileState>(f_file_states_[id]);
			if (state != FileState::DELETED && (f_file_sizes_[id] || state == FileState::UPLOADING))
				r.push_back(file(id));
		});
		sort(r.begin(), r.end(), [](auto&& l, auto&& r) { return l.name < r.name; });
		return r;
	}

    arr<FileState> directory::states_for_file_ids(const arr<n4>& file_ids) const
    {
        auto r = arr<FileState>();
        r.reserve(file_ids.size());
        std::transform(file_ids.begin(), file_ids.end(), std::back_inserter(r), [this](n4 file_id) { return static_cast<FileState>(f_file_states_[file_id]); });
        return r;
    }

    arr<FileState> directory::list_file_states(n4 parent_id) const
    {
        if (!(0 <= parent_id && parent_id <= f_dir_name_ids_.size()))
            return {};
        
        build_file_index();
        
        auto r = arr<FileState>();
        
        {
            auto rr = arr<par<FileState, str>>();
            auto b = id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id];
            auto e = id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id + 1];
            for_each(b, e, [this, &rr](auto id)
            {
                auto state = static_cast<FileState>(f_file_states_[id]);
                if (state != FileState::DELETED && (f_file_sizes_[id] || state == FileState::UPLOADING))
                    rr.push_back({ state, t_file_segment_map_[f_file_name_ids_[id]] });
            });
            sort(rr.begin(), rr.end(), [](auto&& l, auto&& r) { return l.second < r.second; });
            transform(rr.cbegin(), rr.cend(), back_inserter(r), [](auto&& item) { return item.first; });
        }
        
        return r;
    }

    FilesDetail directory::list_file_columns(n4 parent_id) const
    {
        if (!(0 <= parent_id && parent_id <= f_dir_name_ids_.size()))
			return FilesDetail({}, {}, {}, {}, {}, {});
        
        build_file_index();

		auto r = FilesDetail({}, {}, {}, {}, {}, {});
        
        {
            auto rr = arr<dir_item>();
            auto b = id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id];
            auto e = id_file_per_directory_.cbegin() + mv_file_ranges_[parent_id + 1];
            for_each(b, e, [this, &rr](auto id)
            {
				auto state = static_cast<FileState>(f_file_states_[id]);
                if (state != FileState::DELETED && (f_file_sizes_[id] || state == FileState::UPLOADING))
                    rr.push_back({ id, t_file_segment_map_[f_file_name_ids_[id]] });
            });
            sort(rr.begin(), rr.end(), [](auto&& l, auto&& r) { return l.name < r.name; });
            transform(rr.cbegin(), rr.cend(), back_inserter(r.ids), [](auto&& item) { return item.id; });
        }

        {
            auto b = r.ids.cbegin();
            auto e = r.ids.cend();
			transform(b, e, back_inserter(r.titles), [this](auto id) { return f_file_name_ids_[id]; });
			transform(b, e, back_inserter(r.sizes), [this](auto id) { return f_file_sizes_[id]; });
			transform(b, e, back_inserter(r.percentages), [this](auto id) { return f_file_percentages_[id]; });
			transform(b, e, back_inserter(r.states), [this](auto id) { return static_cast<FileState>(f_file_states_[id]); });
			transform(b, e, back_inserter(r.thumbnails), [this](auto id) { return f_file_thumbnails_[id]; });
		}

        return r;
    }
    
	arr<dir_item> directory::list_directories(n4 parent_id) const
	{
		if (!(0 <= parent_id && parent_id <= f_dir_name_ids_.size()))
			return {};
		build_dir_index();

		auto r = arr<dir_item>();
		for_each(id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id], id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id + 1], [this, &r](auto id)
		{
			if (f_dir_hidden_[id])
				return;
			r.push_back({ id, t_dir_segment_map_[f_dir_name_ids_[id]] });
		});
		sort(r.begin(), r.end(), [](auto&& l, auto&& r) { return l.name < r.name; });
		return r;
	}

	n4 directory::parent_folder(n4 folder_id) const
	{
		return f_dir_parent_ids_[folder_id];
	}

	void directory::hide_folder(n4 folder_id)
	{
		f_dir_hidden_.at(folder_id) = 1;
		++version_;
	}

    DirectoriesDetail directory::list_directory_columns(n4 parent_id) const
    {
        if (!(0 <= parent_id && parent_id <= f_dir_name_ids_.size()))
            return DirectoriesDetail({}, {});
        build_dir_index();
        
        auto r = DirectoriesDetail({}, {});
        
        {
            auto rr = arr<dir_item>();
            auto b = id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id];
            auto e = id_dir_per_directory_.cbegin() + mv_dir_ranges_[parent_id + 1];
            for_each(b, e, [this, &rr](auto id)
            {
				if (f_dir_hidden_[id])
					return;
				rr.push_back({ id, t_dir_segment_map_[f_dir_name_ids_[id]] });
            });
            sort(rr.begin(), rr.end(), [](auto&& l, auto&& r) { return l.name < r.name; });
            transform(rr.cbegin(), rr.cend(), back_inserter(r.ids), [](auto&& item) { return item.id; });
        }
        
        transform(r.ids.cbegin(), r.ids.cend(), back_inserter(r.titles), [this](auto id) { return f_dir_name_ids_[id]; });
        
        return r;
    }

	p8 segment_key(n4 parent, n4 child)
	{
		p8 r = parent;
		r <<= 32;
		r |= child;
		return r;
	}

	n4 directory::file_from_name(n4 parent_dir_id, const str& name)
	{
		if (!(0 <= parent_dir_id && parent_dir_id <= f_dir_name_ids_.size()))
			return -1;

		auto segment_id = t_file_segment_map_.find_position(name);
		if (-1 == segment_id)
			return -1;
		auto key = segment_key(parent_dir_id, segment_id);
		auto key_it = parent_child_segment_to_file_id_.find(key);
		if (parent_child_segment_to_file_id_.end() == key_it)
			return -1;
		return key_it->second;
	}

	n4 directory::dir_from_name(n4 parent_dir_id, const str& name)
	{
		if (!(0 <= parent_dir_id && parent_dir_id <= f_dir_name_ids_.size()))
			return -1;

		auto segment_id = t_dir_segment_map_.find_position(name);
		if (-1 == segment_id)
			return -1;
		auto key = segment_key(parent_dir_id, segment_id);
		auto key_it = parent_child_segment_to_dir_id_.find(key);
		if (parent_child_segment_to_dir_id_.end() == key_it)
			return -1;
		return key_it->second;
	}

	n4 directory::dir_from_path(const str& path)
	{
		auto b = path.cbegin();
		auto e = find(b, path.cend(), '/');
		auto parent_dir_id = dir_from_name(0, str(b, e));
		for (; -1 != parent_dir_id && path.cend() != e; b = e + 1, e = find(b, path.cend(), '/'))
		{
			parent_dir_id = dir_from_name(parent_dir_id, str(b, e));
		};
		return parent_dir_id;
	}

	void directory::match_all_file_paths(const arr<str>& lines)
	{
		auto ids = arr<n4>();
		ids.reserve(f_file_dir_ids_.size());
		for_each(lines.cbegin(), lines.cend(), [this, &ids](auto&& line)
		{
			if (line.empty())
				return;
			ids.push_back(push_back(line));
		});
		sort(ids.begin(), ids.end());
		auto e = unique(ids.begin(), ids.end());
		auto to_be_checked = arr<par<n4, n4>>();
        for_each(ids.begin(), e, [&to_be_checked, next = 0](auto file_id) mutable
		{
			if (next != file_id)
				to_be_checked.push_back(make_pair(next, file_id));
			next = file_id + 1;
		});

		auto to_be_marked = arr<n4>();
		for_each(to_be_checked.cbegin(), to_be_checked.cend(), [this, &to_be_marked](auto&& p)
		{
            for_each(f_file_states_.cbegin() + p.first, f_file_states_.cbegin() + p.second, [&to_be_marked, p, i = 0](auto state) mutable
			{
				auto s = static_cast<FileState>(state);
				if (FileState::DELETED != s && FileState::UPLOADING != s)
				{
					to_be_marked.push_back(p.first + i);
				}
				++i;
			});
		});

		for_each(to_be_marked.cbegin(), to_be_marked.cend(), [this](auto file_id)
		{
			f_file_states_.at(file_id) = static_cast<n4>(FileState::DELETED);
		});

		if (!to_be_marked.empty())
			++version_;
	}

	arr<n4> directory::file_ids_with_state(FileState mark) const
	{
		auto r = arr<n4>();
        for_each(f_file_states_.cbegin(), f_file_states_.cend(), [i = 0, mark, &r](auto state) mutable
		{
			if (mark == static_cast<FileState>(state))
				r.push_back(i);
			++i;
		});
		return r;
	}

	arr<n4> directory::reset_states_if(funk<bool(FileState)> should_reset_fun)
	{
		auto indexes = arr<n4>();
		for_each(f_file_states_.cbegin(), f_file_states_.cend(), [this, i = 0, &indexes, should_reset_fun](auto state) mutable
		{
			auto s = static_cast<FileState>(state);
			if (should_reset_fun(s))
				indexes.push_back(i);
			++i;
		});
		for_each(indexes.cbegin(), indexes.cend(), [this](int i) { f_file_states_.at(i) = static_cast<n4>(FileState::NORMAL); });
		for_each(indexes.cbegin(), indexes.cend(), [this](int i) { f_file_percentages_.at(i) = 0; });
		if (!indexes.empty())
			++version_;
        return indexes;
	}

	arr<n4> directory::reset_upload_states()
	{
		return reset_states_if([](FileState s) { return FileState::UPLOADING == s; });
	}

	arr<n4> directory::reset_active_states()
	{
		return reset_states_if([](FileState s) { return FileState::UPLOADING == s || FileState::DOWNLOADING == s || FileState::DELETING == s; });
	}

	n4 directory::push_back(const str& file_path)
	{
		assert(!file_path.empty());

		auto fs = find_file_name(file_path);
		if (fs.file_name.empty())
			return 0;
		return push_back(fs.folder_id, fs.file_name);
	}

	n4 directory::add_folder(n4 parent_dir_id, const str& name)
	{
		auto r = add_folder_keep_index(parent_dir_id, name);
		++version_;
		return r;
	}
	
	directory::file_descriptor directory::find_file_name(const str& file_path)
	{
		n4 parent_dir_id = 0;
		auto b = file_path.cbegin();
		for (auto e = find(b, file_path.cend(), '/'); file_path.cend() != e; e = find(b, file_path.cend(), '/'))
		{
			auto name = str(b, e);
			if (name.empty())
				return file_descriptor{ 0, "" };

			b = e + 1;

			parent_dir_id = add_folder_keep_index(parent_dir_id, name);
		}
		return file_descriptor{ parent_dir_id, str(b, file_path.cend()) };
	}

	void directory::build_dir_index() const
	{
		if (version_ == dir_version_)
			return;
		dir_version_ = version_;
		mv_dir_ranges_.clear();
		id_dir_per_directory_.clear();

		auto dir_count = f_dir_name_ids_.size();
		mv_dir_ranges_.reset(dir_count + 1, dir_count);
		id_dir_per_directory_.reset(dir_count, dir_count);

		auto range_it = mv_dir_ranges_.begin();
        auto total = accumulate(f_dir_child_counts_.cbegin(), f_dir_child_counts_.cend(), 0, [&range_it](auto&& total, auto&& count)
		{
			*range_it++ = total;
			return total + count;
		});
		assert(f_dir_child_counts_.size() == total + 1);
		*range_it = total;
		auto dir_child_counts = arr<n4>(total, 0);
		for_each(f_dir_parent_ids_.cbegin() + 1, f_dir_parent_ids_.cend(), [this, i = 1, &dir_child_counts](auto parent_id) mutable
		{
			auto& child_count = dir_child_counts[parent_id];
			auto pos = mv_dir_ranges_[parent_id] + child_count;
			child_count++;
			id_dir_per_directory_.at(pos) = i;
			i++;
		});
	}

	void directory::build_file_index() const
	{
		if (version_ == file_version_)
			return;
		file_version_ = version_;
		mv_file_ranges_.clear();
		id_file_per_directory_.clear();


		{
			auto file_count = f_file_name_ids_.size();
			mv_file_ranges_.reset(file_count + 1, file_count);
			id_file_per_directory_.reset(file_count, file_count);
		}

		auto range_it = mv_file_ranges_.begin();
        auto total = accumulate(f_dir_file_counts_.cbegin(), f_dir_file_counts_.cend(), 0, [&range_it](auto&& total, auto&& count)
		{
			*range_it++ = total;
			return total + count;
		});
		assert(f_file_name_ids_.size() == total);
		*range_it = total;

		auto dir_child_counts = arr<n4>(f_dir_name_ids_.size(), 0);
		for_each(f_file_dir_ids_.cbegin(), f_file_dir_ids_.cend(), [this, i = 0, &dir_child_counts](auto parent_id) mutable
		{
			auto& child_count = dir_child_counts[parent_id];
			auto pos = mv_file_ranges_[parent_id] + child_count;
			child_count++;
			id_file_per_directory_.at(pos) = i;
			i++;
		});
	}

	arr<n4> directory::path_from_folder(n4 folder_id) const
	{
		if (!(0 <= folder_id && folder_id <= f_dir_name_ids_.size()))
			return {};
		auto r = arr<n4>();
		while (folder_id)
		{
			r.push_back(folder_id);
			folder_id = f_dir_parent_ids_[folder_id];
		}

		reverse(r.begin(), r.end());
		return r;
	}

	void directory::update_files(const str& path, const arr<str>& names, const arr<n8>& sizes, const arr<p16>& md5s)
	{
		auto parent_dir_id = dir_from_path(path);
		if (parent_dir_id >= f_dir_name_ids_.size())
			return;
		build_file_index();

		for (int i = 0; i < names.size(); ++i)
		{
			auto segment_id = t_file_segment_map_.add_if_not_exists(names[i]);
			auto key = segment_key(parent_dir_id, segment_id);
			auto key_it = parent_child_segment_to_file_id_.find(key);
			if (parent_child_segment_to_file_id_.end() == key_it)
				continue;
			auto file_id = key_it->second;
			f_file_sizes_.at(file_id) = sizes[i];
			f_file_md5s_.at(file_id) = md5s[i];
		}
	}

	n4 directory::file_percentage(n4 file_id) const
	{
		return f_file_percentages_[file_id];
	}

	const arr<p1>& directory::thumbnail(n4 file_id) const
	{
		return f_file_thumbnails_[file_id];
	}

	p16 directory::file_hash(n4 id) const
	{
		if (f_file_md5s_.size() > id)
			return f_file_md5s_[id];
		return { 0,0 };
	}

	bool directory::update_file_state(n4 file_id, FileState state)
	{
		if (!(0 <= file_id && file_id <= f_file_name_ids_.size()) || f_file_states_[file_id] == static_cast<n4>(state))
			return false;

		f_file_states_.at(file_id) = static_cast<n4>(state);
        return true;
	}

	void directory::update_file_percentage(n4 file_id, n4 percentage)
	{
		if (!(0 <= file_id && file_id <= f_file_name_ids_.size()))
			return;

		f_file_percentages_.at(file_id) = percentage;
	}

	n4 directory::file_parent(n4 file_id) const
	{
		if (!(0 <= file_id && file_id <= f_file_dir_ids_.size()))
			return 0;
		return f_file_dir_ids_[file_id];
	}

    str url_encode(const str& path)
    {
        auto r = str();
        r.reserve(path.size());
        for_each(path.cbegin(), path.cend(), [&r](char c)
        {
            if (' ' == c)
                r += "%20";
            else
                r += c;
        });
        return r;
    }
    
    arr<str> directory::dir_names(const arr<n4>& title_ids) const
    {
        arr<str> r;
        r.resize(title_ids.size());
        std::transform(title_ids.cbegin(), title_ids.cend(), r.begin(), [this](n4 title_id)
        {
            return t_dir_segment_map_[title_id];
        });
        return r;
    }
    arr<str> directory::file_names(const arr<n4>& title_ids) const
    {
        arr<str> r;
        r.resize(title_ids.size());
        std::transform(title_ids.cbegin(), title_ids.cend(), r.begin(), [this](n4 title_id)
        {
            return t_file_segment_map_[title_id];
        });
        return r;
    }

    str directory::encoded_path_from_dir(n4 folder_id) const
    {
        auto path = path_from_folder(folder_id);
        return accumulate(path.cbegin(), path.cend(), str(), [this](str s, auto folder_id)
                          {
                              auto segment_id = f_dir_name_ids_[folder_id];
                              auto r = url_encode(t_dir_segment_map_[segment_id]);
                              return s.empty() ? r : s + "/" + r;
                          });

    }
    str directory::encoded_path_from_file(n4 file_id) const
    {
        auto parent_id = f_file_dir_ids_[file_id];
        return parent_id ? encoded_path_from_dir(parent_id) + "/" + url_encode(file_name(file_id)) : url_encode(file_name(file_id));
    }

    n4 directory::folder_name_id(n4 folder_id) const
    {
        return f_dir_name_ids_[folder_id];
    }
    
    n4 directory::file_name_id(n4 file_id) const
    {
        return f_file_name_ids_[file_id];
    }

	str directory::file_name(n4 file_id) const
	{
		return t_file_segment_map_[f_file_name_ids_[file_id]];
	}

    str directory::folder_name(n4 folder_id) const
    {
        return t_dir_segment_map_[f_dir_name_ids_[folder_id]];
    }
    
	n8 directory::file_size(n4 file_id) const
	{
		return f_file_sizes_[file_id];
	}

	FileState directory::file_file_state(n4 file_id) const
	{
		return static_cast<FileState>(f_file_states_[file_id]);
	}

	void directory::update_file_hash(n4 file_id, p16 md5)
	{
		if (!(0 <= file_id && file_id <= f_file_md5s_.size()))
			return;

		f_file_md5s_[file_id] = md5;
	}

	void directory::update_file_size(n4 file_id, n8 size)
	{
		if (!(0 <= file_id && file_id <= f_file_sizes_.size()))
			return;

		f_file_sizes_.at(file_id) = size;
	}

    arr<n4> directory::update_paths(const arr<str>& names, const arr<n8>& sizes, const arr<p16>& md5s)
    {
        auto r = arr<n4>();
        for (int i = 0; i < names.size(); ++i)
        {
            auto detail = find_file_name(names[i]);
            if (detail.file_name.empty())
                continue;
            auto segment_id = t_file_segment_map_.add_if_not_exists(detail.file_name);
            auto key = segment_key(detail.folder_id, segment_id);
            auto key_it = parent_child_segment_to_file_id_.find(key);
            if (parent_child_segment_to_file_id_.end() == key_it)
                continue;
            auto file_id = key_it->second;
            
            bool modified = false;
            
            auto size = sizes[i];
            if (f_file_sizes_[file_id] != size)
            {
                f_file_sizes_.at(file_id) = sizes[i];
                ++version_;
                modified = true;
            }
            
            auto hash = md5s[i];
            if (f_file_md5s_[file_id] != hash)
            {
                f_file_md5s_.at(file_id) = md5s[i];
                ++version_;
                modified = true;
            }
            
            if (modified)
                r.push_back(detail.folder_id);
        }
        std::sort(r.begin(), r.end());
        r.erase(std::unique(r.begin(), r.end()), r.end());
        return r;
    }
    
    n4 directory::push_back(n4 parent_dir_id, const str& name, p16 md5)
    {
        auto segment_id = t_file_segment_map_.add_if_not_exists(name);
        auto key = segment_key(parent_dir_id, segment_id);
        auto key_it = parent_child_segment_to_file_id_.find(key);
        if (parent_child_segment_to_file_id_.end() == key_it)
        {
            auto file_id = f_file_dir_ids_.size();
            parent_child_segment_to_file_id_[key] = file_id;
            f_file_dir_ids_.push_back(parent_dir_id);
            ++f_dir_file_counts_.at(parent_dir_id);
            f_file_md5s_.push_back(md5);
            f_file_sizes_.push_back(0);
            f_file_percentages_.push_back(0);
            f_file_states_.push_back(0);
            f_file_name_ids_.push_back(segment_id);
            f_file_thumbnails_.push_back({});
            ++version_;
            return file_id;
        }
        
        {
            auto file_id = key_it->second;
            auto state = static_cast<FileState>(f_file_states_[file_id]);
            if (FileState::FAILED == state || FileState::DELETED == state)
            {
                ++version_;
                f_file_states_.at(file_id) = static_cast<n4>(FileState::NORMAL);
            }
            return file_id;
        }
    }
    
    n4 directory::delete_empty_folder(n4 folder_id)
    {
        if (0 == folder_id)
            return -1;
        if (folder_empty(folder_id))
        {
            f_dir_hidden_.at(folder_id) = 1;
            n4 deleted_parent = delete_empty_folder(parent_folder(folder_id));
            return -1 != deleted_parent ? deleted_parent : folder_id;
        }
        return -1;
    }

	n4 directory::delete_file(n4 file_id)
	{
		if (!update_file_state(file_id, FileState::DELETED))
            return -1;

        ++version_;
        n4 parent_id = file_parent(file_id);
        return delete_empty_folder(parent_id);
	}
    
    n4 directory::file_copy(n4 old_file_id, n4 parent_dir_id, n4 segment_id)
    {
        auto state = static_cast<FileState>(f_file_states_[old_file_id]);
        if (!(FileState::NORMAL == state || FileState::LOCAL == state))
            return -1;
        auto md5 = f_file_md5s_[old_file_id];
        auto size = f_file_sizes_[old_file_id];
        auto thumb = f_file_thumbnails_[old_file_id];
        
        auto file_id = f_file_dir_ids_.size();
        auto key = segment_key(parent_dir_id, segment_id);
        parent_child_segment_to_file_id_[key] = file_id;
        f_file_dir_ids_.push_back(parent_dir_id);
        ++f_dir_file_counts_.at(parent_dir_id);
        f_file_md5s_.push_back(md5);
        f_file_sizes_.push_back(size);
        f_file_percentages_.push_back(0);
        f_file_states_.push_back(static_cast<n4>(state));
        f_file_name_ids_.push_back(segment_id);
        f_file_thumbnails_.push_back(thumb);
        ++version_;
        return file_id;
    }

    n4 directory::file_copy(n4 old_file_id, str new_name)
    {
        auto parent_dir_id = f_file_dir_ids_[old_file_id];
        auto segment_id = t_file_segment_map_.add_if_not_exists(new_name);
        auto key = segment_key(parent_dir_id, segment_id);
        auto key_it = parent_child_segment_to_file_id_.find(key);
        if (parent_child_segment_to_file_id_.end() != key_it)
            return -1;
        
        return file_copy(old_file_id, parent_dir_id, segment_id);
    }

    str directory::path_from_dir(n4 folder_id) const
    {
        auto path = path_from_folder(folder_id);
        return accumulate(path.cbegin(), path.cend(), str(), [this](str s, auto folder_id)
                          {
                              auto segment_id = f_dir_name_ids_[folder_id];
                              auto r = t_dir_segment_map_[segment_id];
                              return s.empty() ? r : s + "/" + r;
                          });
    }
    
    str directory::path_from_file(n4 file_id) const
    {
        auto parent_id = f_file_dir_ids_[file_id];
        return parent_id ? path_from_dir(parent_id) + "/" + file_name(file_id) : file_name(file_id);
    }
    
    n4 directory::add_folder(n4 parent_id, n4 segment_id)
    {
        auto child_dir_id = f_dir_child_counts_.size();
        auto key = segment_key(parent_id, segment_id);

        parent_child_segment_to_dir_id_[key] = child_dir_id;
        
        f_dir_child_counts_.push_back(0);
        f_dir_file_counts_.push_back(0);
        f_dir_hidden_.push_back(0);
        
        f_dir_name_ids_.push_back(segment_id);
        f_dir_parent_ids_.push_back(parent_id);
        ++f_dir_child_counts_.at(parent_id);
        return child_dir_id;
    }

    n4 directory::add_folder_keep_index(n4 parent_dir_id, const str& name)
    {
        auto child_segment_id = t_dir_segment_map_.add_if_not_exists(name);
        auto key = segment_key(parent_dir_id, child_segment_id);
        auto key_it = parent_child_segment_to_dir_id_.find(key);
        if (parent_child_segment_to_dir_id_.end() != key_it)
        {
            auto folder_id = key_it->second;
            f_dir_hidden_.at(folder_id) = 0;
            return folder_id;
        }
        return add_folder(parent_dir_id, child_segment_id);
    }
    
    bool directory::folder_empty(n4 folder_id) const
    {
        build_dir_index();
        
        {
            auto b = id_dir_per_directory_.cbegin() + mv_dir_ranges_[folder_id];
            auto e = id_dir_per_directory_.cbegin() + mv_dir_ranges_[folder_id + 1];
            auto it = find_if(b, e, [this](auto id)
                              {
                                  return 0 == f_dir_hidden_[id];
                              });
            if (e != it)
                return false;
        }
        
        build_file_index();
        
        {
            auto b = id_file_per_directory_.cbegin() + mv_file_ranges_[folder_id];
            auto e = id_file_per_directory_.cbegin() + mv_file_ranges_[folder_id + 1];
            auto it = find_if(b, e, [this](auto id)
                              {
                                  auto state = static_cast<FileState>(f_file_states_[id]);
                                  return state != FileState::DELETED && (f_file_sizes_[id] || state == FileState::UPLOADING);
                              });
            return e == it;
        }
    }
 
    arr<par<n4,n4>> directory::folder_copy(n4 folder_id, str new_name)
    {
        
        auto parent_dir_id = f_dir_parent_ids_[folder_id];
        n4 new_folder_id = -1;
        
        {
            auto child_segment_id = t_dir_segment_map_.add_if_not_exists(new_name);
            auto key = segment_key(parent_dir_id, child_segment_id);
            auto key_it = parent_child_segment_to_dir_id_.find(key);
            if (parent_child_segment_to_dir_id_.end() != key_it)
            {
                new_folder_id = key_it->second;
                if (0 == f_dir_hidden_[new_folder_id]) // If existing folder is visible, abort
                    return {};
                f_dir_hidden_.at(new_folder_id) = 0; // Set hidden existing folder visible
            }
            else
            {
                new_folder_id = add_folder(parent_dir_id, child_segment_id);
            }
        }
        
        build_dir_index();
        build_file_index();

        auto old_new_file_ids = arr<par<n4,n4>>();
        auto old_new_folder_ids = arr<par<n4,n4>>{{folder_id, new_folder_id}};
        while (!old_new_folder_ids.empty())
        {
            auto folder_ids = move(old_new_folder_ids);
            for_each(folder_ids.begin(), folder_ids.end(), [this, &old_new_file_ids, &old_new_folder_ids](par<n4,n4> old_new_folder_id)
            {
                auto old_parent_id = old_new_folder_id.first;
                auto new_parent_id = old_new_folder_id.second;
                
                { // folders
                    auto b = id_dir_per_directory_.cbegin() + mv_dir_ranges_[old_parent_id];
                    auto e = id_dir_per_directory_.cbegin() + mv_dir_ranges_[old_parent_id + 1];
                    for_each(b, e, [this, new_parent_id, &old_new_folder_ids](n4 old_folder_id)
                    {
                        auto child_segment_id = f_dir_name_ids_[old_folder_id];
                        auto key = segment_key(new_parent_id, child_segment_id);
                        auto key_it = parent_child_segment_to_dir_id_.find(key);
                        n4 new_folder_id = -1;
                        if (parent_child_segment_to_dir_id_.end() != key_it)
                        {
                            new_folder_id = key_it->second;
                            f_dir_hidden_.at(new_folder_id) = 0; // Set folder visible
                        }
                        else
                        {
                            new_folder_id = add_folder(new_parent_id, child_segment_id);
                        }

                        old_new_folder_ids.push_back(make_pair(old_folder_id, new_folder_id));
                    });
                }
                
                { // files
                    auto b = id_file_per_directory_.cbegin() + mv_file_ranges_[old_parent_id];
                    auto e = id_file_per_directory_.cbegin() + mv_file_ranges_[old_parent_id + 1];
                    for_each(b, e, [this, new_parent_id, &old_new_file_ids](n4 old_file_id)
                    {
                        n4 segment_id = f_file_name_ids_[old_file_id];
                        n4 new_file_id = file_copy(old_file_id, new_parent_id, segment_id);
                        if (-1 == new_file_id)
                            return;
                        old_new_file_ids.push_back(make_pair(old_file_id, new_file_id));

                    });
                }
            });
        }
        ++version_;
        return old_new_file_ids;
    }

    arr<par<n4, n4>> directory::sorted_folders_for_files(arr<n4> file_ids) const
    {
        auto specified_file_parent_ids = arr<par<n4, n4>>();
        specified_file_parent_ids.reserve(file_ids.size());
        transform(file_ids.cbegin(), file_ids.cend(), back_inserter(specified_file_parent_ids), [this](auto file_id)
                  {
                      return std::make_pair(f_file_dir_ids_[file_id], file_id);
                  });
        
        sort(specified_file_parent_ids.begin(), specified_file_parent_ids.end(), [](auto&& l, auto&& r)
             {
                 if (l.first != r.first)
                     return l.first < r.first;
                 return l.second < r.second;
             });
        
        specified_file_parent_ids.erase(std::unique(specified_file_parent_ids.begin(), specified_file_parent_ids.end()), specified_file_parent_ids.end());
        return specified_file_parent_ids;
    }
    
    arr<n4> directory::unique_file_parents(const arr<n4>& file_ids) const
    {
        auto specified_file_parent_ids = arr<n4>();
        specified_file_parent_ids.reserve(file_ids.size());
        transform(file_ids.cbegin(), file_ids.cend(), back_inserter(specified_file_parent_ids), [this](auto file_id)
                  {
                      return f_file_dir_ids_[file_id];
                  });
        sort(specified_file_parent_ids.begin(), specified_file_parent_ids.end());
        specified_file_parent_ids.erase(unique(specified_file_parent_ids.begin(), specified_file_parent_ids.end()), specified_file_parent_ids.end());
        return specified_file_parent_ids;
    }
    
    arr<n4> directory::unique_folder_ancestors(const arr<n4>& folder_ids)
    {
        auto r = arr<n4>();
        std::for_each(folder_ids.begin(), folder_ids.end(), [this, &r](n4 folder_id)
        {
            while (folder_id)
            {
                r.push_back(folder_id);
                folder_id = f_dir_parent_ids_[folder_id];
            }
        });

        std::sort(r.begin(), r.end());
        r.erase(std::unique(r.begin(), r.end()), r.end());

        return r;
    }

    n4 directory::folder_parent(n4 folder_id)
    {
        if (!(0 <= folder_id && folder_id <= f_dir_name_ids_.size()))
            return -1;
        return f_dir_parent_ids_[folder_id];
    }
//    arr<n4> directory::path_from_file_id(n4 file_id) const
//    {
//        auto parent_id = f_file_dir_ids_[file_id];
//        return path_from_folder(parent_id);
//    }
}
