#ifndef GOS__VIEW__SVG_TEXTURE_H__INCLUDED
#define GOS__VIEW__SVG_TEXTURE_H__INCLUDED

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <gos/types.h>
#include <gos/util/logging.h>

#include <SDL.h>

#include <string>
#include <vector>

struct NSVGimage;
struct NSVGrasterizer;

namespace gos {
namespace view {

class svg_texture {
  NSVGimage          * _svg_image  = nullptr;
	NSVGrasterizer     * _rasterizer = nullptr;
	SDL_Surface        * _surface    = nullptr;
  std::vector<Uint8>   _img_data;
  gos::extents         _extents { };
  std::string          _filename;

 public:
  svg_texture(
    const std::string & filename,
    int                 dpi = 0);

  ~svg_texture();

  SDL_Surface * surface() {
    return _surface;
  }

  void render(SDL_Renderer * renderer, const SDL_Rect & rect) const;
};

} // namespace view
} // namespace gos

#endif // GOS__VIEW__SVG_TEXTURE_H__INCLUDED
