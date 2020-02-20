#include "../test/catch.h"
#include "fr24_login.h"

namespace ai
{
	namespace fr24
	{
		TEST_CASE("cookie")
		{
			REQUIRE("__cfduid=dcf9c32e5e4c3fcccea5e0caef12b2ee91537692705" == login::cookie("__cfduid=dcf9c32e5e4c3fcccea5e0caef12b2ee91537692705; expires=Mon, 23-Sep-19 08:51:45 GMT; path=/; domain=.flightradar24.com; HttpOnly"));
			REQUIRE("FR24ID=03pfqn76l3cbu4lfn1mdbl5u9kj5nuo52olk3abtv1add2nv17a0" == login::cookie("FR24ID=03pfqn76l3cbu4lfn1mdbl5u9kj5nuo52olk3abtv1add2nv17a0; path=/; domain=.flightradar24.com; secure; HttpOnly"));

		}
	}
}