#include "qtdtmainwindow.h"

#include <QFile>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileDialog>
#include <QStandardPaths>
#include "qt_worker.h"
#include "qt_os_io.h"
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QInputDialog>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QString>
#include <QScrollArea>
#include <QMimeData>
#include "qt_treeview.h"
#include <unordered_set>
#include <QProcess>
#include <QMenuBar>

using namespace std;

namespace ai
{

    struct selected_ids
    {
        arr<n4> file_ids;
        arr<n4> folder_ids;
    };

    selected_ids current_selection(QStandardItemModel* model, QTreeView* view)
    {
        auto list = view->selectionModel()->selectedRows();

        auto cookies = arr<file_desc>();
        std::for_each(list.cbegin(), list.cend(), [model, &cookies](const QModelIndex& index)
        {
            if (!index.isValid())
                return;

            auto item = model->itemFromIndex(index);
            auto id = cookie_from_item(item);
            cookies.push_back(id);
        });

        auto selected_folder_ids = unordered_set<n4>();
        std::for_each(cookies.cbegin(), cookies.cend(), [&selected_folder_ids](const file_desc& cookie)
        {
            if (cookie.is_folder() && !cookie.is_file())
                selected_folder_ids.insert(cookie.folder_id);
        });

        auto file_ids = arr<n4>();
        std::for_each(cookies.cbegin(), cookies.cend(), [&selected_folder_ids, &file_ids](const file_desc& cookie)
        {
            if (cookie.is_file() && selected_folder_ids.find(cookie.folder_id) == selected_folder_ids.end())
                file_ids.push_back(cookie.file_id);
        });

        auto r = selected_ids();
        std::for_each(selected_folder_ids.cbegin(), selected_folder_ids.cend(), [&r](n4 folder_id)
        {
            r.folder_ids.push_back(folder_id);
        });
        r.file_ids = file_ids;

        return r;
    }

    QIcon tinted_icon(const QString& path, QColor colour)
	{
		QPixmap pixmap = QPixmap(path);
		QPainter painter(&pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

        painter.setBrush(colour);
        painter.setPen(colour);

		painter.drawRect(pixmap.rect());

		return QIcon(pixmap);
	}

	MainWindow::MainWindow(sptr<Portal> portal, QWidget *parent)
		: 
		QMainWindow(parent),
		portal_(portal)
	{
		setWindowIcon(QIcon(":/icons/icon.png"));
		//{
		//	QProcess update;
		//	update.setWorkingDirectory(R"(C:\Program Files\Blomp)");
		//	update.startDetached(R"(BlompSetup.exe --checkupdates)");

		//	update.waitForFinished();
		//	auto code = update.exitCode();
		//	auto error = update.error();
		//	auto es = update.errorString();
		//	code = 123;
		//}
        {
            if (objectName().isEmpty())
                setObjectName(QStringLiteral("MainWindow"));
            resize(573, 468);

            auto centralwidget = new QWidget(this);
            centralwidget->setObjectName(QStringLiteral("centralwidget"));
            auto vboxLayout = new QVBoxLayout(centralwidget);
            vboxLayout->setSpacing(0);
            vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
            vboxLayout->setContentsMargins(0, 0, 0, 0);
            setCentralWidget(centralwidget);

            view = new qt_treeview(centralwidget, model_, portal_);
            view->header()->setStyleSheet("QHeaderView{background-color:white;}");
			view->setDropIndicatorShown(true);
			view->setAcceptDrops(true);
			view->setDragDropMode(QAbstractItemView::DragDropMode::DropOnly);
            
			view->setObjectName(QStringLiteral("view"));
            view->setAlternatingRowColors(true);
            view->setSelectionBehavior(QAbstractItemView::SelectRows);
			view->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
            view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            view->setAnimated(false);
			//            view->setAllColumnsShowFocus(true);
			
			{
				preview_panel_ = new QHBoxLayout();
				vboxLayout->addLayout(preview_panel_);
				preview_panel_->setSpacing(0);
				preview_panel_->setContentsMargins(0, 0, 0, 0);
				preview_panel_->addWidget(view);

				{
					preview_ = new QLabel();
					preview_->setBackgroundRole(QPalette::Base);
					preview_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
					preview_->setScaledContents(true);
					preview_->setStyleSheet("QLabel{background-color:white;}");

					auto scrollArea = new QScrollArea();
					scrollArea->setStyleSheet("QScrollArea{background-color:white;}");
					scrollArea->setBackgroundRole(QPalette::Light);
					scrollArea->setWidget(preview_);
					scrollArea->setVisible(true);
					//scrollArea->setWidgetResizable(true);

					preview_panel_->addWidget(scrollArea);

				}
			}

//            statusbar = new QStatusBar(MainWindow);
//            statusbar->setObjectName(QStringLiteral("statusbar"));
//            MainWindow->setStatusBar(statusbar);

            setWindowTitle(QApplication::translate("MainWindow", "Blomp", nullptr));
            QMetaObject::connectSlotsByName(this);
        }

		QStringList headers;
		headers.push_back("Title");
		headers.push_back("Description");

		{
			model_->setHorizontalHeaderLabels({ "Name", "Size", "" });
			auto root = model_->invisibleRootItem();
			auto first = new QStandardItem("root");
			first->setEditable(false);
			first->setData(QVariant::fromValue(qt_directory_view::to_data(0)));
			first->appendRow(new QStandardItem("Please wait..."));

			root->appendRow(first);
			view->setModel(model_);
			view->expandAll();
			view->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		}

		connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::selectionChanged);
		connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_dubbel_click(QModelIndex)));
		connect(view, SIGNAL(expanded(const QModelIndex &)), this, SLOT(on_expanded(const QModelIndex &)));
		connect(view, &QTreeView::clicked, [this](const QModelIndex& index)
		{
			if (!index.isValid())
				return;

			auto item = model_->itemFromIndex(index);
			auto cookie = cookie_from_item(item);
			if (cookie.is_file())
				portal_->preview(cookie.file_id);
		});
		view->setUniformRowHeights(true);

		icon_down_ = QIcon(":/icons/ic_cloud_download_24px.svg");
		icon_up_ = QIcon(":/icons/ic_cloud_upload_24px.svg");
		icon_done_ = QIcon(":/icons/ic_cloud_done_24px.svg");
		icon_save_ = QIcon(":/icons/ic_save_24px.svg");
		icon_delete_ = QIcon(":/icons/ic_delete_forever_24px.svg");

        {
			auto toolBar = new QToolBar(this);
            toolBar->setObjectName(QStringLiteral("toolBar"));
            addToolBar(Qt::TopToolBarArea, toolBar);

            tool_upload_ = toolBar->addAction(tinted_icon(":/icons/ic_cloud_upload_24px.svg", theme_.accent()), "&Upload files to a Blomp folder");
			connect(tool_upload_, &QAction::triggered, [this]()
			{
				auto index = view->selectionModel()->currentIndex();
				if (!index.isValid())
					return;

				auto item = model_->itemFromIndex(index);
				auto id = cookie_from_item(item);
				if (!id.is_folder() || id.is_file())
					return;

				QFileDialog dialog(this);
				dialog.setFileMode(QFileDialog::ExistingFiles);
				if (dialog.exec())
				{
					auto files = dialog.selectedFiles();
					auto filepaths = arr<str>();
                    for_each(files.cbegin(), files.cend(), [&filepaths](auto&& filename)
					{
						filepaths.push_back(filename.toStdString());
					});
					qt_treeview::upload_files(portal_, id.folder_id, std::move(filepaths));
				}
			});

            tool_create_folder_ = toolBar->addAction(tinted_icon(":/icons/ic_create_new_folder_24px.svg", theme_.accent()), "&Create new folder");
			connect(tool_create_folder_, &QAction::triggered, [this]()
			{
				auto index = view->selectionModel()->currentIndex();
				if (!index.isValid())
					return;

				auto item = model_->itemFromIndex(index);
				auto id = cookie_from_item(item);
				if (!id.is_folder() || id.is_file())
					return;

				auto folder_name = QInputDialog::getText(this, "New Folder", "Folder Name", QLineEdit::Normal, "", nullptr, Qt::WindowFlags(), Qt::ImhNone);
				portal_->create_folder(id.folder_id, folder_name.toStdString(), nullptr);
			});

            tool_download_ = toolBar->addAction(tinted_icon(":/icons/ic_cloud_download_24px.svg", theme_.accent()), "&Download selected files from Blomp");
			connect(tool_download_, &QAction::triggered, [this]()
			{
				download();
			});

            tool_delete_ = toolBar->addAction(tinted_icon(":/icons/ic_delete_forever_24px.svg", theme_.error()), "Dele&te selected files from Blomp");
			connect(tool_delete_, &QAction::triggered, [this]()
			{
				auto ids = current_selection(model_, view);
				portal_->delete_all(ids.folder_ids, ids.file_ids);
			});

			tool_save_ = toolBar->addAction(tinted_icon(":/icons/ic_save_24px.svg", theme_.accent()), "&Save selected files already downloaded to disk");
			connect(tool_save_, &QAction::triggered, [this]()
			{
				auto ids = current_selection(model_, view);
				if (ids.file_ids.empty() && ids.folder_ids.empty())
					return;

				QFileDialog dialog(this);
				dialog.setFileMode(QFileDialog::DirectoryOnly);
				if (!dialog.exec())
					return;
				auto files = dialog.selectedFiles();
				if (files.empty())
					return;

				portal_->save_all(files[0].toStdString(), ids.folder_ids, ids.file_ids);
			});

			tool_cancel_ = toolBar->addAction(tinted_icon(":/icons/ic_not_interested_24px.svg", theme_.accent()), "&Cancel all network operations in progress");
			connect(tool_cancel_, &QAction::triggered, [this]()
			{
				portal_->cancel_network_operations();
			});

			//tool_settings_ = toolBar->addAction(tinted_icon(":/icons/ic_settings_applications_24px.svg", theme_.accent()), "Settings");
			//connect(tool_settings_, &QAction::triggered, [this]()
			//{
			//	portal_->settings();
			//});

			tool_signout_ = toolBar->addAction(tinted_icon(":/icons/ic_lock_open_24px.svg", theme_.accent()), "&Sign Out");
			connect(tool_signout_, &QAction::triggered, [this]()
			{
				portal_->sign_out();
			});

			{
                menu_ = new QMenu();

				menu_->addAction(tool_upload_);
				menu_->addAction(tool_create_folder_);
				menu_->addSeparator();
				menu_->addAction(tool_download_);
				menu_->addAction(tool_delete_);
				menu_->addAction(tool_save_);
			}

			{
                menuBar()->setStyleSheet("QMenuBar{background-color:white;}");

                auto filemenu = menuBar()->addMenu(tr("&File"));
                filemenu->addAction(tool_upload_);
                filemenu->addAction(tool_create_folder_);
                filemenu->addSeparator();
                filemenu->addAction(tool_download_);
                filemenu->addAction(tool_delete_);
                filemenu->addAction(tool_save_);

                auto toolsmenu = menuBar()->addMenu(tr("&Tools"));
                toolsmenu->addAction(tool_cancel_);
                toolsmenu->addAction(tool_signout_);

                auto helpabout = toolsmenu->addAction(" A&bout");
                connect(helpabout, &QAction::triggered, [this]()
				{
					portal_->about();
				});

            }
			view->setContextMenuPolicy(Qt::CustomContextMenu);
			connect(view, &QTreeView::customContextMenuRequested, [this](const QPoint &point)
			{
				menu_->exec(view->mapToGlobal(point));
			});

        }

		tool_download_->setDisabled(true);
		tool_save_->setDisabled(true);
		tool_upload_->setDisabled(true);
		tool_delete_->setDisabled(true);
		tool_create_folder_->setDisabled(true);

		QTimer::singleShot(0, [this]()
		{
			auto dir_view = make_shared<qt_directory_view>
			(
				model_,
				0,
				icon_up_, 
				icon_down_,
				icon_done_, 
				icon_delete_,
				dirs_,
				files_, 
				folder_change_mutex_
			);
            folder_views_[0] = dir_view;
            portal_->watch_directory(0, dir_view);
		});
	}

	MainWindow::~MainWindow()
	{
	}

	//void MainWindow::reset()
	//{
	//	auto root = model_->invisibleRootItem()->child(0);
	//	root->removeRows(0, root->rowCount());

	//	auto no_files = new QStandardItem("No files");
	//	no_files->setEditable(false);
	//	root->appendRow(no_files);
	//}

	void MainWindow::preview(const QImage& image) 
	{
		if (!preview_)
		{
			preview_ = new QLabel();
			preview_->setBackgroundRole(QPalette::Base);
			preview_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
			preview_->setScaledContents(true);
			preview_->setStyleSheet("QLabel{background-color:white;}");

			auto scrollArea = new QScrollArea();
			scrollArea->setStyleSheet("QScrollArea{background-color:white;}");
			scrollArea->setBackgroundRole(QPalette::Light);
			scrollArea->setWidget(preview_);
			scrollArea->setVisible(true);
			//scrollArea->setWidgetResizable(true);

			preview_panel_->addWidget(scrollArea);
			//preview_panel_->addWidget(preview_);
		}

		auto parent_size = preview_->parentWidget()->size();
		auto window_ratio = double(parent_size.width()) / double(parent_size.height());
		auto img_ratio = double(image.width()) / double(image.height());
		preview_->setPixmap(QPixmap::fromImage(image));
		if (img_ratio > window_ratio)
		{
			auto w = double(parent_size.width());
			auto h = w / img_ratio;
			preview_->resize(static_cast<int>(w), static_cast<int>(h));
		}
		else
		{
			auto h = double(parent_size.height());
			auto w = h * img_ratio;
			preview_->resize(static_cast<int>(w), static_cast<int>(h));
		}
		//imageLabel->adjustSize();
		//imageLabel->resize(QSize(100, 100));
	}

    str item_path(QStandardItem* item)
    {
        if (!item)
            return "";

        auto r = item->text().toStdString();
        while (item->parent())
        {
            item = item->parent();
            r = item->text().toStdString() + "/" + r;
        }
        return r;
    }

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		//cache_.finish_and_wait();
		event->accept();
	}

	void MainWindow::on_expanded(const QModelIndex &index)
	{
		if (!index.isValid())
			return;

		auto item = model_->itemFromIndex(index);
		if (item && item->data().isValid())
		{
			auto cookie = cookie_from_item(item);
            auto found = folder_views_.find(cookie.folder_id);
            auto dir_view = folder_views_.end() == found
                ? make_shared<qt_directory_view>
				(
					model_,
					cookie.folder_id,
					icon_up_, 
					icon_down_,
					icon_done_, 
					icon_delete_,
					dirs_, 
					files_, 
					folder_change_mutex_
				)
                : found->second;
            folder_views_[cookie.folder_id] = dir_view;
            portal_->watch_directory(cookie.folder_id, dir_view);
		}
	}

	void MainWindow::download()
	{
		auto ids = current_selection(model_, view);
		portal_->download_all(ids.folder_ids, ids.file_ids);
	}

	void MainWindow::on_dubbel_click(const QModelIndex&)
	{
		download();
	}

	void MainWindow::selectionChanged(const QItemSelection&, const QItemSelection&)
	{
		auto ids = current_selection(model_, view);
		
		if (ids.file_ids.empty() && ids.folder_ids.size() == 1)
		{
			tool_create_folder_->setDisabled(false);
			tool_upload_->setDisabled(false);
			tool_download_->setDisabled(false);
			tool_save_->setDisabled(false);
			tool_delete_->setDisabled(false);
			return;
		}

		tool_create_folder_->setDisabled(true);
		tool_upload_->setDisabled(true);

		if (ids.file_ids.empty() && ids.folder_ids.empty())
		{
			tool_download_->setDisabled(true);
			tool_save_->setDisabled(true);
			tool_delete_->setDisabled(true);
		}
		else
		{
			tool_download_->setDisabled(false);
			tool_save_->setDisabled(false);
			tool_delete_->setDisabled(false);
		}
	}
}
