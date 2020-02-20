#pragma once
#include "../../general.h"

namespace ai
{
    struct store_interface
    {
        virtual ~store_interface() {}
        
        virtual void register_store_vm(const std::shared_ptr<StoreViewmodel> & vm) = 0;
        
        virtual bool is_authorized_for_payments() = 0;

        virtual void fetch_products(const std::vector<std::string> & product_ids) = 0;

        virtual void subscription_purchase(const std::string & email, const std::string & product_id) = 0;

        virtual void subscription_restore(const std::string & email) = 0;

        virtual void subscription_cancel(const std::string & product_id) = 0;

        virtual std::string subscription_receipt(const std::string & product_id) = 0;
        
        virtual void subscription_manage() = 0;
    };
}
