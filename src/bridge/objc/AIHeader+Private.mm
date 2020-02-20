// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIHeader+Private.h"
#import "DJIMarshal+Private.h"
#include <cassert>

namespace djinni_generated {

auto Header::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni::String::toCpp(obj.name),
            ::djinni::String::toCpp(obj.value)};
}

auto Header::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[AIHeader alloc] initWithName:(::djinni::String::fromCpp(cpp.name))
                                    value:(::djinni::String::fromCpp(cpp.value))];
}

}  // namespace djinni_generated
