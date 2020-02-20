#pragma once
#include "../platform/plat.h"
#include "object_detail.h"

namespace ai
{
	namespace openstack
	{
		struct object_list
		{
			const char* data = nullptr;
			arr<object_detail> objects;
			arr<par<p4, p4>> subdirs;
		};

		enum class timetrade
		{
			freshest,
			fastest
		};

		enum class scope
		{
			all_files,
			subdir
		};

		struct session
		{
			static sptr<CancellationToken> connect(plat&, const str& user, const str& pwd, const str& tenant, const str& auth_url, funk<void(uptr<session>)> fun);
			session(plat& platform, const str& token, const str& url, str container) : platform_(platform), token_(token), url_(url), container_(container)
			{
			}

			sptr<CancellationToken> quick_list(const str& path, const str& marker, funk<void(arr<str>)>);
			sptr<CancellationToken> list
			(
				scope,
				const str& prefix,
				int limit,
				par<str, str> range,
				const str& path,
				timetrade,
				funk<void(object_list)>
			);
            sptr<CancellationToken> copy_file(const str& src_cloud_path, const str& dest_cloud_path, p8 content_length, funk<void(n4)>);
			sptr<CancellationToken> upload_file(const str& cloud_path, const str& file_path, p16, const str& content_type, funk<void(Progress)>, funk<void(n4,n8,p16)>);
			sptr<CancellationToken> delete_file(const str& cloud_path, funk<void(n4)>);
			sptr<CancellationToken> download_file(const str& cloud_path, const str& file_path, n8 size, p16 md5, funk<void(Progress)>, funk<void(n4, n8, p16)>);
			static arr<str> parse_quick_list(const str& source)
			{
				auto ss = arr<str>();
				auto b = source.cbegin();
				auto e = source.cend();

				while (true)
				{
					b = std::find_if_not(b, e, [](char c) { return '\n' == c || '\r' == c || '\t' == c || ' ' == c; });
					if (e == b)
						break;
					auto eol = find_if(b, e, [](char c) { return '\n' == c || '\r' == c; });
					ss.push_back(str(b, eol));
					b = eol;
				}
				return ss;
			}
		private:
			plat& platform_;
			str token_;
			str url_;
			str container_;
		};
	}
}
