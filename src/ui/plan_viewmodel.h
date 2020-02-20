#pragma once
#include "viewmodel.h"
#include "plan/upgrade_viewmodel.h"
#include "../bridge/cpp/product.hpp"
#include "../bridge/cpp/store_viewmodel.hpp"
#include "../bridge/cpp/transaction_detail.hpp"
#include "../bridge/cpp/transaction_state.hpp"
#include "plan/store_interface.h"

namespace ai
{
    struct plan_viewmodel : FormViewmodel, StoreViewmodel, std::enable_shared_from_this<plan_viewmodel>
    {
        plan_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, Diagnostic& log, const std::shared_ptr<store_interface> &store)
            :
            ui_cb_(ui_cb),
            screentype_(type),
            services_(services),
            log_(log),
            main_menu_(store)
        {
            fields_ =
                center
                (
                    medium
                    (
                        {
                            fld(0).label("").idd(id_busy).light_grey(),
                        }
                    )
                );
        }

        void show()
        {
        }
        
        virtual ~plan_viewmodel()
        {
        
        }
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        Diagnostic& log_;
        
        sptr<store_interface> main_menu_;
        
        sptr<GenericForm> form_;
        sptr<FormView> view_;
        
        arr<Field> fields_;
        
        blomp_session::billing billing_;
        arr<str> product_ids_;
        bool first_load_ = true;
        bool busy_loading_billing_ = false;
        bool busy_loading_plans_ = false;
        bool busy_verifying_plan_ = false;
        bool busy_restoring_ = false;
        bool vm_registered_ = false;
        bool has_restored_subscriptions_ = false;
        
        uptr<bool> blomp_connected;
        
        hmap<str, TransactionDetail> transaction_result_;
        hmap<str, Product> product_map_;
        hmap<str, blomp_session::plan> plans_;
        
        bool product_has_purchased_transaction(const str& product_id)
        {
            auto it = transaction_result_.find(product_id);
            if (transaction_result_.end() == it)
                return false;
            return transaction_purchased(it->second.state);
        }
        
        enum class control : n4
        {
            current_plan = 1,
            upgrade,
            busy,
            refresh_plans,
            restore,
            restore_message,
            subscription_manage,
            error
        };
        
        static constexpr n4 id_plan_title_start = 1000;
        static constexpr n4 id_plan_busy_start = 2000;
        static constexpr n4 id_plan_error_start = 3000;
        static constexpr n4 id_plan_upgrade_start = 4000;
        static constexpr n4 id_plan_cancel_start = 5000;

        static constexpr n4 id_upgrade = static_cast<n4>(control::upgrade);
        static constexpr n4 id_busy = static_cast<n4>(control::busy);
        static constexpr n4 id_refresh_plans = static_cast<n4>(control::refresh_plans);
        static constexpr n4 id_restore = static_cast<n4>(control::restore);
        static constexpr n4 id_restore_message = static_cast<n4>(control::restore_message);
        static constexpr n4 id_subscription_manage = static_cast<n4>(control::subscription_manage);
        static constexpr n4 id_error = static_cast<n4>(control::error);

        std::vector<Field> fields() final
        {
            return fields_;
        }

        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view_ = view;
            view->set_screen_name("Plan");
            
            if (first_load_)
            {
                first_load_ = false;
                refresh_blomp_account_detail();
                return;
            }
            
            update_transaction_states();
            if (blomp_connected && !(*blomp_connected))
            {
                form_->enable(-1, false);
                form_->error(-1, "Not connected to Blomp");
            }
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            form_->error(element_id, "");
            
            if (id_plan_upgrade_start <= element_id && element_id < id_plan_upgrade_start + product_ids_.size())
            {
                int idd = element_id - id_plan_upgrade_start;
                form_->input_set(idd + id_plan_busy_start, "Contacting Store...");
//                assert(!billing_.email.empty());
                main_menu_->subscription_purchase(billing_.email, product_ids_[idd]);
                return;
            }
            
            if (id_plan_cancel_start <= element_id && element_id < id_plan_cancel_start + product_ids_.size())
            {
                int idd = element_id - id_plan_cancel_start;
                main_menu_->subscription_cancel(product_ids_[idd]);
                return;
            }
            
            switch (static_cast<control>(element_id))
            {
                case control::restore:
                {
                    form_->enable(id_restore, false);
                    form_->enable(id_restore_message, false);
                    
//                    assert(!billing_.email.empty());
                    busy_restoring_ = true;
                    main_menu_->subscription_restore(billing_.email);
                    break;
                }
                case control::subscription_manage:
                {
                    main_menu_->subscription_manage();
                    break;
                }
//                case control::refresh_plans:
//                {
//                    refresh();
//                    break;
//                }
//                case control::upgrade:
//                {
//                    if (!main_menu_->is_authorized_for_fayments())
//                    {
//                        form_->input_set(id_busy, "You are not currently able to make payments. Please ensure you are signed in to the App Store");
//                        return;
//                    }
//                    form_->input_set(id_busy, "Fetching plans...");
//                    break;
//                }
                default: break;
            }
        }
        
        void back() final
        {
            
        }

        str busy_message()
        {
            if (!busy_loading_plans_ && !busy_restoring_ && !busy_loading_billing_ && !busy_verifying_plan_)
                return "";
            
            str message = "Busy";
            if (busy_loading_plans_)
                message += " fetching current available plans,";
            if (busy_loading_billing_)
                message += " fetching Blomp account detail,";
            if (busy_restoring_)
                message += " verifying subscriptions with store,";
            if (busy_verifying_plan_)
                message += " verifying plan with Blomp,";
            return message;
        }
        
        void update_busy_indicator()
        {
            if (!form_)
                return;
            
            form_->input_set(id_busy, busy_message());
        }
        
        void refresh_blomp_account_detail()
        {
            if (busy_loading_plans_ || busy_loading_billing_)
                return;
            
            busy_loading_plans_ = true;
            
            services_.plans([ui_cb = ui_cb_, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())](auto status, auto plans)
            {
                run_ui(ui_cb, [ui_cb, wself, plans, status]()
                {
                    auto sself = wself.lock();
                    if (!sself) return;
                    auto& vm = *sself;
                    
                    vm.busy_loading_plans_ = false;
                    vm.update_busy_indicator();
                    
                    if (plans.empty())
                        return;
                    
                    auto product_ids = arr<str>();
                    std::for_each(plans.begin(), plans.end(), [&vm, &product_ids](auto&& plan)
                    {
                        if (plan.product_id.empty())
                            return;
                        product_ids.push_back(plan.product_id);
                        vm.plans_[plan.product_id] = plan;
                    });
                    
                    vm.product_ids_ = product_ids;
                    if (!vm.vm_registered_)
                    {
                        vm.vm_registered_ = true;
                        vm.main_menu_->register_store_vm(sself);
                    }

                    vm.main_menu_->fetch_products(product_ids);
                });

            });

            load_billing();
            
            update_busy_indicator();
        }

        void load_billing()
        {
            busy_loading_billing_ = true;
            
            services_.get_billing([ui_cb = ui_cb_, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())](auto status, auto billing)
            {
                run_ui(ui_cb, [wself, status, billing]()
                {
                    auto sself = wself.lock();
                    if (!sself) return;
                    auto& vm = *sself;

                    vm.busy_loading_billing_ = false;
                    vm.update_busy_indicator();
                    
                    switch (status)
                    {
                    case http_status::success: vm.billing_ = billing; break;
                    case http_status::blomp_session: vm.form_->error(-1, "Not connected to Blomp"); vm.blomp_connected = std::make_unique<bool>(false); return;
                    default: vm.form_->error(-1, "Failed to fetch billing information from Blomp"); vm.blomp_connected = std::make_unique<bool>(false); return;
                    }
                    
                    vm.blomp_connected = std::make_unique<bool>(true);

                    if (!vm.vm_registered_)
                    {
                        vm.vm_registered_ = true;
                        vm.main_menu_->register_store_vm(sself);
                    }
                    if (!vm.billing_.email.empty() && !vm.has_restored_subscriptions_)
                    {
                        vm.busy_restoring_ = true;
                        vm.main_menu_->subscription_restore(vm.billing_.email);
                    }

                    vm.render();

                });
            });
        }
        
        static bool transaction_purchased(TransactionState state) { return TransactionState::PURCHASED == state || TransactionState::RESTORED == state; }
        
        bool can_restore()
        {
            return true;
//            if (billing_.plan.empty() || product_map_.empty())
//                return false;
//
//            auto billed_product_it = std::find_if(product_map_.begin(), product_map_.end(), [this](auto&& product_pair)
//            {
//                return product_pair.first == billing_.product_id;
//            });
//            if (product_map_.end() != billed_product_it)
//                return false;
//
//            auto purchased_transaction_it = std::find_if(transaction_result_.begin(), transaction_result_.end(), [](auto&& detail)
//            {
//                return transaction_purchased(detail.second.state);
//            });
//            return transaction_result_.end() == purchased_transaction_it;
        }
        
        void render()
        {
        
            int row = 0;
            fields_.clear();
            if (!billing_.plan.empty())
            {
                auto product_it = product_map_.find(billing_.product_id);
                str plan_description = product_map_.end() != product_it ? product_it->second.title : billing_.plan;
                auto fs =
                    center
                    (
                        medium
                        (
                            {
                                fld(row++).label(busy_message()).idd(id_busy).light_grey(),
                                fld(row++).label("").idd(id_error).red(),
                                fld(row++).vspace(),
                                fld(row++).label("Current Plan").large().bold(),
                                fld(row++).label(billing_.email).medium(),
                                fld(row++).label(plan_description).large().purple(),
                                fld(row++).label("Billing Date").small(),
                                fld(row++).label(billing_.billing_date).medium(),
                                fld(row++).vspace(),
                                fld(row++).vspace(),
                            }
                        )
                    );
                std::copy(fs.begin(), fs.end(), std::back_inserter(fields_));
            }
            else
            {
                services_.log(__LINE__, __FILE__, "billing plan empty");
            }
            
            bool can_pay = main_menu_->is_authorized_for_payments();
            if (!can_pay)
                fields_.push_back(fld(row++).label("Unable to purchase subscriptions").red().center());
            auto plan_count = std::accumulate(product_ids_.begin(), product_ids_.end(), 0, [this, &row, idd = 0, can_pay](int count, const str& product_id) mutable
            {

                auto it = product_map_.find(product_id);
                if (product_map_.end() == it)
                {
                    ++idd;
                    services_.log(__LINE__, __FILE__, "product_id not found [" + product_id + "]");

                    return count;
                }
                
                auto& r = fields_;
                if (0 == count)
                {
                    form_->input_set(id_busy, "");

                    r.push_back(fld(row++).label("Available Plans").large().bold().center());
                    if (!can_pay)
                    {
                        r.push_back(fld(row++).label("You are not currently able to make payments. Please ensure you are signed in to the App Store").small().red().center());
                    }
                    r.push_back(fld(row++).vspace());
                    r.push_back(fld(row++).vspace());
                }

                auto& plan = it->second;
                
                r.push_back(fld(row++).label("").center().medium().red().idd(idd + id_plan_error_start));
                r.push_back(fld(row++).label("").center().medium().light_grey().idd(idd + id_plan_busy_start));
                r.push_back(fld(row++).label(plan.title).large().center().purple());
                r.push_back(fld(row++).label(plan.product_description).center().small());
                r.push_back(fld(row++).label(plan.formatted_price).center().large().bold());
//                r.push_back(fld(row++).label("per month").small().center());
                services_.log(__LINE__, __FILE__, product_id);
                if (can_pay)
                {
                    if (product_id == billing_.product_id)
                    {
                        r.push_back(fld(row++).label("Verified").medium().center().purple());
                        r.push_back(fld(row++).button(idd + id_plan_cancel_start, "Cancel " + plan.title).center().medium().w(-300));
                    }
                    else
                    {
                        if (product_has_purchased_transaction(product_id))
                        {
                            r.push_back(fld(row++).button(idd + id_plan_cancel_start, "Cancel " + plan.title).center().medium().w(-300));
//                            r.push_back(fld(row++).label("Verifying purchase with Blomp").center().purple());
                        }
                        else if (!busy_restoring_)
                        {
                            r.push_back(fld(row++).button(idd + id_plan_upgrade_start, "Upgrade to " + plan.title).center().medium().w(-300));
                        }
                        else
                        {
                            r.push_back(fld(row++).label(plan.title + " Restoring...").medium().center().light_grey());
                            services_.log(__LINE__, __FILE__, "no button [" + product_id + "]");
                        }
                    }
                }
                r.push_back(fld(row++).vspace().large());
                r.push_back(fld(row++).vspace().large());
                
                ++idd;
                return count + 1;
            });
            
            if (0 == plan_count && !busy_loading_plans_)
                fields_.push_back(fld(row++).label("No Available Plans").large().bold().center());

            if (can_restore())
            {
            
                fields_.push_back(fld(row++).idd(id_restore_message).label("If your purchase is not reflecting in the subscription detail please click the restore button").medium().center().w(-500));
                fields_.push_back(fld(row++).button(id_restore, "Restore").medium().center().w(-300));
            }
            else
            {
                services_.log(__LINE__, __FILE__, "cannot restore");
            }
            
            fields_.push_back(fld(row++).idd(id_restore_message).label("Downgrade or cancel your subscriptions, if you have any.").medium().center().w(-500));
            fields_.push_back(fld(row++).button(id_subscription_manage, "Manage Subscriptions").medium().center().w(-300));

            view_->replace_form(shared_from_this());
        }
        
        void on_transaction_states(const std::unordered_map<std::string, TransactionDetail> & states) final
        {
            services_.log_transactions(states);
            run_ui(ui_cb_, [states, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())]()
            {
                auto sself = wself.lock();
                if (!sself) return;
                auto& vm = *sself.get();

                vm.transaction_result_.clear();
                std::for_each(states.begin(), states.end(), [&vm](auto&& state_pair)
                {
                    auto it = vm.transaction_result_.find(state_pair.first);
                    if (vm.transaction_result_.end() == it)
                        vm.transaction_result_.insert(state_pair);
                    else
                        it->second = state_pair.second;
                });
//                vm.update_transaction_states();
                vm.busy_restoring_ = false;
                vm.render();
            });
        }

        void on_restore_finished() final
        {
            services_.log(__LINE__, __FILE__, "on_restore_finished");

            run_ui(ui_cb_, [wself = std::weak_ptr<plan_viewmodel>(shared_from_this())]()
            {
                auto sself = wself.lock();
                if (!sself) return;
                auto& vm = *sself;

                vm.has_restored_subscriptions_ = true;
                if (!vm.form_)
                    return;

                vm.form_->input_set(id_busy, "");
                vm.form_->enable(id_restore, true);
                vm.form_->enable(id_restore_message, true);
                if (vm.busy_restoring_)
                {
                    vm.busy_restoring_ = false;
                    vm.render();
                }
            });
        }
        
        void on_restore_fail(const std::string & error_message) final
        {
            services_.log(__LINE__, __FILE__, "Subscription restore failed: " + error_message);
            
            run_ui(ui_cb_, [error_message, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())]() mutable
            {
                auto sself = wself.lock();
                if (!sself) return;
                auto& vm = *sself;

                vm.has_restored_subscriptions_ = true;
                if (!vm.form_)
                    return;

                vm.form_->input_set(id_restore_message, error_message);
                vm.busy_restoring_ = false;
                vm.form_->input_set(id_busy, "");
                vm.form_->enable(id_restore, true);
                vm.form_->enable(id_restore_message, true);
            });
        }

        void on_products(const std::vector<Product> & products) final
        {
            services_.log_products(products);
            run_ui(ui_cb_, [products, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())]() mutable
            {
                auto sself = wself.lock();
                if (!sself) return;
                auto& vm = *sself;

                vm.services_.log(__LINE__, __FILE__, std::to_string(products.size()));
                std::for_each(products.begin(), products.end(), [&vm](const Product& product)
                {
                    vm.product_map_.insert(std::make_pair(product.product_id, product));
                });
                
                vm.render();
            });
        }

        void update_transaction_states()
        {
            if (product_map_.empty() || !form_)
            {
                return;
            }
            
            if (!busy_verifying_plan_)
            {
                int purchase_count = std::accumulate(product_ids_.begin(), product_ids_.end(), 0, [this](int purchase_count, const str& product_id)
                {
                    return product_id != billing_.product_id && product_has_purchased_transaction(product_id) ? purchase_count + 1 : purchase_count;
                });
                if (1 < purchase_count)
                {
                    form_->input_set(id_error, "You have purchased more than one subscription. You should click the Manage Subscriptions button below and cancel the unneeded subscriptions");
                }
                else if (1 == purchase_count)
                {
                    auto render_needed_it = std::find_if(product_ids_.begin(), product_ids_.end(), [this](const str& product_id)
                    {
                        return product_id != billing_.product_id && product_has_purchased_transaction(product_id);
                    });

                    assert(product_ids_.end() != render_needed_it);
                    str product_id = *render_needed_it;
                    auto transaction_it = transaction_result_.find(product_id);
                    assert(transaction_result_.end() != transaction_it);
                    auto product_it = product_map_.find(product_id);
                    assert(product_map_.end() != product_it);
                    auto plan_it = plans_.find(product_id);
                    assert(plans_.end() != plan_it);
                    
                    auto receipt = main_menu_->subscription_receipt(*render_needed_it);
                    if (receipt.empty())
                    {
                        transaction_it->second.state = TransactionState::ERROR;
                        transaction_it->second.error_message = "No receipt available";
                    }
                    else
                    {
                        busy_verifying_plan_ = true;

                        transaction_it->second.state = TransactionState::VERIFY_BLOMP;
                        services_.blomp_validate_receipt(plan_it->second.name, std::move(receipt), [product_id, ui_cb = ui_cb_, wself = std::weak_ptr<plan_viewmodel>(shared_from_this())](auto status)
                        {
                            run_ui(ui_cb, [status, product_id, wself]()
                            {
                                auto sself = wself.lock();
                                if (!sself) return;
                                auto& vm = *sself;

                                vm.busy_verifying_plan_ = false;
                                auto transaction_it = vm.transaction_result_.find(product_id);
                                if (vm.transaction_result_.end() != transaction_it)
                                {
                                    if (http_status::success != status)
                                    {
                                        vm.services_.log(__LINE__, __FILE__, status_to_string(status));
                                        transaction_it->second.state = TransactionState::ERROR;
                                        transaction_it->second.error_message = "Failed to verify purchase with Blomp";
                                    }
                                    else
                                    {
                                        transaction_it->second.state = TransactionState::PURCHASED;
                                        vm.load_billing();
                                    }
                                }

                                vm.update_busy_indicator();
                            });

                        });
                        render();
                        return;
                    }
                }
            }
            
            form_->enable(id_restore, true);
            form_->enable(id_restore_message, true);

            std::for_each(product_ids_.begin(), product_ids_.end(), [this, idd = 0](const str& product_id) mutable
            {
                auto it = transaction_result_.find(product_id);
                if (transaction_result_.end() == it)
                {
                    ++idd;
                    return;
                }
                
                switch (it->second.state)
                {
                    case TransactionState::ERROR:
                    {
                        form_->input_set(idd + id_plan_busy_start, "");
                        form_->input_set(idd + id_plan_error_start, it->second.error_message);
                        break;
                    }
                    default:
                    {
                        form_->input_set(idd + id_plan_error_start, "");
                        switch (it->second.state)
                        {
                            case TransactionState::BUSY:
                            {
                                form_->input_set(idd + id_plan_busy_start, "Purchasing...");
                                break;
                            }
                            case TransactionState::PURCHASED:
                            {
                                form_->input_set(idd + id_plan_busy_start, "Purchased");
                                break;
                            }
                            case TransactionState::RESTORED:
                            {
                                form_->input_set(idd + id_plan_busy_start, "Purchased");
                                break;
                            }
                            case TransactionState::VERIFY_BLOMP:
                            {
                                form_->input_set(idd + id_plan_busy_start, "Updating account at Blomp...");
                                break;
                            }
                            default: break;
                        }
                        break;
                    }
                }
                ++idd;
            });
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return true; }

    };
}
