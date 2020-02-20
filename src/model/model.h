#pragma once
#include "../general.h"
#include "../platform/plat.h"
#include "../platform/device_platform.h"
#include <mutex>
#include "../directory.h"
#include "../openstack/openstack.h"
#include "directory_notification.h"
#include "../bridge/cpp/ui_callback.hpp"
#include "../platform/blomp_session.h"
#include "custom_platform.h"

namespace ai
{
    struct model_thread;
    
    struct model
    {
        model(str app_data_location, const sptr<OsIo>& os, const std::shared_ptr<UiCallback> & view, funk<void(funk<void(model& m)>)> run_model)
        :
        platform_(app_data_location, os),
        run_model_(run_model),
        view_(view)
        {
            
        }
        void file_rename(n4 file_id, str new_name, funk<void(file_rename_result)> fun);
        void file_path(n4 file_id, funk<void(FileType, str)>);
        void login(str email, str pwd, bool remember_me, funk<void(str)> fun);
        void read_credentials(funk<void(str, str)> fun)
        {
            read_safely("config.bin", [run_model = run_model_, fun](arr<p1> content)
            {
                if (content.empty())
                {
                    fun("", "");
                    return true;
                }
                
                auto data = content.data();
                p8 version = 0;
                auto r = deserialise_number<p8>(data, version);
                str email;
                r += deserialise_str(data + r, email);
                if (1 > version)
                {
                    fun(email, "");
                    return true;
                }
                
                p8 size = 0;
                r += deserialise_number<p8>(data + r, size);
                if (size != r)
                    return false;
                
                run_model([email, fun](model& m)
                {
                    m.platform_.file_read_password(email, [email, fun](str pwd)
                    {
                        fun(email, pwd);
                    });
                });
                
                return true;
            });

        }
        
        void cancel_network_operations();
        void read_or_create_local_cache(funk<void(bool)>);
        DirectoriesDetail dirs(n4 folder_id)
        {
            return dir_->list_directory_columns(folder_id);
        }
        FilesDetail files(n4 folder_id)
        {
            return dir_->list_file_columns(folder_id);
        }
        str folder_name(n4 folder_id)
        {
            return dir_->folder_name(folder_id);
        }
        void register_listener(n4 folder_id, const wptr<directory_notification>& listener)
        {
            active_listener_ = std::make_pair(folder_id, listener);
            auto found = listeners_.find(folder_id);
            if (listeners_.end() != found)
            {
                found->second.listener = listener;
                return;
            }
            listener_detail detail;
            detail.listener = listener;
            listeners_[folder_id] = detail;
        }
        arr<str> dir_names(const arr<n4>& title_ids) const { return dir_->dir_names(title_ids); }
        arr<str> file_names(const arr<n4>& title_ids) const { return dir_->file_names(title_ids); }
        void schedule_batch_download(arr<n4> folder_ids, arr<n4> file_ids);
        void schedule_batch_delete(arr<n4> folder_ids, arr<n4> file_ids);
        void direct_upload(n4 folder_id, const str& filename, const str& path, arr<p1> thumb);
        void files_upload(n4 folder_id, const std::vector<std::string> & filenames, const std::vector<std::string> & paths, const std::vector<std::vector<uint8_t>> & thumbnails);
        void create_folder(n4 parent_folder_id, const str& folder_name, funk<void(folder_create_result)> fun);
        void rename_folder(n4 folder_id, str new_name, funk<void(folder_rename_result)> fun);
        void cancel_networkoperations(funk<void(sz)> fun);
        void sign_out(funk<void()> fun);
        void get_user_profile(funk<void(http_status, blomp_session::user_profile)> fun);
        void is_forum_name_availible(str forum_name, funk<void(http_status, str)> fun);
        void signup(str email, str name, str forum_name, str pwd, funk<void(http_status, str)> fun);
        void get_billing(funk<void(http_status, blomp_session::billing)> fun);
        void plans(funk<void(http_status, arr<blomp_session::plan>)> fun);
        void blomp_validate_receipt(str size, str receipt_data, funk<void(http_status)>);
        void update_profile(str name, str forum_name, str address, str phone, str company, funk<void(http_status)> fun);
        void change_password(str old_pwd, str new_pwd, funk<void(http_status)> fun);
        void change_forumname(str name, funk<void(http_status)> fun);
        bool logging_enabled() { return platform_.logging_enabled(); }
        void enable_logging(bool enabled) { platform_.enable_logging(enabled); }
        str log_content() { return platform_.log_content(); }
        void log_products(const std::vector<Product> & products) { platform_.log_products(products); }
        void log_transactions(const std::unordered_map<std::string, TransactionDetail> & states) { platform_.log_transactions(states); }
        void log(int line, const char* file, const str& message) { platform_.log(LogType::LDEBUG, line, file, message); }
        void log(int line, const char* file, const arr<str>& messages) { platform_.log(LogType::LDEBUG, line, file, messages); }
    private:
        funk<void(funk<void(model& m)>)> run_model_;
        custom_platform platform_;
        std::shared_ptr<UiCallback> view_;
        
        str content_path_;
        
        par<n4, wptr<directory_notification>> active_listener_;
        struct listener_detail
        {
            wptr<directory_notification> listener;
            bool refresh_all = false;
            arr<n4> changed_folder_ids;
            arr<n4> changed_file_ids;
        };
        hmap<n4, listener_detail> listeners_;
        hmap<n4, n4> previous_percentage_for_file_id_;
        
        uptr<directory> dir_;
        uptr<openstack::session> session_;
        uptr<blomp_session> blomp_session_;
        
        std::string email_;
        
        int directory_version_ = 0;
        bool saving_ = false;
        
        n8 netid_ = 0;
        hmap<n8, sptr<CancellationToken>> netops_;

        void net_add(const sptr<CancellationToken>&);
        void net_remove(n8);
        sptr<CancellationToken> net_cancellation(n8);
        n8 net_nextid();
        sz direct_cancel_network_operations();
        
        void read_safely(const str& filename, funk<bool(arr<p1>)> process_fun);
        void log(const char* file, int line, const str&, LogType type = LogType::LDEBUG);
        void save();
        void direct_download(arr<n4> file_ids);
        void direct_delete(arr<n4> file_ids);
        void direct_rename_file(n4 old_file_id, n4 new_file_id);

        FileType type_from_path(const str& path);
        str hash_path(p16 md5, const str& filename_or_path);
        void report_thumbnail(n4 file_id, p16 md5, n4 percentage, FileState state, bool fallback_only);
        
        sptr<directory_notification> find_listener(n4 folder_id);
        void notify_state_change(const arr<par<n4, n4>>& sorted_folder_files);
        void notify_file_percentage_change(n4 file_id, int percentage);
        void notify_file_change(n4 file_id);
        void notify_file_delete(n4 file_id);
        void notify_folder_delete(n4 folder_id);
        void notify_folder_change(const arr<n4>& folder_ids);
        void notify_file_added(n4 file_id, n4 next_to_file_id);
        void notify_folder_added(n4 child_folder_id, n4 next_to_folder_id);

        void load_fresh_folder(n4 folder_id);

    };
}
