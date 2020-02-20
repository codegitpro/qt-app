#pragma once
#include "plat.h"
#include "task_scheduler.h"
#include "../bridge/cpp/os_io.hpp"
#include "../bridge/cpp/http_verb.hpp"
#include "../bridge/cpp/header.hpp"
#include "../bridge.h"

namespace ai
{
	struct device_platform : plat
	{
		void wait(n4 millis, funk<void()> fun) final
		{
			os_->wait(millis, std::make_shared<lambda_void_result>(fun));
		}

        void file_size(const str& path, funk<void(n8)> fun) final
        {
            os_->file_size(path, std::make_shared<lambda_long_result>(fun));
        }

		str app_data_location() final
		{
			return data_location_;
		}

        void log(LogType type, int line, const char* file, const str& message) override
        {
            os_->log(type, line, file, message);
        }

        void log(LogType type, int line, const char* file, const arr<str>& messages) override
        {
            os_->log_lines(type, line, file, messages);
        }

		void copy_file(const str& current_path, const str& new_path, funk<void(bool)> fun) final
		{
            os_->copy_file(current_path, new_path, std::make_shared<lambda_bool_result>(fun));
		}

		void make_path(const str& path, funk<void(bool)> fun) final
		{
            os_->make_path(path, std::make_shared<lambda_bool_result>(fun));
		}

		void rename_file(const str& path, const str& new_path, funk<void(bool)> fun) final
		{
            os_->rename_file(path, new_path, std::make_shared<lambda_bool_result>(fun));
		}

		void file_expand_directories(const arr<str>& paths, funk<void(const arr<str>&)> fun) final
		{
			os_->file_expand_directories(paths, std::make_shared<lambda_strings_result>(fun));
		}

		void file_copy_hierarchy(const str& dest_root_path, const arr<str>& dest_relative_paths, const arr<str>& src_paths) final
		{
			os_->file_copy_hierarchy(dest_root_path, dest_relative_paths, src_paths);
		}

		void finish_io_and_wait() final
		{
			log_debug("finish_io_and_wait");
		}

		sptr<CancellationToken> http_request
		(
			HttpVerb verb,
			const str& url,
			const arr<Header>& headers,
			str body,
			funk<void(n4 status_code, const arr<Header>& headers, str data)> fun
		) override
		{
            return os_->http_request(verb, url, headers, body, std::make_shared<lambda_http_result>(fun));
		}

		sptr<CancellationToken> http_upload_file
		(
			HttpVerb verb,
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			funk<void(Progress)> progress_fun,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
		) override
		{
            return os_->http_upload_file(verb, url, file_path, headers, std::make_shared<lambda_http_progress_result>(progress_fun, fun));
		}

		sptr<CancellationToken> http_download_file
		(
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			n8 size,
			p16 md5,
			funk<void(Progress)> progress_fun,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
		) override
		{
            return os_->http_download_file(url, file_path, headers, size, to_hex(md5), std::make_shared<lambda_http_progress_result>(progress_fun, fun));
		}

		str current_working_directory() final
		{
			log_debug("current_working_directory");
			return "";
		}

        bool file_thumbnail(const str& path, FileType type, funk<void(arr<p1>)> fun) final
        {
            return os_->file_thumbnail(path, type, std::make_shared<lambda_binary_result>(fun));
        }

		void file_readall(const str& path, funk<void(arr<p1>)> fun) final
		{
			os_->file_readall(path, std::make_shared<lambda_binary_result>(fun));
		}

		void file_writeall(const str& path, arr<p1> content, funk<void(bool)> fun) final
		{
			os_->file_writeall(path, move(content), std::make_shared<lambda_bool_result>(fun));
		}

		void file_writeall_safely(const str& path, arr<p1> content, funk<void(bool)> fun) final
		{
			os_->file_writeall_safely(path, move(content), std::make_shared<lambda_bool_result>(fun));
		}

        void file_write_password(const str& username, const str& password, funk<void(bool)> fun) final
        {
            os_->file_write_password(username, password, std::make_shared<lambda_bool_result>(fun));
        }
        
        void file_read_password(const str& username, funk<void(str)> fun) final
        {
            os_->file_read_password(username, std::make_shared<lambda_string_result>(fun));
        }

        void file_clear_cache(const str& username, funk<void(bool)> fun) final
        {
            os_->file_clear_cache(username, std::make_shared<lambda_bool_result>(fun));
        }

		void stop_taking_tasks() final
		{
			log_debug("stop_taking_tasks");
		}

        device_platform(const str& data_location, const sptr<OsIo>& os) : data_location_(data_location), os_(os)
		{

		}

		virtual ~device_platform()
		{

		}
	private:
		str data_location_;
        sptr<OsIo> os_;
	};
}
