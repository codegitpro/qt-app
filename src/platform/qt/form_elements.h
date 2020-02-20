#pragma once
#include "../../general.h"
#include "../../zz_algorithms.h"
#include "../../ui/globals.h"

#include "../../bridge.h"
#include "../../bridge/cpp/element.hpp"
#include "../../bridge/cpp/viewmodel.hpp"
#include "../../bridge/cpp/reference.hpp"
#include "../../bridge/cpp/field.hpp"
#include "../../bridge/cpp/style.hpp"
#include "../../bridge/cpp/weight.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QImage>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStandardPaths>
#include <QString>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPlainTextEdit>

namespace ai
{
	static const char* colour_purple = "#454873";

	template<typename Layout>
	static Layout* add_layout(Layout* layout, QGridLayout &grid_layout, const Field& field)
	{
		auto& rect = field.rect;
		grid_layout.addLayout(layout, rect.row_beg, rect.col_beg, bl_rowspan(rect), bl_colspan(rect), x_align(field.style.align));
		
		return layout;
	}
	
	template<typename Widget>
	static Widget* add_widget(Widget* widget, QGridLayout &grid_layout, const Field& field)
	{
		auto& rect = field.rect;
		grid_layout.addWidget(widget, rect.row_beg, rect.col_beg, bl_rowspan(rect), bl_colspan(rect), x_align(field.style.align));
		if (rect.col_end < 0)
			widget->setMinimumWidth(-rect.col_end);
		return widget;
	}

	template<typename T>
	T* szfix(T* widget)
	{
		widget->setFixedSize(widget->sizeHint());
		return widget;
	}

	template<typename T>
	T* szmin(T* widget)
	{
		widget->setMinimumSize(widget->sizeHint());
		return widget;
	}

	static Qt::Alignment x_align(Align x)
	{
		switch (x)
		{
		case Align::CENTER: return Qt::AlignCenter;
		case Align::LEFT: return Qt::AlignLeft;
		case Align::RIGHT: return Qt::AlignRight;
		default: return Qt::Alignment();
		}
	}

	struct interactive_element
	{
		virtual ~interactive_element() {}
		virtual void enable(bool) = 0;
		virtual void value(str) = 0;
	};

	struct input_field : interactive_element
	{
		input_field(int input_id, str required_message) : input_id_(input_id), m_required_message(required_message) {}
		virtual ~input_field() {}
		virtual str value() = 0;
		virtual void error(str) = 0;
		n4 input_id_;
		virtual bool validate()
		{
			if (!m_required_message.empty() && value().empty())
			{
				error(m_required_message);
				return false;
			}
			return true;
		}
		virtual void configure(const str&) {}
	private:
		str m_required_message;
	};

	struct bool_field : input_field
	{
		bool_field(int input_id, QCheckBox& check) : input_field(input_id, ""), m_field(check) {}
		str value() final
		{
			return m_field.isChecked() ? "true" : "false";
		}
		void value(str val) final
		{
			m_field.setChecked(val == "true");
		}
		void error(str /*msg*/) final
		{
		}
		void enable(bool enabled) final
		{
			m_field.setEnabled(enabled);
		}
	private:
		QCheckBox& m_field;
	};

	struct text_field : input_field
	{
		text_field(int input_id, QLineEdit& edit, QLabel& error_label, const str& required_message = "")
			:
			input_field(input_id, required_message),
			m_field(edit),
			m_error_label(error_label)
		{}
		void value(str val) final
		{
			m_field.setText(QString::fromStdString(val));
		}
		void error(str msg) final
		{
			m_error_label.setText(QString::fromStdString(msg));
		}
		void enable(bool enabled) final
		{
			m_field.setEnabled(enabled);
		}
	private:
		QLineEdit& m_field;
		QLabel& m_error_label;
		str value() final
		{
			return m_field.text().toStdString();
		}
	};

	struct multiline_text_field : input_field
	{
		multiline_text_field(int input_id, QPlainTextEdit& edit, QLabel& error_label, const str& required_message = "")
			:
			input_field(input_id, required_message),
			m_field(edit),
			m_error_label(error_label)
		{}
		str value() final
		{
			auto text = m_field.toPlainText();
			return text.toStdString();
		}
		void value(str val) final
		{
			m_field.setPlainText(QString::fromStdString(val));
		}
		void error(str msg) final
		{
			m_error_label.setText(QString::fromStdString(msg));
		}
		void enable(bool enabled) final
		{
			m_field.setEnabled(enabled);
		}
	private:
		QPlainTextEdit& m_field;
		QLabel& m_error_label;
	};

	struct combo_field : input_field
	{
		combo_field(int input_id, QComboBox& edit, QLabel& error_label, const str& required_message = "")
			:
			input_field(input_id, required_message),
			m_field(edit),
			m_error_label(error_label)
		{}
		str value() final
		{
			int index = m_field.currentIndex();
			return 0 == index ? "" : std::to_string(index);
		}
		void value(str val) final
		{
			m_field.setCurrentIndex(val.empty() ? 0 : std::stoi(val));
		}
		void error(str msg) final
		{
			m_error_label.setText(QString::fromStdString(msg));
		}
		void enable(bool enabled) final
		{
			m_field.setEnabled(enabled);
		}
		bool validate() override
		{
			auto count = m_field.count();
			if (count == 0)
				return true;
			return input_field::validate();
		}
		void configure(const str& config) override 
		{
			m_field.clear();
			auto options = split(config, '|');
			std::for_each(options.begin(), options.end(), [this](const str& option)
			{
				m_field.addItem(QString::fromStdString(option));
			});
			szfix(&m_field);
		}
	private:
		QComboBox& m_field;
		QLabel& m_error_label;
	};

	struct button : interactive_element
	{
		button(QPushButton& button) : m_button(button) {}
		void enable(bool enabled) final
		{
			m_button.setEnabled(enabled);
		}
		void value(str val) final
		{
			m_button.setText(QString::fromStdString(val));
		}
	private:
		QPushButton& m_button;
	};

	struct label : interactive_element
	{
		label(QLabel& lbl) : m_label(lbl) {}
		void enable(bool enabled) final
		{
			m_label.setEnabled(enabled);
		}
		void value(str val) final
		{
			m_label.setText(QString::fromStdString(val));
			szfix(&m_label);
		}
	private:
		QLabel& m_label;
	};

	static str pt_size(Size size = Size::SMALL)
	{
		switch (size)
		{
		case Size::SMALL: return "12";
		case Size::MEDIUM: return "14";
		case Size::LARGE: return "18";
		}
		return "12";
	}
	static QPushButton* create_borderless_button(str title, Size size = Size::SMALL)
	{
		auto s = pt_size(size);
		auto btn = new QPushButton(QString::fromStdString(title));
		btn->setStyleSheet(QString::fromStdString("QPushButton{border-top:none;border-right:none;border-left:none;border-bottom:2px solid #0d0f1e;font-size: "+s+"pt;border-radius: 0px;background-color:#0d0f1e} QPushButton:hover{border-bottom:2px solid #01b7c4;color: #01b7c4;} QPushButton:pressed{color:white;border-bottom:2px solid white;}"));
		return btn;
	}

	static QPushButton* create_borderless_button(QIcon icon, str title)
	{
		auto btn = new QPushButton(icon, QString::fromStdString(title));
		btn->setStyleSheet("QPushButton{border-top:none;border-right:none;border-left:none;border-bottom:2px solid #0d0f1e;font-size: 10pt;border-radius: 0px;background-color:#0d0f1e} QPushButton:hover{border-bottom:2px solid #01b7c4;color: #01b7c4;} QPushButton:pressed{color:white;border-bottom:2px solid white;}");
		return btn;
	}

	static QIcon create_close_icon()
	{
		return QIcon(":/icons/baseline_close_white_24dp.png");
	}

	static QPushButton* create_close_button(bool is_dialog = false)
	{
		auto close_button = new QPushButton();
		close_button->setStyleSheet(is_dialog ? "background-color:#121528;border:none;qproperty-iconSize: 24px;" : "border:none;qproperty-iconSize: 24px;");
		close_button->setIcon(create_close_icon());
		szfix(close_button);
		return close_button;
	}

	static QIcon tinted_icon(const QString& path, QColor colour)
	{
		QPixmap pixmap = QPixmap(path);
		QPainter painter(&pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

		painter.setBrush(colour);
		painter.setPen(colour);

		painter.drawRect(pixmap.rect());

		return QIcon(pixmap);
	}

	static QColor to_qcolor(Colour colour)
	{
		return QColor(QString::fromStdString("#" + text_colour(colour)));
	}

	static QPushButton* create_back_button(sptr<Viewmodel> viewmodel, const str& title = "Return")
	{
		auto btn = create_borderless_button(tinted_icon(":/icons/baseline-chevron_left-24px.svg", QColor("#01b7c4")), title);
		//auto btn = new QPushButton(tinted_icon(":/icons/baseline-chevron_left-24px.svg", QColor("#01b7c4")), "Return");
		//btn->setStyleSheet("QPushButton{border-top:none;border-right:none;border-left:none;border-bottom:2px solid #0d0f1e;font-size: 10pt;border-radius: 0px;background-color:#0d0f1e} QPushButton:hover{border-bottom:2px solid #01b7c4;color: #01b7c4;} QPushButton:pressed{color:white;border-bottom:2px solid white;}");
		QObject::connect(btn, &QPushButton::clicked, [viewmodel]()
		{
			viewmodel->back();
		});
		return btn;
	}

	static QComboBox* create_combo(arr<str> options, int input_id, sptr<Viewmodel> viewmodel)
	{
		auto control = new QComboBox();
		std::for_each(options.begin(), options.end(), [control](auto&& item)
		{
			control->addItem(QString::fromStdString(item));
		});
		control->setSizeAdjustPolicy(QComboBox::AdjustToContents);

		QObject::connect(control, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), [input_id, viewmodel](const QString&)
		{
			viewmodel->action(input_id);
		});
		return control;
	}

	static arr<str> refs(hmap<str, arr<str>>& references, str reference_title)
	{
		auto ref_it = references.find(reference_title);
		auto r = arr<str>();
		std::for_each(ref_it->second.begin(), ref_it->second.end(), [&r](auto&& item)
		{
			r.push_back(item);
		});
		return r;
	}

	static QComboBox* create_combo(const arr<str>& options, int input_id, sptr<Viewmodel> viewmodel, hmap<n4, sptr<input_field>>& inputs, QLabel& error_label, str required = "")
	{
		auto control = create_combo(options, input_id, viewmodel);
		inputs.insert({ input_id,  std::make_shared<combo_field>(input_id, *control, error_label, required) });
		return control;

	}

	static QLabel* nowrap(QLabel* lbl)
	{
		lbl->setWordWrap(false);
		return lbl;
	}

	static QLabel* empty_dialog_label(bool is_dialog = false)
	{
		auto lbl = new QLabel();
		if (is_dialog)
			lbl->setStyleSheet("background-color:#121528;");
		return lbl;
	}

	static QLabel* create_label(str title, Size size = Size::SMALL, bool is_dialog = false, Align align = Align::CENTER, bool is_bold = false, Colour c = Colour::WHITE, bool word_wrap = true)
	{
		auto lbl = new QLabel(QString::fromStdString(title));

		auto style = str("color:#") + text_colour(c) + str(";padding-left:10px;margin-left:10px;padding-right:10px;margin-right:10px;");
		if (is_dialog)
			style += "background-color:#121528;";
		switch (size)
		{
		case Size::MEDIUM: style += "font-size:16pt;padding:14px;margin:14px;"; break;
		case Size::LARGE: style += "font-size:24pt;padding:18px;margin:18px;"; break;
		default: style += "font-size:12pt;"; break;
		}
		if (is_bold)
			style += "font-weight:bold;";

		if (!style.empty())
			lbl->setStyleSheet(QString::fromStdString(style));
		lbl->setAlignment(x_align(align));
		lbl->setWordWrap(word_wrap);
		return lbl;
	}

	static QLabel* create_label(str title, Style style, bool is_dialog = false)
	{
		return create_label(title, style.size, is_dialog, style.align, style.weight == Weight::BOLD, style.colour);
	}

	static QLabel* create_text(str title, Size size = Size::SMALL, bool is_dialog = false, Align align = Align::CENTER, bool is_bold = false)
	{
		return create_label(title, size, is_dialog, align, is_bold, Colour::CYAN);
	}

	static QCheckBox* create_check(str title, Size size, bool is_dialog)
	{
		auto chk = new QCheckBox(QString::fromStdString(title));
		auto style = str("");
		switch (size)
		{
		case Size::MEDIUM: style += "font-size:16pt;padding:14px;margin:14px;"; break;
		case Size::LARGE: style += "font-size:20pt;padding:16px;margin:16px;"; break;
		default: style += "font-size:12pt"; break;
		}
		if (is_dialog)
			style += "background-color:#121528;";

		if (!style.empty())
			chk->setStyleSheet(QString::fromStdString(style));

		return chk;
	}

	static QCheckBox* create_check(str title, int input_id, sptr<Viewmodel> viewmodel, Size size, bool is_dialog)
	{
		auto chk = create_check(title, size, is_dialog);
		QObject::connect(chk, &QCheckBox::stateChanged, [input_id, viewmodel]()
		{
			viewmodel->action(input_id);
		});
		return chk;
	}

	static void add_field(hmap<n4, sptr<input_field>>& inputs, int input_id, QGridLayout& grid_layout, int row, int col, bool is_dialog, funk<sptr<input_field>(QVBoxLayout&, QLabel&)> fun)
	{
		auto vlayout = new QVBoxLayout();
		grid_layout.addLayout(vlayout, row, col);

		auto error_label = new QLabel();
		vlayout->addWidget(error_label);
		auto style = str("color:red;padding-left:10px;margin-left:10px;padding-right:10px;margin-right:10px;font-size:14pt;");
		if (is_dialog)
			style += "background-color:#121528;";
		error_label->setStyleSheet(QString::fromStdString(style));

		inputs.insert({ input_id, fun(*vlayout, *error_label) });
	}

	static void add_field(hmap<n4, sptr<input_field>>& inputs, QGridLayout& grid_layout, const Field& field, funk<sptr<input_field>(QVBoxLayout&, QLabel&)> fun)
	{
		auto vlayout = add_layout(new QVBoxLayout(), grid_layout, field);

		auto error_label = new QLabel();
		vlayout->addWidget(error_label);
		auto style = str("color:red;padding-left:10px;margin-left:10px;padding-right:10px;margin-right:10px;font-size:14pt;");
		error_label->setStyleSheet(QString::fromStdString(style));

		inputs.insert({ field.field_id, fun(*vlayout, *error_label) });
	}

	static QLineEdit* add_text_field
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		int input_id,
		str title,
		QGridLayout &grid_layout,
		int row,
		int col,
		const str& required_msg = "",
		ElementType type = ElementType::TEXT_INPUT,
		bool is_dialog = false
	)
	{
		auto edit = new QLineEdit();
		edit->setAttribute(Qt::WA_MacShowFocusRect, false);
		QObject::connect(edit, &QLineEdit::textChanged, [viewmodel, input_id](const QString&) { viewmodel->action(input_id); });

		edit->setPlaceholderText(QString::fromStdString(title));
		if (ElementType::PASSWORD_INPUT == type)
			edit->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);

		add_field(inputs, input_id, grid_layout, row, col, is_dialog, [edit, input_id, required_msg](QVBoxLayout& vlayout, QLabel& error_label)
		{
			vlayout.addWidget(edit);
			return std::make_shared<text_field>(input_id, *edit, error_label, required_msg);
		});
		return edit;
	}

	static QPlainTextEdit* create_multiline_text(str title, int input_id, sptr<Viewmodel> viewmodel, hmap<n4, sptr<input_field>>& inputs, QLabel& error_label, str required = "")
	{
		auto control = new QPlainTextEdit();
		control->setPlaceholderText(QString::fromStdString(title));

		inputs.insert({ input_id,  std::make_shared<multiline_text_field>(input_id, *control, error_label, required) });

		QObject::connect(control, &QPlainTextEdit::textChanged, [input_id, viewmodel]()
		{
			viewmodel->action(input_id);
		});

		return control;
	}

	static QPlainTextEdit* add_multiline_text_field
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		QGridLayout &grid_layout,
		const Field& field
	)
	{
		assert(field.data_t == DataType::MULTILINE_TEXT);

		auto edit = new QPlainTextEdit();
		edit->setAttribute(Qt::WA_MacShowFocusRect, false);
		QObject::connect(edit, &QPlainTextEdit::textChanged, [viewmodel, input_id = field.field_id]() { viewmodel->action(input_id); });

		edit->setPlaceholderText(QString::fromStdString(field.title));

		add_field(inputs, grid_layout, field, [edit, &field](QVBoxLayout& vlayout, QLabel& error_label)
		{
			vlayout.addWidget(edit);
			return std::make_shared<multiline_text_field>(field.field_id, *edit, error_label, field.required ? "Please enter a value" : "");
		});

		if (field.rect.col_end < 0)
			edit->setMinimumWidth(-field.rect.col_end);

		return edit;
	}

	static QLineEdit* add_text_field
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		QGridLayout &grid_layout,
		const Field& field
	)
	{
		auto edit = new QLineEdit();
		edit->setAttribute(Qt::WA_MacShowFocusRect, false);
		QObject::connect(edit, &QLineEdit::textChanged, [viewmodel, input_id = field.field_id](const QString&) { viewmodel->action(input_id); });

		edit->setPlaceholderText(QString::fromStdString(field.title));
		if (DataType::PASSWORD == field.data_t)
			edit->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);

		add_field(inputs, grid_layout, field, [edit, &field](QVBoxLayout& vlayout, QLabel& error_label)
		{
			vlayout.addWidget(edit);
			return std::make_shared<text_field>(field.field_id, *edit, error_label, field.required ? "Please enter a value" : "");
		});

		if (field.rect.col_end < 0)
			edit->setMinimumWidth(-field.rect.col_end);

		return edit;
	}

	static QComboBox* add_combo
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		const arr<str>& options,
		int input_id,
		QGridLayout &grid_layout,
		int row,
		int col,
		const str& required_msg = "",
		bool is_dialog = false,
		Size size = Size::SMALL
	)
	{
		auto control = create_combo(options, input_id, viewmodel);
		auto style = str("");
		if (is_dialog)
			style += "background-color:#121528;";
		switch (size)
		{
		case Size::MEDIUM: style += "font-size:16pt;padding:14px;margin:14px;"; break;
		case Size::LARGE: style += "font-size:20pt;padding:16px;margin:16px;"; break;
		}
		if (!style.empty())
			control->setStyleSheet(QString::fromStdString(style));

		add_field(inputs, input_id, grid_layout, row, col, is_dialog, [control, input_id, required_msg](QVBoxLayout& vlayout, QLabel& error_label)
		{
			vlayout.addWidget(control);
			return std::make_shared<combo_field>(input_id, *control, error_label, required_msg);
		});
		return control;
	}


	static QComboBox* add_combo
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		QGridLayout &grid_layout,
		const Field& field,
		const arr<str>& options
	)
	{
		auto control = create_combo(options, field.field_id, viewmodel);
		auto style = str("");
		switch (field.style.size)
		{
		case Size::MEDIUM: style += "font-size:16pt;padding:14px;margin:14px;"; break;
		case Size::LARGE: style += "font-size:20pt;padding:16px;margin:16px;"; break;
		}
		if (!style.empty())
			control->setStyleSheet(QString::fromStdString(style));

		add_field(inputs, grid_layout, field, [control, &field](QVBoxLayout& vlayout, QLabel& error_label)
		{
			vlayout.addWidget(control);
			return std::make_shared<combo_field>(field.field_id, *control, error_label, field.required ? "Please make a choice" : "");
		});

		if (field.rect.col_end < 0)
		{
			control->setMinimumWidth(-field.rect.col_end);
			return control;
		}
		return szfix(control);
	}

	static QCheckBox* add_check
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		QGridLayout &grid_layout,
		const Field& field,
		bool is_dialog = false
	)
	{
		auto chk = add_widget(create_check(field.title, field.style.size, is_dialog), grid_layout, field);
		QObject::connect(chk, &QCheckBox::clicked, [viewmodel, input_id = field.field_id]() { viewmodel->action(input_id); });

		inputs.insert({ field.field_id, std::make_shared<bool_field>(field.field_id, *chk) });
		return chk;
	}

	static QCheckBox* add_check
	(
		hmap<n4, sptr<input_field>>& inputs,
		sptr<Viewmodel> viewmodel,
		int input_id,
		str title,
		QGridLayout &grid_layout,
		int row,
		int col,
		bool is_dialog = false,
		Size size = Size::SMALL
	)
	{
		auto chk = create_check(title, size, is_dialog);
		QObject::connect(chk, &QCheckBox::clicked, [viewmodel, input_id]() { viewmodel->action(input_id); });
		grid_layout.addWidget(chk, row, col);
		auto field = std::make_shared<bool_field>(input_id, *chk);
		inputs.insert({ input_id, field });
		return chk;
	}

	static QLabel* add_text
	(
		hmap<n4, sptr<interactive_element>>& interactives,
		int input_id,
		str title,
		QGridLayout &grid_layout,
		int row,
		int col,
		bool is_dialog = false,
		Size size = Size::SMALL,
		Align align = Align::CENTER,
		bool is_bold = false
	)
	{
		auto r = create_label(title, size, is_dialog, align, is_bold, Colour::CYAN);
		grid_layout.addWidget(r, row, col);
		auto field = std::make_shared<label>(*r);
		interactives.insert({ input_id, field });
		return r;
	}

	static QLabel* add_label
	(
		hmap<n4, sptr<interactive_element>>& interactives,
		QGridLayout &grid_layout,
		const Field& field,
		bool is_dialog = false
	)
	{
		auto r = add_widget(create_label(field.title, field.style.size, is_dialog, field.style.align, field.required, field.style.colour, false), grid_layout, field);
		auto rect = field.rect;
		interactives.insert({ field.field_id, std::make_shared<label>(*r) });

		return field.rect.col_end < 0 ? r : szfix(r);
	}

	static QLabel* add_label
	(
		hmap<n4, sptr<interactive_element>>& interactives,
		int input_id,
		str title,
		QGridLayout &grid_layout,
		int row,
		int col,
		bool is_dialog = false,
		Size size = Size::SMALL,
		Align align = Align::CENTER,
		bool is_bold = false
	)
	{
		auto r = create_label(title, size, is_dialog, align, is_bold);
		grid_layout.addWidget(r, row, col);
		auto field = std::make_shared<label>(*r);
		interactives.insert({ input_id, field });
		return r;
	}

	static void clear_errors(hmap<n4, sptr<input_field>>& inputs, QLabel* errors)
	{
		if (errors)
			errors->setText("");
		std::for_each(inputs.cbegin(), inputs.cend(), [](auto&& item)
		{
			item.second->error("");
		});
	}

	static QPushButton* create_button(str title, funk<void()> fun, Size size = Size::SMALL, Colour c = Colour::CYAN)
	{
		auto cc = "#"+text_colour(c)+";";
		auto style = str("QPushButton{color:") + cc + "border:2px solid "+cc+"}QPushButton:hover{background-color:" + cc + "color:#0d0f1e;}QPushButton:disabled{color:gray;border:2px solid gray;}";
		QPushButton* control = new QPushButton(QString::fromStdString(title));
		switch (size)
		{
		case Size::MEDIUM: style += "font-size:16pt;padding-top:14px;padding-bottom:14px;padding-left:28px;padding-right:28px;margin:14px;"; break;
		case Size::LARGE: style += "font-size:20pt;padding-top:18px;padding-bottom:18px;padding-left:36px;padding-right:36px;margin:16px;"; break;
		default: style += "font-size:12pt;padding-top:10px;padding-bottom:10px;padding-left:20px;padding-right:20px;margin:10px;"; break;
		}
		control->setStyleSheet(QString::fromStdString(style));

		if (fun)
			QObject::connect(control, &QPushButton::clicked, [fun]()
			{
				fun();
			});

		return control;
	}

	static QPushButton* create_button(str title, Style style)
	{
		return create_button(title, nullptr, style.size, style.colour);
	}

	static QPushButton* create_button(str title, int input_id, sptr<Viewmodel> viewmodel)
	{
		return create_button(title, [input_id, viewmodel]() { viewmodel->action(input_id); });
	}

	static QPushButton* create_button(str title, int input_id, sptr<Viewmodel> viewmodel, hmap<n4, sptr<interactive_element>>& inputs)
	{
		auto control = create_button(title, input_id, viewmodel);
		inputs.insert({ input_id, std::make_shared<button>(*control) });
		return control;
	}

	static bool validate_inputs
	(
		hmap<n4, sptr<input_field>>& inputs,
		QLabel* errors
	)
	{
		clear_errors(inputs, errors);
		bool found_error = std::accumulate(inputs.begin(), inputs.end(), false, [](bool acc, auto&& input)
		{
			return !input.second->validate() || acc;
		});
		return !found_error;
	}

	static QPushButton* create_button
	(
		hmap<n4, sptr<input_field>>& inputs,
		hmap<n4, sptr<interactive_element>>& interactives,
		sptr<Viewmodel> viewmodel,
		QLabel* errors,
		int input_id,
		str title,
		Size size = Size::SMALL,
		bool must_validate = true
	)
	{
		auto fun = [&inputs, viewmodel, input_id, errors, must_validate]()
		{
			if (!must_validate || validate_inputs(inputs, errors))
				viewmodel->action(input_id);
		};

		auto btn = create_button(title, fun, size);
		//if (is_dialog)
		//	btn->setStyleSheet("QPushButton{background-color:#121528;}");
		interactives.insert({ input_id, std::make_shared<button>(*btn) });
		return btn;
	}

	static QPushButton* add_button
	(
		hmap<n4, sptr<input_field>>& inputs,
		hmap<n4, sptr<interactive_element>>& interactives,
		sptr<Viewmodel> viewmodel,
		QLabel* errors,
		int input_id,
		str title,
		QGridLayout &grid_layout,
		int row,
		int col,
		Size size = Size::SMALL,
		bool must_validate = true
	)
	{
		auto btn = create_button(inputs, interactives, viewmodel, errors, input_id, title, size, must_validate);
		grid_layout.addWidget(btn, row++, col, Qt::AlignCenter);
		return btn;
	}

	static QPushButton* add_button
	(
		hmap<n4, sptr<input_field>>& inputs,
		hmap<n4, sptr<interactive_element>>& interactives,
		sptr<Viewmodel> viewmodel,
		QLabel& errors,
		Field field,
		QGridLayout &grid_layout
	)
	{
		auto btn = add_widget
		(
			FieldType::BACK == field.field_t 
				? create_back_button(viewmodel, field.title) 
				: create_button(inputs, interactives, viewmodel, &errors, field.field_id, field.title, field.style.size, field.required),
			grid_layout, 
			field
		);

		if (field.rect.col_end < 0)
			btn->setMinimumWidth(-field.rect.col_end);
		else
			szfix(btn);

		return btn;
	}

	static sptr<input_field> get_input(const hmap<n4, sptr<input_field>>& inputs, n4 element_id)
	{
		auto found = inputs.find(element_id);
		return inputs.end() != found ? found->second : nullptr;
	}

	static sptr<interactive_element> get_interactive(const hmap<n4, sptr<input_field>>& inputs, const hmap<n4, sptr<interactive_element>>& interactives, n4 element_id)
	{
		auto found = interactives.find(element_id);
		if (interactives.end() != found)
			return found->second;

		return get_input(inputs, element_id);
	}

	struct problem_resolved_widget : QWidget
	{
		problem_resolved_widget(bool resolved, Size size = Size::MEDIUM)
		{
			auto layout = new QVBoxLayout();
			setLayout(layout);

			layout->setContentsMargins(0, 0, 0, 0);
			layout->setSpacing(0);

			layout->addWidget(create_label(""), 1);

			{
				layout->addWidget(szmin(create_label("PROBLEM RESOLVED", size, false, Align::CENTER, true)));
			}

			{
				auto chk = szmin(create_check("", size, false));
				if (resolved)
					chk->setChecked(true);

				auto hbox = new QHBoxLayout();
				hbox->addWidget(new QLabel(), 1);
				hbox->addWidget(chk);
				hbox->addWidget(new QLabel(), 1);
				layout->addLayout(hbox);
			}

			layout->addWidget(create_label(""), 1);

		}
	};

}