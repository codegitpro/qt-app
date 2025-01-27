// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#import "AIFolderViewmodel+Private.h"
#import "AIFolderViewmodel.h"
#import "AIFolderView+Private.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface AIFolderViewmodel ()

- (id)initWithCpp:(const std::shared_ptr<::ai::FolderViewmodel>&)cppRef;

@end

@implementation AIFolderViewmodel {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::ai::FolderViewmodel>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::ai::FolderViewmodel>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (void)onLoad:(nullable id<AIFolderView>)view {
    try {
        _cppRefHandle.get()->on_load(::djinni_generated::FolderView::toCpp(view));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (int32_t)folderId {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->folder_id();
        return ::djinni::I32::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nonnull NSData *)listThumbnailContent:(int32_t)row {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->list_thumbnail_content(::djinni::I32::toCpp(row));
        return ::djinni::Binary::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (BOOL)listSelected:(int32_t)section
                 row:(int32_t)row {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->list_selected(::djinni::I32::toCpp(section),
                                                                 ::djinni::I32::toCpp(row));
        return ::djinni::Bool::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)listAction:(int32_t)section
               row:(int32_t)row
          subIndex:(int32_t)subIndex {
    try {
        _cppRefHandle.get()->list_action(::djinni::I32::toCpp(section),
                                         ::djinni::I32::toCpp(row),
                                         ::djinni::I32::toCpp(subIndex));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)uploadFileOnly:(nonnull NSString *)filename
                  path:(nonnull NSString *)path
             thumbnail:(nonnull NSData *)thumbnail {
    try {
        _cppRefHandle.get()->upload_file_only(::djinni::String::toCpp(filename),
                                              ::djinni::String::toCpp(path),
                                              ::djinni::Binary::toCpp(thumbnail));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)uploadFilesOnly:(nonnull NSArray<NSString *> *)filenames
                  paths:(nonnull NSArray<NSString *> *)paths
             thumbnails:(nonnull NSArray<NSData *> *)thumbnails {
    try {
        _cppRefHandle.get()->upload_files_only(::djinni::List<::djinni::String>::toCpp(filenames),
                                               ::djinni::List<::djinni::String>::toCpp(paths),
                                               ::djinni::List<::djinni::Binary>::toCpp(thumbnails));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto FolderViewmodel::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto FolderViewmodel::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<AIFolderViewmodel>(cpp);
}

}  // namespace djinni_generated

@end
