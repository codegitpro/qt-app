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
	struct about_form : IntResult
	{
		static void show(const sptr<OsGui>& gui, const str& version)
		{
			auto els = arr<Element>();
			els.push_back(Element(-1, ElementType::TEXT, -1, "Blomp offers the best free cloud file, photo, video and more storage.", ""));
			els.push_back(Element(-1, ElementType::TEXT, -1, "Version: " + version, ""));
			auto form = std::make_shared<about_form>(gui);
			gui->gui_thread(std::make_shared<lambda_void_result>([gui, form, els]()
			{
				auto gui_form = gui->create_form("About", true, 400, 400, form, els);
				form->form(gui_form);
			}));
		}

		about_form(const sptr<OsGui>& gui) : gui_(gui) {}

		virtual ~about_form() {}
	private:
		sptr<OsGui> gui_;
		sptr<OsForm> form_ = nullptr;

		void form(const sptr<OsForm>& f) { form_ = f; }

		void on_result(n4) final
		{
			gui_->gui_thread(std::make_shared<lambda_void_result>([this]() 
			{ 
				form_ = nullptr; 
			}));
		}
	};
}
