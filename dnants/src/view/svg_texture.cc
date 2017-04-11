
#include <stdio.h>
#include <string.h>
#include <math.h>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include <nanosvg/nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg/nanosvgrast.h>

#include <gos/view/svg_texture.h>

#include <sstream>


namespace gos {
namespace view {

svg_texture::svg_texture(
  const std::string & filename,
  int                 dpi)
: _filename(filename)
{
  std::ostringstream ss;
  ss << "res/" << _filename;
  auto filepath = ss.str();

  _svg_image = nsvgParseFromFile(
                 filepath.c_str(), "px", 96.0);
  if (_svg_image == nullptr) {
    GOS__LOG_ERROR("svg_texture",
                   "failed to parse SVG from file " << filepath);
    return;
  }
  _extents = { (int)_svg_image->width,
               (int)_svg_image->height };

  GOS__LOG_DEBUG("svg_texture", "file: " << filepath << " " <<
                 "svg size: " <<
                 "(" << _svg_image->width <<
                 "," << _svg_image->height << ")");

  _rasterizer = nsvgCreateRasterizer();
  if (_rasterizer == nullptr) {
    GOS__LOG_ERROR("svg_texture",
                   "failed to create rasterizer for file " << filepath);
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
               0x000000FF,      // red mask
               0x0000FF00,      // green mask
               0x00FF0000,      // blue mask
               0xFF000000       // alpha mask (alpha in this format)
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

void svg_texture::render(
  SDL_Renderer   * renderer,
  const SDL_Rect & rect) const
{
  SDL_SetRenderDrawBlendMode(
    renderer,
    SDL_BLENDMODE_BLEND);
  SDL_Texture * texture =
    SDL_CreateTextureFromSurface(renderer, _surface);
  SDL_RenderCopy(renderer, texture, 0, &rect);
  SDL_DestroyTexture(texture);
}

} // namespace view
} // namespace gos
