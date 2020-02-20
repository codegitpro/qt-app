#pragma once
#include "../general.h"
#include "../bridge/cpp/os_gui.hpp"
#include "../bridge/cpp/os_form.hpp"
#include "../bridge/cpp/element.hpp"
#include "../bridge/cpp/element_type.hpp"
#include "../bridge/cpp/int_result.hpp"
#include "../bridge.h"
#include "../caching_engine.h"

namespace ai
{
	struct login_form : IntResult
	{
		virtual ~login_form() {}

		enum class id : n4
		{
			email = 1,
			pwd,
			login_button,
			signup_button,
			forgot_button,
			remember_me
		};
		static void show(cache& cache, const sptr<OsGui>& gui, str email)
		{
			auto els = arr<Element>();
			els.push_back(Element(-1, ElementType::LABEL, -1, "Get Up to 200 GB for Free!", ""));
			els.push_back(Element(-1, ElementType::LABEL, -1, "Sign Up Today", ""));
			els.push_back(Element(0, ElementType::EMAIL_INPUT, static_cast<int>(id::email), "Email", email));
			els.push_back(Element(0, ElementType::PASSWORD_INPUT, static_cast<int>(id::pwd), "Password", ""));
			els.push_back(Element(0, ElementType::BOOL_INPUT, static_cast<int>(id::remember_me), "Remember Me", "false"));
			els.push_back(Element(0, ElementType::BUTTON, static_cast<int>(id::login_button), "Login", ""));
			els.push_back(Element(-1, ElementType::BUTTON, static_cast<int>(id::signup_button), "Sign Up Now!", ""));
			els.push_back(Element(-1, ElementType::BUTTON, static_cast<int>(id::forgot_button), "Forgot Password", ""));
			auto login = std::make_shared<login_form>(cache, gui);
			gui->gui_thread(std::make_shared<lambda_void_result>([gui, login, els]()
			{
				auto gui_form = gui->create_form("Blomp", true, 400, 700, login, els);
				login->form(gui_form);
			}));
		}

		login_form(cache& cache, const sptr<OsGui>& gui) : cache_(cache), gui_(gui) {}
	private:
		cache& cache_;
		sptr<OsGui> gui_;
		sptr<OsForm> form_ = nullptr;

		void form(const sptr<OsForm>& f) { form_ = f; }
		void on_result(n4 value) final
		{
			switch (static_cast<id>(value))
			{
			case id::login_button:
			{
				auto data = form_->input();
				auto email = data[static_cast<int>(id::email)];
				auto pwd = data[static_cast<int>(id::pwd)];
				if (email.empty() && pwd.empty())
				{
                    form_->action_result(Status::FAIL, { Message(static_cast<n4>(id::login_button), "Please enter an email and password") });
					return;
				}

				auto errors = arr<Message>();
				if (email.empty())
					errors.push_back(Message(static_cast<n4>(id::email), "Please enter an email"));
				else
				{
					//if (find(email.cbegin(), email.cend(), '@') == email.cend())
					//	errors.push_back(Message("email", "A valid email contains the @ character"));
				}
				if (pwd.empty())
					errors.push_back(Message(static_cast<n4>(id::pwd), "Please enter a password"));
				if (!errors.empty())
				{
					form_->action_result(Status::FAIL, errors);
					return;
				}
				auto remember_me = data[static_cast<int>(id::remember_me)];

				form_->disable(0);
				cache_.login(email, pwd, "true" == remember_me, [this](str error)
				{
					gui_->gui_thread(std::make_shared<lambda_void_result>([this, error]()
					{
						if (!error.empty())
						{
							form_->action_result(Status::FAIL, { Message(static_cast<n4>(id::login_button), error) });
							form_->enable(0);
							return;
						}
						form_->action_result(Status::SUCCESS, {});
					}));
				});
				break;
			}
			case id::signup_button:
			{
				gui_->open_link_in_browser("https://dashboard.blomp.com/register/");
				break;
			}
			case id::forgot_button:
			{
				gui_->open_link_in_browser("https://dashboard.blomp.com/recovery/");
				break;
			}
            default: break;
			}
		}
	};
}
