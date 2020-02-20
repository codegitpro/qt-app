#pragma once
#include "../general.h"
#include "../bridge/cpp/colour.hpp"
#include "../bridge/cpp/field.hpp"
#include "../bridge/cpp/size.hpp"
#include "../bridge/cpp/align.hpp"
#include "../bridge/cpp/weight.hpp"
#include "../bridge/cpp/style.hpp"

namespace ai
{
	struct style_wrapper
	{
		style_wrapper() : m_style(Size::NONE, Align::NONE, Colour::NONE, Weight::NORMAL, 0) {}
		operator Style() { return std::move(m_style); }
		style_wrapper w(n4 x) { m_style.width = x; return std::move(*this); }
		style_wrapper large() { m_style.size = Size::LARGE; return std::move(*this); }
		style_wrapper small() { m_style.size = Size::SMALL; return std::move(*this); }
		style_wrapper medium() { m_style.size = Size::MEDIUM; return std::move(*this); }
		style_wrapper left() { m_style.align = Align::LEFT; return std::move(*this); }
		style_wrapper right() { m_style.align = Align::RIGHT; return std::move(*this); }
		style_wrapper center() { m_style.align = Align::CENTER; return std::move(*this); }
		style_wrapper cyan() { m_style.colour = Colour::CYAN; return std::move(*this); }
		style_wrapper white() { m_style.colour = Colour::WHITE; return std::move(*this); }
		style_wrapper red() { m_style.colour = Colour::RED; return std::move(*this); }
		style_wrapper purple() { m_style.colour = Colour::PURPLE; return std::move(*this); }
		style_wrapper bold() { m_style.weight = Weight::BOLD; return std::move(*this); }
	private:
		Style m_style;
	};

	static style_wrapper st() { return style_wrapper(); }

	static str text_colour(Colour c)
	{
		switch (c)
		{
		case Colour::CYAN: return "01b7c4";
		case Colour::PURPLE: return "454873";
		case Colour::RED: return "ff0000";
		case Colour::WHITE: return "ffffff";
		default: return "ffffff";
		}
	}

	static str choices(const arr<str>& categories)
	{
		return std::accumulate(categories.begin(), categories.end(), str(), [](str acc, const str& category) { return acc.empty() ? category : acc + "|" + category; });
	}

	static n4 bl_rowspan(const Block& rect) { return rect.row_end < 0 ? 1 : rect.row_end - rect.row_beg; }
	static n4 bl_colspan(const Block& rect) { return rect.col_end < 0 ? 1 : rect.col_end - rect.col_beg; }
	static n4 f_row(const Field& field) { return field.rect.row_beg; }
	static n4 f_rowspan(const Field& field) { return bl_rowspan(field.rect); }
	static n4 f_col(const Field& field) { return field.rect.col_beg; }
	static n4 f_colspan(const Field& field) { return bl_colspan(field.rect); }

	static str elapsed(int seconds)
	{
		auto hour = std::to_string(seconds / 3600);
		auto min = std::to_string((seconds % 3600) / 60);
		auto secs = std::to_string(seconds % 60);
		auto pad = [](str s) { return s.size() == 1 ? "0" + s : s; };
		return pad(hour) + ":" + pad(min) + ":" + pad(secs);
	}

	static str seconds_to_str(int seconds)
	{
		return std::to_string(seconds / 3600) + ":" + std::to_string(seconds / 60) + ":" + std::to_string(seconds % 60);
	}

	static str cents_to_str(int cents)
	{
		auto cents_part = std::to_string(cents % 100);
		if (1 == cents_part.size())
			cents_part = "0" + cents_part;
		return "$" + std::to_string(cents / 100) + "." + cents_part;
	}


}
