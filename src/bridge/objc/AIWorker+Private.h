// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#include "worker.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@protocol AIWorker;

namespace djinni_generated {

class Worker
{
public:
    using CppType = std::shared_ptr<::ai::Worker>;
    using CppOptType = std::shared_ptr<::ai::Worker>;
    using ObjcType = id<AIWorker>;

    using Boxed = Worker;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

