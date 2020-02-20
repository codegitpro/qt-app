#pragma once
#include "../../general.h"
#include "../../bridge/cpp/os_form.hpp"
#include "../../bridge/cpp/int_result.hpp"
#include "../../bridge/cpp/status.hpp"
#include "../../bridge/cpp/message.hpp"
#include "../../bridge/cpp/element.hpp"
#include "../../bridge/cpp/element_type.hpp"

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
#include <QMainWindow>
#include "qt_screen.h"

namespace ai
{
	struct qt_os_form : OsForm, qt_screen
	{
		struct interactive_element
		{
			interactive_element(const Element& element) : group_(element.group) {}
			virtual ~interactive_element() {}
			n4 group_;
			virtual void enable(bool) = 0;
		};
		struct input_field
		{
			input_field(const Element& element) : group_(element.group), input_id_(element.input_id) {}
			virtual ~input_field() {}
			virtual str value() = 0;
			virtual void value(str) = 0;
			virtual void error(str) = 0;
			n4 group_;
			n4 input_id_;
		};
		struct bool_field : input_field, interactive_element
		{
			bool_field(const Element& element, QCheckBox& check) : input_field(element), interactive_element(element), check_(check) {}
			str value() final
			{
				return check_.isChecked() ? "true" : "false";
			}
			void value(str val) final
			{
				check_.setChecked(val == "true");
			}
			void error(str /*msg*/) final
			{
			}
			void enable(bool enabled) final
			{
				check_.setEnabled(enabled);
			}
		private:
			QCheckBox& check_;
		};
		struct text_field : input_field, interactive_element
		{
			text_field(const Element& element, QLineEdit& edit, QLabel& error_label) : input_field(element), interactive_element(element), edit_(edit), error_label_(error_label) {}
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
			void enable(bool enabled) final
			{
				edit_.setEnabled(enabled);
			}
		private:
			QLineEdit& edit_;
			QLabel& error_label_;
		};
		struct button : interactive_element
		{
			button(const Element& element, QPushButton& button) : interactive_element(element), button_(button) {}
			void enable(bool enabled) final
			{
				button_.setEnabled(enabled);
			}
		private:
			QPushButton& button_;
		};
		qt_os_form(QWidget* parent, const str& title, n4 width, n4 height, const sptr<IntResult>& action, const arr<Element>& elements) : title_(title), landing_(parent), action_(action)
		{
			panel_ = new QVBoxLayout();
			panel_->setObjectName("panel");
			errors_ = new QLabel();
			errors_->setObjectName("errors");
			panel_->addWidget(errors_);
			errors_->setStyleSheet("QLabel{color:red;padding-left:10px;margin-left:10px;font-size:10pt}");

			landing_.setWindowTitle(QString::fromStdString(title));
			landing_.setWindowFlag(Qt::WindowType::Dialog);
			landing_.resize(width, height);
			landing_.setLayout(panel_);
			landing_.setObjectName("main");

			std::for_each(elements.cbegin(), elements.cend(), [this](const Element& element)
			{
				switch (element.type)
				{
				case ElementType::TEXT_INPUT: add_text_field(element); break;
				case ElementType::EMAIL_INPUT: add_text_field(element); break;
				case ElementType::PASSWORD_INPUT: add_text_field(element); break;
				case ElementType::BOOL_INPUT:
				{
					auto chk = new QCheckBox(QString::fromStdString(element.title));
					QObject::connect(chk, &QCheckBox::clicked, [this, element]() { action_->on_result(element.input_id); });
					panel_->addWidget(chk);
					auto field = std::make_shared<bool_field>(element, *chk);
					fields_.push_back(field);
					interactive_elements_.push_back(field);
					break;
				}
				case ElementType::BUTTON:
				{
					auto btn = new QPushButton(QString::fromStdString(element.title));
					panel_->addWidget(btn);
					btn->setGraphicsEffect(rounded());
					QObject::connect(btn, &QPushButton::clicked, [this, element]()
					{
						clear_errors();
						action_->on_result(element.input_id);
					});
					interactive_elements_.push_back(std::make_shared<button>(element, *btn));
					break;
				}
				case ElementType::LABEL:
				{
					auto lbl = new QLabel(QString::fromStdString(element.title));
					lbl->setAlignment(Qt::AlignCenter);
					panel_->addWidget(lbl);
					break;
				}
				case ElementType::TEXT:
				{
					auto lbl = new QLabel(QString::fromStdString(element.title));
					lbl->setAlignment(Qt::AlignLeft);
					lbl->setStyleSheet("QLabel{color:black;padding-left:10px;margin-left:10px;font-size:10pt}");
					panel_->addWidget(lbl);
					break;
				}
				}
			});

			landing_.show();
		}
		virtual ~qt_os_form() {}

		void action_result(Status outcome, const arr<Message>& lines) final
		{
			if (Status::SUCCESS == outcome)
			{
				landing_.hide();
				//remove_all_widgets();
				return;
			}
			str error_text;
                        std::for_each(lines.cbegin(), lines.cend(), [&error_text](const Message& msg)
			{
				//auto it = fields_.find(msg.input_field);
				//if (fields_.end() != it)
				//	it->second->error(msg.content);
				//else
					error_text += msg.content + "<br/>";
			});
			errors_->setText(QString::fromStdString(error_text));

		}

		void show() final { landing_.show(); }
		void hide() final { landing_.hide(); }
		str title() final { return title_; };

		void progress(n4 /*position*/, n4 /*total*/) final
		{
		}

		void enable(n4 group) final
		{
			std::for_each(interactive_elements_.cbegin(), interactive_elements_.cend(), [group](auto&& field)
			{
				if (field->group_ == group)
					field->enable(true);
			});
		}

		void disable(n4 group) final
		{
			std::for_each(interactive_elements_.cbegin(), interactive_elements_.cend(), [group](auto&& field)
			{
				if (field->group_ == group)
					field->enable(false);
			});
		}

		hmap<n4, str> input() final
		{
			auto r = hmap<n4, str>();
			std::for_each(fields_.cbegin(), fields_.cend(), [&r](auto&& field)
			{
				r[field->input_id_] = field->value();
			});
			return r;
		}

	private:
		str title_;
		QWidget landing_;
		sptr<IntResult> action_;
		QVBoxLayout* panel_;
		QLabel* errors_;

		arr<sptr<input_field>> fields_;
		arr<sptr<interactive_element>> interactive_elements_;

		void add_text_field(const Element& element)
		{
			bool sensitive = false;
			switch (element.type)
			{
			case ElementType::PASSWORD_INPUT: sensitive = true; break;
			default: break;
			}

			auto vlayout = new QVBoxLayout();
			panel_->addLayout(vlayout);

			auto error_label = new QLabel();
			vlayout->addWidget(error_label);
			error_label->setStyleSheet("QLabel{color:red;padding-left:10px;margin-left:10px;font-size:10pt}");

			auto edit = new QLineEdit();
			vlayout->addWidget(edit);
			edit->setAttribute(Qt::WA_MacShowFocusRect, false);
			QObject::connect(edit, &QLineEdit::textChanged, [this, element](const QString&) { action_->on_result(element.input_id); });

			edit->setPlaceholderText(QString::fromStdString(element.title));
			edit->setText(QString::fromStdString(element.value));
			if (sensitive)
				edit->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);

			auto field = std::make_shared<text_field>(element, *edit, *error_label);
			fields_.push_back(field);
			interactive_elements_.push_back(field);
		}

		void clear_errors()
		{
			errors_->setText("");
			std::for_each(fields_.cbegin(), fields_.cend(), [](auto&& item)
			{
				item->error("");
			});
		}

		static QGraphicsDropShadowEffect* rounded()
		{
			auto r = new QGraphicsDropShadowEffect();
			r->setBlurRadius(10);
			r->setXOffset(0);
			r->setYOffset(2);
			return r;
		}
	};
}
