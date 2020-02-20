#pragma once
#include "plat.h"

namespace ai
{
    enum class http_status
    {
        success,
        fail,
        not_found,
        authz,
        authn,
        bad_request,
        internal_server,
        blomp_session
    };
    
    static str status_to_string(http_status status)
    {
        switch (status)
        {
        case http_status::success: return "success";
        case http_status::fail: return "fail";
        case http_status::not_found: return "not found";
        case http_status::authz: return "authz";
        case http_status::authn: return "authn";
        case http_status::bad_request: return "bad request";
        case http_status::internal_server: return "internal server";
        case http_status::blomp_session: return "blomp session";
        }
        return "unknown";
    }
    
    static http_status http_status_from(n4 code)
    {
        if (200 <= code && code < 300)
            return http_status::success;
        if (500 <= code && code < 600)
            return http_status::internal_server;
        
        switch (code)
        {
            case 401: return http_status::authn;
            case 403: return http_status::authz;
            case 404: return http_status::not_found;
            case 400: return http_status::bad_request;
            default: return http_status::fail;
        }
    }
    
    static bool http_success(n4 code) { return 200 <= code && code < 300; }
    
    struct blomp_session
    {
        struct user_profile
        {
            /*
             HTTP/1.1 200 OK
             Server: nginx/1.14.0 (Ubuntu)
             Date: Mon, 17 Jun 2019 21:09:54 GMT
             Content-Type: application/json
             Content-Length: 215
             Connection: keep-alive
             
             {"id": 566, "email": "kleutervreter@gmail.com", "name": "Piet Pompies", "address": null, "phone": null, "company": null, "forum username": "", "registration date": "November 27, 2018 at 17:04:30", "confirmed": true}
             */
            n4 user_id;
            str email;
            str name;
            str address;
            str phone;
            str company;
            str forum_username;
            str registration_date;
            bool confirmed;
        };
        
        struct billing
        {
            n4 idd;
            str plan;
            str billing_date;
            str email;
            str product_id;
        };
        
        struct plan
        {
            str features;
            str price;
            str requirements;
            str size;
            str name;
            str product_id;
        };
        
        static sptr<CancellationToken> connect(plat&, const str& email, const str& pwd, funk<void(http_status, uptr<blomp_session>)> fun);
        blomp_session(plat& platform, const str& email, const str& token) : platform_(platform), email_(email), token_(token)
        {
        }
        
        sptr<CancellationToken> get_profile(funk<void(http_status, user_profile)> fun);
        sptr<CancellationToken> get_billing(funk<void(http_status, billing)> fun);
        sptr<CancellationToken> update_profile(str name, str forum_name, str address, str phone, str company, funk<void(http_status)> fun);
        sptr<CancellationToken> change_password(str old_pwd, str new_pwd, funk<void(http_status)> fun);
        sptr<CancellationToken> change_forumname(str name, funk<void(http_status)> fun);

        static sptr<CancellationToken> validate_receipt(plat&, str email, str size, str receipt_data, funk<void(http_status)>);
        static sptr<CancellationToken> plans(plat&, funk<void(http_status, arr<plan>)> fun);
        static sptr<CancellationToken> is_forum_name_availible(plat&, str forum_name, funk<void(http_status, str)> fun);
        static sptr<CancellationToken> signup(plat&, str email, str name, str forum_name, str pwd, funk<void(http_status, str)> fun);
    private:
        plat& platform_;
        str email_;
        str token_;
        
        static str base_url();
    };
}
