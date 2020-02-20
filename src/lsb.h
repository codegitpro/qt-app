#pragma once

#include "endian.h"

namespace ai
{
	template<typename T>
	struct lsb
	{
		static lsb writer(T* stream, sz bit_position, int width)
		{
			return lsb(stream, bit_position, width, true);
		}

		static lsb reader(T* stream, sz bit_position, int width)
		{
			return lsb(stream, bit_position, width, false);
		}

		static T read_single(T* stream, sz bit_position, int width)
		{
			auto l = reader(stream, bit_position + width, width);
			return l.read_backwards();
		}

		static void write_single(T* stream, sz bit_position, int width, T x)
		{
			auto l = writer(stream, bit_position + width, width);
			l.write_backwards(x);
			l.flush();
		}

		static constexpr int bit_width() { return sizeof(T) * 8; }

		inline static T mask(int bit_count)
		{
			return std::numeric_limits<T>::max() >> (bit_width() - bit_count);
		}

		T read_backwards()
		{
			if (!bit_count_)
			{
				bit_count_ = bit_width();
				current_value_ = order_le(*current_place_--);
			}

			if (width_ > bit_count_)
			{
				int nuwe_bisse = width_ - bit_count_;
				T r = (current_value_ & mask(bit_count_)) << nuwe_bisse;
				current_value_ = order_le(*current_place_--);
				bit_count_ = bit_width() - nuwe_bisse;
				r |= current_value_ >> bit_count_;
				return r;
			}

			{
				T r = (current_value_ & mask(bit_count_)) >> (bit_count_ - width_);
				bit_count_ -= width_;
				return r;
			}
		}

		void write_backwards(T x)
		{
			if (width_ <= bit_count_)
			{
				bit_count_ -= width_;
				if (bit_count_)
					current_value_ |= x << bit_count_;
				else
				{
					current_value_ |= x;
					*current_place_-- = order_le(current_value_);
					bit_count_ = bit_width();
					current_value_ = 0;
				}
			}
			else
			{
				int nuwe_bisse = width_ - bit_count_;
				current_value_ |= x >> nuwe_bisse;

				*current_place_-- = order_le(current_value_);
				bit_count_ = bit_width() - nuwe_bisse;
				current_value_ = x << bit_count_;
			}
		}

		void flush()
		{
			if (bit_count_ < bit_width())
				*current_place_ = current_value_ | (*current_place_ & mask(bit_count_));
		}
	private:
		int width_;
		int bit_count_;
		T current_value_;
		T* current_place_;

		lsb(T* stream, sz bit_position, int width, bool vir_skryf) : width_(width)
		{
			current_place_ = stream + bit_position / bit_width();
			bit_count_ = bit_position % bit_width();
			if (!bit_count_)
			{
				--current_place_;
				bit_count_ = bit_width();
			}
			current_value_ = vir_skryf ? order_le(*current_place_) & (~mask(bit_count_)) : order_le(*current_place_--);
		}
	};
}