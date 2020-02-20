#pragma once
#include "../bridge/cpp/directories_detail.hpp"
#include "../bridge/cpp/files_detail.hpp"
#include "../bridge/cpp/file_state.hpp"

namespace ai
{
    enum class file_rename_result
    {
        success,
        exist,
        fail
    };
    
    enum class folder_create_result
    {
        success,
        fail,
        contain_alpha
    };
    
    enum class folder_rename_result
    {
        success,
        fail,
        not_found
    };
    
    struct directory_notification
    {
        virtual void on_refresh(DirectoriesDetail dirs, FilesDetail files, arr<str> dir_names, arr<str> file_names) = 0;
        virtual void on_state_changes(arr<n4> file_ids, arr<FileState> states) = 0;
        virtual void on_file_changed(n4 file_id, n4 title_id, n8 size, n4 percentage, FileState state, arr<p1> thumbnail) = 0;
        virtual void on_percentage_changed(n4 file_id, n4 percentage) = 0;
        virtual void on_file_removed(n4 file_id) = 0;
        virtual void on_folder_removed(n4 folder_id) = 0;
        virtual void on_file_name_changed(n4 file_id, n4 title_id, str title) = 0;
        virtual void on_folder_name_changed(n4 folder_id, n4 title_id, str title) = 0;
        
        virtual void on_file_added(n4 file_id, n4 title_id, n8 size, n4 percentage, FileState state, str title, n4 next_to_file_id) = 0;
        virtual void on_folder_added(n4 folder_id, n4 title_id, str title, n4 next_to_folder_id) = 0;
        virtual ~directory_notification() {}
    };
}
