#pragma once
#include "text_map.h"
#include "bridge/cpp/file_state.hpp"
#include "bridge/cpp/directories_detail.hpp"
#include "bridge/cpp/files_detail.hpp"
#include <tuple>

namespace ai
{
    constexpr p8 VERSION_CACHE = 5;
    constexpr p8 VERSION_CONFIG = 1;
    static auto VERSION_TEXT = "0.8.2";
    

	struct dir_item
	{
		n4 id;
		str name;
	};

	struct files_and_folders
	{
		arr<n4> file_ids;
		arr<n4> folder_ids;
	};
	struct file_item : dir_item
	{
		n8 size;
		p16 md5;
		n4 percentage;
		FileState state;
		arr<p1> thumbnail;
		str aside() const
		{
			if (size)
				return std::to_string(size);
			if (percentage)
				return std::to_string(percentage) + "%";
			return str("");
		}
	};
    
	struct directory
	{
		directory();
        arr<n4> unique_folder_ancestors(const arr<n4>& folder_ids);
        n4 folder_parent(n4 folder_id);

        arr<n4> path_from_folder(n4) const;
//        arr<n4> path_from_file_id(n4) const;

        dir_item dir(n4 folder_id) const;
        n4 folder_name_id(n4 folder_id) const;
        arr<par<n4, n4>> sorted_folders_for_files(arr<n4> file_ids) const;
        arr<FileState> list_file_states(n4 parent_id) const;
        arr<FileState> states_for_file_ids(const arr<n4>& file_ids) const;
        arr<n4> update_paths(const arr<str>& names, const arr<n8>& sizes, const arr<p16>& md5s);

        arr<n4> unique_file_parents(const arr<n4>& file_ids) const;
        arr<str> dir_names(const arr<n4>& title_ids) const;
        arr<str> file_names(const arr<n4>& title_ids) const;
        
        n4 file_copy(n4 file_id, str new_name);
        arr<par<n4,n4>> folder_copy(n4 folder_id, str new_name);
		arr<n4> file_ids_with_state(FileState state) const;
		files_and_folders expand(const arr<n4>& folder_ids, const arr<n4>& file_ids, funk<bool(n4)> file_is_valid_fun);
		files_and_folders mark_for_download(const arr<n4>& folder_ids, const arr<n4>& file_ids);
		files_and_folders mark_for_delete(const arr<n4>& folder_ids, const arr<n4>& file_ids);
		void hide_folder(n4 folder_id);
		n4 parent_folder(n4 folder_id) const;
		arr<n4> reset_active_states();
		arr<n4> reset_upload_states();
		sz size_on_disk() const;
		sz deserialise(p8, const p1* data);
		sz serialise(p1* data) const;

		n4 file_name_id(n4) const;
		n8 file_size(n4) const;
		n4 file_percentage(n4) const;
		const arr<p1>& thumbnail(n4) const;
		str file_name(n4) const;
        str folder_name(n4 folder_id) const;

		file_item file(n4) const;
		str path_from_dir(n4 folder_id) const;
		str path_from_file(n4) const;
		str encoded_path_from_dir(n4 folder_id) const;
		str encoded_path_from_file(n4) const;
		bool empty();
		bool folder_empty(n4 folder_id) const;
		void match_all_file_paths(const arr<str>&);
		n4 push_back(const str& file_path);
		n4 push_back(n4 folder_id, const str& name, p16 md5 = { 0,0 });
		n4 delete_file(n4 file_id);
        n4 delete_empty_folder(n4 folder_id);
		void update_files(const str& path, const arr<str>&, const arr<n8>&, const arr<p16>&);
		bool update_file_state(n4 file_id, FileState);
		void update_file_percentage(n4 file_id, n4 percentage);
		void update_file_hash(n4 file_id, p16 md5);
		void update_file_size(n4 file_id, n8 size);
		FilesDetail list_file_columns(n4) const;
		arr<file_item> list_files(n4) const;
		DirectoriesDetail list_directory_columns(n4 parent_id) const;
		arr<dir_item> list_directories(n4) const;
		n4 dir_from_name(n4, const str&);
		n4 dir_from_path(const str&);
		n4 file_from_name(n4, const str&);
		n4 file_parent(n4 file_id) const;
		p16 file_hash(n4 id) const;
		FileState file_file_state(n4 file_id) const;
		const text_map& file_segments() const { return t_file_segment_map_; }
		const text_map& folder_segments() const { return t_dir_segment_map_; }
		n4 add_folder(n4 parent_dir_id, const str& name);
		int version() const { return version_; }
		void inc_version() { ++version_; }
		void thumbnail(n4 file_id, arr<p1> data) { f_file_thumbnails_[file_id] = move(data); }
	private:
		int version_ = 0;
		mutable int dir_version_ = -1;
		mutable int file_version_ = -1;
		mutable packed mv_dir_ranges_;
		mutable packed id_dir_per_directory_;
		mutable packed mv_file_ranges_;
		mutable packed id_file_per_directory_;

		packed f_dir_child_counts_;
		packed f_dir_file_counts_;
		packed f_dir_hidden_;
		packed f_dir_parent_ids_;
		packed f_dir_name_ids_;
		text_map t_dir_segment_map_;
		hmap<p8, n4> parent_child_segment_to_dir_id_;

		packed f_file_dir_ids_;
		packed_d f_file_sizes_;
		packed f_file_name_ids_;
		packed f_file_states_;
		packed f_file_percentages_;
		text_map t_file_segment_map_;
		arr<p16> f_file_md5s_;
		hmap<p8, n4> parent_child_segment_to_file_id_;
		arr<arr<p1>> f_file_thumbnails_;

		void build_dir_index() const;
		void build_file_index() const;
		struct file_descriptor
		{
			n4 folder_id;
			str file_name;
		};
		file_descriptor find_file_name(const str& file_path);
		n4 add_folder_keep_index(n4 parent_dir_id, const str& name);
		files_and_folders expand_and_mark(const arr<n4>& folder_ids, const arr<n4>& file_ids, FileState mark, funk<bool(n4)> file_is_valid_fun);
		arr<n4> reset_states_if(funk<bool(FileState)>);
        n4 add_folder(n4 parent_id, n4 segment_id);
        n4 file_copy(n4 old_file_id, n4 parent_dir_id, n4 segment_id);
	};
}
