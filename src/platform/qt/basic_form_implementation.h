#pragma once
#include "form_elements.h"
#include "../../bridge/cpp/generic_form.hpp"

namespace ai
{
	struct basic_form_implementation : GenericForm
	{
		basic_form_implementation
		(
			const sptr<Viewmodel>& viewmodel,
			bool is_dialog = false
		)
			:
			m_viewmodel(viewmodel),
			m_is_dialog(is_dialog)
		{
		}

		bool enable(n4 element_id, bool enabled) override
		{
			if (-1 == element_id)
			{
				for (int i = 1; ; ++i)
				{
					auto element = get_interactive(i);
					if (!element)
						return true;
					element->enable(enabled);
				}
				return true;
			}

			auto field = get_interactive(element_id);
			if (!field)
				return false;
			field->enable(enabled);
			return true;
		}

		std::string input_get(int32_t input_id) override
		{
			auto input = get_input(input_id);
			return input ? input->value() : "";
		}

		void input_set(int32_t input_id, const std::string & value) override
		{
			auto input = get_interactive(input_id);
			if (input)
				input->value(value);
		}

		void input_configure(int32_t input_id, const std::string & config) override
		{
			auto input = get_input(input_id);
			if (input)
			{
				input->configure(config);
			}
		}

		void error(int32_t element_id, const std::string & message) override
		{
			auto field = get_input(element_id);
			if (field)
				field->error(message);
			else if (m_error_label)
				m_error_label->setText(QString::fromStdString(message));
		}

		bool validate() override
		{
			return validate_inputs(m_inputs, m_error_label);
		}

		void close() override {}

		void disable_actions()
		{
			m_viewmodel = nullptr;
		}

		static void add_sub_col_fun(QWidget*& subcol_widget, QHBoxLayout*& subcol_hbox, QVBoxLayout*& vbox, arr<QWidget*>& col_widgets)
		{
			if (subcol_hbox)
			{
				subcol_widget = new QWidget();
				subcol_widget->setLayout(subcol_hbox);
				subcol_hbox = nullptr;
			}
			assert(subcol_widget);
			if (vbox)
			{
				vbox->addWidget(subcol_widget);
				vbox->addWidget(create_label(""), 1);
				subcol_widget = new QWidget();
				subcol_widget->setLayout(vbox);
				vbox = nullptr;
			}
			assert(subcol_widget);
			col_widgets.push_back(subcol_widget);
			subcol_widget = nullptr;
		}

		static void add_widget_fun(QWidget* w, QWidget*& subcol_widget, QHBoxLayout*& subcol_hbox, int width)
		{
			if (width > 0)
				w->setMinimumWidth(width);
			if (subcol_widget)
			{
				subcol_hbox = new QHBoxLayout();
				subcol_hbox->setContentsMargins(0, 0, 0, 0);
				subcol_hbox->setSpacing(0);
				subcol_hbox->addWidget(subcol_widget);
				subcol_widget = nullptr;
			}
			if (subcol_hbox)
				subcol_hbox->addWidget(w);
			else
				subcol_widget = w;
		};


		
	protected:
		sptr<Viewmodel> vm() { return m_viewmodel; }
		QLabel& error_label() const 
		{
			if (!m_error_label)
				m_error_label = create_label("", Size::SMALL, m_is_dialog, Align::CENTER, false, Colour::RED);

			return *m_error_label; 
		}
		QTableWidget* table() const { return m_table; }
		hmap<n4, sptr<interactive_element>>& interactives() { return m_interactives; }
		hmap<n4, sptr<input_field>>& inputs() { return m_inputs; }
	private:
		sptr<Viewmodel> m_viewmodel;
		bool m_is_dialog;
		hmap<n4, sptr<interactive_element>> m_interactives;
		hmap<n4, sptr<input_field>> m_inputs;
		QTableWidget *m_table = nullptr;
		arr<int> m_row_ids;

		mutable QLabel* m_error_label = nullptr;

		sptr<interactive_element> get_interactive(n4 element_id)
		{
			auto found = m_interactives.find(element_id);
			if (m_interactives.end() != found)
				return found->second;

			return get_input(element_id);
		}

		sptr<input_field> get_input(n4 element_id)
		{
			auto found = m_inputs.find(element_id);
			return m_inputs.end() != found ? found->second : nullptr;
		}
	};
}