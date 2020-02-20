#include "caching_engine.h"
#include "openstack/openstack.h"
#include "directory.h"
#include <condition_variable>
#include "bridge/cpp/status.hpp"
#include "bridge/cpp/message.hpp"
#include "bridge/cpp/file_type.hpp"
#include <atomic>

#include "gui/login_form.h"
#include "gui/confirm_form.h"
#include "bridge.h"
#include "platform/content.h"

namespace ai
{
	str cache::version_text() const
	{
		return VERSION_TEXT;
	}

    bool cache::is_image_ext(const str& ext)
    {
        return ext == "jpg" || ext == "png" || ext == "jpeg" || ext == "gif" || ext == "heic";
    }

	void cache::report_thumbnail(n4 file_id, p16 md5, n4 percentage, FileState state, directory* dir, bool fallback_only)
	{
		if (!md5.first)
        {
            return;
        }
        if (100 != percentage && FileState::LOCAL != state)
        {
            return;
        }
        auto filename = dir->file_name(file_id);
        auto type = type_from_path(filename);
        auto hashed_path = hash_path(md5, filename);
        if (!fallback_only && platform_.file_thumbnail(hashed_path, type, [this, file_id](arr<p1> thumb)
                                     {
                                         if (thumb.empty())
                                         {
                                             return;
                                         }
                                         add_task([this, file_id, thumbnail = move(thumb)](auto dir)
                                                  {
                                                      dir->thumbnail(file_id, thumbnail);
                                                      callback_->file_change(file_id, *dir);
                                                      dir->inc_version();
                                                      save();
                                                  });
                                         
                                     }))
            return;
        if (FileType::IMAGE != type)
            return;

		create_thumbnail(hashed_path, platform_, [this, file_id](arr<p1> data)
		{
			if (data.empty())
				return;
			add_task([this, file_id, thumbnail = move(data)](auto dir)
			{
				dir->thumbnail(file_id, thumbnail);
				callback_->file_change(file_id, *dir);
				dir->inc_version();
				save();
			});
		});
	}

	void cache::read_safely(const str& filename, funk<bool(arr<p1>)> process_fun)
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

	cache::cache(n4 thumb_px, task_scheduler& scheduler, plat& platform, sptr<OsGui> gui) : thumb_px_(thumb_px), platform_(platform), gui_(gui)
	{
		scheduler.do_task([this]()
		{
			{
				std::mutex start_mutex;
				std::condition_variable start_cv;

                read_safely("config.bin", [this, &start_mutex, &start_cv](arr<p1> content)
				{
					if (content.empty())
					{
						login_form::show(*this, gui_, "");
                        gate lck(start_mutex);
                        start_cv.notify_one();
                        return true;
					}
                    
                    auto data = content.data();
                    p8 version = 0;
                    auto r = deserialise_number<p8>(data, version);
                    str email;
                    r += deserialise_str(data + r, email);
                    if (1 > version)
                    {
                        login_form::show(*this, gui_, email);
                        gate lck(start_mutex);
                        start_cv.notify_one();
                        return true;
                    }
                    
                    p8 size = 0;
                    r += deserialise_number<p8>(data + r, size);
                    if (size != r)
                        return false;

                    platform_.file_read_password(email, [this, &start_mutex, &start_cv, email](str pwd) mutable
                    {
                        if (pwd.empty())
						{
							login_form::show(*this, gui_, email);
						}
                        else
                        {
                            login(email, pwd, true, [this, email](str error)
                                  {
                                      if (!error.empty())
                                          login_form::show(*this, gui_, email);
                                  });
                        }
                        gate lck(start_mutex);
                        start_cv.notify_one();
                    });
                    
					return true;
				});
				
				gate lck(start_mutex);
				start_cv.wait(lck);
			}

			while (true)
			{
				funk<void(directory*)> task;

				{
					gate lck(task_mutex_);
					if (0 == tasks_.size())
						task_cv_.wait(lck);
					if (stopping_)
						return;
					task = tasks_.front();
					tasks_.pop_front();
				}

                if (!dir_)
                    log(__FILE__, __LINE__, "null dir");
                task(dir_.get());
			}
		});
	}

	cache::~cache()
	{
		{
			gate lck(task_mutex_);
			stopping_ = true;
			task_cv_.notify_one();
		}
		platform_.finish_io_and_wait();
	}

	void cache::sign_out()
	{
		add_task([this](directory* dir)
		{
            if (!dir)
                return;
            
            platform_.file_write_password(email_, "", [this](bool success)
            {
                if (!success)
                    log(__FILE__, __LINE__, "Failed to delete password", LogType::LERROR);

                add_task([this](directory *dir)
                {
                    if (!dir)
                        return;
                    
                    direct_cancel_network_operations(*dir);
                    
                    session_ = nullptr;
                    dir_ = nullptr;
                    callback_->reset();
                    gui_->reset();
                    login_form::show(*this, gui_, email_);
                });
            });
		});
	}

	void cache::login(const str& email, const str& pwd, bool remember_me, funk<void(str)> fun)
	{
        add_task([this, email, pwd, remember_me, fun](directory*)
		{
			auto netid = net_nextid();

            net_add(openstack::session::connect(platform_, email, pwd, "Storage", "http://205.134.171.234:5000/v2.0/tokens", [this, fun, email, remember_me, pwd, netid](auto sess)
			{
				net_remove(netid);
				if (!sess)
				{
                    fun("Login failed");
					return;
				}

				fun("");

				email_ = email;
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

                platform_.file_writeall_safely(platform_.app_data_location() + "/config.bin", move(content), [this, pwd, remember_me, email](bool success)
                {
                    if (!remember_me || !success)
                        return;
                    
                    platform_.file_write_password(email, pwd, [this](bool success)
                    {
                        if (!success)
                            log(__FILE__, __LINE__, "Failed to write password", LogType::LERROR);
                    });
                });

				read_safely(email + ".bin", [this, session = sess.get()](arr<p1> content)
				{
                    if (content.empty())
						dir_ = std::make_unique<directory>();
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

						dir_ = move(dir);
					}

					directory_version_ = dir_->version();

					add_task([this, session](auto dir) mutable
					{
                        if (!dir)
					        return;

                        gui_->gui_thread(std::make_shared<lambda_void_result>([this]()
                        {
                            gui_->show_cloudstore();
                        }));

						session_ = uptr<openstack::session>(session);
						if (!dir->empty())
							callback_->folder_change(-1, *dir);

                        auto netid = net_nextid();
						net_add(session_->quick_list("", "", [this, netid](auto lines)
						{
							net_remove(netid);

							check x(__FILE__, __LINE__);
							if (lines.empty())
							{
								report_message(severity::error, "Could not get list");
								return;
							}

							add_task([this, lines = move(lines)](auto dir)
							{
                                if (!dir)
							        return;
								dir->match_all_file_paths(lines);

                                direct_download(dir->file_ids_with_state(FileState::DOWNLOADING), dir);
								direct_delete(dir->file_ids_with_state(FileState::DELETING), dir);
								dir->reset_upload_states();

                                if (dir->version() != directory_version_)
                                    report_folder_change(-1);

                                load_fresh_folder(0, [this](directory* dir)
								{
                                    if (!dir)
                                        return;
                                    
									auto files = dir->list_files(0);
									for_each(files.cbegin(), files.cend(), [this, dir](const file_item& item)
									{
                                        if (item.thumbnail.empty())
                                            report_thumbnail(item.id, item.md5, item.percentage, item.state, dir, false);
									});
									callback_->folder_change(-1, *dir);
                                });
								save();
							});
						}));
					});

					return true;
				});

				sess.release();
			}));
		});
	}

	void cache::register_callback(cache_callback& callback)
	{
		check x(__FILE__, __LINE__);
		add_task([this, &callback](directory*)
		{
			callback_ = &callback;
			content_path_ = platform_.app_data_location() + "/cache/content";
			check x(__FILE__, __LINE__);

			platform_.make_path(content_path_, [this](bool result)
			{
				check x(__FILE__, __LINE__);
				if (result)
					report_message(severity::message, "Content directory " + content_path_);
				else
					report_message(severity::critical, "Cannot create content directory " + content_path_);
			});
			if (!session_)
				return;
			net_add(session_->quick_list("", "", [this, netid = net_nextid()](auto lines)
			{
				net_remove(netid);
				add_task([this, lines = move(lines)](auto dir)
				{
					check x(__FILE__, __LINE__);
					if (lines.empty())
					{
						report_message(severity::error, "Could not get list");
						return;
					}
					dir->match_all_file_paths(lines);
					report_folder_change(-1);
					load_fresh_folder(0, [this](directory* dir)
					{
                        if (!dir)
                            return;
						callback_->folder_change(-1, *dir);
					});
				});
			}));
		});
	}

	void cache::load_fresh_folder(n4 folder_id, funk<void(directory*)> fun)
	{
		check x(__FILE__, __LINE__);

		add_task([this, folder_id, fun](directory* dir)
		{
            if ( !dir)
                return;
			check x(__FILE__, __LINE__);

			auto path = dir->path_from_dir(folder_id);
			if (!session_)
				return;
            net_add(session_->list(openstack::scope::all_files, "", 0, { "","" }, path, openstack::timetrade::fastest, [this, fun, path, netid = net_nextid()](auto list)
			{
				net_remove(netid);

				check x(__FILE__, __LINE__);

				if (!list.data)
				{
					report_message(severity::error, "Cannot list Cloud folder");
					return;
				}

				auto names = arr<str>();
				auto md5s = arr<p16>();
				auto sizes = arr<n8>();
				for_each(list.objects.cbegin(), list.objects.cend(), [&names, &md5s, &sizes, data = list.data, offset = path.empty() ? 0 : path.size() + 1](auto&& obj)
				{
					if (offset >= obj.name.second - obj.name.first)
						return;

					names.push_back(obj.get_name(data, offset));
					sizes.push_back(obj.get_size(data));
					md5s.push_back(obj.get_hash(data));
				});

                add_task([this, names = move(names), md5s = move(md5s), sizes = move(sizes), path, fun](directory* dir)
				{
                    if (!dir)
                        return;
                    
					check x(__FILE__, __LINE__);

					dir->update_paths(names, sizes, md5s);
					fun(dir);
                    save();
				});
			}));
		});
	}

	void cache::refresh_folder(n4 folder_id)
	{
		load_fresh_folder(folder_id, [this, folder_id](directory* dir)
		{
            if (!dir)
                return;
            
			callback_->folder_change(folder_id, *dir);
			save();
		});
	}

    str cache::hash_path(p16 md5, const str& filename_or_path)
    {
        auto ext = filename_extension(filename_or_path);

        str hash = to_hex(md5);
        auto r = content_path_ + "/" + hash + "." + ext;
        return r;
    }

	void cache::direct_download(const arr<n4>& file_ids, directory* dir)
	{
        if (!dir)
            return;
        
		for_each(file_ids.cbegin(), file_ids.cend(), [this, dir](auto file_id)
		{
			auto progress_fun = [this, file_id, x = std::make_shared<std::atomic<Progress>>(Progress(-1, -1))](Progress p)
			{
				auto pp = x->exchange(p);
				if (!(-1 == pp.count && -1 == pp.total))
					return;
				add_task([this, file_id, x](auto dir)
				{
					auto p = x->exchange(Progress(-1, -1));
					if (FileState::DOWNLOADING == dir->file_file_state(file_id))
					{
						dir->update_file_percentage(file_id, percentage(p));
						callback_->file_change(file_id, *dir);
					}
				});
			};

			auto relative_path = dir->encoded_path_from_file(file_id);
			auto size = dir->file_size(file_id);
			auto md5 = dir->file_hash(file_id);

            auto netid = net_nextid();
            net_add(session_->download_file(relative_path, hash_path(md5, relative_path), size, md5, progress_fun, [this, file_id, netid, old_md5 = md5](n4 sc, n8, p16 md5)
			{
				net_remove(netid);
                add_task([this, file_id, sc, md5, old_md5](auto dir)
				{
					auto state = (200 <= sc && sc < 300) || 304 == sc ? FileState::LOCAL : FileState::NORMAL;
					dir->update_file_percentage(file_id, 0);
					dir->update_file_state(file_id, state);

					if (200 <= sc && sc < 300)
					{
						//if (len)
						//	dir->update_file_size(file_id, len);
						if (md5.first)
							dir->update_file_hash(file_id, md5);
                        report_thumbnail(file_id, md5.first ? md5 : old_md5, 0, state, dir, false);
						dir->inc_version();
						save();
					}
					callback_->file_change(file_id, *dir);
				});
			}));
		});

	}

	void cache::download_all(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		add_task([this, folder_ids, file_ids](directory* dir)
		{
			if (!dir || !session_)
				return;

			auto downloading_file_ids = dir->mark_for_download(folder_ids, file_ids);
			callback_->folder_state_changes(downloading_file_ids.folder_ids, *dir);
			direct_download(downloading_file_ids.file_ids, dir);
			save();
		});
	}

	void cache::save_all(const str& target_folder, const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		add_task([this, target_folder, folder_ids, file_ids](directory* dir)
		{
			if (!dir || !session_)
				return;

            auto all_file_ids = dir->expand(folder_ids, file_ids, [dir](n4 file_id)
			{
				auto state = dir->file_file_state(file_id);
				return FileState::LOCAL == state;
			});

			struct path_hash
			{
				str path;
				p16 hash;
			};
			auto paths = arr<path_hash>();
            for_each(all_file_ids.file_ids.cbegin(), all_file_ids.file_ids.cend(), [dir, &paths](auto file_id)
			{
				auto size = dir->file_size(file_id);
				auto md5 = dir->file_hash(file_id);
				if (!size || !md5.first)
					return;
				auto r = path_hash();
				r.path = dir->path_from_file(file_id);
				r.hash = md5;
				paths.push_back(r);
			});
			if (paths.empty())
				return;
			sort(paths.begin(), paths.end(), [](auto&& l, auto&& r) { return l.path < r.path; });

			sz len = common_file_path_length(paths.cbegin()->path, paths.rbegin()->path);
			auto filenames = arr<str>();
			auto hashes = arr<str>();
			std::for_each(paths.cbegin(), paths.cend(), [this, &filenames, &hashes, len](auto&& ph)
			{
                auto path = str(ph.path.cbegin() + len, ph.path.cend());
				filenames.push_back(path);
				hashes.push_back(hash_path(ph.hash, path));
			});
			platform_.file_copy_hierarchy(target_folder, filenames, hashes);
		});

	}

	void cache::direct_delete(const arr<n4>& file_ids, directory* dir)
	{
        if (!dir)
            return;
        
		for_each(file_ids.cbegin(), file_ids.cend(), [this, dir](auto file_id)
		{
			auto relative_path = dir->encoded_path_from_file(file_id);
            net_add(session_->delete_file(relative_path, [this, file_id, netid = net_nextid()](n4)
			{
				net_remove(netid);

				add_task([this, file_id](directory* dir)
				{
                    if (!dir)
                        return;
                    
					dir->delete_file(file_id);
					callback_->folder_change(dir->file_parent(file_id), *dir);
					save();
				});
			}));
		});
	}

	void cache::delete_all(const arr<n4>& folder_ids, const arr<n4>& file_ids)
	{
		add_task([this, folder_ids, file_ids](directory* dir)
		{
			if (!dir || !session_)
				return;

			auto active_file_ids = dir->mark_for_delete(folder_ids, file_ids);
			if (active_file_ids.file_ids.empty() && 1 == folder_ids.size())
			{
				auto folder_id = *folder_ids.cbegin();
				auto parent_id = dir->parent_folder(folder_id);
				dir->hide_folder(folder_id);
				callback_->folder_change(parent_id, *dir);
			}
			else
				callback_->folder_state_changes(active_file_ids.folder_ids, *dir);
			direct_delete(active_file_ids.file_ids, dir);
			save();
		});
	}

    FileType cache::type_from_path(const str& path)
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
    
    void cache::create_folder(n4 parent_folder_id, const str& folder_name, const sptr<OsForm>& form)
    {
        add_task([this, parent_folder_id, folder_name, form](directory* dir)
        {
            if (!dir)
                return;

            if (find_if(folder_name.cbegin(), folder_name.cend(), [](char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }) == folder_name.end())
            {
                if (form)
                    form->action_result(Status::FAIL, { Message(-1, "Enter a name containing an alphabetic character") });
                return;
            }
            auto success = dir->add_folder(parent_folder_id, folder_name);
            if (form)
                form->action_result(success ? Status::SUCCESS : Status::FAIL, {});
            callback_->folder_change(parent_folder_id, *dir);
        });
    }
    
    void cache::direct_rename_file(directory& dir, n4 old_file_id, n4 new_file_id)
    {
        auto size = dir.file_size(old_file_id);
        auto old_path = dir.encoded_path_from_file(old_file_id);
        auto new_path = dir.encoded_path_from_file(new_file_id);
        net_add(session_->copy_file(old_path, new_path, size, [this, old_path, old_file_id, new_file_id, netid = net_nextid()](n4 sc)
        {
            net_remove(netid);
            if (!(200 <= sc && sc < 300))
			{
                gui_->show_message("Failed to copy " + old_path);
            	add_task([this, new_file_id](directory* dir)
				{
            		if (!dir)
            			return;

					n4 deleted_parent_folder_id = dir->delete_file(new_file_id);
					callback_->folder_change(dir->file_parent(new_file_id), *dir);
                    if (-1 != deleted_parent_folder_id)
                        callback_->folder_change(dir->parent_folder(deleted_parent_folder_id), *dir);
				});

				return;
			}

            net_add(session_->delete_file(old_path, [this, old_path, old_file_id, new_file_id, netid = net_nextid()](n4 sc)
            {
                net_remove(netid);

                if (!(200 <= sc && sc < 300))
                {
                    gui_->show_message("Failed to delete " + old_path);
                    return;
                }

                add_task([this, old_file_id](directory* dir)
                {
                    if (!dir)
                        return;

                    n4 deleted_parent_folder_id = dir->delete_file(old_file_id);
                    callback_->folder_change(dir->file_parent(old_file_id), *dir);
                    save();
                    if (-1 != deleted_parent_folder_id)
                        callback_->folder_change(dir->parent_folder(deleted_parent_folder_id), *dir);
                });
            }));
        }));

    }

    void cache::rename_file(n4 file_id, str new_name, const sptr<OsForm>& form)
    {
        add_task([this, file_id, new_name, form](directory* dir)
        {
            if (!dir)
                return;
            
            auto item = dir->file(file_id);
            auto old_name = item.name;
            
            if (!form)
                return;
            
            n4 new_file_id = dir->file_copy(file_id, new_name);
            if (-1 == new_file_id)
            {
                form->action_result(Status::FAIL, {Message(-1,"File already exists")});
                return;
            }
            direct_rename_file(*dir, file_id, new_file_id);
            form->action_result(Status::SUCCESS, {});
        });
    }
    
    void cache::rename_folder(n4 folder_id, str new_name, const sptr<OsForm>& form)
    {
        add_task([this, folder_id, new_name, form](directory* dir)
        {
            if (!dir || !form)
                return;

            auto file_ids = dir->folder_copy(folder_id, new_name);
            if (file_ids.empty())
            {
                form->action_result(Status::FAIL, {Message(-1,"No eligible files found to rename")});
                return;
            }
            for_each(file_ids.begin(), file_ids.end(), [this, dir](auto&& old_new)
            {
                direct_rename_file(*dir, old_new.first, old_new.second);
            });
            form->action_result(Status::SUCCESS, {});
            if (1 == file_ids.size())
                gui_->show_message("Moving one file");
            else
                gui_->show_message("Moving " + std::to_string(file_ids.size()) + " files");
        });
    }

    void cache::direct_upload(directory* dir, n4 folder_id, const str& filename, const str& path, arr<p1> thumb)
    {
        auto file_id = dir->file_from_name(folder_id, filename);
        if (-1 != file_id)
        {
            auto state = dir->file_file_state(file_id);
            if (FileState::UPLOADING == state || FileState::DOWNLOADING == state || FileState::DELETING == state)
                return;
        }
        else
            file_id = dir->push_back(folder_id, filename);
        dir->update_file_state(file_id, FileState::UPLOADING);
        dir->thumbnail(file_id, thumb);
        dir->inc_version();
        
        auto relative_path = dir->encoded_path_from_file(file_id);

        auto progress_fun = [this, file_id, x = std::make_shared<std::atomic<Progress>>(Progress(-1, -1))](Progress p)
        {
			auto pp = x->exchange(p);
			if (!(-1 == pp.count && -1 == pp.total))
				return;
            add_task([this, file_id, x](auto dir)
            {
				auto p = x->exchange(Progress(-1, -1));
                if (FileState::UPLOADING == dir->file_file_state(file_id))
                {
                    auto perc = percentage(p);
                    dir->update_file_percentage(file_id, perc);
                    callback_->file_change(file_id, *dir);
                }
            });
        };
        callback_->folder_change(folder_id, *dir);
        net_add(session_->upload_file(relative_path, path, { 0,0 }, content_type_from_path(path), progress_fun, [this, file_id, netid = net_nextid(), path, thumbempty = thumb.empty(), filename](n4 sc, n8 len, p16 md5)
        {
            net_remove(netid);

            add_task([this, sc, len, md5, file_id, path, thumbempty, filename](directory* dir)
            {
                if (!dir)
                    return;

                if (200 <= sc && sc < 300)
                {
                    if (299 == sc)
                    {
                        dir->update_file_state(file_id, FileState::LOCAL);
                        if (thumbempty)
                            report_thumbnail(file_id, md5, 100, FileState::LOCAL, dir, false);
                    }
                    else if (298 == sc)
                    {
                        dir->update_file_state(file_id, FileState::NORMAL);
                    }
                    else
                    {
                        dir->update_file_state(file_id, FileState::NORMAL);
                        platform_.copy_file(path, hash_path(md5, filename), [this, file_id, md5, thumbempty](bool success)
                                            {
                                                if (!success)
                                                    return;
                                                add_task([this, file_id, md5, thumbempty](directory* dir)
                                                         {
                                                             if (!dir)
                                                                 return;
                                                             
                                                             dir->update_file_state(file_id, FileState::LOCAL);
                                                             if (thumbempty)
                                                                 report_thumbnail(file_id, md5, 100, FileState::LOCAL, dir, false);
                                                             callback_->file_change(file_id, *dir);
                                                             dir->inc_version();
                                                             save();
                                                         });
                                            });

                    }

                }
                else
                    dir->update_file_state(file_id, FileState::DELETED);
                dir->update_file_percentage(file_id, 0);
                dir->update_file_size(file_id, len);
                dir->update_file_hash(file_id, md5);
                callback_->file_change(file_id, *dir);
                dir->inc_version();
                save();
            });
        }));

    }
    
    void cache::upload_files_only(n4 target_folder_id, const arr<str>& filenames, const arr<str>& paths, arr<arr<p1>> thumbnails)
    {
        assert(filenames.size() == paths.size());
        
        add_task([this, target_folder_id, filenames, paths, thumbnails](directory* dir)
        {
            if (!dir)
                return;
            
            for (int i = 0; i < filenames.size(); ++i)
            {
                direct_upload(dir, target_folder_id, filenames[i], paths[i], move(thumbnails[i]));
            }
        });
    }
    
	void cache::upload_all(n4 target_folder_id, const str& common_folder, const arr<str>& relative_file_paths)
	{
		add_task([this, target_folder_id, common_folder, relative_file_paths](directory*)
		{
			auto paths = arr<str>();
			for_each(relative_file_paths.cbegin(), relative_file_paths.cend(), [common_folder, &paths](const str& path) { paths.push_back(common_folder + "/" + path); });
			platform_.file_expand_directories(paths, [this, target_folder_id, common_folder](const arr<str>& paths)
			{
				add_task([this, target_folder_id, common_folder, paths](directory* dir)
				{
					if (!dir || !session_)
						return;

					for_each(paths.cbegin(), paths.cend(), [this, target_folder_id, common_folder, dir](const str& path)
					{
						auto folder_id = target_folder_id;
						str filename;

						{
							if (path.size() <= common_folder.size() || 0 != strncmp(path.c_str(), common_folder.c_str(), common_folder.size()))
								return;
							auto relative_path = str(path.cbegin() + common_folder.size(), path.cend());
							if (0 == relative_path.size() || '/' != relative_path[0])
								return;

							auto it = relative_path.cbegin() + 1;
							while (true)
							{
								auto next = find(it, relative_path.cend(), '/');
								if (relative_path.cend() != next)
								{
									auto dir_name = str(it, next);
									it = next + 1;
									if (dir_name.empty())
										continue;
									folder_id = dir->add_folder(folder_id, dir_name);
								}
								else
								{
									filename = str(it, relative_path.cend());
									break;
								}
							}

							if (filename.empty())
								return;
						}
                        
                        direct_upload(dir, folder_id, filename, path, {});
					});
					callback_->folder_change(target_folder_id, *dir);
					save();
				});
			});
		});
	}
	void cache::preview(n4 file_id)
	{
		add_task([this, file_id](directory* dir)
		{
            if (!dir)
                return;
            
			auto state = dir->file_file_state(file_id);
			if (FileState::LOCAL != state)
				return;

			auto size = dir->file_size(file_id);
			auto md5 = dir->file_hash(file_id);
			if (0 == size || md5.first == 0)
				return;

            auto filename = dir->file_name(file_id);
            auto type = type_from_path(filename);
			gui_->preview(type, hash_path(md5, filename), file_id, dir->file_name(file_id));
		});
	}

	void cache::list(n4 parent_dir_id)
	{
		add_task([this, parent_dir_id](directory* dir)
		{
			if (!dir || !session_ || !callback_)
				return;

			callback_->folder_change(parent_dir_id, *dir);
		});
	}

	void cache::list_directory(const str& path, freshness /*policy*/, funk<void(status, const arr<file>&, const arr<dir>&)> fun)
	{
        add_task([this, path, fun](directory*)
		{
			if (!session_)
			{
				fun(status::error, {}, {});
				return;
			}
			session_->list(openstack::scope::subdir, path, 0, { "","" }, "", openstack::timetrade::fastest, [this, fun, path](auto list)
			{
				plat::log_debug("list " + path + " " + std::to_string(list.objects.size()));
				if (!list.data)
				{
					fun(status::error, {}, {});
					return;
				}
				auto files = arr<file>();
				for_each(list.objects.cbegin(), list.objects.cend(), [&files, data = list.data](auto&& obj)
				{
					auto detail = file();
					detail.name = str(data + obj.name.first, data + obj.name.second);
					detail.size = str(data + obj.bytes.first, data + obj.bytes.second);
					detail.last_modified = str(data + obj.last_modified.first, data + obj.last_modified.second);
					files.push_back(detail);
				});
				auto dirs = arr<dir>();
				for_each(list.subdirs.cbegin(), list.subdirs.cend(), [&dirs, data = list.data](auto&& dir)
				{
					dirs.push_back({ str(data + dir.first, data + dir.second) });
				});

				add_task([files = move(files), dirs = move(dirs), fun](auto)
				{
					fun(status::success, files, dirs);
				});
			});
		});

	}

	void cache::add_task(funk<void(directory*)> fun)
	{
		gate lck(task_mutex_);
		tasks_.push_back(fun);
		if (1 == tasks_.size())
			task_cv_.notify_one();
	}

	void cache::report_message(severity s, const str& msg)
	{
		add_task([this, s, msg](auto)
		{
			callback_->report(s, msg);
		});
	}

	void cache::report_folder_change(n4 id)
	{
		add_task([this, id](auto dir)
		{
			callback_->folder_change(id, *dir);
		});
	}

	void cache::save()
	{
		add_task([this](auto dir)
		{
			if ((dir->version() == directory_version_ || saving_))
				return;

			saving_ = true;
			directory_version_ = dir->version();

			auto content = arr<p1>();

			{
				auto r = size_on_disk_number<p8>();
				r += dir->size_on_disk();
				r += size_on_disk_number<sz>();
				content.resize(r);
			}

			auto data = content.data();

			{
				auto r = serialise_number<p8>(data, VERSION_CACHE);
				r += dir->serialise(data + r);
				r += serialise_number<sz>(data + r, r + size_on_disk_number<sz>());
			}

			platform_.file_writeall_safely(platform_.app_data_location() + "/" + email_ + ".bin", move(content), [this](bool)
			{
				saving_ = false;
				save();
			});
		});
	}

	void cache::net_add(const sptr<CancellationToken>& cancellation)
	{
		if (!cancellation)
			return;

		std::lock_guard<std::mutex> lck(net_mutex_);
		netops_[netid_] = cancellation;
	}

	void cache::net_remove(n8 netid)
	{
		std::lock_guard<std::mutex> lck(net_mutex_);
		netops_.erase(netid);
	}

	sptr<CancellationToken> cache::net_cancellation(n8 netid)
	{
		std::lock_guard<std::mutex> lck(net_mutex_);
		
		auto it = netops_.find(netid);
		
		return netops_.end() == it ? nullptr : it->second;
	}

	n8 cache::net_nextid()
	{
		std::lock_guard<std::mutex> lck(net_mutex_);
		return ++netid_;
	}

	void cache::cancel_network_operations()
	{
		add_task([this](directory* dir)
		{
            if (!dir)
                return;
            
			{
                auto count = direct_cancel_network_operations(*dir);
                auto msg = [count]()
                {
                    str r;
                    switch (count)
                    {
                        case 0: r = "Not a single active task was found"; break;
                        case 1: r = "One task was cancelled"; break;
                        default: r = std::to_string(count) + " tasks were cancelled"; break;
                    }
                    return r;
                }();
                gui_->show_message(msg);
			}

			save();
		});
	}
    
    sz cache::direct_cancel_network_operations(directory& dir)
    {
        std::lock_guard<std::mutex> lck(net_mutex_);
        int count = accumulate(netops_.cbegin(), netops_.cend(), 0, [](int acc, auto&& item)
                               {
                                   if (!item.second)
                                       return acc;
                                   item.second->cancel();
                                   return acc + 1;
                               });
        dir.reset_active_states();
        return count;
    }

    void cache::log(const char* file, int line, const str& message, LogType type)
    {
        platform_.log(type, line, file, message);

    }
}
