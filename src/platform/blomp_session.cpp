#include "blomp_session.h"
#include "../json.h"
#include "../bridge/cpp/header.hpp"
#include "../zz_algorithms.h"

namespace ai
{
    auto validate_receipt_path = "/users/payments/apple";
//    auto validate_receipt_path = "/users/payments/test";
    auto connect_path = "/users/tokens";
    auto plans_path = "/blomp/plans";
    auto signup_path = "/users/register";
    auto is_forum_name_availible_path = "/users/forum/availability/";
    auto change_password_path = "/users/password/change/";
    auto change_forumname_path = "/users/forum/username/change/";
    auto get_billing_path = "/users/billing/";
    auto update_profile_path = "/users/profile/";
    auto get_profile_path = "/users/profile/";

    struct key_value_parser
    {
        key_value_parser(const char* data)
        {
            data_ = data;
            json::parse_value(data, json::parse_space(data, 0), *this);
        }
        void begin_array()
        {
            key_ = std::make_pair(0, 0);
        }
        void end_array()
        {
        }
        void begin_object()
        {
            key_ = std::make_pair(0, 0);
        }
        void end_object()
        {
        }
        void text(json::type type, p4 begin, p4 end)
        {
            str key = make_str(key_);
            switch (type)
            {
            case json::type::nul: properties_[key] = ""; break;
            case json::type::text: properties_[key] = unescape_json(make_str(begin, end)); break;
            default: properties_[key] = make_str(begin, end);
            }
        }
        void key(p4 begin, p4 end)
        {
            key_ = std::make_pair(begin, end);
        }
        str value(const str& key) { return properties_[key]; }
    private:
        const char* data_;
        par<p4, p4> key_;
        hmap<str, str> properties_;
        
        bool is_equal(const char* s, par<p4, p4> piece)
        {
            return !strncmp(data_ + piece.first, s, piece.second - piece.first);
        }
        
        str make_str(par<p4, p4> segment)
        {
            return str(data_ + segment.first, data_ + segment.second);
        }
        str make_str(p4 b, p4 e)
        {
            return str(data_ + b, data_ + e);
        }
    };

    struct plan_parser
    {
        plan_parser(const char* data)
        {
            data_ = data;
            json::parse_value(data, json::parse_space(data, 0), *this);
        }
        void begin_array()
        {
            key_ = std::make_pair(0, 0);
        }
        void end_array()
        {
        }
        void begin_object()
        {
            key_ = std::make_pair(0, 0);
        }
        void end_object()
        {
            blomp_session::plan x;
            x.name = properties_["name"];
            x.size = properties_["size"];
            x.price = properties_["price"];
            x.features = properties_["features"];
            x.requirements = properties_["requirements"];
            x.product_id = properties_["product_id"];
            plans_.push_back(x);
            properties_.clear();
        }
        void text(json::type type, p4 begin, p4 end)
        {
            str key = make_str(key_);
            switch (type)
            {
            case json::type::nul: properties_[key] = ""; break;
            case json::type::text: properties_[key] = unescape_json(make_str(begin, end)); break;
            default: properties_[key] = make_str(begin, end);
            }

//            properties_[make_str(key_)] = json::type::nul == type ? "" : make_str(begin, end);
        }
        void key(p4 begin, p4 end)
        {
            key_ = std::make_pair(begin, end);
        }
        arr<blomp_session::plan> plans() const { return plans_; }

    private:
        const char* data_;
        par<p4, p4> key_;
        hmap<str, str> properties_;
        arr<blomp_session::plan> plans_;
        
        bool is_equal(const char* s, par<p4, p4> piece)
        {
            return !strncmp(data_ + piece.first, s, piece.second - piece.first);
        }
        
        str make_str(par<p4, p4> segment)
        {
            return str(data_ + segment.first, data_ + segment.second);
        }
        str make_str(p4 b, p4 e)
        {
            return str(data_ + b, data_ + e);
        }
    };

    sptr<CancellationToken> blomp_session::connect(plat& platform, const str& email, const str& pwd, funk<void(http_status, uptr<blomp_session>)> fun)
    {
        auto body = str("{\"email\":\"" + escape_json(email) + "\",\"password\":\"" + escape_json(pwd) + "\"}");
        return platform.http_request(HttpVerb::POST, base_url() + connect_path, {}, body, [&platform, email, fun](n4 sc, auto&& headers, str data)
        {
            if (!http_success(sc))
            {
                fun(http_status_from(sc), nullptr);
                return;
            }

            key_value_parser parser(data.c_str());
            auto token = parser.value("token");
            if (token.empty())
            {
                fun(http_status::not_found, nullptr);
                return;
            }
            fun(http_status::success, std::make_unique<blomp_session>(platform, email, token));
        });

        /*
         
         POST https://api.blomp.com/users/tokens HTTP/1.1
         User-Agent: Fiddler
         Host: api.blomp.com
         Content-Length: 63
         
         {
         "email":"kleutervreter@gmail.com",
         "password":"password"
         }
         
         */
        
    }

    sptr<CancellationToken> blomp_session::plans(plat& platform, funk<void(http_status, arr<plan>)> fun)
    {
        return platform.http_request(HttpVerb::GET, base_url() + plans_path, {}, "", [&platform, fun](n4 sc, const arr<Header>& headers, str data)
        {
            if (!http_success(sc))
            {
                fun(http_status_from(sc), {});
                return;
            }

            plan_parser parser(data.c_str());
            
            fun(http_status_from(sc), parser.plans());
        });
    }

    sptr<CancellationToken> blomp_session::signup(plat& platform, str email, str name, str forum_name, str pwd, funk<void(http_status, str)> fun)
    {
        auto headers = arr<Header>{ Header{ "app-name", "BlompLive" } };
        
        str body = "{\"email\":\""+escape_json(email)+"\",\"password\":\""+escape_json(pwd)+"\",\"name\":\""+escape_json(name)+"\",\"forum_username\":\""+escape_json(forum_name)+"\"}";
        
        return platform.http_request(HttpVerb::POST, base_url() + signup_path, headers, body, [&platform, fun](n4 sc, auto&& headers, str data)
        {
            if (!data.empty() && !http_success(sc))
            {
                key_value_parser parser(data.c_str());
                data = parser.value("message");
            }
            fun(http_status_from(sc), data);
        });
    }
    
    sptr<CancellationToken> blomp_session::validate_receipt(plat& platform, str email, str size, str receipt_data, funk<void(http_status)> fun)
    {
        str body = "{\"email\":\""+escape_json(email)+"\",\"plan\":\""+escape_json(size)+"\",\"receipt-data\":\""+escape_json(receipt_data)+"\"}";

        return platform.http_request(HttpVerb::POST, base_url() + validate_receipt_path, {}, body, [&platform, fun](n4 sc, auto&& headers, str data)
        {
            fun(http_status_from(sc));
        });
    }

    sptr<CancellationToken> blomp_session::is_forum_name_availible(plat& platform, str forum_name, funk<void(http_status, str)> fun)
    {
        /*
         GET https://api.blomp.com/users/forum/availability/vitomir HTTP/1.1
         User-Agent: Fiddler
         Host: api.blomp.com
         Content-Length: 0
         

         */
        return platform.http_request(HttpVerb::GET, base_url() + is_forum_name_availible_path + forum_name, {}, "", [&platform, fun](n4 sc, auto&& headers, str data)
        {
            if (!http_success(sc))
            {
                fun(http_status_from(sc), "Not available");
                return;
            }

            key_value_parser parser(data.c_str());

            str message = parser.value("message");
            fun(http_status_from(sc), message);
        });

    }

    sptr<CancellationToken> blomp_session::change_password(str old_pwd, str new_pwd, funk<void(http_status)> fun)
    {
        auto headers = arr<Header>{ Header{ "token", token_ } };
        str body = "{\"current_password\":\""+escape_json(old_pwd)+"\",\"new_password\":\""+escape_json(new_pwd)+"\",\"repeat_password\":\""+escape_json(new_pwd)+"\"}";

        return platform_.http_request(HttpVerb::POST, base_url() + change_password_path + email_, headers, body, [&platform = platform_, fun](n4 sc, auto&& headers, str data)
        {
            if (!http_success(sc))
            {
                fun(http_status_from(sc));
                return;
            }

            fun(http_status::success);
        });
    }

    sptr<CancellationToken> blomp_session::change_forumname(str name, funk<void (http_status)> fun)
    {
        auto headers = arr<Header>{ Header{ "token", token_ } };
        str body = "{\"username\":\""+escape_json(name)+"\"}";
        return platform_.http_request(HttpVerb::POST, base_url() + change_forumname_path + email_, headers, body, [&platform = platform_, fun](n4 sc, auto&& headers, str data)
        {
            if (!http_success(sc))
            {
                fun(http_status_from(sc));
                return;
            }

            fun(http_status::success);
        });
    }

    sptr<CancellationToken> blomp_session::get_billing(funk<void(http_status, billing)> fun)
    {
        auto headers = arr<Header>{ Header{ "token", token_ } };
        
        return platform_.http_request(HttpVerb::GET, base_url() + get_billing_path + email_, headers, "", [&platform = platform_, fun](n4 sc, auto&& headers, str data)
        {
            billing x;
            if (!http_success(sc))
            {
                fun(http_status_from(sc), x);
                return;
            }

            key_value_parser parser(data.c_str());
            x.billing_date = parser.value("billing_date");
            x.plan = parser.value("plan");
            x.email = parser.value("email");
            x.product_id = parser.value("product_id");
            x.idd = to_integer<n4>(parser.value("id"));
            fun(http_status::success, x);
        });
    }

    sptr<CancellationToken> blomp_session::update_profile(str name, str forum_name, str address, str phone, str company, funk<void(http_status)> fun)
    {
        auto headers = arr<Header>{ Header{ "token", token_ } };
        str body = "{\"name\":\""+escape_json(name)+"\",\"address\":\""+escape_json(address)+"\",\"phone\":\""+escape_json(phone)+"\",\"company\":\""+escape_json(company)+"\"}";
        return platform_.http_request(HttpVerb::POST, base_url() + update_profile_path + email_, headers, body, [&platform = platform_, fun](n4 sc, auto&& headers, str data)
        {
            fun(http_status_from(sc));
        });

    }

    sptr<CancellationToken> blomp_session::get_profile(funk<void(http_status, user_profile)> fun)
    {
        auto headers = arr<Header>{ Header{ "token", token_ } };

        return platform_.http_request(HttpVerb::GET, base_url() + get_profile_path + email_, headers, "", [&platform = platform_, fun](n4 sc, auto&& headers, str data)
        {
            user_profile x;
            if (!http_success(sc))
            {
                fun(http_status_from(sc), x);
                return;
            }

            key_value_parser parser(data.c_str());

            x.user_id = to_integer<n4>(parser.value("id"));
            x.address = parser.value("address");
            x.email = parser.value("email");
            x.name = parser.value("name");
            x.phone = parser.value("phone");
            x.company = parser.value("company");
            x.forum_username = parser.value("forum username");
            x.registration_date = parser.value("registration date");
            x.confirmed = parser.value("confirmed") == "true";
            fun(http_status::success, x);
        });
    }
    
    str blomp_session::base_url() { return "https://api.blomp.com"; }
}
