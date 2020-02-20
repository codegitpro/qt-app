#include "openstack.h"
#include "../json.h"
#include "list_parser.h"
#include "../bridge/cpp/header.hpp"

using namespace std;

namespace ai
{
	namespace openstack
	{
		struct auth_parser
		{
			auth_parser(const char* data)
			{
				data_ = data;
				json::parse_value(data, json::parse_space(data, 0), *this);
			}
			void begin_array()
			{
				path_.push_back(key_);
				key_ = make_pair(0, 0);
			}
			void end_array()
			{
				path_.pop_back();
			}
			void begin_object()
			{
				path_.push_back(key_);
				key_ = make_pair(0, 0);
			}
			void end_object()
			{
				path_.pop_back();
			}
			void text(json::type type, p4 begin, p4 end)
			{
				if (json::type::text != type)
					return;

				if (3 == path_.size() && is_equal("token", path_[2]) && is_equal("access", path_[1]))
				{
					if (token_id.empty() && is_equal("id", key_))
						token_id = str(data_ + begin, data_ + end);
					//else if (token_expires.empty() && is_equal("expires", key_))
					//	token_expires = str(data_ + begin, data_ + end);
					return;
				}
				if (!swift_url.empty() || begin == end)
					return;
				if (4 == path_.size() && is_equal("serviceCatalog", path_[2]) && is_equal("access", path_[1]) && is_equal("name", key_))
				{
					endpoint_name_ = make_pair(begin, end);
					check_url();
					return;
				}
				if (6 == path_.size() && is_equal("endpoints", path_[4]) && is_equal("serviceCatalog", path_[2]) && is_equal("access", path_[1]) && is_equal("publicURL", key_))
				{
					public_url_ = make_pair(begin, end);
					check_url();
					return;
				}
			}
			void key(p4 begin, p4 end)
			{
				key_ = make_pair(begin, end);
			}
			str token_id;
			//str token_expires;
			str swift_url;

		private:
			const char* data_;
			par<p4, p4> key_;

			arr<par<p4, p4>> path_;
			par<p4, p4> endpoint_name_ = make_pair(0, 0);
			par<p4, p4> public_url_ = make_pair(0, 0);

			bool is_equal(const char* s, par<p4, p4> piece)
			{
				return !strncmp(data_ + piece.first, s, piece.second - piece.first);
			}

			void check_url()
			{
				if (!endpoint_name_.first || !public_url_.first)
					return;
				if (!strncmp(data_ + endpoint_name_.first, "swift", endpoint_name_.second - endpoint_name_.first))
					swift_url = str(data_ + public_url_.first, data_ + public_url_.second);
				endpoint_name_.first = 0;
				public_url_.first = 0;
			}
		};

		sptr<CancellationToken> session::list
		(
			scope sc,
			const str& prefix,
			int limit,
			par<str, str> range,
			const str& path,
			timetrade tt,
			funk<void(object_list)> fun
		)
		{
			auto headers = arr<Header>{ Header{ "X-Auth-Token", token_ } };
			auto query = arr<par<str, str>>();

			if (scope::subdir == sc)
				query.push_back({ "delimiter","/" });

			if (!prefix.empty())
				query.push_back({ "prefix",prefix });

			if (limit)
				query.push_back({ "limit",std::to_string(limit) });

			if (!range.first.empty())
				query.push_back({ "marker",range.first });
			if (!range.second.empty())
				query.push_back({ "end_marker",range.second });

			if (!path.empty())
				query.push_back({ "path",path });

			if (timetrade::freshest == tt)
				query.push_back({ "X-Newest","true" });

			auto q = accumulate(query.cbegin(), query.cend(), str(""), [](str acc, auto item)
			{
				return acc + "&" + item.first + "=" + item.second;
			});
			return platform_.http_request(HttpVerb::GET, url_ + "/" + container_ + "?format=json" + q, headers, "", [fun](n4 sc, auto&&, str data)
			{
				if (200 != sc)
				{
					fun({});
					return;
				}
				list_parser parser(data.c_str());
				fun(object_list{ data.c_str(), move(parser.objects), move(parser.subdir) });
			});
		}

		sptr<CancellationToken> session::quick_list(const str& path, const str& marker, funk<void(arr<str>)> fun)
		{
			auto headers = arr<Header>{ Header{ "X-Auth-Token", token_ } };
			auto query = arr<par<str, str>>();

			if (!path.empty())
			{
				query.push_back({ "delimiter","/" });

				query.push_back({ "prefix",!path.empty() && '/' != *path.rbegin() ? path + "/" : path });
			}

			if (!marker.empty())
				query.push_back({ "marker",marker });

			auto q = accumulate(query.cbegin(), query.cend(), str(""), [](str acc, auto item)
			{
				return acc + "&" + item.first + "=" + item.second;
			});
			return platform_.http_request(HttpVerb::GET, url_ + "/" + container_ + "?format=plain" + q, headers, "", [fun](n4 sc, auto&&, str data)
			{
				if (200 != sc)
				{
					fun({});
					return;
				}
				fun(parse_quick_list(data));
			});
		}

		sptr<CancellationToken> session::connect(plat& platform, const str& user, const str& pwd, const str& tenant, const str& auth_url, funk<void(uptr<session>)> fun)
		{
			auto body = str("{\"auth\":{\"passwordCredentials\":{\"password\":\"" + pwd + "\",\"username\":\"" + user + "\"},\"tenantName\":\"" + tenant + "\"}}");
			return platform.http_request(HttpVerb::POST, auth_url, { { "Content-Type", "application/json" } }, body, [&platform, fun, user](n4 sc, auto&&, str data)
			{
				if (sc < 200 || 300 <= sc)
				{
					fun(nullptr);
					return;
				}

				auth_parser parser(data.c_str());
				if (parser.token_id.empty() || parser.swift_url.empty())
				{
					fun(nullptr);
					return;
				}
				fun(make_unique<session>(platform, parser.token_id, parser.swift_url, user));
			});
		}

        p16 md5_from_etag(const arr<Header>& headers)
        {
            auto etag_it = find_if(headers.cbegin(), headers.cend(), [](auto&& hv)
            {
                auto name = hv.name;
                transform(name.begin(), name.end(), name.begin(), [](char c) { return 'A' <= c && c <= 'Z' ? c + 'a' - 'A' : c; });
                return "etag" == name;
            });
            return headers.cend() != etag_it ? from_hex_16(etag_it->value.c_str()) : std::make_pair<p8,p8>(0,0);
        }
        
        sptr<CancellationToken> session::copy_file(const str& src_cloud_path, const str& dest_cloud_path, p8 /*content_length*/, funk<void(n4)> fun)
        {
            auto headers = arr<Header>
            {
                Header{ "X-Auth-Token", token_ },
                Header{ "Content-Length", "0" },
                Header{ "X-Copy-From", container_ + "/" + src_cloud_path }
            };
            
            return platform_.http_request
            (
                HttpVerb::PUT,
                url_ + "/" + container_ + "/" + dest_cloud_path,
                headers,
                "",
                [fun](n4 status_code, const arr<Header>& headers, str data)
                {
                    fun(status_code);
                }
            );
        }

		sptr<CancellationToken> session::upload_file(const str& cloud_path, const str& file_path, p16, const str& content_type, funk<void(Progress)> f_progress, funk<void(n4,n8,p16)> fun)
		{
			auto headers = arr<Header>{ Header{ "X-Auth-Token", token_ } };
            if (!content_type.empty())
                headers.push_back(Header{"Content-Type", content_type});

			//if (hash.first || hash.second)
			//	headers.push_back({ "If-None-Match", to_hex(hash) });
			//If-None-Match only supports *

			return platform_.http_upload_file
			(
				HttpVerb::PUT,
				url_ + "/" + container_ + "/" + cloud_path,
				file_path,
				headers,
				f_progress,
				[fun](n4 sc, auto& headers, n8 content_length)
                {
                    p16 md5 = md5_from_etag(headers);
                    fun(sc, content_length, md5);
                }
			);
		}

		sptr<CancellationToken> session::delete_file(const str& cloud_path, funk<void(n4)> fun)
		{
			auto headers = arr<Header>{ Header{ "X-Auth-Token", token_ } };

			return platform_.http_request
			(
				HttpVerb::REMOVE,
				url_ + "/" + container_ + "/" + cloud_path,
				headers,
				"",
				[fun](n4 sc, auto&&, auto&&)
				{
					fun(sc);
				}
			);
		}

		sptr<CancellationToken> session::download_file(const str& cloud_path, const str& file_path, n8 size, p16 hash, funk<void(Progress)> f_progress, funk<void(n4, n8, p16)> fun)
		{
			auto headers = arr<Header>{ Header{ "X-Auth-Token", token_ } };

			//if (hash.first || hash.second)
			//	headers.push_back({ "If-None-Match", to_hex(hash) });
			//If-None-Match only supports *

			return platform_.http_download_file
			(
				url_ + "/" + container_ + "/" + cloud_path,
				file_path,
				headers,
				size,
				hash,
				f_progress,
				[fun](n4 sc, auto& headers, n8 content_length)
				{
					p16 md5 = md5_from_etag(headers);
					fun(sc, content_length, md5);
				}
			);
		}
	}
}
