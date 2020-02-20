#pragma once
#include "../platform/lambda_scheduler.h"
#include "../general.h"
#include "model.h"
#include "../bridge.h"
#include "../bridge/cpp/worker.hpp"
#include <unordered_set>
#include "../bridge/cpp/ui_callback.hpp"

namespace ai
{
    struct services
    {
        enum task_type : n4
        {
            Init,
            Login,
            ReadCredentials,
//            t_read_or_create_local_cache,
        };

        services
        (
            const std::shared_ptr<Worker> & worker,
            str app_data_location,
            const sptr<OsIo>& os,
            const std::shared_ptr<UiCallback> & view
        )
        :
        model_(app_data_location, os, view, [this](auto&& fun) { run_model(fun); })
        {
            auto t = std::make_shared<lambda_task>([this]()
            {
                while (true)
                {
                    funk<void(model&)> task;

                    {
                        gate lck(task_mutex_);
                        if (0 == tasks_.size())
                            task_cv_.wait(lck);
                        if (stopping_)
                            return;
                        task = *tasks_.rbegin();
                        tasks_.pop_back();
                    }

                    task(model_);
                }
            });
            
            worker->schedule(t);
        }
        
        void sign_out(funk<void()> fun)
        {
            run_model([fun](model& m)
            {
                m.sign_out(fun);
            });

        }
        
        void file_path(n4 file_id, funk<void(FileType, str)> fun)
        {
            run_model([file_id, fun](model& m)
            {
                m.file_path(file_id, fun);
            });
        }
        
        void read_credentials(funk<void(str, str)> fun)
        {
            schedule_task(task_type::ReadCredentials, [fun](model& m)
            {
                m.read_credentials([fun](str email, str password)
                {
                    fun(email, password);
                });
            });
        }
        void login(str email, str pwd, bool remember_me, funk<void(str)> fun)
        {
            schedule_task(task_type::Login, [email, pwd, remember_me, fun](model& m)
            {
                m.login(email, pwd, remember_me, fun);
            });
        }
//        void read_or_create_local_cache(funk<void(bool)> fun)
//        {
//            schedule_task(task_type::t_read_or_create_local_cache, [fun](model& m)
//            {
//                m.read_or_create_local_cache(fun);
//            });
//        }
        void listen(n4 folder_id, wptr<directory_notification> listener, funk<void(str, DirectoriesDetail, FilesDetail, arr<str>, arr<str>)> fun)
        {
            run_model([fun, folder_id, listener](model& m)
            {
                m.register_listener(folder_id, listener);

                auto dirs = m.dirs(folder_id);
                auto files = m.files(folder_id);
                auto dir_names = m.dir_names(dirs.titles);
                auto file_names = m.file_names(files.titles);
                fun(m.folder_name(folder_id), std::move(dirs), std::move(files), std::move(dir_names), std::move(file_names));
            });
        }
        
        void enable_logging(bool enabled) { model_.enable_logging(enabled); }
        bool logging_enabled() { return model_.logging_enabled(); }
        str log_content() { return model_.log_content(); }
        void log_products(const std::vector<Product> & products) { model_.log_products(products); }
        void log_transactions(const std::unordered_map<std::string, TransactionDetail> & states) { model_.log_transactions(states); }
        void log(int line, const char* file, const str& message) { model_.log(line, file, message); }
        void log(int line, const char* file, const arr<str>& messages) { model_.log(line, file, messages); }

        void cancel_networkoperations(funk<void(sz)> fun)
        {
            run_model([fun](model& m)
            {
                m.cancel_networkoperations(fun);
            });
        }

        void change_password(str old_pwd, str new_pwd, funk<void(http_status)> fun)
        {
            run_model([old_pwd, new_pwd, fun](model& m)
            {
                m.change_password(old_pwd, new_pwd, fun);
            });
        }

        void change_forumname(str name, funk<void(http_status)> fun)
        {
            run_model([name, fun](model& m)
            {
                m.change_forumname(name, fun);
            });
        }

        void update_profile(str name, str forum_name, str address, str phone, str company, funk<void(http_status)> fun)
        {
            run_model([name, forum_name, address, phone, company, fun](model& m)
            {
                m.update_profile(name, forum_name, address, phone, company, fun);
            });
        }

        void blomp_validate_receipt(str size, str receipt_data, funk<void(http_status)> fun)
        {
            run_model([size, receipt_data = std::move(receipt_data), fun](model& m)
            {
                m.blomp_validate_receipt(size, std::move(receipt_data), fun);
            });
        }
        
        void folder_rename(n4 folder_id, str new_name, funk<void(folder_rename_result)> fun)
        {
            run_model([folder_id, new_name, fun](model& m)
            {
                m.rename_folder(folder_id, new_name, fun);
            });
        }

        void is_forum_name_availible(str forum_name, funk<void(http_status, str)> fun)
        {
            run_model([forum_name, fun](model& m)
            {
                m.is_forum_name_availible(forum_name, [fun](auto status, str message)
                {
                    fun(status, message);
                });
            });

        }
        
        void signup(str email, str name, str forum_name, str pwd, funk<void(http_status, str)> fun)
        {
            run_model([fun, email, name, forum_name, pwd](model& m)
            {
                m.signup(email, name, forum_name, pwd, [fun](auto status, str msg)
                {
                    fun(status, msg);

                });
            });

        }
        
        void plans(funk<void(http_status, arr<blomp_session::plan>)> fun)
        {
            run_model([fun](model& m)
            {
                m.plans([fun](auto status, auto plans)
                {
                    fun(status, plans);
                });
            });
        }
        
        void get_billing(funk<void(http_status, blomp_session::billing)> fun)
        {
            run_model([fun](model& m)
            {
                m.get_billing([fun](auto status, auto billing)
                {
                    fun(status, billing);
                });
            });
        }
        
        void get_user_profile(funk<void(http_status, blomp_session::user_profile)> fun)
        {
            run_model([fun](model& m)
            {
                m.get_user_profile([fun](auto status, auto profile)
                {
                    fun(status, profile);

                });
            });
        }
        
        void folder_create(n4 parent_folder_id, const str& folder_name, funk<void(folder_create_result)> fun)
        {
            run_model([parent_folder_id, folder_name, fun](model& m)
            {
                m.create_folder(parent_folder_id, folder_name, fun);
            });
        }
        
        void files_upload(n4 folder_id, const std::vector<std::string> & filenames, const std::vector<std::string> & paths, const std::vector<std::vector<uint8_t>> & thumbnails)
        {
            run_model([folder_id, filenames, paths, thumbnails](model& m)
            {
                m.files_upload(folder_id, std::move(filenames), std::move(paths), std::move(thumbnails));
            });
        }
        
        void file_upload(n4 folder_id, const str& filename, const str& path, arr<p1> thumb)
        {
            run_model([folder_id, filename, path, thumb = std::move(thumb)](model& m)
            {
                m.direct_upload(folder_id, filename, path, std::move(thumb));
            });
        }
        void file_rename(n4 file_id, str new_name, funk<void(file_rename_result)> fun)
        {
            run_model([file_id, new_name, fun](model& m)
            {
                m.file_rename(file_id, new_name, fun);
            });
        }
        void schedule_batch_download(arr<n4> folder_ids, arr<n4> file_ids)
        {
            run_model([folder_ids = std::move(folder_ids), file_ids = std::move(file_ids)](model& m)
            {
                m.schedule_batch_download(std::move(folder_ids), std::move(file_ids));
            });
        }
        void schedule_batch_delete(arr<n4> folder_ids, arr<n4> file_ids)
        {
            run_model([folder_ids = std::move(folder_ids), file_ids = std::move(file_ids)](model& m)
            {
                m.schedule_batch_delete(std::move(folder_ids), std::move(file_ids));
            });
        }

    private:
        model model_;
        arr<funk<void(model&)>> tasks_;
        
        std::mutex task_mutex_;
        std::condition_variable task_cv_;
        bool stopping_ = false;
        std::unordered_set<task_type> service_busy_;

        void schedule_task(task_type type, funk<void(model&)> fun)
        {
            begin_task(type, [this, type, fun](model& m)
            {
                fun(m);
                end_task(type);
            });
        }
        
        void begin_task(task_type type, funk<void(model&)> fun)
        {
            gate lck(task_mutex_);
            if (service_busy_.end() != service_busy_.find(type))
                return;
            service_busy_.insert(type);
            tasks_.push_back(fun);
            if (1 == tasks_.size())
                task_cv_.notify_one();
        }
        
        void end_task(task_type type)
        {
            gate lck(task_mutex_);
            service_busy_.erase(type);
        }

        void run_model(funk<void(model&)> fun)
        {
            gate lck(task_mutex_);
            tasks_.push_back(fun);
            if (1 == tasks_.size())
                task_cv_.notify_one();
        }
    };
}
