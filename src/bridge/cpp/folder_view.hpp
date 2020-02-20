// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ai {

class FolderViewmodel;
class FormViewmodel;
class PreViewmodel;
struct FileItemDetail;
struct FolderItemDetail;
struct StateChange;

class FolderView {
public:
    virtual ~FolderView() {}

    virtual void create_folder_view(const std::shared_ptr<FolderViewmodel> & folder_vm, const std::shared_ptr<FormViewmodel> & form_vm) = 0;

    virtual void create_photo_preview(const std::shared_ptr<PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) = 0;

    virtual void create_video_preview(const std::shared_ptr<PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) = 0;

    virtual void create_text_preview(const std::shared_ptr<PreViewmodel> & vm, const std::string & file_name, const std::string & file_path) = 0;

    virtual void reload(int32_t section, int32_t row) = 0;

    virtual void on_reset(const std::vector<FolderItemDetail> & folders, const std::vector<FileItemDetail> & files) = 0;

    virtual void on_file_states_change(const std::vector<StateChange> & state_changes) = 0;

    virtual void on_file_remove(int32_t row) = 0;

    virtual void on_folder_remove(int32_t row) = 0;

    virtual void on_file_detail_change(int32_t row, const std::string & detail) = 0;

    virtual void on_clear_selection(const std::vector<int32_t> & folder_indexes, const std::vector<int32_t> & file_indexes) = 0;

    virtual void on_file_change(int32_t row, const FileItemDetail & item) = 0;

    virtual void on_folder_change(int32_t row, const FolderItemDetail & item) = 0;

    virtual void on_select_all() = 0;

    virtual void on_file_select(int32_t row, bool selected) = 0;

    virtual void on_folder_select(int32_t row, bool selected) = 0;

    virtual void on_file_added(int32_t row, const FileItemDetail & item) = 0;

    virtual void on_folder_added(int32_t row, const FolderItemDetail & item) = 0;
};

}  // namespace ai
