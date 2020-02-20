// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace ai {

class FormView;
class GenericForm;
struct Field;

class FormViewmodel {
public:
    virtual ~FormViewmodel() {}

    virtual void action(int32_t element_id) = 0;

    virtual void back() = 0;

    virtual std::vector<Field> fields() = 0;

    virtual void on_load(const std::shared_ptr<FormView> & view, const std::shared_ptr<GenericForm> & form) = 0;

    virtual bool hide_navigation() = 0;

    virtual bool holdon_to_vm() = 0;
};

}  // namespace ai
