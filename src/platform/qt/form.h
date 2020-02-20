#pragma once
#include "src/general.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QCheckBox>
#include "src/bridge/cpp/os_form.hpp"
#include "src/bridge/cpp/status.hpp"
#include "src/bridge/cpp/message.hpp"
#include "src/platform/qt/qt_os_io.h"

namespace ai
{

	struct field
	{
		virtual ~field() {}
		virtual str value() = 0;
		virtual void value(str) = 0;
		virtual void error(str) = 0;
	};
	struct bool_field : field
	{
		bool_field(QCheckBox& check) : check_(check) {}
		str value() final
		{
			return check_.isChecked() ? "true" : "false";
		}
		void value(str val) final
		{
			check_.setChecked(val == "true");
		}
		void error(str msg) final
		{
		}
	private:
		QCheckBox& check_;
	};
	struct text_field : field
	{
		text_field(QLineEdit& edit, QLabel& error_label) : edit_(edit), error_label_(error_label) {}
		str value() final
		{
			return edit_.text().toStdString();
		}
		void value(str val) final
		{
			edit_.setText(QString::fromStdString(val));
		}
		void error(str msg) final
		{
			error_label_.setText(QString::fromStdString(msg));
		}
	private:
		QLineEdit& edit_;
		QLabel& error_label_;
	};

	struct screen
	{
		virtual ~screen() {}

	};
	struct form : OsForm
	{
		form(str title, int w, int h)
		{
			panel_ = new QVBoxLayout();
			panel_->setObjectName("panel");
			errors_ = new QLabel();
			errors_->setObjectName("errors");
			panel_->addWidget(errors_);
			errors_->setStyleSheet("QLabel{color:red;padding-left:10px;margin-left:10px;font-size:10pt}");

			landing_.setWindowTitle(QString::fromStdString(title));
			landing_.resize(w, h);
			landing_.setLayout(panel_);
            landing_.setObjectName("main");
		}

		virtual ~form() {}

		void enable(n4 group) final {}

		void disable(n4 group) final {}

		std::unordered_map<n4, str> input() final
		{
			return {};
		}

		void show()
		{
			landing_.show();
		}
		
		sptr<field> add_checkbox(str label, funk<void()> fun = nullptr)
		{
			if (!fun)
			{
				auto chk = new QCheckBox(QString::fromStdString(label));
				panel_->addWidget(chk);
				return std::make_shared<bool_field>(*chk);
			}

			{
				auto hlayout = new QHBoxLayout();
				//hlayout->
				panel_->addLayout(hlayout);
				auto chk = new QCheckBox();
				chk->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
				hlayout->addWidget(chk);

				auto lbl = new QPushButton(QString::fromStdString(label));
                lbl->setStyleSheet("QPushButton{text-align:left;background-color:white;font-size:14pt;border-radius:0;color:#6788e5;padding:0;margin:0;border:none}");
				lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
				hlayout->addWidget(lbl);
				QObject::connect(lbl, &QPushButton::clicked, fun);
				return std::make_shared<bool_field>(*chk);
			}
		}

		void add_label(str label)
		{
			auto lbl = new QLabel(QString::fromStdString(label));
			lbl->setAlignment(Qt::AlignCenter);
			panel_->addWidget(lbl);
		}

		sptr<field> add_field(str name, str placeholder, bool sensitive = false)
		{
			auto vlayout = new QVBoxLayout();
			panel_->addLayout(vlayout);
			vlayout->setObjectName(QString::fromStdString(name + "_layout"));

			auto error_label = new QLabel();
			vlayout->addWidget(error_label);
			error_label->setObjectName(QString::fromStdString(name + "_label"));
			error_label->setStyleSheet("QLabel{color:red;padding-left:10px;margin-left:10px;font-size:10pt}");

			auto edit = new QLineEdit();
			vlayout->addWidget(edit);
                        edit->setAttribute(Qt::WA_MacShowFocusRect, false);

			edit->setObjectName(QString::fromStdString(name + "_edit"));
			edit->setPlaceholderText(QString::fromStdString(placeholder));
			if (sensitive)
				edit->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);

			auto r = std::make_shared<text_field>(*edit, *error_label);
			fields_[name] = r;
			return r;
		}

		void add_button(str title, funk<void()> fun)
		{
            auto btn = new QPushButton(QString::fromStdString(title));
			panel_->addWidget(btn);
			btn->setGraphicsEffect(rounded());
			QObject::connect(btn, &QPushButton::clicked, [this, fun]()
			{
				clear_errors();
				fun();
			});
		}

		void add_buttons(arr<par<str, funk<void()>>> buttons)
		{
			auto hlayout = new QHBoxLayout();
			panel_->addLayout(hlayout);
			std::for_each(buttons.cbegin(), buttons.cend(), [this, hlayout](auto& pbtn)
			{
				auto btn = new QPushButton(QString::fromStdString(pbtn.first));
				hlayout->addWidget(btn);
				btn->setGraphicsEffect(rounded());
				QObject::connect(btn, &QPushButton::clicked, [this, fun=pbtn.second]()
				{
					clear_errors();
					fun();
				});
			});
		}

		void remove_all_widgets()
		{
			fields_.clear();
			clear_layout(panel_, 0);
		}

		void clear_errors()
		{
			errors_->setText("");
			std::for_each(fields_.cbegin(), fields_.cend(), [](auto&& item)
			{
				item.second->error("");
			});
		}
	private:
		QWidget landing_;
		QVBoxLayout* panel_;
		QLabel* errors_;
		hmap<str, sptr<field>> fields_;

		static QGraphicsDropShadowEffect* rounded()
		{
			auto r = new QGraphicsDropShadowEffect();
			r->setBlurRadius(10);
			r->setXOffset(0);
			r->setYOffset(2);
			return r;
		}

		void action_result(Status outcome, const arr<Message>& lines) final
		{

			auto fun = [this, lines, outcome]()
			{
				if (Status::SUCCESS == outcome)
				{
					landing_.hide();
					//remove_all_widgets();
					return;
				}
				str error_text;
				std::for_each(lines.cbegin(), lines.cend(), [this, &error_text](const Message& msg)
				{
					auto it = fields_.find(msg.field);
					if (fields_.end() != it)
						it->second->error(msg.content);
					else
						error_text += msg.content + "<br/>";
				});
				errors_->setText(QString::fromStdString(error_text));
			};
			if (QApplication::instance()->thread() == QThread::currentThread())
				fun();
			else
				QTimer::singleShot(0, QApplication::instance(), [fun]()
				{
					fun();
				});
		}

		void progress(n4 /*position*/, n4 /*total*/) final
		{

		}

		void clear_layout(QLayout *layout, int level)
		{
			
			while (QLayoutItem *item = layout->takeAt(0))
			{
				
				{
					auto l = item->layout();
					if (l) 
					{
						auto name = l->objectName();
						clear_layout(l, level + 1);
						delete l;
						continue;
					}
				}

				{
					auto widget = item->widget();
					if (widget) 
						delete item->widget();
				}
				delete item;
			}
		}
	};
}
