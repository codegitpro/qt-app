// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "http_progress_result.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class AIHttpProgressResult;

namespace djinni_generated {

class HttpProgressResult
{
public:
    using CppType = std::shared_ptr<::ai::HttpProgressResult>;
    using CppOptType = std::shared_ptr<::ai::HttpProgressResult>;
    using ObjcType = AIHttpProgressResult*;

    using Boxed = HttpProgressResult;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

