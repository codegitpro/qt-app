#pragma once
#include <QTreeView>
#include <QtDebug>
#include "qt_util.h"
#include "../../bridge/cpp/portal.hpp"

namespace ai
{
	struct qt_treeview : QTreeView
	{
		qt_treeview(QWidget* parent, QStandardItemModel* model, sptr<ai::Portal> portal) : QTreeView(parent), model_(model), portal_(portal)
		{
		}

		void dragEnterEvent(QDragEnterEvent* e) final
		{
			if (!e->mimeData()->hasUrls() || e->mimeData()->urls().empty())
				return;
			
			foreach(const QUrl &url, e->mimeData()->urls())
			{
				if (!url.isLocalFile())
					continue;
				QString fileName = url.toLocalFile();
				if (fileName.isEmpty())
					continue;
				QFileInfo info(fileName);
				if (info.exists())
				{
					e->acceptProposedAction();
					return;
				}
			}
		}

		void dragMoveEvent(QDragMoveEvent* e) final
		{
			//QTreeView::dragMoveEvent(e);
			//if (!e->isAccepted())
			//	return;

			if (e->mimeData()->hasUrls())
			{
				auto index = indexAt(e->pos());
				if (index == index_)
					return;
				if (index.isValid())
				{
					auto item = model_->itemFromIndex(index);
					auto cookie = cookie_from_item(item);
					if (cookie.is_folder())
					{
						if (cookie.is_file())
							index = index.parent();
						if (index == index_)
							return;
						e->acceptProposedAction();
						selectionModel()->select(index_, QItemSelectionModel::Deselect);
						index_ = index;
						selectionModel()->select(index, QItemSelectionModel::Select);
						return;
					}
				}
			}
			QTreeView::dragMoveEvent(e);
		}

		static void upload_files(const sptr<Portal>& portal, n4 target_folder_id, arr<str> filepaths)
		{
			std::sort(filepaths.begin(), filepaths.end());

			auto first = *filepaths.cbegin();
			auto len = common_file_path_length(first, *filepaths.rbegin());
			auto filenames = arr<str>();
			std::for_each(filepaths.cbegin(), filepaths.cend(), [&filenames, len](const str& path)
			{
				filenames.push_back(str(path.cbegin() + len, path.cend()));
			});
			portal->upload_all(target_folder_id, str(first.cbegin(), first.cbegin() + len - 1), filenames);
		}

		void dropEvent(QDropEvent* e) final
		{
			if (index_.isValid() && e->mimeData()->hasUrls())
			{
				auto item = model_->itemFromIndex(index_);
				auto cookie = cookie_from_item(item);
				if (!cookie.is_file() && cookie.is_folder())
				{
					auto filepaths = arr<str>();
					foreach(const QUrl &url, e->mimeData()->urls())
					{
						if (!url.isLocalFile())
							continue;
						QString fileName = url.toLocalFile();
						if (fileName.isEmpty())
							continue;
						filepaths.push_back(fileName.toStdString());
						//portal_->upload_file(cookie.folder_id, url.fileName().toStdString(), url.toLocalFile().toStdString());
					}
					if (!filepaths.empty())
						upload_files(portal_, cookie.folder_id, std::move(filepaths));
					
				}
				selectionModel()->select(index_, QItemSelectionModel::Deselect);
				index_ = QModelIndex();
			}
		}
	private:
		QStandardItemModel* model_;
		sptr<ai::Portal> portal_;

		QModelIndex index_;
	};
}
