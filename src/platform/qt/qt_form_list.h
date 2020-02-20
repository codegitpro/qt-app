#pragma once
#include "basic_form_implementation.h"
#include "../../ui/globals.h"

namespace ai
{
	struct qt_form_list : basic_form_implementation
	{
		qt_form_list
		(
			QVBoxLayout& container,
			sptr<Viewmodel> viewmodel,
			hmap<str, arr<str>>& references,
			const std::string & /*title*/,
			arr<Field> fields,
			bool is_dialog = false
		)
			:
			basic_form_implementation(viewmodel, false),
			m_references(references)
		{
			assert(!fields.empty());

			auto header = new QGridLayout;
			container.addLayout(header);

			//if (0 == items_per_row)
			//{
			//	auto scrollArea = new QScrollArea();
			//	//scrollArea->setStyleSheet("QScrollArea{background-color:white;}");
			//	scrollArea->setBackgroundRole(QPalette::Light);
			//	auto w = new QWidget();
			//	w->setLayout(header);
			//	scrollArea->setWidget(w);
			//	scrollArea->setVisible(true);
			//	scrollArea->setWidgetResizable(true);

			//	container.addWidget(scrollArea);
			//}

			std::for_each(fields.begin(), fields.end(), [this, header, is_dialog](const Field& field)
			{
				assert((field.rect.row_end < 0 || field.rect.row_beg < field.rect.row_end) && (field.rect.col_end < 0 || field.rect.col_beg < field.rect.col_end));

				switch (field.field_t)
				{
				case FieldType::BACK:
				case FieldType::BUTTON: add_button(inputs(), interactives(), vm(), error_label(), field, *header); break;
				case FieldType::EXPAND: header->setColumnStretch(field.rect.col_beg, 1); break;
				case FieldType::LABEL: add_label(interactives(), *header, field, is_dialog); break;
				case FieldType::ERROR: header->addWidget(&error_label(), f_row(field), f_col(field), f_rowspan(field), f_colspan(field), x_align(field.style.align)); break;
				case FieldType::CONSOLE:
					break;
				case FieldType::CHAT: 
					break;
				case FieldType::INPUT:
				{
					switch (field.data_t)
					{
					case DataType::BOOLEAN: add_check(inputs(), vm(), *header, field, is_dialog); break;
					case DataType::RATING: 
						break;
					case DataType::REFERENCE: add_combo(inputs(), vm(), *header, field, refs(m_references, field.title)); break;
					case DataType::CHOICE: add_combo(inputs(), vm(), *header, field, split(field.title, '|')); break;
					case DataType::MULTILINE_TEXT: add_multiline_text_field(inputs(), vm(), *header, field); break;
					default: add_text_field(inputs(), vm(), *header, field); break;
					}
					break;
				}
				case FieldType::LINK: break;
				}
			});

			int end_row = std::max_element(fields.begin(), fields.end(), [](const Field& l, const Field& r) { return l.rect.row_end < r.rect.row_end; })->rect.row_end;

			auto tab = table();
			if (tab)
			{
				int end_col = std::max_element(fields.begin(), fields.end(), [](const Field& l, const Field& r) { return l.rect.col_end < r.rect.col_end; })->rect.col_end;
				header->addWidget(tab, end_row, 0, 1, end_col);
			}
			header->setRowStretch(end_row, 3);
		}
	private:
		hmap<str, arr<str>>& m_references;

	};
}