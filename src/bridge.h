#pragma once
#include "general.h"
#include "bridge/cpp/void_result.hpp"
#include "bridge/cpp/string_result.hpp"
#include "bridge/cpp/strings_result.hpp"
#include "bridge/cpp/http_result.hpp"
#include "bridge/cpp/http_progress_result.hpp"
#include "bridge/cpp/progress.hpp"
#include "bridge/cpp/long_result.hpp"
#include "bridge/cpp/binary_result.hpp"
#include "bridge/cpp/bool_result.hpp"

namespace ai
{
	struct lambda_void_result : VoidResult
	{
		lambda_void_result(funk<void()> fun) : fun_(fun) {}
		void on_result() final
		{
			fun_();
		}
	private:
		funk<void()> fun_;
	};

	struct lambda_string_result : StringResult
	{
		lambda_string_result(funk<void(str)> fun) : fun_(fun) {}
		void on_result(const str& content) final
		{
			fun_(content);
		}
	private:
		funk<void(str)> fun_;
	};

	struct lambda_strings_result : StringsResult
	{
		lambda_strings_result(funk<void(const arr<str>&)> fun) : fun_(fun) {}
		void on_result(const arr<str>& content) final
		{
			fun_(content);
		}
	private:
		funk<void(const arr<str>&)> fun_;
	};

	struct lambda_binary_result : BinaryResult
	{
		lambda_binary_result(funk<void(arr<p1>)> fun) : fun_(fun) {}
		void on_result(const arr<p1>& content) final
		{
			fun_(content);
		}
	private:
		funk<void(arr<p1>)> fun_;
	};

	struct lambda_bool_result : BoolResult
	{
		void on_result(bool success) final
		{
			fun_(success);
		}
		lambda_bool_result(funk<void(bool)> fun) : fun_(fun) {}
	private:
		funk<void(bool)> fun_;
	};

	struct lambda_long_result : LongResult
	{
		void on_result(n8 size) final
		{
			fun_(size);
		}
		lambda_long_result(funk<void(n8)> fun) : fun_(fun) {}
	private:
		funk<void(n8)> fun_;
	};

	struct lambda_http_result : HttpResult
	{
		void on_result(n4 status_code, const std::vector<Header> & headers, const str& data) final
		{
			fun_(status_code, headers, data);
		}

		lambda_http_result(funk<void(n4 status_code, const arr<Header>& headers, str data)> fun) : fun_(fun) {}
	private:
		funk<void(n4 status_code, const arr<Header>& headers, str data)> fun_;
	};

	struct lambda_http_progress_result : HttpProgressResult
	{
		void on_progress(const Progress& at) final
		{
			progress_fun_(at);
		}

		void on_result(n4 status_code, const std::vector<Header> & headers, n8 content_length)
		{
			fun_(status_code, headers, content_length);
		}

		lambda_http_progress_result(funk<void(Progress)> progress_fun, funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun) : progress_fun_(progress_fun), fun_(fun)
		{

		}
	private:
		funk<void(Progress)> progress_fun_;
		funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun_;
	};
}
