#pragma once
#include "../viewmodel.h"
#include "../../directory.h"
#include "../../bridge/cpp/product.hpp"


namespace ai
{
    struct upgrade_viewmodel : FormViewmodel, std::enable_shared_from_this<upgrade_viewmodel>
    {
        upgrade_viewmodel(const sptr<UiCallback>& ui_cb, ScreenType type, services& services, const sptr<View>& main_menu, arr<Product> plans)
        :
        ui_cb_(ui_cb),
        screentype_(type),
        services_(services),
        main_menu_(main_menu),
        plans_(plans)
        {
        }
        virtual ~upgrade_viewmodel()
        {
        }
        
    private:
        sptr<UiCallback> ui_cb_;
        ScreenType screentype_;
        services& services_;
        sptr<View> main_menu_;
        
        arr<Product> plans_;
        
        sptr<GenericForm> form_;
        
        enum class control : n4
        {
            name = 1,
        };
        
        static constexpr n4 id_name = static_cast<n4>(control::name);
        
        
        std::vector<Field> fields() final
        {
            int row = 0;
            auto r =
                center
                (
                    medium
                    (
                        {
                            fld(row++).label("").red()
                        }
                    )
                );
            std::for_each(plans_.begin(), plans_.end(), [&r, &row, idd = 100](auto&& plan) mutable
            {
                str title = plan.title;
//                str title = plan.size + " - " + std::to_string(plan.price);
//                if ("Free" != plan.price)
//                {
//                    title += "/MONTH";
//                    r.push_back(fld(row++).label(title).large().purple().center());
//                    r.push_back(fld(row++).button(-1, "Upgrade to " + plan.name).center().medium().w(-200));
//
//                }
//                else
                {
                    r.push_back(fld(row++).link(idd++, title).large().center());

                }
                r.push_back(fld(row++).label(plan.product_description).center().small());
                r.push_back(fld(row).label(plan.formatted_price).center().large().bold());
                r.push_back(fld(row++).label("per month").small().center());
                r.push_back(fld(row++).vspace().large());
                r.push_back(fld(row++).vspace().large());
            });
            return r;
        }
        
        void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) final
        {
            services_.log(__LINE__, __FILE__, "onload");
            form_ = form;
            view->set_screen_name("Upgrade");
        }
        
        void action(n4 element_id) final
        {
            if (!form_)
                return;
            if (100 <= element_id && element_id < 100+plans_.size())
            {
//                main_menu_->subscribe(plans_[element_id-100].product_id);
                return;
            }
            
            form_->error(element_id, "");
            switch (static_cast<control>(element_id))
            {
                default: break;
            }
        }
        
        void back() final
        {
        }
        bool hide_navigation() final { return false; }
        bool holdon_to_vm() final { return false; }
    };
}
