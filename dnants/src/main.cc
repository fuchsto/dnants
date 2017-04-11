
#include <gos/app_engine.h>
#include <gos/app_play_state.h>

#include <gos/types.h>
#include <gos/state/ant_state.h>
#include <gos/util/logging.h>

#include <cstdlib>
#if defined(_POSIX_VERSION)
#include <getopt.h>
#endif

#include <pybind11/embedded.h>



using gos::app_engine;
using gos::app_play_state;
using gos::view::window;

using namespace gos;
using namespace gos::state;

namespace py = pybind11;


gos::app_settings process_args(int argc, char** argv);

int main(int argc, char * argv[])
{
  py::scoped_interpreter guard { };

  auto app_options = process_args(argc, argv);

  app_engine app(
    "game'-._.of'._.survive",
    app_options);

  app.run(app_play_state::get());

  while (app.is_running()) {
    app.handle_events();
    app.update();
    app.draw();
  }

  return EXIT_SUCCESS;
}

gos::app_settings process_args(int argc, char** argv)
{
  gos::app_settings app_opts;

  app_opts.grid_extents   = { 23, 23 };
  app_opts.grid_spacing   = 32;
  app_opts.rounds_per_sec =  4;
  app_opts.frames_per_sec = 30;
  app_opts.init_team_size =  5;
  app_opts.num_teams      =  0;
  app_opts.trace_rounds   = 20;
  app_opts.show_grid      = false;
  app_opts.show_traces    = true;

  bool use_32px           = false;
#if defined(_POSIX_VERSION)
  const char* const short_opts = "g:w:h:r:t:s:b";
  const option long_opts[] = {
          {"grid",         1, nullptr, 'g'},
          {"grid-w",       1, nullptr, 'w'},
          {"grid-h",       1, nullptr, 'h'},
          {"trace-rounds", 1, nullptr, 'r'},
          {"team",         1, nullptr, 't'},
          {"team-size",    1, nullptr, 's'},
          {"big",          0, nullptr, 'b'},
          { nullptr,       0, nullptr,  0 }
  };
  while (true) {
    const auto opt = getopt_long(
                       argc, argv,
                       short_opts, long_opts,
                       nullptr);
    if (-1 == opt) { break; }

    switch (opt) {
      case 'g' : app_opts.grid_extents.w = std::atoi(optarg);
                 app_opts.grid_extents.h = std::atoi(optarg);
                 break;
      case 'w' : app_opts.grid_extents.w = std::atoi(optarg);
                 break;
      case 'h' : app_opts.grid_extents.h = std::atoi(optarg);
                 break;
      case 'r' : app_opts.trace_rounds   = std::atoi(optarg);
                 break;
      case 't' : app_opts.team_codes.push_back(optarg);
                 break;
      case 's' : app_opts.init_team_size = std::atoi(optarg);
                 break;
      case 'b' : app_opts.grid_spacing   = 32;
                 use_32px                = true;
                 break;
      default:   break;
    }
  }
  app_opts.num_teams = app_opts.team_codes.size();
#endif
  if (app_opts.grid_extents.w > 30 ||
      app_opts.grid_extents.h > 23) {
    app_opts.grid_spacing = 16;
  }
  if (use_32px) {
    app_opts.grid_spacing = 32;
  }
  return app_opts;
}

