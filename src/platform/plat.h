#pragma once
#include "../general.h"
#include "../ai_http.h"
#include "../bridge/cpp/http_verb.hpp"
#include "../bridge/cpp/progress.hpp"
#include "../bridge/cpp/header.hpp"
#include "../bridge/cpp/cancellation_token.hpp"
#include "../bridge/cpp/file_type.hpp"
#include "../bridge/cpp/log_type.hpp"

namespace ai
{
    static str verb_to_string(HttpVerb verb)
    {
        switch (verb)
        {
            case HttpVerb::GET: return "GET";
            case HttpVerb::POST: return "POST";
            case HttpVerb::PUT: return "PUT";
            case HttpVerb::PATCH: return "PATCH";
            case HttpVerb::REMOVE: return "DELETE";
            case HttpVerb::OPTIONS: return "OPTIONS";
        }
        return "UNKNOWN";
    }
	
    enum class file_open
	{
		read,
		write
	};

	enum class file_offset
	{
		append,
		truncate,
		any
	};

	struct net_server
	{
        virtual ~net_server(){}
		virtual bool net_server_send(str) = 0;
		virtual bool net_server_start_reading() = 0;
		virtual void net_server_close() = 0;
	};

	struct net_client
	{
        virtual ~net_client(){}
		virtual void net_client_send_complete() = 0;
		virtual void net_client_error() = 0;
		virtual void net_client_close() = 0;
		virtual bool net_client_receive(str) = 0;
	};

	static int percentage(const Progress& pr)
	{
		return pr.count == pr.total ? 100 : (int)((100.0*pr.count) / pr.total);
	}

	struct plat
	{
		static n8 cputicks();
		static void log_debug(const str&);

		virtual void wait(n4 millis, funk<void()>) = 0;

		virtual str current_working_directory() = 0;
        virtual void file_readall(const str&, funk<void(arr<p1>)>) = 0;
        virtual bool file_thumbnail(const str&, FileType, funk<void(arr<p1>)>) = 0;
		virtual void file_writeall(const str&, arr<p1>, funk<void(bool)>) = 0;
        virtual void file_writeall_safely(const str&, arr<p1>, funk<void(bool)>) = 0;
        virtual void file_write_password(const str& username, const str& password, funk<void(bool)>) = 0;
        virtual void file_read_password(const str& username, funk<void(str)>) = 0;
		virtual void stop_taking_tasks() = 0;
        virtual void file_clear_cache(const str& username, funk<void(bool)>) = 0;
		virtual void file_size(const str&, funk<void(n8)>) = 0;
		virtual str app_data_location() = 0;
		virtual void copy_file(const str& current_path, const str& new_path, funk<void(bool)>) = 0;
		virtual void make_path(const str& path, funk<void(bool)>) = 0;
		virtual void rename_file(const str& path, const str& new_path, funk<void(bool)>) = 0;
		virtual void file_expand_directories(const arr<str>& paths, funk<void(const arr<str>&)>) = 0;
		virtual void file_copy_hierarchy(const str& dest_root_path, const arr<str>& dest_relative_paths, const arr<str>& src_paths) = 0;
		virtual void finish_io_and_wait() = 0;
        virtual void log(LogType type, int line, const char* file, const str& message) = 0;
        virtual void log(LogType type, int line, const char* file, const arr<str>& messages) = 0;
		virtual sptr<CancellationToken> http_request
		(
			HttpVerb verb,
			const str& url,
			const arr<Header>& headers,
			str body,
			funk<void(n4 status_code, const arr<Header>& headers, str data)> f_response
		) = 0;

		virtual sptr<CancellationToken> http_upload_file
		(
			HttpVerb verb,
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			funk<void(Progress)>,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> f_response
		) = 0;

		virtual sptr<CancellationToken> http_download_file
		(
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			n8 size,
			p16 md5,
			funk<void(Progress)>,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> f_response
		) = 0;

		virtual ~plat()
		{
        }
	};

	struct check
	{
		check(const char* file, int line) : file_(file), line_(line)
		{
			plat::log_debug(str(file_) + " " + std::to_string(line_));
			ticks_ = plat::cputicks();
		}
		~check()
		{
			plat::log_debug(str(file_) + " " + std::to_string(line_) + " " + std::to_string(plat::cputicks() - ticks_));
		}
	private:
		const char* file_;
		int line_;
		n8 ticks_;
	};
}
