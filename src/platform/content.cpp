#include "content.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4100 )
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb/stb_image.h"

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../deps/stb/stb_image_resize.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../deps/stb/stb_image_write.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace ai
{
    str content_type_from_path(const str& path)
    {
        auto ext = filename_extension(path);
        if (ext == "JPG" || ext == "JPEG")
            return "image/jpeg";
        if (ext == "PNG")
            return "image/png";
        if (ext == "TXT")
            return "text/plain";
        if (ext == "MP4")
            return "video/mp4";
        if (ext == "GIF")
            return "image/gif";
        if (ext == "HEIC")
            return "image/heic";
        if (ext == "MOV")
            return "video/quicktime";
        if ("HEIF" == ext)
            return "image/heif";
        return "";
    }

    str filename_extension(const str& name)
    {
        auto it = std::find(name.crbegin(), name.crend(), '.');
        if (name.crend() == it)
            return "";
        
        auto ext = str(it.base(), name.cend());
        transform(ext.begin(), ext.end(), ext.begin(), [](char c) { return 'a' <= c && c <= 'z' ? c - 'a' + 'A' : c; });
        return ext;
    }
    
    void create_thumbnail(str path, plat& platform, funk<void(arr<p1>)> fun)
    {
        platform.file_readall(path, [fun](arr<p1> content)
                              {
                                  if (content.empty())
                                  {
                                      fun({});
                                      return;
                                  }
                                  
                                  int x;
                                  int y;
                                  int channels;
                                  auto bmp = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(content.data()), static_cast<int>(content.size()), &x, &y, &channels, STBI_default);
                                  if (!bmp)
                                  {
                                      fun({});
                                      return;
                                  }
                                  int thumb_w = 24;
                                  int thumb_h = 24;
                                  if (x < y)
                                      thumb_w = static_cast<int>(round((24.0 * x) / double(y)));
                                  else if (y < x)
                                      thumb_h = static_cast<int>(round((24.0 * y) / double(x)));
                                  auto pixels = arr<unsigned char>();
                                  pixels.resize(thumb_w*thumb_h*channels);
                                  int success = stbir_resize_uint8(bmp, x, y, 0, pixels.data(), thumb_w, thumb_h, 0, channels);
                                  if (success)
                                  {
                                      int png_mem_size;
                                      auto png_mem = stbi_write_png_to_mem(pixels.data(), 0, thumb_w, thumb_h, channels, &png_mem_size);
                                      if (png_mem)
                                      {
                                          auto thumbnail = arr<p1>();
                                          copy(png_mem, png_mem + png_mem_size, back_inserter(thumbnail));
                                          fun(move(thumbnail));
                                          //add_task([this, thumbnail = move(thumbnail), file_id](auto dir)
                                          //{
                                          //    dir->thumbnail(file_id, thumbnail);
                                          //    callback_->file_change(file_id, *dir);
                                          //});
                                          free(png_mem);
                                      }
                                      else
                                          fun({});
                                  }
                                  else
                                      fun({});
                                  free(bmp);
                              });
    }

}
