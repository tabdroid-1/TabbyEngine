#include <Tabby/Math/Random.h>

namespace Tabby {

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

double Random::Range(double start, double end)
{
    std::uniform_real_distribution<double> dist(start, end);
    return dist(s_Engine);
}

}
