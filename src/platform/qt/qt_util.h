#pragma once
#include "../../general.h"
#include <QStandardItem>

namespace ai
{
	struct file_desc
	{
		n4 folder_id;
		n4 file_id;

		bool is_file() const { return -1 != file_id; }
		bool is_folder() const { return -1 != folder_id; }
	};

	static file_desc cookie_from_item(QStandardItem* item)
	{
		if (!item)
			return { -1,-1 };
		auto v = item->data();
		if (!v.isValid())
			return { -1,-1 };
		p8 x = v.toULongLong();
		file_desc r;
		r.folder_id = static_cast<n4>(x >> 32);
		r.file_id = static_cast<n4>(x);
		return r;
	}
}