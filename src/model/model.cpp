#include "model.h"
#include <atomic>
#include "../bridge/cpp/progress.hpp"
#include "../platform/content.h"
#include "../ui/viewmodel.h"

namespace ai
{
    void model::change_forumname(str name, funk<void (http_status)> fun)
    {
        if (!blomp_session_)
        {
            fun(http_status::blomp_session);
            return;
        }
    
        net_add(blomp_session_->change_forumname(name, [fun, run_model = run_model_, netid = net_nextid()](http_status status)
        {
            run_model([fun, status, netid](model& m)
            {
                m.net_remove(netid);

                fun(status);
            });
        }));
    }
    
    void model::change_password(str old_pwd, str new_pwd, funk<void(http_status)> fun)
    {
        if (!blomp_session_)
        {
            fun(http_status::blomp_session);
            return;
        }
    
        net_add(blomp_session_->change_password(old_pwd, new_pwd, [fun, run_model = run_model_, netid = net_nextid()](http_status status)
        {
            run_model([fun, status, netid](model& m)
            {
                m.net_remove(netid);

                fun(status);
            });
        }));
    }
    
    void model::update_profile(str name, str forum_name, str address, str phone, str company, funk<void(http_status)> fun)
    {
        if (!blomp_session_)
        {
            fun(http_status::blomp_session);
            return;
        }
        net_add(blomp_session_->update_profile(name, forum_name, address, phone, company, [fun, run_model = run_model_, netid = net_nextid()](http_status status)
        {
            run_model([fun, status, netid](model& m)
            {
                m.net_remove(netid);

                fun(status);
            });
        }));
    }
    
    void model::blomp_validate_receipt(str size, str receipt_data, funk<void(http_status)> fun)
    {
        net_add(blomp_session::validate_receipt(platform_, email_, size, std::move(receipt_data), [fun, run_model = run_model_, netid = net_nextid()](auto status)
        {
            run_model([fun, status, netid](model& m)
            {
                m.net_remove(netid);

                fun(status);
            });
        }));
    }

    void model::plans(funk<void(http_status, arr<blomp_session::plan>)> fun)
    {
        net_add(blomp_session::plans(platform_, [fun, run_model = run_model_, netid = net_nextid()](auto status, auto plans)
        {
            run_model([fun, plans, netid, status](model& m)
            {
                m.net_remove(netid);
                
                m.log(__FILE__, __LINE__, std::to_string(plans.size()));

                fun(status, plans);
            });
        }));
    }
    
    void model::signup(str email, str name, str forum_name, str pwd, funk<void(http_status, str)> fun)
    {
        net_add(blomp_session::signup(platform_, email, name, forum_name, pwd, [fun, run_model = run_model_, netid = net_nextid()](auto status, str message)
        {
            run_model([fun, status, netid, message](model& m)
            {
                m.net_remove(netid);

                fun(status, message);
            });
        }));
    }

    void model::is_forum_name_availible(str forum_name, funk<void(http_status, str)> fun)
    {
        net_add(blomp_session::is_forum_name_availible(platform_, forum_name, [fun, run_model = run_model_, netid = net_nextid()](auto status, str message)
        {
            run_model([fun, message, netid, status](model& m)
            {
                m.net_remove(netid);

                fun(status, message);
            });
        }));

    }
    
    void model::get_billing(funk<void(http_status, blomp_session::billing)> fun)
    {
        if (!blomp_session_)
        {
            fun(http_status::blomp_session, blomp_session::billing());
            return;
        }
        net_add(blomp_session_->get_billing([fun, run_model = run_model_, netid = net_nextid()](auto status, auto billing)
        {
            run_model([fun, status, billing, netid](model& m)
            {
                m.net_remove(netid);

                fun(status, billing);
            });
        }));
    }

    void model::get_user_profile(funk<void(http_status, blomp_session::user_profile)> fun)
    {
        if (!blomp_session_)
        {
            fun(http_status::blomp_session, blomp_session::user_profile());
            return;
        }
        
        net_add(blomp_session_->get_profile([fun, run_model = run_model_, netid = net_nextid()](auto status, auto profile)
        {
            run_model([fun, status, profile, netid](model& m)
            {
                m.net_remove(netid);
                
                fun(status, profile);
            });
        }));
    }
    
    void model::sign_out(funk<void()> fun)
    {
        direct_cancel_network_operations();
        platform_.file_clear_cache(email_, [run_model = run_model_, fun](bool success)
        {
        
//        });
//        save();
//        platform_.file_write_password(email_, "", [run_model = run_model_, fun](bool success)
//        {
            run_model([fun](model& m)
            {
                m.direct_cancel_network_operations();
                
                m.session_ = nullptr;
                m.dir_ = nullptr;
                m.listeners_.clear();
                m.previous_percentage_for_file_id_.clear();
                m.email_ = "";
                m.directory_version_ = 0;
                fun();
            });
        });
    }

    void model::cancel_networkoperations(funk<void(sz)> fun)
    {
        auto count = direct_cancel_network_operations();
        save();
        fun(count);
    }

    sz model::direct_cancel_network_operations()
    {
        if (!dir_)
            return 0;
//        lock_guard<mutex> lck(net_mutex_);
        int count = accumulate(netops_.cbegin(), netops_.cend(), 0, [](int acc, auto&& item)
                               {
                                   if (!item.second)
                                       return acc;
                                   item.second->cancel();
                                   return acc + 1;
                               });
        dir_->reset_active_states();
        return count;
    }

    void model::rename_folder(n4 folder_id, str new_name, funk<void(folder_rename_result)> fun)
    {
        if (!dir_)
            return;
        
        auto file_ids = dir_->folder_copy(folder_id, new_name);
        if (file_ids.empty())
        {
            fun(folder_rename_result::not_found);
            return;
        }
        auto new_file_ids = arr<n4>();
        for_each(file_ids.begin(), file_ids.end(), [this, &new_file_ids](auto&& old_new)
        {
            new_file_ids.push_back(old_new.second);
            direct_rename_file(old_new.first, old_new.second);
        });
        fun(folder_rename_result::success);
        
        {
            auto parent_ids = dir_->unique_file_parents(new_file_ids);
            auto ancestor_ids = dir_->unique_folder_ancestors(parent_ids);
            std::for_each(ancestor_ids.begin(), ancestor_ids.end(), [this](n4 folder_id)
            {
                notify_folder_added(folder_id, -1);
            });
        }
//        if (1 == file_ids.size())
//        gui_->show_message("Moving one file");
//        else
//        gui_->show_message("Moving " + to_string(file_ids.size()) + " files");
    }

    void model::notify_folder_added(n4 child_folder_id, n4 next_to_folder_id)
    {
        if (!dir_)
            return;

        auto parent_folder_id = dir_->folder_parent(child_folder_id);
        auto found = find_listener(parent_folder_id);
        if (!found)
            return;
        
        auto item = dir_->dir(child_folder_id);
        run_ui(view_, [found, item = std::move(item), title_id = dir_->folder_name_id(child_folder_id), next_to_folder_id]()
        {
            found->on_folder_added(item.id, title_id, item.name, next_to_folder_id);
        });
    }
    
    void model::notify_file_added(n4 file_id, n4 next_to_file_id)
    {
        if (!dir_)
            return;

        auto folder_id = dir_->file_parent(file_id);
        auto found = find_listener(folder_id);
        if (!found)
            return;
        
        auto item = dir_->file(file_id);
        auto file_name = dir_->file_name(file_id);
        run_ui(view_, [found, file_id, item = std::move(item), title_id = dir_->file_name_id(file_id), next_to_file_id, file_name]()
               {
                   found->on_file_added(file_id, title_id, item.size, item.percentage, item.state, file_name, next_to_file_id);
               });
    }
    

    void model::create_folder(n4 parent_folder_id, const str& folder_name, funk<void(folder_create_result)> fun)
    {
        if (!dir_)
            return;

        if (find_if(folder_name.cbegin(), folder_name.cend(), [](char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }) == folder_name.end())
        {
            fun(folder_create_result::contain_alpha);
            return;
        }
        auto folder_id = dir_->add_folder(parent_folder_id, folder_name);
        if (!folder_id)
        {
            fun(folder_create_result::fail);
            return;
        }
        notify_folder_added(folder_id, -1);
        fun(folder_create_result::success);
    }

    void model::files_upload(n4 folder_id, const std::vector<std::string> & filenames, const std::vector<std::string> & paths, const std::vector<std::vector<uint8_t>> & thumbnails)
    {
        for (int i = 0; i < filenames.size(); ++i)
        {
            direct_upload(folder_id, filenames[i], paths[i], move(thumbnails[i]));
        }
    }
    
    void model::direct_upload(n4 folder_id, const str& filename, const str& path, arr<p1> thumb)
    {
        if (!dir_)
            return;

        auto file_id = dir_->file_from_name(folder_id, filename);
        if (-1 != file_id)
        {
            auto state = dir_->file_file_state(file_id);
            if (FileState::UPLOADING == state || FileState::DOWNLOADING == state || FileState::DELETING == state)
                return;
        }
        else
            file_id = dir_->push_back(folder_id, filename);
        
        dir_->update_file_state(file_id, FileState::UPLOADING);
        dir_->thumbnail(file_id, thumb);
        dir_->inc_version();
        
        auto relative_path = dir_->encoded_path_from_file(file_id);
        
        auto progress_fun = [run_model = run_model_, file_id, x = std::make_shared<std::atomic<Progress>>(Progress(-1, -1))](Progress p)
        {
            auto pp = x->exchange(p);
            if (!(-1 == pp.count && -1 == pp.total))
                return;
            run_model([file_id, x](model& m)
            {
                if (!m.dir_)
                    return;

                auto p = x->exchange(Progress(-1, -1));
                if (FileState::UPLOADING == m.dir_->file_file_state(file_id))
                {
                    int perc = percentage(p);
                    m.dir_->update_file_percentage(file_id, perc);
                    m.notify_file_percentage_change(file_id, perc);
                }
            });
        };

        notify_file_added(file_id, -1);
        
        if (!session_)
            return;
        
        net_add(session_->upload_file(relative_path, path, { 0,0 }, content_type_from_path(path), progress_fun, [run_model = run_model_, file_id, netid = net_nextid(), path, thumbempty = thumb.empty(), filename](n4 sc, n8 len, p16 md5)
        {
            run_model([run_model, file_id, netid, path, filename, thumbempty, sc, len, md5](model& m)
            {
                m.net_remove(netid);
                if (!m.dir_)
                    return;

                if (200 <= sc && sc < 300)
                {
                    if (299 == sc)
                    {
                        m.dir_->update_file_state(file_id, FileState::LOCAL);
                        if (thumbempty)
                            m.report_thumbnail(file_id, md5, 100, FileState::LOCAL, false);
                    }
                    else if (298 == sc)
                    {
                        m.dir_->update_file_state(file_id, FileState::NORMAL);
                    }
                    else
                    {
                        m.dir_->update_file_state(file_id, FileState::NORMAL);
                        m.platform_.copy_file(path, m.hash_path(md5, filename), [run_model, file_id, md5, thumbempty](bool success)
                        {
                            if (!success)
                                return;
                            run_model([file_id, md5, thumbempty](model& m)
                            {
                                m.dir_->update_file_state(file_id, FileState::LOCAL);
                                if (thumbempty)
                                    m.report_thumbnail(file_id, md5, 100, FileState::LOCAL, false);
                                m.notify_file_change(file_id);
                                m.dir_->inc_version();
                                m.save();
                            });
                        });
                    }
                }
                else
                    m.dir_->update_file_state(file_id, FileState::DELETED);
                m.dir_->update_file_percentage(file_id, 0);
                m.dir_->update_file_size(file_id, len);
                m.dir_->update_file_hash(file_id, md5);
                m.notify_file_change(file_id);
                m.dir_->inc_version();
                m.save();
            });
        }));
    }
    

    void model::direct_rename_file(n4 old_file_id, n4 new_file_id)
    {
        if (!dir_)
            return;

        auto size = dir_->file_size(old_file_id);
        auto old_path = dir_->encoded_path_from_file(old_file_id);
        auto new_path = dir_->encoded_path_from_file(new_file_id);

        if (!session_)
            return;
        net_add(session_->copy_file(old_path, new_path, size, [run_model = run_model_, old_path, old_file_id, new_file_id, netid = net_nextid()](n4 sc)
        {
            run_model([run_model, sc, old_path, old_file_id, new_file_id, netid](model& m)
            {
                m.net_remove(netid);
                if (!m.dir_)
                    return;

                if (!(200 <= sc && sc < 300))
                {
//                    gui_->show_message("Failed to copy " + old_path);
                    n4 deleted_parent_folder_id = m.dir_->delete_file(new_file_id);
                    m.notify_file_delete(new_file_id);
                    if (-1 != deleted_parent_folder_id)
                        m.notify_folder_delete(deleted_parent_folder_id);
                    return;
                }

                if (!m.session_)
                    return;

                m.net_add(m.session_->delete_file(old_path, [run_model, old_path, old_file_id, new_file_id, netid = m.net_nextid()](n4 sc)
                {
                    run_model([run_model, sc, old_path, old_file_id, new_file_id, netid](model& m)
                    {
                        m.net_remove(netid);
                        if (!m.dir_)
                            return;

                        if (!(200 <= sc && sc < 300))
                        {
//                            gui_->show_message("Failed to delete " + old_path);
                            return;
                        }

                        n4 deleted_parent_folder_id = m.dir_->delete_file(old_file_id);
                        m.notify_file_delete(old_file_id);
                        m.save();
                        if (-1 != deleted_parent_folder_id)
                            m.notify_folder_delete(deleted_parent_folder_id);
                    });

                }));
            });

        }));
        
    }

    void model::file_rename(n4 file_id, str new_name, funk<void(file_rename_result)> fun)
    {
        if (!dir_)
            return;

        auto item = dir_->file(file_id);
        auto old_name = item.name;
        
        n4 new_file_id = dir_->file_copy(file_id, new_name);
        if (-1 == new_file_id)
        {
            fun(file_rename_result::exist);
            return;
        }
        notify_file_added(new_file_id, file_id);
        direct_rename_file(file_id, new_file_id);
        fun(file_rename_result::success);
    }
    
    void model::load_fresh_folder(n4 folder_id)
    {
        if (!dir_)
            return;

        auto path = dir_->path_from_dir(folder_id);
        log(__FILE__, __LINE__, "fetch start");
        if (!session_)
            return;
        net_add(session_->list(openstack::scope::all_files, "", 0, { "","" }, path, openstack::timetrade::fastest, [run_model = run_model_, path, netid = net_nextid(), folder_id](auto list)
        {
            auto names = arr<str>();
            auto md5s = arr<p16>();
            auto sizes = arr<n8>();
            if (list.data)
            {
                std::for_each(list.objects.cbegin(), list.objects.cend(), [&names, &md5s, &sizes, data = list.data, offset = path.empty() ? 0 : path.size() + 1](auto&& obj)
                {
                    if (offset >= obj.name.second - obj.name.first)
                        return;

                    names.push_back(obj.get_name(data, offset));
                    sizes.push_back(obj.get_size(data));
                    md5s.push_back(obj.get_hash(data));
                });
            }
            run_model([run_model, netid, path, folder_id, names = std::move(names), md5s = std::move(md5s), sizes = std::move(sizes)](model& m)
            {
                m.net_remove(netid);
                if (!m.dir_)
                    return;

                m.log(__FILE__, __LINE__, "fetch end");

                if (names.empty())
                    return;
                
                auto folder_ids = m.dir_->update_paths(names, sizes, md5s);
                m.save();
                m.notify_folder_change(folder_ids);
                m.log(__FILE__, __LINE__, "processed");
            });
        }));
    }

    void model::file_path(n4 file_id, funk<void(FileType, str)> fun)
    {
        if (!dir_)
            return;

        auto filename = dir_->file_name(file_id);
        auto md5 = dir_->file_hash(file_id);
        auto hashed_path = hash_path(md5, filename);

        auto type = type_from_path(filename);
        run_ui(view_, [fun, type, hashed_path]()
        {
            fun(type, hashed_path);
        });
    }

    void model::read_safely(const str& filename, funk<bool(arr<p1>)> process_fun)
    {
        auto read_dir = [this, process_fun](const str& filename, funk<void(bool)> fun)
        {
            platform_.file_readall(platform_.app_data_location() + "/" + filename, [fun, process_fun](arr<p1> content)
            {
                if (content.empty())
                {
                    fun(false);
                    return;
                }

                fun(process_fun(move(content)));
            });
        };
        
        read_dir(filename, [read_dir, process_fun, filename](bool success)
        {
            if (!success)
            {
                read_dir(filename + ".bak", [process_fun](bool success)
                {
                    if (!success)
                        process_fun({});
                });
            }
        });

    }

    void model::net_add(const sptr<CancellationToken>& cancellation)
    {
        if (!cancellation)
            return;
        
//        std::lock_guard<std::mutex> lck(net_mutex_);
        netops_[netid_] = cancellation;
    }

    void model::net_remove(n8 netid)
    {
//        lock_guard<mutex> lck(net_mutex_);
        netops_.erase(netid);
    }
    
    sptr<CancellationToken> model::net_cancellation(n8 netid)
    {
//        std::lock_guard<std::mutex> lck(net_mutex_);
        
        auto it = netops_.find(netid);
        
        return netops_.end() == it ? nullptr : it->second;
    }
    
    n8 model::net_nextid()
    {
//        std::lock_guard<std::mutex> lck(net_mutex_);
        return ++netid_;
    }
    
    void model::log(const char* file, int line, const str& message, LogType type)
    {
        platform_.log(type, line, file, message);
    }

    void model::read_or_create_local_cache(funk<void(bool)> fun)
    {
        content_path_ = platform_.app_data_location() + "/cache/content";

        platform_.make_path(content_path_, [run_model = run_model_, fun](bool result)
        {
            run_model([run_model, fun, result](model& m)
            {
                if (!result)
                {
                    fun(false);
                    return;
                }
                
                m.read_safely(m.email_ + ".bin", [run_model, fun](arr<p1> content)
                {
                    if (content.empty())
                    {
                        run_model([fun](model& m)
                        {
                            m.dir_ = std::make_unique<directory>();
                            m.directory_version_ = m.dir_->version();

                            fun(true);
                        });
                    }
                    else
                    {
                        auto data = content.data();
                        p8 version = 0;
                        auto r = deserialise_number<p8>(data, version);
                        if (version < VERSION_CACHE)
                            return false;
                        uptr<directory> dir = std::make_unique<directory>();
                        r += dir->deserialise(version, data + r);
                        sz size = 0;
                        r += deserialise_number<sz>(data + r, size);
                        if (size != r)
                            return false;
                        
                        run_model([fun, dir = dir.get()](model& m)
                        {
                            m.dir_ = uptr<directory>(dir);
                            m.directory_version_ = m.dir_->version();
                            fun(true);
                        });
                        dir.release();
                    }
                    
                    return true;
                });
            });
        });
    }
    
    void model::schedule_batch_download(arr<n4> folder_ids, arr<n4> file_ids)
    {
        if (!dir_)
            return;

        auto downloading_file_ids = dir_->mark_for_download(folder_ids, file_ids);
        notify_state_change(dir_->sorted_folders_for_files(downloading_file_ids.file_ids));
        direct_download(downloading_file_ids.file_ids);
        save();
    }

    sptr<directory_notification> model::find_listener(n4 folder_id)
    {
        auto found = listeners_.find(folder_id);
        if (listeners_.end() == found)
            return nullptr;
        auto s_listener = found->second.listener.lock();
        if (!s_listener)
            listeners_.erase(found);
        return s_listener;
    }

    void model::notify_folder_change(const arr<n4>& folder_ids)
    {
        std::for_each(folder_ids.begin(), folder_ids.end(), [this](n4 folder_id)
        {
            auto found = find_listener(folder_id);
            if (!found)
                return;

            auto ds = dirs(folder_id);
            auto fs = files(folder_id);
            auto d_names = dir_names(ds.titles);
            auto f_names = file_names(fs.titles);
            run_ui(view_, [found, ds = std::move(ds), fs = std::move(fs), d_names = std::move(d_names), f_names = std::move(f_names)]()
            {
                found->on_refresh(std::move(ds), std::move(fs), std::move(d_names), std::move(f_names));
            });
        });
    }

    void model::notify_state_change(const arr<par<n4, n4>>& sorted_folder_files)
    {
        if (!dir_)
            return;

        for (auto b = sorted_folder_files.begin(); b != sorted_folder_files.end(); )
        {
            auto folder_id = b->first;
            auto e = std::find_if_not(b, sorted_folder_files.end(), [folder_id](auto&& item) { return item.first == folder_id; });
            auto found = find_listener(folder_id);
            if (found)
            {
                auto file_ids = arr<n4>();
                file_ids.reserve(std::distance(b, e));
                std::transform(b, e, std::back_inserter(file_ids), [](auto&& item) { return item.second; });
                auto states = dir_->states_for_file_ids(file_ids);
                run_ui(view_, [states = std::move(states), file_ids = std::move(file_ids), found]()
                {
                    found->on_state_changes(std::move(file_ids), std::move(states));
                });
            }
            b = e;
        }
//        std::for_each(folder_ids.begin(), folder_ids.end(), [this](n4 folder_id)
//        {
//            auto found = find_listener(folder_id);
//            if (!found)
//                return;
//
//            auto states = dir_->list_file_states(folder_id);
//            run_ui(view_, [states, found]()
//            {
//                found->on_state_changes(states);
//            });
//        });
    }
    
    void model::notify_file_percentage_change(n4 file_id, int percentage)
    {
        if (!dir_)
            return;

        auto folder_id = dir_->file_parent(file_id);
        auto found = find_listener(folder_id);
        if (!found)
            return;
        auto percentage_it = previous_percentage_for_file_id_.find(file_id);
        if (previous_percentage_for_file_id_.end() != percentage_it && percentage_it->second == percentage)
            return;
        previous_percentage_for_file_id_[file_id] = percentage;
        run_ui(view_, [found, percentage, file_id]()
        {
            found->on_percentage_changed(file_id, percentage);
        });
    }
    
    void model::notify_file_change(n4 file_id)
    {
        if (!dir_)
            return;

        auto folder_id = dir_->file_parent(file_id);
        auto found = find_listener(folder_id);
        if (!found)
            return;
        
        auto item = dir_->file(file_id);
        run_ui(view_, [found, file_id, item = std::move(item), title_id = dir_->file_name_id(file_id)]()
        {
            found->on_file_changed(file_id, title_id, item.size, item.percentage, item.state, item.thumbnail);
        });
    }

    void model::notify_file_delete(n4 file_id)
    {
        if (!dir_)
            return;

        auto folder_id = dir_->file_parent(file_id);
        auto found = find_listener(folder_id);
        if (!found)
            return;
        
        run_ui(view_, [found, file_id]()
        {
            found->on_file_removed(file_id);
        });
    }

    void model::notify_folder_delete(n4 folder_id)
    {
        if (!dir_)
            return;

        n4 parent_id = dir_->parent_folder(folder_id);
        auto found = find_listener(parent_id);
        if (!found)
            return;

        run_ui(view_, [found, folder_id]()
        {
            found->on_folder_removed(folder_id);
        });
    }
    

    void model::direct_download(arr<n4> file_ids)
    {
        if (!dir_)
            return;

        for_each(file_ids.cbegin(), file_ids.cend(), [this](auto file_id)
        {
            auto progress_fun = [run_model = run_model_, file_id, x = std::make_shared<std::atomic<Progress>>(Progress(-1, -1))](Progress p)
            {
                auto pp = x->exchange(p);
                if (!(-1 == pp.count && -1 == pp.total))
                    return;
                run_model([file_id, x](model& m)
                {
                    if (!m.dir_)
                        return;

                    auto p = x->exchange(Progress(-1, -1));
                    if (FileState::DOWNLOADING == m.dir_->file_file_state(file_id))
                    {
                        int perc = percentage(p);
                        m.dir_->update_file_percentage(file_id, perc);
                        m.notify_file_percentage_change(file_id, perc);
                    }
                });
            };

            auto relative_path = dir_->encoded_path_from_file(file_id);
            auto size = dir_->file_size(file_id);
            auto md5 = dir_->file_hash(file_id);

            if (!session_)
                return;
            auto netid = net_nextid();
            net_add(session_->download_file(relative_path, hash_path(md5, relative_path), size, md5, progress_fun, [run_model = run_model_, file_id, netid, old_md5 = md5](n4 sc, n8, p16 md5)
            {
                run_model([file_id, sc, md5, old_md5, netid](model& m)
                {
                    if (!m.dir_)
                        return;

                    m.net_remove(netid);
                    auto state = (200 <= sc && sc < 300) || 304 == sc ? FileState::LOCAL : FileState::NORMAL;
                    m.dir_->update_file_percentage(file_id, 0);
                    m.dir_->update_file_state(file_id, state);

                    if (200 <= sc && sc < 300)
                    {
                        if (md5.first)
                            m.dir_->update_file_hash(file_id, md5);
                        m.report_thumbnail(file_id, md5.first ? md5 : old_md5, 0, state, false);
                        m.dir_->inc_version();
                        m.save();
                    }
                    m.notify_file_change(file_id);
                });
            }));
        });

    }

    void model::schedule_batch_delete(arr<n4> folder_ids, arr<n4> file_ids)
    {
        if (!dir_)
            return;

        auto active_file_ids = dir_->mark_for_delete(folder_ids, file_ids);
        if (active_file_ids.file_ids.empty() && 1 == folder_ids.size())
        {
            auto folder_id = *folder_ids.cbegin();
            dir_->hide_folder(folder_id);
            notify_folder_delete(folder_id);
        }
        else
        {
            notify_state_change(dir_->sorted_folders_for_files(active_file_ids.file_ids));
            direct_delete(std::move(active_file_ids.file_ids));
        }
        save();
    }

    void model::direct_delete(arr<n4> file_ids)
    {
        if (!dir_)
            return;

        std::for_each(file_ids.cbegin(), file_ids.cend(), [this](auto file_id)
        {
            auto relative_path = dir_->encoded_path_from_file(file_id);
            if (!session_)
                return;
            net_add(session_->delete_file(relative_path, [run_model = run_model_, file_id, netid = net_nextid()](n4)
            {
                run_model([file_id, netid](model& m)
                {
                    m.net_remove(netid);
                    if (!m.dir_)
                        return;

                    n4 deleted_parent_folder_id = m.dir_->delete_file(file_id);
                    m.notify_file_delete(file_id);
                    m.save();
                    if (-1 != deleted_parent_folder_id)
                        m.notify_folder_delete(deleted_parent_folder_id);
                });
            }));
        });
    }

    void model::save()
    {
        if (!dir_)
            return;

        if ((dir_->version() == directory_version_ || saving_))
            return;
        
        saving_ = true;
        directory_version_ = dir_->version();
        
        auto content = arr<p1>();
        
        {
            auto r = size_on_disk_number<p8>();
            r += dir_->size_on_disk();
            r += size_on_disk_number<sz>();
            content.resize(r);
        }
        
        auto data = content.data();
        
        {
            auto r = serialise_number<p8>(data, VERSION_CACHE);
            r += dir_->serialise(data + r);
            r += serialise_number<sz>(data + r, r + size_on_disk_number<sz>());
        }
        
        platform_.file_writeall_safely(platform_.app_data_location() + "/" + email_ + ".bin", move(content), [run_model = run_model_](bool)
        {
            run_model([](model& m)
            {
                m.saving_ = false;
                m.save();
            });
        });

    }

    void model::login(str email, str pwd, bool remember_me, funk<void(str)> fun)
    {
        auto netid = net_nextid();

        net_add(openstack::session::connect(platform_, email, pwd, "Storage", "http://205.134.171.234:5000/v2.0/tokens", [fun, email, remember_me, pwd, netid, run_model = run_model_](auto sess)
        {
            run_model([fun, email, remember_me, pwd, netid, run_model, sess = sess.get()](model& m)
            {
                m.net_remove(netid);
                
                if (!sess)
                {
                    fun("Login failed");
                    return;
                }
                m.session_ = uptr<openstack::session>(sess);
                m.email_ = email;
                
                m.net_add(blomp_session::connect(m.platform_, email, pwd, [fun, netid = m.net_nextid(), run_model](http_status status, auto sess)
                {
                    run_model([run_model, fun, netid, status, sess = sess.get()](model& m)
                    {
                        m.net_remove(netid);

                        m.blomp_session_ = uptr<blomp_session>(sess);

                        m.read_or_create_local_cache([fun, run_model](bool)
                        {
                            fun("");

                            run_model([run_model](model& m)
                            {
                                auto netid = m.net_nextid();
                                if (!m.session_)
                                    return;
                                m.net_add(m.session_->quick_list("", "", [run_model, netid](auto lines)
                                {
                                    if (lines.empty())
                                        return;
                                    run_model([netid, run_model, lines = std::move(lines)](model& m)
                                    {
                                        m.net_remove(netid);
                                        if (!m.dir_)
                                            return;

                                        m.dir_->match_all_file_paths(lines);

                                        m.direct_download(m.dir_->file_ids_with_state(FileState::DOWNLOADING));
                                        m.direct_delete(m.dir_->file_ids_with_state(FileState::DELETING));
                                        {
                                            auto uploading_file_ids = m.dir_->reset_upload_states();
                                            m.notify_state_change(m.dir_->sorted_folders_for_files(uploading_file_ids));
                                        }
                                        m.load_fresh_folder(0);
                                    });
                                }));
                            });
                        });
                    });
                    sess.release();
                }));
                
                auto content = arr<p1>();
                {
                    auto r = size_on_disk_number<p8>();
                    r += size_on_disk_str(email);
                    r += size_on_disk_number<p8>();
                    content.resize(r);
                }
                
                auto data = content.data();
                
                {
                    auto r = serialise_number<p8>(data, VERSION_CONFIG);
                    r += serialise_str(data + r, email);
                    r += serialise_number<p8>(data + r, r + size_on_disk_number<p8>());
                }
                
                m.platform_.file_writeall_safely(m.platform_.app_data_location() + "/config.bin", move(content), [run_model, pwd, remember_me, email](bool success)
                {
                    run_model([run_model, email, pwd = remember_me ? pwd : ""](model& m)
                    {
                        m.platform_.file_write_password(email, pwd, [run_model](bool success)
                        {
                            if (!success)
                            {
                                run_model([](model& m)
                                {
                                    m.log(__FILE__, __LINE__, "Failed to write password", LogType::LERROR);
                                });
                            }
                        });
                    });
                });
                
            });
            sess.release();
        }));
    }

    FileType model::type_from_path(const str& path)
    {
        auto ext = filename_extension(path);
        if (ext == "JPG" || ext == "PNG" || ext == "JPEG" || ext == "GIF" || ext == "HEIC" || ext == "HEIF")
            return FileType::IMAGE;
        else if (ext == "TXT")
            return FileType::TEXT;
        else if (ext == "MOV" || ext == "MP4")
            return FileType::VIDEO;
        
        return FileType::OTHER;
    }

    str model::hash_path(p16 md5, const str& filename_or_path)
    {
        auto ext = filename_extension(filename_or_path);
        
        str hash = to_hex(md5);
        auto r = content_path_ + "/" + hash + "." + ext;
        return r;
    }

    void model::report_thumbnail(n4 file_id, p16 md5, n4 percentage, FileState state, bool fallback_only)
    {
        if (!dir_)
            return;

        if (!md5.first)
        {
            return;
        }
        if (100 != percentage && FileState::LOCAL != state)
        {
            return;
        }
        auto filename = dir_->file_name(file_id);
        auto type = type_from_path(filename);
        auto hashed_path = hash_path(md5, filename);
        if (!fallback_only && platform_.file_thumbnail(hashed_path, type, [run_model = run_model_, file_id](arr<p1> thumb)
        {
            if (thumb.empty())
                return;
            run_model([file_id, thumbnail = move(thumb)](model& m)
            {
                m.dir_->thumbnail(file_id, thumbnail);
                m.notify_file_change(file_id);
                m.dir_->inc_version();
                m.save();
            });
        }))
            return;
        if (FileType::IMAGE != type)
            return;

        create_thumbnail(hashed_path, platform_, [run_model = run_model_, file_id](arr<p1> data)
        {
            if (data.empty())
                return;
            run_model([file_id, thumbnail = move(data)](model& m)
            {
                m.dir_->thumbnail(file_id, thumbnail);
                m.notify_file_change(file_id);
                m.dir_->inc_version();
                m.save();
            });
        });
    }

}
