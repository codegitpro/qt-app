#include <iostream>
#include <uv_plat.h>
#include "endian.h"
#include <zlib.h>
#include "pack.h"
#include "text_map.h"
#include "mobile_phones.h"
#include <pkzip.h>
#include "platform/std_task_scheduler.h"
#include "general.h"
#include "lookup_field.h"

using namespace std;

namespace ai
{

	void run_on_main_thread(funk<void()> fun)
	{
		thread x(fun);
		x.detach();
	}

	arr<arr<p4>> parse_rows(arr<str>& words, const char* b, const char* e)
	{
		auto r = arr<arr<p4>>();
		for (int i = 0; i < 13; ++i) { r.push_back({}); }

		auto re_b = "</row>";
		auto re_e = re_b + strlen(re_b);
		auto me_b = "<";
		auto me_e = me_b + strlen(me_b);
		auto cs_b = "t=\"s\"><v>";
		auto cs_e = cs_b + strlen(cs_b);
		auto ce_b = "</v></c>";
		auto ce_e = ce_b + strlen(ce_b);
		auto cb_b = "<v>";
		auto cb_e = cb_b + strlen(cb_b);
		auto cletter_b = "<c r=\"";
		auto cletter_e = cletter_b + strlen(cletter_b);

		for (p4 i = 0;; ++i)
		{
			auto row_e = search(b, e, re_b, re_e);
			if (e == row_e)
				break;

			auto previous_size = r[0].size();
			for (;;)
			{
				b = search(b, row_e, cletter_b, cletter_e);
				if (row_e == b)
					break;
				b += distance(cletter_b, cletter_e);

				char col_letter = *b;
				int j = col_letter - 'A';

				auto col_e = search(b, row_e, ce_b, ce_e);
				if (row_e == col_e)
					break;

				if (!(j < r.size()))
					continue;

				auto& col = r[j];

				auto sb = search(b, col_e, cs_b, cs_e);
				if (col_e == sb)
				{
					b = search(b, col_e, cb_b, cb_e);
					if (col_e == b)
						break;
					b += distance(cb_b, cb_e);
					col.push_back(static_cast<p4>(words.size()));
					auto s = str(b, col_e);
					words.push_back(s);
				}
				else //shared
				{
					b = sb + distance(cs_b, cs_e);
					auto s = str(b, col_e);
					col.push_back(static_cast<p4>(stoi(s)));
				}
				b = col_e + distance(ce_b, ce_e);
			}
			for (int j = 0; j < 13; ++j)
			{
				auto& col = r[j];
				if (col.size() == previous_size)
					col.push_back(p4max);
			}
			b = row_e + distance(re_b, re_e);
		}
		return r;
	}

	arr<str> parse_words(const char* b, const char* e)
	{
		auto r = arr<str>();

		auto me_b = "</t></si>";
		auto me_e = me_b + strlen(me_b);
		auto m1_b = "<si";
		auto m1_e = m1_b + strlen(m1_b);
		auto m2_b = "<t";
		auto m2_e = m2_b + strlen(m2_b);
		auto m3_b = ">";
		auto m3_e = m3_b + strlen(m3_b);
		while (true)
		{
			auto x_e = search(b, e, me_b, me_e);
			if (e == x_e)
				break;

			b = search(b, x_e, m1_b, m1_e);
			if (x_e == b)
				break;
			b += distance(m1_b, m1_e);

			b = search(b, x_e, m2_b, m2_e);
			if (x_e == b)
				break;
			b += distance(m2_b, m2_e);

			b = search(b, x_e, m3_b, m3_e);
			if (x_e == b)
				break;
			b += distance(m3_b, m3_e);
			r.push_back(strip(b, x_e));
			b = x_e + distance(me_b, me_e);
		}
		return r;
	}

	auto valid_yn = arr<str>({ "", "Y", "N" });
	auto valid_contract = arr<str>({ "Prepaid", "Postpaid", "Top Up" });
	auto valid_type = arr<str>({ "Feature Phone", "Smart Phone", "Not Yet Classified Devices", "Tablet", "Basic Phone", "Data Devices", "Other" });
	auto valid_muni = arr<str>
		(
			{
				"Umkhanyakude District Municipality",
				"Nelson Mandela Bay Metropolitan Municipality",
				"West Rand District Municipality",
				"eThekwini Metropolitan Municipality",
				"Waterberg District Municipality",
				"City of Cape Town Metropolitan Municipality",
				"Capricorn District Municipality",
				"Nkangala District Municipality",
				"Mopani District Municipality",
				"Vhembe District Municipality",
				"Dr Kenneth Kaunda District Municipality",
				"Alfred Nzo District Municipality",
				"UMgungundlovu District Municipality",
				"Greater Sekhukhune District Municipality",
				"Amathole District Municipality",
				"City of Johannesburg Metropolitan Municipality",
				"Ehlanzeni District Municipality",
				"Gert Sibande District Municipality",
				"Ekurhuleni Metropolitan Municipality",
				"City of Tshwane Metropolitan Municipality",
				"Zululand District Municipality",
				"John Taolo Gaetsewe District Municipality",
				"Uthungulu District Municipality",
				"Ngaka Modiri Molema District Municipality",
				"Mangaung Metropolitan Municipality",
				"Sedibeng District Municipality",
				"Pixley Ka Seme District Municipality",
				"Bojanala District Municipality",
				"Buffalo City Metropolitan Municipality",
				"iLembe District Municipality",
				"Amajuba District Municipality",
				"Umzinyathi District Municipality",
				"Joe Gqabi District Municipality",
				"Cacadu District Municipality",
				"Uthukela District Municipality",
				"Frances Baard District Municipality",
				"Dr Ruth Segomotsi Mompati District Municipality",
				"Cape Winelands District Municipality",
				"Sisonke District Municipality",
				"Siyanda District Municipality",
				"Overberg District Municipality",
				"O.R.Tambo District Municipality",
				"Fezile Dabi District Municipality",
				"Eden District Municipality",
				"Ugu District Municipality",
				"Lejweleputswa District Municipality",
				"Thabo Mofutsanyane District Municipality",
				"Xhariep District Municipality",
				"West Coast District Municipality",
				"Chris Hani District Municipality",
				"Namakwa District Municipality",
				"Central Karoo District Municipality"
			}
	);

	void process_file(uv_plat& x, const str& path, mobile_phones& phones, funk<void()> fun)
	{
		//"C:\\temp\\Scorebet\\lead1\\lead1 (2).xlsx"
		x.file_readall(path, [&phones, fun, path](str data)
		{
			if (data.empty())
				return;
			auto p = data.c_str();
			//auto headers = arr<pkzip_header>();
			char* shared_strings = nullptr;
			sz shared_strings_len = 0;
			char* sheet = nullptr;
			sz sheet_len = 0;

			auto shared_b = "sharedStrings.xml";
			auto shared_e = shared_b + strlen(shared_b);
			auto sheet_b = "sheet1.xml";
			auto sheet_e = sheet_b + strlen(sheet_b);

			while (0x04034b50 == read_le4(p))
			{
				pkzip pk(&p);
				if (!shared_strings && search(pk.file_name, pk.file_name + pk.file_name_length, shared_b, shared_e) != pk.file_name + pk.file_name_length)
				{
					shared_strings = pk.decompress();
					shared_strings_len = pk.uncompressed_size;
				}
				else if (!sheet && search(pk.file_name, pk.file_name + pk.file_name_length, sheet_b, sheet_e) != pk.file_name + pk.file_name_length)
				{
					sheet = pk.decompress();
					sheet_len = pk.uncompressed_size;
				}
				if (sheet && shared_strings)
					break;
			}

			auto words = parse_words(shared_strings, shared_strings + shared_strings_len);
			auto rows = parse_rows(words, sheet, sheet + sheet_len);

			free(sheet);
			free(shared_strings);

			static constexpr int ROW_PHONE = 0;
			static constexpr int ROW_CONTRACT = 1;
			static constexpr int ROW_ACTIVE = 2;
			static constexpr int ROW_PROVINCE = 3;
			static constexpr int ROW_MUNICIPALITY = 4;
			static constexpr int ROW_CITY = 5;
			static constexpr int ROW_TOWN = 6;
			static constexpr int ROW_SUBURB = 7;
			static constexpr int ROW_VENDOR = 8;
			static constexpr int ROW_MODEL = 9;
			static constexpr int ROW_TYPE = 10;
			static constexpr int ROW_YN = 11;
			static constexpr int ROW_BANDWIDTH = 12;

			pack<p4> invalid_rows;
			invalid_rows.resize(rows[ROW_PHONE].size());

			auto mark_invalid_fun = [&words, path](pack<p4>& invalid_rows, const arr<p4>& cols, const arr<str>& allowed)
			{
				return accumulate(cols.cbegin(), cols.cend(), 0, [&words, path, i = 0, &allowed, &invalid_rows](int acc, p4 id) mutable
				{
					++i;
					if (!(id < words.size()))
						return acc;
					auto& word = words[id];
					auto invalid = invalid_rows.at(i - 1);
					if (invalid)
						return acc;
					auto found_valid = find(allowed.cbegin(), allowed.cend(), word);
					if (allowed.cend() == found_valid)
					{
						invalid = 1;
						//cout << "! " << path << " " << i << "[" << word << "]\n";
						++acc;
					}
					return acc;
				});
			};

			mark_invalid_fun(invalid_rows, rows[ROW_YN], valid_yn);
			mark_invalid_fun(invalid_rows, rows[ROW_CONTRACT], valid_contract);
			mark_invalid_fun(invalid_rows, rows[ROW_TYPE], valid_type);
			mark_invalid_fun(invalid_rows, rows[ROW_MUNICIPALITY], valid_muni);

			auto process_section_fun = [&rows, &phones, &words, path](sz index_b, sz index_e)
			{
				for_each(rows[ROW_PHONE].cbegin() + index_b, rows[ROW_PHONE].cbegin() + index_e, [&phones, &words](p4 id)
				{
					phones.fld_phone_number.push_back(0 <= id && id < words.size() ? words[id] : "");
					//fld.push_back(txt.add_without_check(words[id]));
				});

				//auto dmp_phone = phones.fld_phone_number.dump();

				auto process = [&words, index_b, index_e](const arr<p4>& cols, lookup_field& fld)
				{
					hmap<p4, p4> shared_to_col;
					//fld.push_back("");
					shared_to_col[p4max] = 0;
					for_each(cols.cbegin() + index_b, cols.cbegin() + index_e, [&fld, &shared_to_col, &words](p4 id)
					{
						auto found = shared_to_col.find(id);
						if (shared_to_col.end() == found)
							shared_to_col[id] = fld.add_to_dictionary(words[id]);
					});
					for_each(cols.cbegin() + index_b, cols.cbegin() + index_e, [&shared_to_col, &fld](p4 id)
					{
						fld.add_existing_word_id(shared_to_col[id]);
					});
				};

				auto process_and_log = [&words, path, index_b, index_e](const arr<p4>& cols, lookup_field& fld)
				{
					hmap<p4, p4> shared_to_col;
					//fld.push_back("");
					shared_to_col[p4max] = 0;
					for_each(cols.cbegin() + index_b, cols.cbegin() + index_e, [&fld, &shared_to_col, &words, path, i = 0](p4 id) mutable
					{
						++i;
						auto found = shared_to_col.find(id);
						if (shared_to_col.end() == found)
						{
							auto current_size = fld.references_size();
							shared_to_col[id] = fld.add_to_dictionary(words[id]);
							if (fld.references_size() != current_size)
							{
								cout << "-> " << path << " " << i << " [" << words[id] << "]\n";
							}
						}
					});
					for_each(cols.cbegin() + index_b, cols.cbegin() + index_e, [&shared_to_col, &fld](p4 id)
					{
						fld.add_existing_word_id(shared_to_col[id]);
					});
				};

				//auto& bandwidth = rows[12];
				//auto found_empty = find(bandwidth.cbegin(), bandwidth.cend(), p4max);
				//sz valid_size = bandwidth.cend() != found_empty ? distance(bandwidth.cbegin(), found_empty) : bandwidth.size();

				process(rows[ROW_CONTRACT], phones.fld_contract);
				//auto dmp_contract = phones.fld_contract.dump();

				process(rows[ROW_ACTIVE], phones.fld_active);
				process(rows[ROW_PROVINCE], phones.fld_province);
				process(rows[ROW_MUNICIPALITY], phones.fld_municipality);
				process(rows[ROW_CITY], phones.fld_city);
				process(rows[ROW_TOWN], phones.fld_town);
				process(rows[ROW_SUBURB], phones.fld_suburb);
				process(rows[ROW_VENDOR], phones.fld_vendor);
				process(rows[ROW_MODEL], phones.fld_model);
				process(rows[ROW_TYPE], phones.fld_type);
				process_and_log(rows[ROW_YN], phones.fld_yn);
				process(rows[ROW_BANDWIDTH], phones.fld_bandwidth);

				p4 file_name_id = phones.fld_source_file.add_to_dictionary(path);
				auto& source_file = phones.fld_source_file;
				auto& source_line = phones.fld_line_number;
				for (auto file_name_index = index_b; file_name_index != index_e; ++file_name_index)
				{
					source_file.add_existing_word_id(file_name_id);
					source_line.push_back(static_cast<sz>(file_name_index));
				}
			};

			auto invalid_b = invalid_rows.cbegin();
			for (;;)
			{
				invalid_b = find(invalid_b, invalid_rows.cend(), 0);
				auto invalid_e = find_if_not(invalid_b, invalid_rows.cend(), [](p4 x) { return 0 == x; });
				if (invalid_b == invalid_e)
					break;
				process_section_fun(distance(invalid_rows.cbegin(), invalid_b), distance(invalid_rows.cbegin(), invalid_e));
				invalid_b = invalid_e;
			}


			fun();
			//for (int i = 0;; ++i)
			//{
			//	if (i >= fields[0].size())
			//		break;
			//	for (int j = 0; j < 13; ++j)
			//	{
			//		auto col = fields[j][i];
			//		cout << text_maps[j][col] << " | ";
			//	}
			//	cout << "\n";
			//	i += 10000;
			//}
			//headers.push_back(&p);

		});
	}

	void process_files(arr<str> files, uv_plat& x, mobile_phones& phones, funk<void()> fun)
	{
		if (files.empty())
		{
			fun();
			return;
		}
		str file = files.back();
		cout << "***   " << phones.fld_phone_number.size() << " " << file << "\n";
		files.pop_back();
		process_file(x, file, phones, [files, &x, &phones, fun]()
		{
			process_files(files, x, phones, fun);
		});
	}

	void accumulate_files(uv_plat& x, arr<str> paths, const arr<str>& files, funk<void(const arr<str>&)> fun)
	{
		if (paths.empty())
		{
			fun(files);
			return;
		}
		auto path = paths.back();
		paths.pop_back();
		x.list_files(path, [path, &x, paths, files, fun](auto&& new_files, auto&&)
		{
			auto old_files = files;
			old_files.reserve(old_files.size() + new_files.size());
			transform(new_files.cbegin(), new_files.cend(), back_inserter(old_files), [path](auto&& file)
			{
				return path + "\\" + file;
			});
			accumulate_files(x, paths, old_files, fun);
		});
	}

	void store_test()
	{
		auto scheduler = std_task_scheduler();
		uv_plat x(scheduler, "");

		mobile_phones phones;
		process_file(x, R"(c:\temp\toets1.xlsx)", phones, []()
		{}
		);
		getchar();
	}
	void store_numbers()
	{
		auto scheduler = std_task_scheduler();
		uv_plat x(scheduler, "");

		mobile_phones phones;

		accumulate_files(x, { "C:\\temp\\Scorebet\\lead1", "C:\\temp\\Scorebet\\leads2", "C:\\temp\\Scorebet\\leads3" }, {}, [&x, &phones](auto&& files)
		{
			//for_each(files.cbegin(), files.cend(), [&x, &text_maps, &fields](auto&& file)
			//{
			//	cout << file << "\n";
			//});
			auto file = files.front();
			process_files(files, x, phones, [&phones, &x]()
			{
				//for (int i = 0; i < 100; ++i)
				//{
				//	if (i >= phones.fld_phone_number.size())
				//		break;
				//	cout << i << ": " << phones.format_line(i, 3) << endl;
				//}

				cout << __LINE__ << " - " << "processing\n";
				auto size_on_disk = phones.size_on_disk();

				auto r = str("");
				r.resize(size_on_disk, 0);

				phones.serialise(const_cast<char*>(r.data()));
				x.file_writeall("c:\\temp\\phone1.bin", move(r), [](bool success)
				{
					cout << __LINE__ << " - " << (success ? "success" : "error") << "\n";
				});
				//mobile_phones newphones;
				//newphones.deserialise(r.data());


				//for (int i = 0; i < fields[0].size(); ++i)
				//{
				//	for (int j = 0; j < 13; ++j)
				//	{
				//		r += "\"" + text_maps[j][fields[j][i]] + "\";";
				//	}
				//	r += "\n";
				//}
				//cout << fields[0].size() << " " << to_string(r.size()) << " done\n";
				//x.file_writeall("c:\\temp\\hans.csv", move(r), [](bool success)
				//{
				//	cout << (success ? "file written\n" : "file error\n");
				//});
			});
		});

		getchar();
	}

	void restore_numbers()
	{
		auto scheduler = std_task_scheduler();
		uv_plat x(scheduler, "");
		x.file_readall("c:\\temp\\phone1.bin", [&x](str data)
		{
			mobile_phones newphones;
			newphones.deserialise(data.data());
			cout << "restored " << newphones.fld_phone_number.size() << "\n";
			cout << "fld_active " << newphones.fld_active.bits() << "\n";
			cout << "fld_bandwidth " << newphones.fld_bandwidth.bits() << "\n";
			cout << "fld_contract " << newphones.fld_contract.bits() << "\n";
			cout << "fld_model " << newphones.fld_model.bits() << "\n";
			cout << "fld_type " << newphones.fld_type.bits() << "\n";
			cout << "fld_vendor " << newphones.fld_vendor.bits() << "\n";
			cout << "fld_yn " << newphones.fld_yn.bits() << "\n";

			auto stats_fun = [](lookup_field& fld)
			{
				auto stats = fld.stats();
				for_each(stats.cbegin(), stats.cend(), [](auto&& line)
				{
					cout << line.second << "\t" << line.first << "\n";
				});
			};

			auto stats_by_count_fun = [](lookup_field& fld)
			{
				auto stats = fld.stats();
				sort(stats.begin(), stats.end(), [](auto&& l, auto&& r)
				{
					return l.second < r.second;
				});
				for_each(stats.cbegin(), stats.cend(), [](auto&& line)
				{
					cout << line.second << "\t" << line.first << "\n";
				});
			};

			cout << "***   yn   ***\n";
			stats_fun(newphones.fld_yn);
			cout << "***   yn   ***\n";

			cout << "***   active   ***\n";
			stats_fun(newphones.fld_active);
			cout << "***   active   ***\n";

			cout << "***   bandwidth   ***\n";
			stats_fun(newphones.fld_bandwidth);
			cout << "***   bandwidth   ***\n";

			cout << "***   contract   ***\n";
			stats_fun(newphones.fld_contract);
			cout << "***   contract   ***\n";

			cout << "***   type   ***\n";
			stats_fun(newphones.fld_type);
			cout << "***   type   ***\n";


			cout << "***   municipality   ***\n";
			stats_fun(newphones.fld_municipality);
			cout << "***   municipality   ***\n";
			cout << "***   province   ***\n";
			stats_fun(newphones.fld_province);
			cout << "***   province   ***\n";
			//cout << "***   city   ***\n";
			//stats_fun(newphones.fld_city);
			//cout << "***   city   ***\n";

			//cout << "***   vendor   ***\n";
			//stats_by_count_fun(newphones.fld_vendor);
			//cout << "***   vendor   ***\n";

			//cout << "***   model   ***\n";
			//stats_fun(newphones.fld_model);
			//cout << "***   model   ***\n";
			//cout << "***   suburb   ***\n";
			//stats_fun(newphones.fld_suburb);
			//cout << "***   suburb   ***\n";
			//cout << "***   town   ***\n";
			//stats_fun(newphones.fld_town);
			//cout << "***   town   ***\n";

			//auto vendor_agcellular = newphones.fld_vendor.reference_index_for("AG Cellular");
			//auto vendor_blackberry = newphones.fld_vendor.reference_index_for("BlackBerry");
			//assert(newphones.fld_phone_number.size() == newphones.fld_vendor.size());
			//for (int i = 0; i < newphones.fld_phone_number.size(); ++i)
			//{
			//	auto vendor_id = newphones.fld_vendor[i];
			//	if (/*vendor_id == vendor_agcellular ||*/ vendor_id == vendor_blackberry)
			//	{
			//		auto model = newphones.fld_model.extract(i, 10);
			//		if (model == "BlackBerry" || model == "Blackberry")
			//			continue;
			//		cout << to_string(i) << " " << newphones.format_line(i, 10) << "\n";
			//	}
			//}

			str body = newphones.vendor_model_type_contract_bandwith();
			x.file_writeall(R"(c:\temp\fstats.csv)", move(body), [](bool success)
			{
				if (success)
					cout << "file written successfully\n";
				else
					cout << "error writing file";
			});
		});

		getchar();
	}
}

int main(int argc, char** argv)
{
	ai::restore_numbers();
	//ai::store_numbers();
	//ai::store_test();
	return 0;
}