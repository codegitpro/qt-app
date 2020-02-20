#pragma once
#include "qt_form_list.h"

namespace ai
{
	struct qt_form_dialog : GenericForm
	{
		qt_form_dialog
		(
			sptr<Viewmodel> viewmodel,
			hmap<str, arr<str>>& references,
			const std::string & title,
			const std::vector<Field> & fields
		) 
		{
			m_dialog = new QDialog();
			m_dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::MaximizeUsingFullscreenGeometryHint);

			{
				auto hbar = new QHBoxLayout();
				m_container.addLayout(hbar);
				hbar->addWidget(create_label(title, Size::SMALL, true));

				{
					auto close_button = create_close_button(true);
					QObject::connect(close_button, &QPushButton::clicked, [viewmodel]()
					{
						viewmodel->back();
					});
					hbar->addWidget(close_button);
				}
			}

			m_form = std::make_shared<qt_form_list>(m_container, viewmodel, references, title, fields, true);
			m_dialog->setLayout(&m_container);
			m_dialog->setModal(true);
			m_dialog->show();
		}
	private:
		QVBoxLayout m_container;
		sptr<qt_form_list> m_form;
		QDialog* m_dialog;

		void close() final
		{
			m_dialog->close();
		}

		void disable_actions() final
		{
			m_form->disable_actions();
		}

		str input_get(n4 input_id) final
		{
			return m_form->input_get(input_id);
		}

		void input_set(n4 input_id, const str& value) final
		{
			m_form->input_set(input_id, value);
		}

		void input_configure(int32_t input_id, const std::string & config) final
		{
			m_form->input_configure(input_id, config);
		}

		bool enable(n4 element_id, bool enabled) final
		{
			return m_form->enable(element_id, enabled);
		}

		void error(n4 element_id, const str& message) final
		{
			m_form->error(element_id, message);
		}

		bool validate() final
		{
			return m_form->validate();
		}

	};
}