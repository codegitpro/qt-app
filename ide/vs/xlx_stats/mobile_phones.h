#pragma once
#include "general.h"
#include "text_field.h"
#include "lookup_field.h"
#include <iostream>

namespace ai
{

	struct mobile_phones
	{
		str vendor_model_type_contract_bandwith()
		{
			auto count_bits = pack<p4>::count_bits(fld_phone_number.size());
			auto fun = [this, count_bits](p4 id)
			{
				p8 r = fld_vendor[id];
				r <<= fld_model.bits();
				r |= fld_model[id];
				//r <<= fld_type.bits();
				//r |= fld_type[id];
				//r <<= fld_contract.bits();
				//r |= fld_contract[id];
				//r <<= fld_bandwidth.bits();
				//r |= fld_bandwidth[id];
				r <<= count_bits;
				r |= id;
				return r;
			};

			auto total_bits = 
				fld_vendor.bits() 
				+ fld_model.bits() 
				//+ fld_type.bits() 
				//+ fld_contract.bits() 
				//+ fld_bandwidth.bits()
				+ count_bits;

			std::cout << "total_bits " << total_bits << "\n";


			auto all = arr<p8>();
			all.reserve(fld_phone_number.size());
			for (p4 i = 0; i < fld_phone_number.size(); ++i)
			{
				all.push_back(fun(i));
			}

			auto distill_fun = [](p8& packed, lookup_field& fld)
			{
				p4 r = packed & lsb<p8>::mask(fld.bits());
				packed >>= fld.bits();
				return r;
			};

			for (p4 i = 0; i < fld_phone_number.size(); ++i)
			{
				p8 packed = all[i];
				p4 index = packed & lsb<p8>::mask(count_bits);
				assert(i == index);
				packed >>= count_bits;
				//p4 bandwidth = distill_fun(packed, fld_bandwidth);
				//assert(bandwidth == fld_bandwidth[i]);
				//p4 contract = distill_fun(packed, fld_contract);
				//assert(contract == fld_contract[i]);
				//p4 type = distill_fun(packed, fld_type);
				//assert(type == fld_type[i]);
				p4 model = distill_fun(packed, fld_model);
				assert(model == fld_model[i]);
				p4 vendor = distill_fun(packed, fld_vendor);
				assert(vendor == fld_vendor[i]);
			}
			std::sort(all.begin(), all.end());
			auto all_counts = arr<par<p8, p4>>();
			auto it = all.cbegin();
			while (all.cend() != it)
			{
				auto next = std::find_if(it, all.cend(), [x = *it, count_bits](p8 y) { return (x >> count_bits) != (y >> count_bits); });
				all_counts.push_back({ *it, static_cast<p4>(std::distance(it, next)) });
				it = next;
			}
			std::cout << "total " << std::to_string(all_counts.size()) << "\n";
			auto r = str();

			auto line_fun = [](str& r, lookup_field& fld, p8 packed)
			{
				sz id = packed & lsb<p8>::mask(fld.bits());
				r += replace(replace(fld.reference(id), "&amp;", "&"), ";", " ") + ";";
				return packed >> fld.bits();
			};

			std::for_each(all_counts.cbegin(), all_counts.cend(), [this, line_fun, &r, count_bits](auto&& v)
			{
				p8 packed = v.first;
				p4 count = v.second;

				p4 index = packed & lsb<p8>::mask(count_bits);
				packed >>= count_bits;
				auto s = format_line(index, 12);

				auto line = str();
				//packed = line_fun(line, fld_bandwidth, packed);
				//packed = line_fun(line, fld_contract, packed);
				//packed = line_fun(line, fld_type, packed);
				packed = line_fun(line, fld_model, packed);
				packed = line_fun(line, fld_vendor, packed);
				line += std::to_string(count) + "\r\n";
				r += line;
			});
			return r;
		}
		sz size_on_disk() const
		{
			return fld_phone_number.size_on_disk()
				+ fld_contract.size_on_disk()
				+ fld_active.size_on_disk()
				+ fld_province.size_on_disk()
				+ fld_municipality.size_on_disk()
				+ fld_city.size_on_disk()
				+ fld_town.size_on_disk()
				+ fld_suburb.size_on_disk()
				+ fld_vendor.size_on_disk()
				+ fld_model.size_on_disk()
				+ fld_type.size_on_disk()
				+ fld_yn.size_on_disk()
				+ fld_bandwidth.size_on_disk()
				+ fld_source_file.size_on_disk()
				+ fld_line_number.size_on_disk();
		}

		sz deserialise(const char* data)
		{
			auto r = fld_phone_number.deserialise(data);
			r += fld_contract.deserialise(data + r);
			r += fld_active.deserialise(data + r);
			r += fld_province.deserialise(data + r);
			r += fld_municipality.deserialise(data + r);
			r += fld_city.deserialise(data + r);
			r += fld_town.deserialise(data + r);
			r += fld_suburb.deserialise(data + r);
			r += fld_vendor.deserialise(data + r);
			r += fld_model.deserialise(data + r);
			r += fld_type.deserialise(data + r);
			r += fld_yn.deserialise(data + r);
			r += fld_bandwidth.deserialise(data + r);
			r += fld_source_file.deserialise(data + r);
			r += fld_line_number.deserialise(data + r);
			return r;
		}

		sz serialise(char* data) const
		{
			auto r = fld_phone_number.serialise(data);
			r += fld_contract.serialise(data + r);
			r += fld_active.serialise(data + r);
			r += fld_province.serialise(data + r);
			r += fld_municipality.serialise(data + r);
			r += fld_city.serialise(data + r);
			r += fld_town.serialise(data + r);
			r += fld_suburb.serialise(data + r);
			r += fld_vendor.serialise(data + r);
			r += fld_model.serialise(data + r);
			r += fld_type.serialise(data + r);
			r += fld_yn.serialise(data + r);
			r += fld_bandwidth.serialise(data + r);
			r += fld_source_file.serialise(data + r);
			r += fld_line_number.serialise(data + r);
			return r;
		}

		text_field fld_phone_number;
		lookup_field fld_contract;
		lookup_field fld_active;
		lookup_field fld_province;
		lookup_field fld_municipality;
		lookup_field fld_city;
		lookup_field fld_town;
		lookup_field fld_suburb;
		lookup_field fld_vendor;
		lookup_field fld_model;
		lookup_field fld_type;
		lookup_field fld_yn;
		lookup_field fld_bandwidth;
		lookup_field fld_source_file;
		pack<p4> fld_line_number;
		
		//arr<p4> column_widths()
		//{
		//	auto r = arr<p4>();
		//	r.push_back(fld_phone_number.max_name_length());
		//	r.push_back(fld_contract.max_name_length());
		//	r.push_back(fld_active.max_name_length());
		//	r.push_back(fld_province.max_name_length());
		//	r.push_back(fld_municipality.max_name_length());
		//	r.push_back(fld_city.max_name_length());
		//	r.push_back(fld_town.max_name_length());
		//	r.push_back(fld_suburb.max_name_length());
		//	r.push_back(fld_vendor.max_name_length());
		//	r.push_back(fld_model.max_name_length());
		//	r.push_back(fld_type.max_name_length());
		//	r.push_back(fld_yn.max_name_length());
		//	r.push_back(fld_bandwidth.max_name_length());
		//	r.push_back(fld_source_file.max_name_length());
		//	return r;
		//}
		str format_line(p4 index, const arr<p4>& widths)
		{

			str line = fld_phone_number.extract(index);
			p4 len = widths[0] + 1;
			line += str(len - line.size(), ' ');

			line += fld_contract.extract(index);
			len += widths[1] + 1;
			line += str(len - line.size(), ' ');

			line += fld_active.extract(index);
			len += widths[2] + 1;
			line += str(len - line.size(), ' ');

			line += fld_province.extract(index);
			len += widths[3] + 1;
			line += str(len - line.size(), ' ');

			line += fld_municipality.extract(index);
			len += widths[4] + 1;
			line += str(len - line.size(), ' ');

			line += fld_city.extract(index);
			len += widths[5] + 1;
			line += str(len - line.size(), ' ');

			line += fld_town.extract(index);
			len += widths[6] + 1;
			line += str(len - line.size(), ' ');

			line += fld_suburb.extract(index);
			len += widths[7] + 1;
			line += str(len - line.size(), ' ');

			line += fld_vendor.extract(index);
			len += widths[8] + 1;
			line += str(len - line.size(), ' ');

			line += fld_model.extract(index);
			len += widths[9] + 1;
			line += str(len - line.size(), ' ');

			line += fld_type.extract(index);
			len += widths[10] + 1;
			line += str(len - line.size(), ' ');

			line += fld_yn.extract(index);
			len += widths[11] + 1;
			line += str(len - line.size(), ' ');

			line += fld_bandwidth.extract(index);
			len += widths[12] + 1;
			line += str(len - line.size(), ' ');

			line += std::to_string(fld_line_number[index]);
			len += 12;
			line += str(len - line.size(), ' ');

			line += fld_source_file.extract(index);

			return line;
		}

		str format_line(p4 index, p4 width)
		{
			str line = fld_phone_number.extract(index, 14);
			p4 len = 15;
			line += str(len - line.size(), ' ');

			line += fld_contract.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_active.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_province.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_municipality.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_city.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_town.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_suburb.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_vendor.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_model.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_type.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_yn.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += fld_bandwidth.extract(index, width);
			len += width + 1;
			line += str(len - line.size(), ' ');

			line += std::to_string(fld_line_number[index]);
			len += 12;
			line += str(len - line.size(), ' ');

			line += fld_source_file.extract(index);

			return line;
		}
	};

}
