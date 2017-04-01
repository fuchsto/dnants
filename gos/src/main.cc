
#include <gos/app_engine.h>
#include <gos/app_play_state.h>

#include <cstdlib>
#include <getopt.h>


using gos::app_engine;
using gos::app_play_state;
using gos::view::window;

gos::app_engine::app_settings process_args(int argc, char** argv);

int main(int argc, char * argv[])
{
  auto app_options = process_args(argc, argv);

  app_engine app(
    "game'-._.of'._.survive",
    app_options,
    900, 900);

  app.run(app_play_state::get());

  while (app.is_running()) {
    app.handle_events();
    app.update();
    app.draw();
  }

  return EXIT_SUCCESS;
}

gos::app_engine::app_settings process_args(int argc, char** argv)
{
  gos::app_engine::app_settings app_opts;

  app_opts.grid_extents   = { 30, 30 };
  app_opts.rounds_per_sec = 10;
  app_opts.init_team_size = 12;
  app_opts.num_teams      = 1;
  app_opts.trace_rounds   = 20;

  const char* const short_opts = "g:w:h:t:n:s";
  const option long_opts[] = {
          {"grid",       1, nullptr, 'g'},
          {"grid-w",     1, nullptr, 'w'},
          {"grid-h",     1, nullptr, 'h'},
          {"trace-time", 1, nullptr, 't'},
          {"num-teams",  1, nullptr, 'n'},
          {"team-size",  1, nullptr, 's'},
          { nullptr,     0, nullptr,  0 }
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
      case 't' : app_opts.trace_rounds   = std::atoi(optarg);
                 break;
      case 'n' : app_opts.num_teams      = std::atoi(optarg);
                 break;
      case 's' : app_opts.init_team_size = std::atoi(optarg);
                 break;
      default:   break;
    }
  }
  return app_opts;
}

