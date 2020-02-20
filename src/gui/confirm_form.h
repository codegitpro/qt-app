#pragma once
#include "../general.h"
#include "../bridge/cpp/os_gui.hpp"
#include "../bridge/cpp/os_form.hpp"
#include "../bridge/cpp/element.hpp"
#include "../bridge/cpp/element_type.hpp"
#include "../bridge/cpp/int_result.hpp"
#include "../bridge.h"

namespace ai
{
	struct confirm_form : IntResult
	{
		static void show(const sptr<OsGui>& gui, str title, str message, str action, funk<void()> on_click)
		{
			auto els = arr<Element>();
			els.push_back(Element(-1, ElementType::LABEL, -1, message, ""));
			els.push_back(Element(0, ElementType::BUTTON, 1, action, ""));
			auto confirm = std::make_shared<confirm_form>(gui, on_click);
			gui->gui_thread(std::make_shared<lambda_void_result>([gui, confirm, els, title]()
			{
				auto gui_form = gui->create_form(title, true, 500, 100, confirm, els);
				confirm->form(gui_form);
			}));
		}

		confirm_form(const sptr<OsGui>& gui, funk<void()> on_click) : gui_(gui), on_click_(on_click) {}

		virtual ~confirm_form() {}
	private:
		sptr<OsGui> gui_;
		sptr<OsForm> form_ = nullptr;
		funk<void()> on_click_;

		void form(const sptr<OsForm>& f) { form_ = f; }

		void on_result(n4) final
		{
			on_click_();
			gui_->gui_thread(std::make_shared<lambda_void_result>([this]() { form_ = nullptr; }));
		}
	};
}