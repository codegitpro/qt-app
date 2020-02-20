#pragma once
#include "store_interface.h"
#include "../viewmodel.h"
#include "../../bridge/cpp/view.hpp"

namespace ai
{
    struct store_proxy : store_interface
    {
        store_proxy(services& services, const std::shared_ptr<View>& view)
            :
            services_(services),
            view_(view)
        {
        }
    private:
        services& services_;
        std::shared_ptr<View> view_;

        void register_store_vm(const std::shared_ptr<StoreViewmodel> & vm) final
        {
            services_.log(__LINE__, __FILE__, "register_store_vm");
            view_->register_store_vm(vm);
        }
        
        bool is_authorized_for_payments() final
        {
            auto r = view_->is_authorized_for_payments();
            services_.log(__LINE__, __FILE__, r ? "authorized for payments" : "not authorized for payments");
            return r;
        }

        void fetch_products(const std::vector<std::string> & product_ids) final
        {
            auto ids = arr<str>(product_ids);
            ids.insert(ids.begin(), "fetch_products");
            services_.log(__LINE__, __FILE__, ids);
            view_->fetch_products(product_ids);
        }

        void subscription_purchase(const std::string & email, const std::string & product_id) final
        {
            services_.log(__LINE__, __FILE__, {"subscription_purchase", email, product_id});
            view_->subscription_purchase(email, product_id);
        }

        void subscription_restore(const std::string & email) final
        {
            services_.log(__LINE__, __FILE__, {"subscription_restore", email});
            view_->subscription_restore(email);
        }

        void subscription_cancel(const std::string & product_id) final
        {
            services_.log(__LINE__, __FILE__, {"subscription_cancel", product_id});
            view_->subscription_cancel(product_id);
        }

        std::string subscription_receipt(const std::string & product_id) final
        {
            services_.log(__LINE__, __FILE__, {"subscription_receipt", product_id});
            return view_->subscription_receipt(product_id);
        }
        
        void subscription_manage() final
        {
            services_.log(__LINE__, __FILE__, "subscription_manage");
            view_->subscription_manage();
        }
    };
}
