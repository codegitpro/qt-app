// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIFilesDetail+Private.h"
#import "AIFileState+Private.h"
#import "DJIMarshal+Private.h"
#include <cassert>

namespace djinni_generated {

auto FilesDetail::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni::List<::djinni::I32>::toCpp(obj.ids),
            ::djinni::List<::djinni::I32>::toCpp(obj.titles),
            ::djinni::List<::djinni::I64>::toCpp(obj.sizes),
            ::djinni::List<::djinni::I32>::toCpp(obj.percentages),
            ::djinni::List<::djinni::Enum<::ai::FileState, AIFileState>>::toCpp(obj.states),
            ::djinni::List<::djinni::Binary>::toCpp(obj.thumbnails)};
}

auto FilesDetail::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[AIFilesDetail alloc] initWithIds:(::djinni::List<::djinni::I32>::fromCpp(cpp.ids))
                                       titles:(::djinni::List<::djinni::I32>::fromCpp(cpp.titles))
                                        sizes:(::djinni::List<::djinni::I64>::fromCpp(cpp.sizes))
                                  percentages:(::djinni::List<::djinni::I32>::fromCpp(cpp.percentages))
                                       states:(::djinni::List<::djinni::Enum<::ai::FileState, AIFileState>>::fromCpp(cpp.states))
                                   thumbnails:(::djinni::List<::djinni::Binary>::fromCpp(cpp.thumbnails))];
}

}  // namespace djinni_generated
