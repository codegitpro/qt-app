#pragma once
#include "../viewmodel.h"
#include "../../bridge/cpp/directories_detail.hpp"
#include "../../bridge/cpp/files_detail.hpp"
#include "../../model/directory_notification.h"
#include "../../bridge/cpp/folder_view.hpp"
#include "rename_file_subviewmodel.h"
#include "preview_viewmodel.h"
#include "../../bridge/cpp/file_item_detail.hpp"
#include "../../bridge/cpp/folder_item_detail.hpp"
#include "../../bridge/cpp/state_change.hpp"
#include "create_folder_viewmodel.h"
#include "rename_folder_viewmodel.h"

namespace ai
{
    struct folder_subviewmodel : FolderViewmodel, FormViewmodel, directory_notification, std::enable_shared_from_this<folder_subviewmodel>
    {
        folder_subviewmodel(const sptr<UiCallback>& view, ScreenType type, services& services, Diagnostic& log, n4 folder_id)
        :
        view_(view),
        screentype_(type),
        services_(services),
        log_(log),
        folder_id_(folder_id),
        dirs_({},{}),
        files_({},{},{},{},{},{})
        {
        }
        virtual ~folder_subviewmodel()
        {
            
        }
        
        std::vector<Field> fields() final { return {}; }
        
        void on_load(const std::shared_ptr<FolderView> & view) final
        {
            folder_view_ = view;
            reset();
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            form_ = form;
            form_view_ = view;
            view->set_screen_name(folder_name_);
        }

        void load_folder(funk<void()> fun)
        {
            services_.listen(folder_id_, shared_from_this(), [fun, view = view_, wself = std::weak_ptr<folder_subviewmodel>(shared_from_this())](str folder_name, DirectoriesDetail dirs, FilesDetail files, arr<str> dir_names, arr<str> file_names)
            {
                run_ui(view, [fun, wself, folder_name, dirs = std::move(dirs), files = std::move(files), dir_names = std::move(dir_names), file_names = std::move(file_names)]()
                {
                    auto sself = wself.lock();
                    if (!sself)
                        return;
                    sself->folder_name_ = folder_name.empty() ? "Directory" : folder_name;
                    if (sself->form_view_)
                        sself->form_view_->set_screen_name(folder_name);
                    
                    sself->folder_selected_.resize(dir_names.size());
                    sself->file_selected_.resize(file_names.size());
                    sself->folder_selected_count_ = 0;
                    sself->file_selected_count_ = 0;

                    sself->dirs_ = std::move(dirs);
                    sself->files_ = std::move(files);
                    sself->folder_names_ = std::move(dir_names);
                    sself->file_names_ = std::move(file_names);
                    
                    sself->reset();
                    
                    fun();
                });
            });
        }
        
        void reset()
        {
            if (!folder_view_)
                return;
            auto file_items = arr<FileItemDetail>();
            for (int i = 0; i < file_names_.size(); ++i)
            {
                file_items.push_back(file_item(i));
            }
            auto folder_items = arr<FolderItemDetail>();
            for (int i = 0; i < folder_names_.size(); ++i)
            {
                folder_items.push_back(folder_item(i));
            }
            folder_view_->on_reset(folder_items, file_items);

        }
        
        void show(const sptr<FolderView> folder_view, funk<void()> fun)
        {
            if (!folder_view)
                return;
            auto sself = shared_from_this();
            folder_view->create_folder_view(sself, sself);

            load_folder([wself = std::weak_ptr<folder_subviewmodel>(shared_from_this()), folder_view, fun]()
            {
                fun();
            });
        }
        

    private:
        sptr<UiCallback> view_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        n4 folder_id_;
        sptr<FolderView> folder_view_;
        sptr<FormView> form_view_;
        
        sptr<GenericForm> form_;
        str folder_name_;
        
        DirectoriesDetail dirs_;
        FilesDetail files_;
        arr<str> file_names_;
        arr<str> folder_names_;
        
        arr<bool> folder_selected_;
        arr<bool> file_selected_;
        int folder_selected_count_ = 0;
        int file_selected_count_ = 0;
        
        arr<par<n4, n4>> sorted_file_id_position_index_;
        
        void back() final
        {
        }
        
        enum class control : n4
        {
            cancel_selections = 1,
            select_all = 2,
            button_bar = 3,
            button_bar_rename = 4,
            download = 5,
            delete_btn = 6,
            rename = 7,
            create_folder = 8
        };
        
        static constexpr n4 id_cancel_selections = static_cast<n4>(control::cancel_selections);
        static constexpr n4 id_select_all = static_cast<n4>(control::select_all);
        static constexpr n4 id_button_bar = static_cast<n4>(control::button_bar);
        static constexpr n4 id_button_bar_rename = static_cast<n4>(control::button_bar_rename);
        static constexpr n4 id_download = static_cast<n4>(control::download);
        static constexpr n4 id_delete_btn = static_cast<n4>(control::delete_btn);
        static constexpr n4 id_rename = static_cast<n4>(control::rename);
        static constexpr n4 id_create_folder = static_cast<n4>(control::create_folder);

        n4 find_file_index(n4 file_id) const
        {
            auto& ids = files_.ids;
            auto found = std::find(ids.begin(), ids.end(), file_id);
            return ids.end() == found ? -1 : static_cast<int>(std::distance(ids.begin(), found));
        }
        
        n4 find_folder_index(n4 folder_id) const
        {
            auto& ids = dirs_.ids;
            auto found = std::find(ids.begin(), ids.end(), folder_id);
            return ids.end() == found ? -1 : static_cast<int>(std::distance(ids.begin(), found));
        }
        
        bool clear_file_selection()
        {
            if (0 < file_selected_count_)
            {
                file_selected_count_ = 0;
                std::fill(file_selected_.begin(), file_selected_.end(), false);
                return true;
            }
            return false;
        }
        
        bool clear_dir_selection()
        {
            if (0 < folder_selected_count_)
            {
                folder_selected_count_ = 0;
                std::fill(folder_selected_.begin(), folder_selected_.end(), false);
                return true;
            }
            return false;
        }

        void action(n4 element_id) final
        {
            if (!form_ || !folder_view_)
                return;
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                case control::cancel_selections:
                {
                    auto file_indexes_selected = harvest_file_selection_indexes();
                    auto folder_indexes_selected = harvest_folder_selection_indexes();
                    if (folder_view_ && (!file_indexes_selected.empty() || !folder_indexes_selected.empty()))
                    {
                        folder_view_->on_clear_selection(folder_indexes_selected, file_indexes_selected);
                    }
                    form_->enable(id_button_bar, false);
                    break;
                }
                case control::select_all:
                {
                    std::fill(folder_selected_.begin(), folder_selected_.end(), true);
                    std::fill(file_selected_.begin(), file_selected_.end(), true);
                    folder_selected_count_ = static_cast<int>(folder_selected_.size());
                    file_selected_count_ = static_cast<int>(file_selected_.size());
                    if (folder_view_)
                        folder_view_->on_select_all();
                    form_->enable(id_button_bar, true);
                    break;
                }
                case control::download:
                {
                    services_.schedule_batch_download(harvest_folder_selection(), harvest_file_selection());
                    form_->enable(id_button_bar, false);
                    break;
                }
                case control::delete_btn:
                {
                    services_.schedule_batch_delete(harvest_folder_selection(), harvest_file_selection());
                    form_->enable(id_button_bar, false);
                    break;
                }
                case control::create_folder:
                {
                    auto vm = std::make_shared<create_folder_viewmodel>(view_, screentype_, services_, folder_id_);
                    form_view_->push_form(vm);

                    break;
                }
                case control::rename:
                {
                    if (!form_view_)
                        break;
                    if (1 == file_selected_count_)
                    {
                        auto found = std::find(file_selected_.begin(), file_selected_.end(), true);
                        if (file_selected_.end() == found)
                            return;
                        auto index = static_cast<n4>(std::distance(file_selected_.begin(), found));
                        auto vm = std::make_shared<rename_file_subviewmodel>(view_, screentype_, services_, files_.ids[index], file_names_[index]);
                        form_view_->push_form(vm);
                        
                        file_selected_[index] = false;
                        file_selected_count_ = 0;
                        if (form_)
                            form_->enable(id_button_bar, false);
                        if (folder_view_)
                        {
                            folder_view_->on_clear_selection({}, {index});
                        }
                    }
                    else if (1 == folder_selected_count_)
                    {
                        auto found = std::find(folder_selected_.begin(), folder_selected_.end(), true);
                        if (folder_selected_.end() == found)
                            return;
                        auto index = static_cast<int>(std::distance(folder_selected_.begin(), found));
                        auto vm = std::make_shared<rename_folder_viewmodel>(view_, screentype_, services_, dirs_.ids[index], folder_names_[index]);
                        form_view_->push_form(vm);
                        
                        folder_selected_[index] = false;
                        folder_selected_count_ = 0;
                        if (form_)
                        {
                            form_->enable(id_button_bar, false);
                        }
                        if (folder_view_)
                        {
                            folder_view_->on_clear_selection({index}, {});
                        }

                        
                    }
                    break;
                }
                default: break;
            }
        }
        
        arr<n4> harvest_file_selection_indexes()
        {
            auto indexes = arr<n4>();
            std::for_each(file_selected_.begin(), file_selected_.end(), [this, &indexes, i = 0](bool selected) mutable
                          {
                              if (selected)
                                  indexes.push_back(i);
                              ++i;
                          });
            clear_file_selection();
            return indexes;
        }
        
        arr<n4> harvest_folder_selection_indexes()
        {
            auto indexes = arr<n4>();
            std::for_each(folder_selected_.begin(), folder_selected_.end(), [this, &indexes, i = 0](bool selected) mutable
                          {
                              if (selected)
                                  indexes.push_back(i);
                              ++i;
                          });
            clear_dir_selection();
            return indexes;
        }
        
        arr<n4> harvest_file_selection()
        {
            auto file_ids = arr<n4>();
            std::for_each(file_selected_.begin(), file_selected_.end(), [this, &file_ids, i = 0](bool selected) mutable
                          {
                              if (selected)
                                  file_ids.push_back(files_.ids[i]);
                              ++i;
                          });
            clear_file_selection();
            return file_ids;
        }
        
        arr<n4> harvest_folder_selection()
        {
            auto folder_ids = arr<n4>();
            std::for_each(folder_selected_.begin(), folder_selected_.end(), [this, &folder_ids, i = 0](bool selected) mutable
                          {
                              if (selected)
                                  folder_ids.push_back(dirs_.ids[i]);
                              ++i;
                          });
            clear_dir_selection();
            return folder_ids;
        }
        
        int32_t folder_id() final
        {
            return folder_id_;
        }
        
        std::string list_detail(int32_t row) const
        {
            if (0 <= row && row < files_.ids.size())
            {
                auto percentage = files_.percentages[row];
                if (0 != percentage && 100 != percentage)
                    return std::to_string(percentage) + " %";
                str r;
                str suffix;
                double size = files_.sizes[row];
                if (size >= 1000000000000)
                {
                    r = std::to_string(round(size/100000000000.0)/10.0);
                    suffix = " TB";
                }
                else if (size >= 1000000000)
                {
                    r = std::to_string(round(size/100000000.0)/10.0);
                    suffix = " GB";
                }
                else if (size >= 1000000)
                {
                    r = std::to_string(round(size/100000.0)/10.0);
                    suffix = " MB";
                }
                else if (size >= 1000)
                {
                    r = std::to_string(round(size/100.0)/10.0);
                    suffix = " KB";
                }
                else
                {
                    r = std::to_string(size);
                }
                auto pos = r.find('.');
                if (std::string::npos != pos)
                {
                    if (r.size() > pos + 1 && '0' == r[pos+1])
                        r.erase(pos, std::string::npos);
                    else
                        r.erase(pos + 2, std::string::npos);
                }
                return r + suffix;
            }
            return "";
        }
        std::vector<uint8_t> list_thumbnail_content(int32_t row) final
        {
            return 0 <= row && row < files_.ids.size() ? files_.thumbnails[row] : arr<p1>();
        }
        bool list_selected(int32_t section, int32_t row) final
        {
            switch (section)
            {
                case 0:
                {
                    if (row < 0 || folder_names_.size() <= row)
                        break;
                    return folder_selected_[row];
                }
                case 1:
                {
                    if (row < 0 || file_names_.size() <= row)
                        break;
                    return file_selected_[row];
                }
            }
            return false;
            
        }
        
        void list_action(int32_t section, int32_t row, int32_t sub_index) final
        {
            int previous_selection_count = folder_selected_count_ + file_selected_count_;
            
            switch (section)
            {
                case 0:
                {
                    if (row < 0 || folder_names_.size() <= row)
                        break;
                    if (0 == sub_index || 1 == sub_index)
                    {
                        bool selected = folder_selected_[row];
                        if (selected)
                        {
                            --folder_selected_count_;
                        }
                        else
                        {
                            if (0 == sub_index && 0 == folder_selected_count_ && 0 == file_selected_count_)
                            {
                                // navigate
                                n4 folder_id = dirs_.ids[row];
                                auto vm = std::make_shared<folder_subviewmodel>(view_, screentype_, services_, log_, folder_id);
                                vm->show(folder_view_, [vm](){});
                                return;
                            }
                            ++folder_selected_count_;
                        }
                        folder_selected_[row] = !selected;
                        if (folder_view_)
                            folder_view_->on_folder_select(row, !selected);

                        break;
                    }
                    break;
                }
                case 1:
                {
                    if (row < 0 || file_names_.size() <= row)
                        break;
                    if (0 == sub_index || 1 == sub_index)
                    {
                        bool selected = file_selected_[row];
                        if (selected)
                        {
                            --file_selected_count_;
                        }
                        else
                        {
                            if (0 == sub_index && 0 == folder_selected_count_ && 0 == file_selected_count_ && FileState::LOCAL == files_.states[row])
                            {
                                // preview
                                n4 file_id = files_.ids[row];
                                auto vm = std::make_shared<preview_viewmodel>(view_, screentype_, services_, file_id, file_names_[row], folder_view_);
                                vm->show([vm](){});
                                return;
                            }

                            ++file_selected_count_;
                        }
                        file_selected_[row] = !selected;
                        if (folder_view_)
                            folder_view_->on_file_select(row, !selected);
                        break;
                    }
                    break;
                }
            }
            int current_selection_count = folder_selected_count_ + file_selected_count_;
            if (previous_selection_count != current_selection_count && previous_selection_count < 3 && current_selection_count < 3)
            {
                switch (current_selection_count)
                {
                    case 0: form_->enable(id_button_bar, false); break;
                    case 1: form_->enable(id_button_bar_rename, true); break;
                    default: form_->enable(id_button_bar, true); break;
                }
            }
        }

        void on_refresh(DirectoriesDetail dirs, FilesDetail files, arr<str> dir_names, arr<str> file_names) final
        {
            folder_selected_.resize(dir_names.size());
            file_selected_.resize(file_names.size());
            folder_selected_count_ = 0;
            file_selected_count_ = 0;

            dirs_ = std::move(dirs);
            files_ = std::move(files);
            folder_names_ = std::move(dir_names);
            file_names_ = std::move(file_names);
            
            reset();
        }

        void build_index()
        {
            if (sorted_file_id_position_index_.size() == files_.ids.size())
                return;
            
            auto indexes = arr<par<n4, n4>>();
            indexes.resize(files_.ids.size());
            std::transform(files_.ids.begin(), files_.ids.end(), indexes.begin(), [i = 0](n4 file_id) mutable { return std::make_pair(file_id, i++); });
            std::sort(indexes.begin(), indexes.end(), [](auto&& l, auto&& r) { return l.first < r.first; });
            
            sorted_file_id_position_index_.clear();
            sorted_file_id_position_index_ = std::move(indexes);
        }
        
        void on_state_changes(arr<n4> file_ids, arr<FileState> states) final
        {
            assert(std::is_sorted(file_ids.begin(), file_ids.end()));
            
            build_index();
            
            auto state_changes = arr<StateChange>();

            auto b = sorted_file_id_position_index_.begin();
            
            for (auto it = file_ids.begin(); it != file_ids.end() && b != sorted_file_id_position_index_.end(); ++it)
            {
                n4 file_id = *it;
                b = std::find_if(b, sorted_file_id_position_index_.end(), [file_id](auto&& item) { return item.first == file_id; });
                if (sorted_file_id_position_index_.end() != b)
                {
                    n4 index = b->second;
                    auto state = states[std::distance(file_ids.begin(), it)];
                    state_changes.push_back(StateChange(index, state));
                    files_.states[index] = state;
                }
            }

            if (folder_view_)
            {
                folder_view_->on_file_states_change(state_changes);
            }

//            auto state_changes = arr<StateChange>();
//            std::for_each(states.begin(), states.end(), [this, &state_changes, i = 0](FileState state) mutable
//            {
//                if (state == files_.states[i++])
//                    return;
//
//                if (state_changes.empty())
//                    state_changes.reserve(files_.ids.size()-i+1);
//                state_changes.push_back(StateChange(i-1, state));
//                files_.states[i-1] = state;
//            });
//            if (folder_view_)
//            {
//                folder_view_->on_file_states_change(state_changes);
//            }
        }

        void on_file_changed(n4 file_id, n4 title_id, n8 size, n4 percentage, FileState state, arr<p1> thumbnail) final
        {
            n4 index = find_file_index(file_id);
            if (index < 0)
                return;
            files_.titles[index] = title_id;
            files_.sizes[index] = size;
            files_.percentages[index] = percentage;
            files_.states[index] = state;
            files_.thumbnails[index] = std::move(thumbnail);
            
            if (folder_view_)
                folder_view_->on_file_change(index, file_item(index));
        }
        
        void on_folder_added(n4 folder_id, n4 title_id, str title, n4 next_to_folder_id) final
        {
            if (find_folder_index(folder_id) >= 0)
                return;

            n4 index = -1 == next_to_folder_id ? 0 : find_folder_index(next_to_folder_id);
            if (index < 0)
            {
                index = 0;
            }
            
            dirs_.ids.insert(dirs_.ids.begin()+index, folder_id);
            dirs_.titles.insert(dirs_.titles.begin()+index, title_id);
            
            folder_names_.insert(folder_names_.begin()+index, title);
            
            folder_selected_.insert(folder_selected_.begin()+index, false);
            
            if (folder_view_)
                folder_view_->on_folder_added(index, folder_item(index));
        }

        void remove_index()
        {
            sorted_file_id_position_index_.clear();
        }

        void on_file_added(n4 file_id, n4 title_id, n8 size, n4 percentage, FileState state, str title, n4 next_to_file_id) final
        {
            if (find_file_index(file_id) >= 0)
                return;

            remove_index();
            
            n4 index = -1 == next_to_file_id ? 0 : find_file_index(next_to_file_id);
            if (index < 0)
            {
                index = 0;
            }

            files_.ids.insert(files_.ids.begin()+index, file_id);
            files_.titles.insert(files_.titles.begin()+index, title_id);
            files_.sizes.insert(files_.sizes.begin()+index, size);
            files_.percentages.insert(files_.percentages.begin()+index, percentage);
            files_.states.insert(files_.states.begin()+index, state);
            files_.thumbnails.insert(files_.thumbnails.begin()+index, arr<p1>());
            
            file_names_.insert(file_names_.begin()+index, title);
            
            file_selected_.insert(file_selected_.begin()+index, false);
            
            if (folder_view_)
                folder_view_->on_file_added(index, file_item(index));
        }

        void on_percentage_changed(n4 file_id, n4 percentage) final
        {
            n4 index = find_file_index(file_id);
            if (index < 0)
                return;
            
            files_.percentages[index] = percentage;
            if (folder_view_)
            {
                folder_view_->on_file_detail_change(index, std::to_string(percentage) + " %");
            }
        }
        
        void on_file_removed(n4 file_id) final
        {
            n4 index = find_file_index(file_id);
            if (index < 0)
                return;
            
            remove_index();
            
            files_.ids.erase(files_.ids.begin()+index);
            files_.titles.erase(files_.titles.begin()+index);
            files_.sizes.erase(files_.sizes.begin()+index);
            files_.percentages.erase(files_.percentages.begin()+index);
            files_.states.erase(files_.states.begin()+index);
            files_.thumbnails.erase(files_.thumbnails.begin()+index);
            
            file_names_.erase(file_names_.begin()+index);
            
            bool selected = *(file_selected_.begin()+index);
            if (selected)
                --file_selected_count_;
            file_selected_.erase(file_selected_.begin()+index);
            
            if (folder_view_)
                folder_view_->on_file_remove(index);
        }

        void on_folder_removed(n4 folder_id) final
        {
            n4 index = find_folder_index(folder_id);
            if (index < 0)
                return;
            
            dirs_.ids.erase(dirs_.ids.begin()+index);
            dirs_.titles.erase(dirs_.titles.begin()+index);
            
            folder_names_.erase(folder_names_.begin()+index);
            
            bool selected = *(folder_selected_.begin()+index);
            if (selected)
                --folder_selected_count_;
            folder_selected_.erase(folder_selected_.begin()+index);

            if (folder_view_)
                folder_view_->on_folder_remove(index);
        }

        void on_file_name_changed(n4 file_id, n4 title_id, str title) final
        {
            n4 index = find_file_index(file_id);
            if (index < 0)
                return;

            files_.titles[index] = title_id;
            file_names_[index] = title;
            
            if (folder_view_)
                folder_view_->on_file_change(index, file_item(index));
        }
        
        void on_folder_name_changed(n4 folder_id, n4 title_id, str title) final
        {
            n4 index = find_folder_index(folder_id);
            if (index < 0)
                return;
            
            dirs_.titles[index] = title_id;
            folder_names_[index] = title;
            
            if (folder_view_)
                folder_view_->on_folder_change(index, folder_item(index));
        }

        FileItemDetail file_item(n4 row) const
        {
            return {files_.ids[row], files_.states[row], file_selected_[row], file_names_[row], list_detail(row)};
        }
        
        FolderItemDetail folder_item(n4 row) const
        {
            return {dirs_.ids[row], folder_selected_[row], folder_names_[row]};
        }
        
        void upload_file_only(const std::string & filename, const std::string & path, const std::vector<uint8_t> & thumbnail) final
        {
            services_.file_upload(folder_id_, filename, path, thumbnail);
        }
        
        void upload_files_only(const std::vector<std::string> & filenames, const std::vector<std::string> & paths, const std::vector<std::vector<uint8_t>> & thumbnails) final
        {
            services_.files_upload(folder_id_, filenames, paths, thumbnails);
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }
    };
}
