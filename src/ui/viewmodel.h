#pragma once
#include "../general.h"
#include "../bridge/cpp/generic_form.hpp"
#include "../bridge/cpp/field.hpp"
#include "../bridge/cpp/field_type.hpp"
#include "../bridge/cpp/block.hpp"
#include "../bridge/cpp/weight.hpp"
#include "../bridge/cpp/style.hpp"
#include "../bridge/cpp/screen_type.hpp"
#include "../bridge/cpp/file_state.hpp"

#include "../bridge.h"
#include "../bridge/cpp/view.hpp"
#include "../model/services.h"
#include "../bridge/cpp/form_viewmodel.hpp"
#include "../bridge/cpp/folder_viewmodel.hpp"
#include "../bridge/cpp/form_view.hpp"
#include "../bridge/cpp/folder_view.hpp"
#include "../bridge/cpp/ui_callback.hpp"
#include "../bridge/cpp/pre_viewmodel.hpp"
#include "../bridge/cpp/diagnostic.hpp"


namespace ai
{
    static bool validate(const sptr<GenericForm>& form, const arr<Field>& fields, n4 button_id)
    {
        if (button_id <= 0)
            return true;
        
        form->error(button_id, "");

        auto found_it = std::find_if(fields.begin(), fields.end(), [button_id](const Field& field)
        {
            return field.field_id == button_id;
        });
        if (fields.end() == found_it || FieldType::BUTTON != found_it->field_t || !found_it->required)
            return true;
        
        return std::accumulate(fields.begin(), fields.end(), true, [form](bool acc, const Field& field)
        {
            if (field.field_id <= 0 || FieldType::INPUT != field.field_t)
                return acc;
            
            funk<str(const str&)> fun;
            switch (field.data_t)
            {
                case DataType::EMAIL:
                {
                    fun = [](const str& val)
                    {
                        auto found_at = std::find(val.begin(), val.end(), '@');
                        if (val.end() != found_at && val.begin() != found_at && val.end() != found_at + 1)
                        {
                            auto found_dot = std::find(found_at + 1, val.end(), '.');
                            if (val.end() != found_dot && found_at+1 != found_dot && val.end() != found_dot + 1)
                                return "";
                        }
                        return "Please enter a valid email";
                    };
                    
                    break;
                }
                case DataType::SKYPE:
                case DataType::PHONE:
                case DataType::CURRENCY:
                case DataType::URL:
                case DataType::INTEGER:
                default: break;
            }
            
            if (!fun)
            {
                form->error(field.field_id, "");
                return acc;
            }

            auto val = form->input_get(field.field_id);
            if (val.empty())
                return acc;
            str error = fun(val);
            form->error(field.field_id, error);
            return acc && error.empty();
        });
    }
    
    static void run_ui(const sptr<UiCallback>& view, funk<void()> fun)
    {
        view->gui_thread(std::make_shared<lambda_void_result>(fun));
    }
    
	static Block block_single(n4 row, n4 col)
	{
		return Block(row, row + 1, col, col + 1);
	}

	static Block bl_rc(n4 row, n4 col)
	{
		return Block(row, row + 1, col, col + 1);
	}

	struct fld_wrapper
	{
		fld_wrapper(int row_beg, int row_end) 
			: 
			m_fld(Block(row_beg, row_end, 0, 1), -1, DataType::TEXT, FieldType::LABEL, "", true, Style(Size::NONE, Align::NONE, Colour::NONE, Weight::NORMAL, 0))
		{
		}
		operator Field()
		{
			return std::move(m_fld);
		}
        fld_wrapper vspace() { m_fld.field_t = FieldType::VERTICAL_SPACE; return std::move(*this); }
        fld_wrapper logo() { m_fld.field_t = FieldType::LOGO; return std::move(*this); }
		fld_wrapper w(int x) { m_fld.style.width = x; return std::move(*this); }
		fld_wrapper col(int x) { m_fld.rect.col_beg = x; m_fld.rect.col_end = x + 1; return std::move(*this); }
		fld_wrapper col(int beg, int end) { m_fld.rect.col_beg = beg; m_fld.rect.col_end = end; return std::move(*this); }
		fld_wrapper back(str title) { m_fld.field_t = FieldType::BACK; m_fld.title = title; return std::move(*this); }
        fld_wrapper link(int idd, str title)
        {
            m_fld.field_id = idd;
            m_fld.title = title;
            m_fld.field_t = FieldType::LINK;
            m_fld.required = false;
            return std::move(*this);
        }
        fld_wrapper button(int idd, str title) { m_fld.field_id = idd; m_fld.title = title;  m_fld.field_t = FieldType::BUTTON; return std::move(*this); }
        fld_wrapper cancel(int idd, str title = "Cancel") { m_fld.field_id = idd; m_fld.title = title;  m_fld.field_t = FieldType::BUTTON; m_fld.required = false; return std::move(*this); }
		fld_wrapper check(int idd, str title) { m_fld.field_id = idd; m_fld.field_t = FieldType::INPUT; m_fld.data_t = DataType::BOOLEAN; m_fld.title = title; return std::move(*this); }
		fld_wrapper choice(int idd, str options) { m_fld.field_id = idd; m_fld.field_t = FieldType::INPUT; m_fld.data_t = DataType::CHOICE; m_fld.title = options; return std::move(*this); }
		fld_wrapper combo(int idd, str name) { m_fld.field_id = idd; m_fld.field_t = FieldType::INPUT; m_fld.data_t = DataType::REFERENCE; m_fld.title = name; return std::move(*this); }
		fld_wrapper multiline(int idd, str name) { m_fld.field_id = idd; m_fld.field_t = FieldType::INPUT; m_fld.data_t = DataType::MULTILINE_TEXT; m_fld.title = name; return std::move(*this); }
		fld_wrapper chat(int idd, str title) { m_fld.field_id = idd; m_fld.field_t = FieldType::CHAT; m_fld.title = title; return std::move(*this); }
		fld_wrapper console(int idd) { m_fld.field_id = idd; m_fld.field_t = FieldType::CONSOLE; return std::move(*this); }

		fld_wrapper input(int id, str name) { m_fld.field_id = id; m_fld.title = name; m_fld.field_t = FieldType::INPUT; return std::move(*this); }
        fld_wrapper label(str name) { m_fld.title = name; m_fld.field_t = FieldType::LABEL; return std::move(*this); }
        fld_wrapper help(str name) { m_fld.title = name; m_fld.field_t = FieldType::HELP; return std::move(*this); }

		fld_wrapper password() { m_fld.data_t = DataType::PASSWORD;		return std::move(*this); }
		fld_wrapper skype() { m_fld.data_t = DataType::SKYPE;		return std::move(*this); }
		fld_wrapper phone()		{ m_fld.data_t = DataType::PHONE;		return std::move(*this); }
		fld_wrapper money()		{ m_fld.data_t = DataType::CURRENCY;	return std::move(*this); }
		fld_wrapper url() { m_fld.data_t = DataType::URL;			return std::move(*this); }
		fld_wrapper email() { m_fld.data_t = DataType::EMAIL;			return std::move(*this); }
		fld_wrapper integer()	{ m_fld.data_t = DataType::INTEGER;		return std::move(*this); }
        fld_wrapper error(int idd) { m_fld.field_id = idd; m_fld.field_t = FieldType::ERROR; return std::move(*this); }

        fld_wrapper light_grey() { m_fld.style.colour = Colour::LIGHT_GREY; return std::move(*this); }
        fld_wrapper dark_grey() { m_fld.style.colour = Colour::DARK_GREY; return std::move(*this); }
        fld_wrapper dirty_blue() { m_fld.style.colour = Colour::DIRTY_BLUE; return std::move(*this); }
        fld_wrapper light_blue() { m_fld.style.colour = Colour::LIGHT_BLUE; return std::move(*this); }
        fld_wrapper white() { m_fld.style.colour = Colour::WHITE; return std::move(*this); }
		fld_wrapper purple() { m_fld.style.colour = Colour::PURPLE; return std::move(*this); }
		fld_wrapper red() { m_fld.style.colour = Colour::RED; return std::move(*this); }

        fld_wrapper medium() { m_fld.style.size = Size::MEDIUM; return std::move(*this); }
        fld_wrapper small() { m_fld.style.size = Size::SMALL; return std::move(*this); }
		fld_wrapper large() { m_fld.style.size = Size::LARGE; return std::move(*this); }
        fld_wrapper right() { m_fld.style.align = Align::RIGHT; return std::move(*this); }
        fld_wrapper left() { m_fld.style.align = Align::LEFT; return std::move(*this); }
		fld_wrapper center() { m_fld.style.align = Align::CENTER; return std::move(*this); }
		fld_wrapper idd(int x) { m_fld.field_id = x; return std::move(*this); }
        fld_wrapper opt() { m_fld.required = false; return std::move(*this); }
        fld_wrapper expand() { m_fld.field_t = FieldType::EXPAND; return std::move(*this); }
        fld_wrapper rating(int x) { m_fld.field_t = FieldType::INPUT; m_fld.data_t = DataType::RATING; m_fld.title = -1 == x ? "" : std::to_string(x); return std::move(*this); }
		fld_wrapper bold() { m_fld.style.weight = Weight::BOLD; return std::move(*this); }
		fld_wrapper no_bold() { m_fld.style.weight = Weight::NORMAL; return std::move(*this); }
		fld_wrapper panel_break() { m_fld.field_t = FieldType::PANEL_BREAK; return std::move(*this); }
	private:
		Field m_fld;
	};

	static fld_wrapper fld(int row) { return fld_wrapper(row, row + 1); }
	static fld_wrapper fld(int row_beg, int row_end) { return fld_wrapper(row_beg, row_end); }

	static arr<Field> bold(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field)
		{
			if (Weight::NORMAL == field.style.weight)
				field.style.weight = Weight::BOLD;
		});
		return fields;
	}
	static arr<Field> medium(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field) 
		{ 
			if (Size::NONE == field.style.size)
				field.style.size = Size::MEDIUM; 
		});
		return fields;
	}
    static arr<Field> large(arr<Field> fields)
    {
        std::for_each(fields.begin(), fields.end(), [](auto&& field) 
		{ 
			if (Size::NONE == field.style.size)
				field.style.size = Size::LARGE; 
		});
        return fields;
    }
	static arr<Field> small(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field)
		{
			if (Size::NONE == field.style.size)
				field.style.size = Size::SMALL;
		});
		return fields;
	}
	static arr<Field> width(n4 w, arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [w](auto&& field)
		{
			if (0 == field.style.width)
				field.style.width = w;
		});
		return fields;
	}
	static arr<Field> center(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field)
		{
			if (Align::NONE == field.style.align)
				field.style.align = Align::CENTER;
		});
		return fields;
	}
	static arr<Field> right(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field)
		{
			if (Align::NONE == field.style.align)
				field.style.align = Align::RIGHT;
		});
		return fields;
	}
	static arr<Field> left(arr<Field> fields)
	{
		std::for_each(fields.begin(), fields.end(), [](auto&& field)
		{
			if (Align::NONE == field.style.align)
				field.style.align = Align::LEFT;
		});
		return fields;
	}
}
