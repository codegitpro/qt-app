#pragma once
#include "../../bridge/cpp/directory_view.hpp"
#include "../../bridge/cpp/directories_detail.hpp"
#include "../../bridge/cpp/files_detail.hpp"
#include "../../bridge/cpp/file_state.hpp"

#include "../../general.h"
#include "../../directory.h"
#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QTimer>
#include <QLabel>
#include <QImageReader>
#include <QImage>
#include "qt_util.h"

namespace ai
{
	struct qt_directory_view : DirectoryView
	{
		static inline p8 to_data(n4 folder_id, n4 file_id = -1)
		{
			p8 r = folder_id;
			r <<= 32;
			r |= static_cast<p4>(file_id);
			return r;
		}

		qt_directory_view
		(
			QStandardItemModel* model,
			n4 folder_id,
			QIcon& icon_up,
			QIcon& icon_down,
			QIcon& icon_done,
			QIcon& icon_delete,
			arr<str>& dirs,
			arr<str>& files,
			std::mutex& folder_change_mutex
		)
			:
			model_(model),
			folder_id_(folder_id),
			icon_up_(icon_up),
			icon_down_(icon_down),
			icon_done_(icon_done),
			icon_delete_(icon_delete),
            dirs_(dirs),
			files_(files),
            folder_change_mutex_(folder_change_mutex),
            folder_change_dirs_({}, {}),
			folder_change_files_({}, {}, {}, {}, {}, {}),
            last_incoming_files_({}, {}, {}, {}, {}, {})
		{
		}

		virtual ~qt_directory_view()
		{
		}

		void file_states_changed(const arr<n4>& folder_path, const arr<n4>& file_ids, const arr<FileState>& states) final
		{
			QTimer::singleShot(0, QApplication::instance(), [this, folder_path, file_ids, states]()
			{
				auto root = path_from_folder(folder_path);
				if (!root)
					return;

				int row = 0;
				for (; row < root->rowCount(); ++row)
				{
					auto child = root->child(row);
					auto cookie = cookie_from_item(child);
					if (cookie.is_file())
						break;
				}

				for (int i = 0; i < static_cast<int>(file_ids.size()); ++i)
				{
					if (!(row < root->rowCount()))
						return;

					auto child = root->child(row);
					auto cookie = cookie_from_item(child);
					assert(cookie.is_file());
					for (; i < file_ids.size() && file_ids[i] != cookie.file_id; ++i)
					{
					}
					if (!(i < file_ids.size() && file_ids[i] == cookie.file_id))
						return;

					{
						switch (states[i])
						{
						case FileState::DOWNLOADING: child->setIcon(icon_down_); break;
						case FileState::UPLOADING: child->setIcon(icon_up_); break;
						case FileState::DELETING: child->setIcon(icon_delete_); break;
						default: break;
						}
					}
					++row;
				}
			});
		}

		void refresh_list(const arr<n4>& folder_path, const DirectoriesDetail& dirs, const FilesDetail& files, const arr<str>& new_directory_names, const arr<str>& new_file_names) final
		{
			gate lck(folder_change_mutex_);
			last_incoming_files_ = files;
 			bool was_empty = folder_change_dirs_.ids.empty() && folder_change_files_.ids.empty();
			dirs_.insert(dirs_.end(), new_directory_names.cbegin(), new_directory_names.cend());
			files_.insert(files_.end(), new_file_names.cbegin(), new_file_names.cend());
			folder_change_folder_path_ = folder_path;
			folder_change_dirs_ = dirs;
			folder_change_files_ = files;
			if (!was_empty)
				return;

			QTimer::singleShot(0, QApplication::instance(), [this]()
			{
				arr<n4> folder_path;
				DirectoriesDetail dirs({}, {});
				FilesDetail files({}, {}, {}, {}, {}, {});

				{
					gate lck(folder_change_mutex_);
					folder_path = std::move(folder_change_folder_path_);

					dirs.ids	= std::move(folder_change_dirs_.ids);
					dirs.titles = std::move(folder_change_dirs_.titles);

					files.ids			= std::move(folder_change_files_.ids);
					files.percentages	= std::move(folder_change_files_.percentages);
					files.sizes			= std::move(folder_change_files_.sizes);
					files.titles		= std::move(folder_change_files_.titles);
					files.states		= std::move(folder_change_files_.states);
					files.thumbnails	= std::move(folder_change_files_.thumbnails);
				}

				auto root = path_from_folder(folder_path);
				if (!root)
					return;

				{ // remove superfluous nodes
					struct node
					{
						int id_;
						int row_;
						node(int id, int row) : id_(id), row_(row) {}
					};
					auto compare = [](const node& l, const node& r) { return l.id_ < r.id_; };

					auto dir_children = arr<node>();
					auto file_children = arr<node>();

					{
						auto row_count = root->rowCount();
						for (int row = 0; row < row_count; ++row)
						{
							auto child = root->child(row);
							auto cookie = cookie_from_item(child);
							if (cookie.is_file())
								file_children.push_back(node(cookie.file_id, row));
							else 
								dir_children.push_back(node(cookie.folder_id, row));
						}
					}

					std::sort(dir_children.begin(), dir_children.end(), compare);
					std::sort(file_children.begin(), file_children.end(), compare);

					auto to_be_deleted = arr<node>();

					auto incoming_dir_ids = arr<node>();
					std::transform(dirs.ids.cbegin(), dirs.ids.cend(), std::back_inserter(incoming_dir_ids), [](int id) { return node(id, -1); });
					std::sort(incoming_dir_ids.begin(), incoming_dir_ids.end(), compare);
					std::set_difference(dir_children.cbegin(), dir_children.cend(), incoming_dir_ids.cbegin(), incoming_dir_ids.cend(), std::back_inserter(to_be_deleted), compare);

					auto incoming_file_ids = arr<node>();
					std::transform(files.ids.cbegin(), files.ids.cend(), std::back_inserter(incoming_file_ids), [](int id) { return node(id, -1); });
					std::sort(incoming_file_ids.begin(), incoming_file_ids.end(), compare);
					std::set_difference(file_children.cbegin(), file_children.cend(), incoming_file_ids.cbegin(), incoming_file_ids.cend(), std::back_inserter(to_be_deleted), compare);

					std::sort(to_be_deleted.begin(), to_be_deleted.end(), [](const node& l, const node& r) { return l.row_ > r.row_; });
					std::for_each(to_be_deleted.cbegin(), to_be_deleted.cend(), [root](const node& item)
					{
						root->removeRow(item.row_);
					});
				}

				int row = 0;

				{
					auto new_dir = [](const str& dir_name, int folder_id)
					{
						auto item = new QStandardItem(QString::fromStdString(dir_name));
						item->setData(QVariant::fromValue(qt_directory_view::to_data(folder_id)));
						item->setEditable(false);
						auto no_files = new QStandardItem("No files");
						no_files->setEditable(false);
						item->appendRow(no_files);
						//root->insertRow(row, item);
						item->setDropEnabled(true);
						item->setDragEnabled(true);
						return item;
					};
 
					auto& folder_ids = dirs.ids;
					auto& folder_name_ids = dirs.titles;

                    for (sz i = 0; i < folder_ids.size(); ++i)
					{
						if (!(row < root->rowCount()))
						{
							root->appendRow(new_dir(dirs_[folder_name_ids[i]], folder_ids[i]));
							++row;
							continue;
						}
						auto child = root->child(row);
						auto cookie = cookie_from_item(child);
						if (cookie.is_file())
						{
							for (; i < folder_ids.size(); ++i)
							{
								root->insertRow(row++, new_dir(dirs_[folder_name_ids[i]], folder_ids[i]));
							}
							break;
						}
						for (; i < folder_ids.size() && folder_ids[i] != cookie.folder_id; ++i)
						{
							root->insertRow(row++, new_dir(dirs_[folder_name_ids[i]], folder_ids[i]));
						}
						assert(i < folder_ids.size() && folder_ids[i] == cookie.folder_id);
						child->setText(QString::fromStdString(dirs_[folder_name_ids[i]]));
						++row;
					}
				}

				{
					auto new_file = [this, &files](const str& file_name, int i)
					{
						auto name_node = new QStandardItem();
						name_node->setEditable(false);
						auto size_node = new QStandardItem();
						size_node->setTextAlignment(Qt::AlignRight);
						auto perc_node = new QStandardItem();

						update_file(folder_id_, files.ids[i], files.states[i], file_name, files.sizes[i], files.percentages[i], *name_node, *size_node, *perc_node, files.thumbnails[i]);
						return QList<QStandardItem*>({ name_node, size_node, perc_node });
					};

					auto& file_ids = files.ids;
					auto& file_name_ids = files.titles;

                    for (int i = 0; i < static_cast<int>(file_ids.size()); ++i)
					{
						if (!(row < root->rowCount()))
						{
							root->appendRow(new_file(files_[file_name_ids[i]], i));
							++row;
							continue;
						}
						auto child = root->child(row);
						auto cookie = cookie_from_item(child);
						assert(cookie.is_file());
						for (; i < file_ids.size() && file_ids[i] != cookie.file_id; ++i)
						{
							root->insertRow(row++, new_file(files_[file_name_ids[i]], i));
						}
						assert(i < file_ids.size() && file_ids[i] == cookie.file_id);

						auto& file_name = files_[file_name_ids[i]];
						child->setText(QString::fromStdString(file_name));
						auto child_size = root->child(row, 1);
						auto child_perc = root->child(row, 2);
						update_file(cookie.folder_id, cookie.file_id, files.states[i], file_name, files.sizes[i], files.percentages[i], *child, *child_size, *child_perc, files.thumbnails[i]);
						++row;
					}
				}

				if (!root->hasChildren())
				{

					auto no_files = new QStandardItem("No files");
					no_files->setEditable(false);
					root->appendRow(no_files);
				}
			});
		}

		void file_changed(const arr<n4>& folder_path, n4 file_id, n4 file_name_id, n8 size, n4 percentage, FileState state, const arr<p1>& thumbnail) final
		{
			gate lck(folder_change_mutex_);

			auto& ids = last_incoming_files_.ids;
			auto found = std::find(ids.cbegin(), ids.cend(), file_id);
			if (ids.cend() != found)
			{
				auto index = std::distance(ids.cbegin(), found);
				last_incoming_files_.percentages[index] = percentage;
				last_incoming_files_.sizes[index] = size;
				last_incoming_files_.states[index] = state;
			}
			QTimer::singleShot(0, QApplication::instance(), [this, folder_path, file_id, file_name_id, state, size, percentage, thumbnail]()
			{
				auto root = path_from_folder(folder_path);
				if (!root)
					return;
				

				n4 parent_id = folder_path.empty() ? 0 : folder_path.back();
				auto item_id = to_data(parent_id, file_id);
				for (int row = 0; row < root->rowCount(); ++row)
				{
					auto child_name = root->child(row);
					auto child_id = child_name->data().toULongLong();
					if (child_id == item_id)
					{
						if (FileState::DELETED == state)
						{
							root->removeRow(row);
							return;
						}
						auto child_size = root->child(row, 1);
						auto child_perc = root->child(row, 2);
						if (files_.size() > file_name_id)
						{
							auto title = files_[file_name_id];
							update_file(parent_id, file_id, state, title, size, percentage, *child_name, *child_size, *child_perc, thumbnail);
						}
						return;
					}
				}
			});
		}

		bool is_downloadable(int file_id)
		{
			gate lck(folder_change_mutex_);

			auto& ids = last_incoming_files_.ids;
			auto found = std::find(ids.cbegin(), ids.cend(), file_id);
			if (ids.cend() == found)
				return false;
			return 0 == last_incoming_files_.percentages[std::distance(ids.cbegin(), found)];
		}

		bool is_busy(int file_id)
		{
			gate lck(folder_change_mutex_);

			auto& ids = last_incoming_files_.ids;
			auto found = std::find(ids.cbegin(), ids.cend(), file_id);
			if (ids.cend() == found)
				return true;
			auto index = std::distance(ids.cbegin(), found);
			auto state = last_incoming_files_.states[index];
			if (FileState::DELETING == state || FileState::DOWNLOADING == state || FileState::UPLOADING == state)
				return true;
			auto perc = last_incoming_files_.percentages[index];
			return 0 < perc && perc < 100;
		}

        bool is_local(int file_id)
        {
			gate lck(folder_change_mutex_);
			
			auto& ids = last_incoming_files_.ids;
            auto found = std::find(ids.cbegin(), ids.cend(), file_id);
            if (ids.cend() == found)
                return false;
            return 100 == last_incoming_files_.percentages[std::distance(ids.cbegin(), found)];
        }

	private:
		QStandardItemModel* model_;
		n4 folder_id_;
		QIcon& icon_up_;
		QIcon& icon_down_;
		QIcon& icon_done_;
		QIcon& icon_delete_;

		arr<str>& dirs_;
		arr<str>& files_;
		std::mutex& folder_change_mutex_;

		arr<n4> folder_change_folder_path_;
		DirectoriesDetail folder_change_dirs_;
		FilesDetail folder_change_files_;
		FilesDetail last_incoming_files_;

		QStandardItem* path_from_folder(const arr<n4>& path) const
		{
            return accumulate(path.cbegin(), path.cend(), model_->invisibleRootItem()->child(0), [](QStandardItem* item, n4 folder_id) -> QStandardItem*
			{
				if (!item)
					return nullptr;
				for (int i = 0; i < item->rowCount(); ++i)
				{
					auto child = item->child(i);
					if (!child)
						return nullptr;
					p8 cookie = child->data().toULongLong();
					auto file_id = static_cast<n4>(cookie);
					if (-1 != file_id)
						return nullptr;
					const auto id = static_cast<n4>(cookie >> 32);
					if (id == folder_id)
						return child;
				}
				return nullptr;
			});
		}

		void update_file
		(
			n4 folder_id, 
			n4 file_id, 
			FileState state, 
			const str& file_name, 
			n8 file_size, 
			n4 file_percentage, 
			QStandardItem& name, 
			QStandardItem& size, 
			QStandardItem& perc, 
			const arr<p1>& thumbnail
		)
		{
			switch (state)
			{
			case FileState::DOWNLOADING: name.setIcon(icon_down_); break;
			case FileState::UPLOADING: name.setIcon(icon_up_); break;
			case FileState::DELETING: name.setIcon(icon_delete_); break;
			case FileState::LOCAL: 
			{
				if (!thumbnail.empty())
				{
					QImage img;
					img.loadFromData(static_cast<const uchar *>(thumbnail.data()), static_cast<int>(thumbnail.size()), "PNG");
					name.setIcon(QIcon(QPixmap::fromImage(img)));

					//name.setData(QPixmap::fromImage(img), Qt::DecorationRole);
					//name.setSizeHint(QSize(64, 64));
				}
				else 
					name.setIcon(icon_done_);
				break;
			}
			default: 
				name.setIcon(QIcon()); 
				break;
			}

			name.setText(QString::fromStdString(file_name));

			name.setData(QVariant::fromValue(to_data(folder_id, file_id)));
			size.setText(QString::fromStdString(file_size ? std::to_string(file_size) : ""));
			perc.setText(QString::fromStdString(0 < file_percentage && file_percentage < 100 ? std::to_string(file_percentage) + " %" : ""));
		}
	};
}
