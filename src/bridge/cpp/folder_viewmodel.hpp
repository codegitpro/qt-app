// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ai {

class FolderView;

class FolderViewmodel {
public:
    virtual ~FolderViewmodel() {}

    virtual void on_load(const std::shared_ptr<FolderView> & view) = 0;

    virtual int32_t folder_id() = 0;

    virtual std::vector<uint8_t> list_thumbnail_content(int32_t row) = 0;

    virtual bool list_selected(int32_t section, int32_t row) = 0;

    virtual void list_action(int32_t section, int32_t row, int32_t sub_index) = 0;

    virtual void upload_file_only(const std::string & filename, const std::string & path, const std::vector<uint8_t> & thumbnail) = 0;

    virtual void upload_files_only(const std::vector<std::string> & filenames, const std::vector<std::string> & paths, const std::vector<std::vector<uint8_t>> & thumbnails) = 0;
};

}  // namespace ai
