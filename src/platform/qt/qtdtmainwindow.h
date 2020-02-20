#pragma once

#include "../../general.h"
#include "../../caching_engine.h"
#include "../../bridge/cpp/file_type.hpp"

#include <QMainWindow>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include "../plat.h"
#include <QTimer>
#include <QThread>
#include <QCloseEvent>
#include "../portal.h"
#include "theme.h"
#include "qt_directory_view.h"
#include <QTreeView>
#include <QLabel>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace ai
{

    class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
        MainWindow(sptr<Portal> portal, QWidget *parent = nullptr);
        void closeEvent(QCloseEvent *event);
		virtual ~MainWindow();
		void preview(const QImage&);
	private:
        QTreeView *view;
        theme theme_;
		sptr<ai::Portal> portal_;
		std::mutex folder_change_mutex_;
        hmap<int, sptr<qt_directory_view>> folder_views_;
		arr<str> dirs_;
		arr<str> files_;
		QStandardItemModel* model_ = new QStandardItemModel();

		QMenu* menu_;

		QAction* tool_cancel_;
        QAction* tool_upload_;
		QAction* tool_create_folder_;
        QAction* tool_download_;
        QAction* tool_save_;
		QAction* tool_delete_;
		//QAction* tool_settings_;
		QAction* tool_signout_;

		QLabel* preview_ = nullptr;
		QHBoxLayout* preview_panel_ = nullptr;

		QIcon icon_down_;
		QIcon icon_up_;
		QIcon icon_done_;
		QIcon icon_save_;
		QIcon icon_delete_;

	private slots:
		void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
		void on_expanded(const QModelIndex &index);
		void on_dubbel_click(const QModelIndex &current);

		void download();
	};
}
