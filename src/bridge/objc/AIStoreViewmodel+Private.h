// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "store_viewmodel.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class AIStoreViewmodel;

namespace djinni_generated {

class StoreViewmodel
{
public:
    using CppType = std::shared_ptr<::ai::StoreViewmodel>;
    using CppOptType = std::shared_ptr<::ai::StoreViewmodel>;
    using ObjcType = AIStoreViewmodel*;

    using Boxed = StoreViewmodel;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

