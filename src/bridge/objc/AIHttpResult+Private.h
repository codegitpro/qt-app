// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "http_result.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class AIHttpResult;

namespace djinni_generated {

class HttpResult
{
public:
    using CppType = std::shared_ptr<::ai::HttpResult>;
    using CppOptType = std::shared_ptr<::ai::HttpResult>;
    using ObjcType = AIHttpResult*;

    using Boxed = HttpResult;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

