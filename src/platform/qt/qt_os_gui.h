#pragma once
#include "../../general.h"
#include "../../bridge/cpp/os_gui.hpp"
#include "../../bridge/cpp/reference.hpp"
#include "../../bridge/cpp/viewmodel.hpp"

#include <QApplication>
#include <QTimer>
#include <QDesktopServices>
#include <QMainWindow>

#include "qt_screen.h"
#include "qt_os_form.h"
#include <atomic>

#include "qt_form_list.h"
#include "qt_form_dialog.h"

namespace ai
{
	static void run_on_gui(funk<void()> fun)
	{
		QTimer::singleShot(0, QApplication::instance(), [fun]()
		{
			fun();
		});
	}

	struct qt_os_gui : OsGui
	{
		qt_os_gui(QWidget* parent) : parent_(parent)
		{
		}
		virtual ~qt_os_gui()
		{

		}
		void gui_thread(const sptr<VoidResult>& cb) final
		{
			QTimer::singleShot(0, QApplication::instance(), [cb]()
			{
				cb->on_result();
			});
		}

		void back() final
		{
			//if (!hierarchy_.empty())
			//	hierarchy_.pop_back();
			//if (!hierarchy_.empty())
			//	hierarchy_[hierarchy_.size() - 1]->show();
		}

		void open_link_in_browser(const str& url) final
		{
			QDesktopServices::openUrl(QUrl(QString::fromStdString(url)));
		}

		sptr<OsForm> create_form
		(
			const str& title, 
			bool,// start_hierarchy,
			n4 width,
			n4 height,
			const sptr<IntResult>& action, 
			const arr<Element>& elements
		) final
		{
			auto r = std::make_shared<qt_os_form>(parent_, title, width, height, action, elements);
			//if (start_hierarchy)
			//	hierarchy_.clear();
			//hierarchy_.push_back(r);
			return r;
		}

		void show_cloudstore() final
		{
			run_on_gui([this]()
			{
				if (window_)
					window_->deleteLater();
				window_ = new MainWindow(portal_, parent_);
				window_->show();
			});
		}

		void show_message(const str& message) final
		{

		}

		void preview(FileType type, const str& path, n4 file_id, const str& file_name) final
		{
			if (FileType::IMAGE != type || -1 == file_id || file_name.empty())
				return;
			QImageReader reader(QString::fromStdString(path));
			reader.setAutoTransform(true);
			const QImage image = reader.read();
			if (image.isNull())
				return;

			run_on_gui([this, image]()
			{
				if (window_)
					window_->preview(image);
			});
		}

		void reset() final
		{
			run_on_gui([this]()
			{
				if (!window_)
					return;
				window_->deleteLater();
				window_ = nullptr;
			});
		}

		std::shared_ptr<GenericForm> create_form_list(const std::string & title, const std::vector<Field> & fields, bool fullscreen) final
		{
			return nullptr;
			//return std::make_shared<qt_form_list>(*m_container, vm_, references_, title, fields);
		}

		std::shared_ptr<GenericForm> create_form_dialog(const std::string & title, const std::vector<Field> & fields) final
		{
			return std::make_shared<qt_form_dialog>(vm_, references_, title, fields);
		}

		void init(const std::shared_ptr<Viewmodel> & vm, const std::vector<Reference> & references) final
		{
			vm_ = vm;
			std::for_each(references.begin(), references.end(), [this](auto&& reference)
			{
				references_.insert({ reference.name, reference.items });
			});
		}

		sptr<Portal> portal_;
	private:
		QWidget* parent_;
		//arr<sptr<qt_screen>> hierarchy_;
		MainWindow* window_ = nullptr;
		std::shared_ptr<Viewmodel> vm_;
		hmap<str, arr<str>> references_;
	};
}