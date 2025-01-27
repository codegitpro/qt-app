// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "folder_view.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@protocol AIFolderView;

namespace djinni_generated {

class FolderView
{
public:
    using CppType = std::shared_ptr<::ai::FolderView>;
    using CppOptType = std::shared_ptr<::ai::FolderView>;
    using ObjcType = id<AIFolderView>;

    using Boxed = FolderView;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

