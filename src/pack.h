#pragma once
#include "lsb.h"

namespace ai
{
	template<typename nT, typename pT, typename SizeType>
	struct packed_data
	{
		struct proxy
		{
			proxy(packed_data<nT, pT, SizeType>& container, SizeType index) : index(index), container(container)
			{
			}
			proxy& operator=(nT value)
			{
				if (value > container.max_value)
					container.make_space(value, 0);
				lsb<pT>::write_single(const_cast<pT*>(container.blocks.data()), index*container.bitcount, container.bitcount, value);
				return *this;
			}
			operator nT() const
			{
				return static_cast<nT>(lsb<pT>::read_single(const_cast<pT*>(container.blocks.data()), index*container.bitcount, container.bitcount));
			}

			nT operator++()
			{
				nT value = static_cast<nT>(lsb<pT>::read_single(const_cast<pT*>(container.blocks.data()), index*container.bitcount, container.bitcount)) + 1;
				if (value > container.max_value)
					container.make_space(value, 0);
				lsb<pT>::write_single(const_cast<pT*>(container.blocks.data()), index*container.bitcount, container.bitcount, value);
				return value;
			}
		private:
			SizeType index;
			packed_data<nT, pT, SizeType>& container;
		};

		typedef nT value_type;
		typedef proxy reference;
		typedef nT const_reference;
		typedef ptrdiff_t difference_type;
		typedef SizeType size_type;

		struct iterator //: std::iterator<std::random_access_iterator_tag, nT, ptrdiff_t, proxy, proxy>
		{
			typedef ptrdiff_t difference_type;
			typedef nT value_type;
			typedef proxy reference;
			typedef proxy pointer;
			typedef std::random_access_iterator_tag iterator_category;

			iterator() : index(0), container(nullptr) {}
			iterator(packed_data<nT, pT, SizeType>& container, SizeType index) : container(&container), index(index) {}

			//iterator(const iterator& it) : container(it.container), index(it.index) {}
			//iterator&		operator=(const iterator& it)
			//{
			//	container = it.container;
			//	index = it.index;
			//	return *this;
			//}

			//forward
			iterator		operator++(int)
			{
				auto r = iterator(*container, index);
				++index;
				return r;
			}
			iterator&		operator++() { ++index; return *this; }
			reference		operator* () const { return proxy(*container, index); }
			//pointer		operator->() const { return proxy(*container, index); }
			bool			operator==(const iterator& rhs) const { return index == rhs.index; }
			bool			operator!=(const iterator& rhs) const { return !(*this == rhs); }

			//bidirectional
			iterator		operator--(int)
			{
				auto r = iterator(*container, index);
				--index;
				return r;
			}
			iterator&		operator--() { --index; return *this; }

			//random
			iterator&		operator+=(size_type x) { index += x; return *this; }
			iterator		operator+(size_type x) const { return iterator(*container, index + x); }
			difference_type	operator+(iterator it) const { return index + it.index; }
			friend iterator operator+(size_type x, const iterator& it) { return iterator(*it.container, it.index + x); }
			iterator&		operator-=(size_type x) { index -= x; return *this; }
			iterator		operator-(size_type x) const { return iterator(*container, index + x); }
			difference_type	operator-(iterator it) const { return index - it.index; }
			friend iterator operator-(size_type x, const iterator& it) { return iterator(*it.container, it.index - x); }

			bool			operator<(const iterator& x) const { return index < x.index; }
			bool			operator>(const iterator& x) const { return x.index < index; }
			bool			operator<=(const iterator& x) const { return !(x.index < index); }
			bool			operator>=(const iterator& x) const { return !(index < x.index); }

			reference operator[](size_type x) const { return proxy(*container, index + x); }

		private:
			packed_data<nT, pT, SizeType>* container;
			SizeType index;
		};

		struct const_iterator
		{
			typedef ptrdiff_t difference_type;
			typedef nT value_type;
			typedef nT& reference;
			typedef nT* pointer;
			typedef std::random_access_iterator_tag iterator_category;

			const_iterator() : index(0), container(nullptr) {}
			const_iterator(const packed_data<nT, pT, SizeType>& container, SizeType index) : container(&container), index(index) {}

			//forward
			const_iterator			operator++(int)
			{
				auto r = const_iterator(*container, index);
				++index;
				return r;
			}
			const_iterator&			operator++() { ++index; return *this; }
			const_reference			operator* () const { return container->read_single(index); }
			bool					operator==(const const_iterator& rhs) const { return index == rhs.index; }
			bool					operator!=(const const_iterator& rhs) const { return !(*this == rhs); }

			//bidirectional
			const_iterator			operator--(int)
			{
				auto r = const_iterator(*container, index);
				--index;
				return r;
			}
			const_iterator&			operator--() { --index; return *this; }

			//random
			const_iterator&			operator+=(size_type x) { index += x; return *this; }
			const_iterator			operator+(size_type x) const { return const_iterator(*container, index + x); }
			difference_type			operator+(const_iterator it) const { return index + it.index; }
			friend const_iterator	operator+(size_type x, const const_iterator& it) { return const_iterator(*it.container, it.index + x); }
			const_iterator&			operator-=(size_type x) { index -= x; return *this; }
			const_iterator			operator-(size_type x) const { return const_iterator(*container, index + x); }
			difference_type			operator-(const_iterator it) const { return index - it.index; }
			friend const_iterator	operator-(size_type x, const const_iterator& it) { return const_iterator(*it.container, it.index - x); }

			bool					operator<(const const_iterator& x) const { return index < x.index; }
			bool					operator>(const const_iterator& x) const { return x.index < index; }
			bool					operator<=(const const_iterator& x) const { return !(x.index < index); }
			bool					operator>=(const const_iterator& x) const { return !(index < x.index); }

			const_reference			operator[](size_type x) const { return container->read_single(index + x); }

		private:
			const packed_data<nT, pT, SizeType>* container;
			SizeType index;
		};

		packed_data() {}
		packed_data(nT maximum) : max_value(maximum)
		{
			bitcount = count_bits(maximum);
		}

		iterator begin() { return iterator(*this, 0); }
		iterator end() { return iterator(*this, count); }
		const_iterator cbegin() const { return const_iterator(*this, 0); }
		const_iterator cend() const { return const_iterator(*this, count); }
		const_iterator begin() const { return const_iterator(*this, 0); }
		const_iterator end() const { return const_iterator(*this, count); }

		const_reference operator[](SizeType index) const { return read_single(index); }
		reference at(SizeType index) { return proxy(*this, index); }
		const_reference at(SizeType index) const { return read_single(index); }

		void pop_back() { --count; }

		//size_type max_size() const;
		inline bool empty() const { return 0 == count; }
		inline void clear() { count = 0; }
		void push_back(nT x)
		{
			make_space(std::max(max_value, x), 1);
			++count;
			auto skrywer = lsb<pT>::writer(const_cast<pT*>(blocks.data()), count*bitcount, bitcount);
			skrywer.write_backwards(x);
			skrywer.flush();
		}
		void push_back(const arr<nT>& waardes)
		{
			assert(!waardes.empty());
			make_space(std::max(max_value, *std::max_element(waardes.begin(), waardes.end())), static_cast<SizeType>(waardes.size()));

			count += static_cast<SizeType>(waardes.size());
			auto skrywer = lsb<pT>::writer(const_cast<pT*>(blocks.data()), count*bitcount, bitcount);
			std::for_each(waardes.rbegin(), waardes.rend(), [&skrywer](nT x) { skrywer.write_backwards(x); });
			skrywer.flush();
		}
		void resize(SizeType new_size)
		{
			if (new_size <= count)
				return;
			count = new_size;
			blocks.resize(block_size(static_cast<sz>(count), bitcount), 0);
		}

		void reset(SizeType max_size, nT maximum_value)
		{
			count = 0;
			make_space(std::max(max_value, maximum_value), max_size);
			std::fill(blocks.begin(), blocks.end(), 0);
			count = max_size;
		}

		SizeType size() const { return count; }
		SizeType where_is(nT x)
		{
			if (x > max_value)
				return -1;

			auto leser = lsb<pT>::reader(const_cast<pT*>(blocks.data()), count*bitcount, bitcount);
			for (SizeType i = count; i > 0; --i)
			{
				if (leser.read_backwards() == x)
					return i - 1;
			}
			return -1;
		}

		static int count_bits(nT maximum)
		{
			int r = 0;
			while (maximum)
			{
				++r;
				maximum >>= 1;
			}
			return r;
		}

		sz size_on_disk() const
		{
			return size_on_disk_number(count) + size_on_disk_number(max_value) + size_on_disk_number(bitcount) + size_on_disk_arr(blocks);
		}

		sz deserialise(const p1* data)
		{
			auto r = deserialise_number(data, count);
			r += deserialise_number(data + r, max_value);
			r += deserialise_number(data + r, bitcount);
			r += deserialise_arr(data + r, blocks);
			return r;
		}

		sz serialise(p1* data) const
		{
			auto r = serialise_number(data, count);
			r += serialise_number(data + r, max_value);
			r += serialise_number(data + r, bitcount);
			r += serialise_arr(data + r, blocks);
			return r;
		}

		nT max() const { return max_value; }
		int bits() const { return bitcount; }
	private:
		arr<pT> blocks;
		SizeType count = 0;
		nT max_value = 1;
		int bitcount = 1;

		inline nT read_single(SizeType index) const
		{
			return static_cast<nT>(lsb<pT>::read_single(const_cast<pT*>(blocks.data()), index*bitcount, bitcount));
		}

		static inline sz block_size(sz count, int bisse)
		{
			return (count*bisse) / (sizeof(pT) * 8) + 2;
		}

		inline void make_space(nT nuwe_maks, SizeType aantal_nuwe_items)
		{
			int nuwe_bisse = count_bits(nuwe_maks);
			blocks.resize(block_size(static_cast<sz>(aantal_nuwe_items + count), nuwe_bisse), 0);

			if (nuwe_bisse <= bitcount)
			{
				max_value = nuwe_maks;
				return;
			}

            if (count) //reset sets count to 0 before calling make_space causing crash when growing
            {
                auto kort = lsb<pT>::reader(const_cast<pT*>(blocks.data()), count*bitcount, bitcount);
                auto lank = lsb<pT>::writer(const_cast<pT*>(blocks.data()), count*nuwe_bisse, nuwe_bisse);
                const SizeType aantal_om_te_skuif = count;
                for (SizeType i = 0; i < aantal_om_te_skuif; ++i)
                {
                    lank.write_backwards(kort.read_backwards());
                }
            }

            bitcount = nuwe_bisse;
			max_value = nuwe_maks;
		}
	};

	using packed = packed_data<n4, p4, int>;
	using packed_d = packed_data<n8, p8, int>;
}
