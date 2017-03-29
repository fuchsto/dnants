
#include <gos/util/random.h>
#include <random>


namespace gos {

static std::random_device rand_dev_;
static std::mt19937       rand_gen_(rand_dev_());

size_t random() {
  return rand_gen_();
}

} // namespace gos
