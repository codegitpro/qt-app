#pragma once
#include "general.h"
#include "bridge/cpp/http_verb.hpp"
#include "bridge/cpp/header.hpp"

namespace ai
{
	static str to_str(HttpVerb v)
	{
		switch (v)
		{
		case HttpVerb::GET: return "GET";
		case HttpVerb::REMOVE: return "DELETE";
		case HttpVerb::OPTIONS: return "OPTIONS";
		case HttpVerb::PATCH: return "PATCH";
		case HttpVerb::POST: return "POST";
		case HttpVerb::PUT: return "PUT";
		}
		return "UNKNOWN";
	}

	namespace http
	{
		struct url
		{
			str schema;
			str authority;
			str host;
			p2 port;
			str path;
		};

		static url parse(const str& u)
		{
			url r;

			constexpr auto schema_m = "://";
			auto schema_e = std::search(u.cbegin(), u.cend(), schema_m, schema_m + 3);
			assert(u.cend() != schema_e);

			r.schema = str(u.cbegin(), schema_e);

			schema_e += 3;
			auto auth_e = std::find_if(schema_e, u.cend(), [](char c) { return '/' == c || '?' == c || '#' == c; });
			r.path = str(auth_e, u.cend());

			auto authority = str(schema_e, auth_e);
			r.authority = authority;

			auto host_e = std::find(authority.cbegin(), authority.cend(), ':');
			r.port = "https" == r.schema ? 443 : 80;
			if (authority.cend() != host_e)
				r.port = static_cast<p2>(std::stoi(str(host_e + 1, authority.cend())));
			r.host = str(authority.cbegin(), host_e);

			return r;
		}

		enum class connection_state
		{
			keepalive,
			close
		};

		static str header_start(HttpVerb v, const str& url, const arr<Header> headers, connection_state state)
		{
			auto u = http::parse(url);
			auto r = to_str(v) + " " + u.path + " HTTP/1.1\r\n";
			r += "Host: " + u.host + "\r\n";
			r += str("Connection: ") + (connection_state::keepalive == state ? "keep-alive\r\n" : "close\r\n");
			r += std::accumulate(headers.cbegin(), headers.cend(), str(""), [](str acc, auto&& header)
			{
				return acc + header.name + ": " + header.value + "\r\n";
			});

			return r;
		}

		static str header(HttpVerb v, const str& url, const arr<Header> headers, str body, connection_state state = connection_state::keepalive)
		{
			auto r = header_start(v, url, headers, state);
			if (HttpVerb::GET != v && HttpVerb::REMOVE != v && HttpVerb::OPTIONS != v)
			{
				r += "Content-Length: " + std::to_string(body.size()) + "\r\n";
				r += "\r\n";
				r += body;
			}
			else
				r += "\r\n";
			return r;
		}

		static str header(HttpVerb v, const str& url, const arr<Header> headers, n8 content_length, connection_state state = connection_state::keepalive)
		{
			auto r = header_start(v, url, headers, state);
			r += "Content-Length: " + std::to_string(content_length) + "\r\n";
			r += "\r\n";
			return r;
		}
	}
}