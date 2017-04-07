
#include <stdio.h>
#include <string.h>
#include <math.h>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include <nanosvg/nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg/nanosvgrast.h>

#include <gos/view/svg_texture.h>


namespace gos {
namespace view {

svg_texture::svg_texture(
  const std::string & filename,
  int                 dpi)
: _filename(filename)
{
  _svg_image = nsvgParseFromFile(
                 _filename.c_str(), "px", 96.0);
  if (_svg_image == nullptr) {
    GOS__LOG_ERROR("svg_texture",
                   "failed to parse SVG from file " << _filename);
    return;
  }
  _extents = { (int)_svg_image->width,
               (int)_svg_image->height };

  GOS__LOG_DEBUG("svg_texture", "file: " << _filename << " " <<
                 "svg size: " <<
                 "(" << _svg_image->width <<
                 "," << _svg_image->height << ")");

  _rasterizer = nsvgCreateRasterizer();
  if (_rasterizer == nullptr) {
    GOS__LOG_ERROR("svg_texture",
                   "failed to create rasterizer for file " << _filename);
    return;
  }

  _img_data.resize(_extents.w * _extents.h * 4);

  nsvgRasterize(_rasterizer,
                _svg_image, 0,0,1,
                _img_data.data(),
                _extents.w, _extents.h,
                _extents.w * 4);

  _surface = SDL_CreateRGBSurfaceFrom(
               static_cast<void *>(_img_data.data()),
               _extents.w, _extents.h,
               32,              // depth
               4 * _extents.w,  // pitch
               0,               // Rmask
               0,               // Gmask
               0,               // Bmask
               0                // Amask
             );
  SDL_SetSurfaceBlendMode(
    _surface, SDL_BLENDMODE_BLEND);
}

svg_texture::~svg_texture() {
  if (_rasterizer != nullptr) {
    nsvgDeleteRasterizer(_rasterizer);
  }
  if (_svg_image != nullptr) {
    nsvgDelete(_svg_image);
  }
  if (_surface != nullptr) {
    SDL_FreeSurface(_surface);
  }
}

} // namespace view
} // namespace gos
