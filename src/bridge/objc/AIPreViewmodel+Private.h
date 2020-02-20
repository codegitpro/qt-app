// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "pre_viewmodel.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class AIPreViewmodel;

namespace djinni_generated {

class PreViewmodel
{
public:
    using CppType = std::shared_ptr<::ai::PreViewmodel>;
    using CppOptType = std::shared_ptr<::ai::PreViewmodel>;
    using ObjcType = AIPreViewmodel*;

    using Boxed = PreViewmodel;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated
