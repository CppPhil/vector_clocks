#include <yaml-cpp/yaml.h>

#include <jaegertracing/Tracer.h>

#include "setup_tracer.hpp"

namespace vc {
void setup_tracer(pl::string_view config_filepath) {
  auto config_yaml = YAML::LoadFile(config_filepath.to_string());
  auto config = jaegertracing::Config::parse(config_yaml);
  auto tracer = jaegertracing::Tracer::make(
    "vector-clocks-app", config, jaegertracing::logging::consoleLogger());

  opentracing::Tracer::InitGlobal(
    std::static_pointer_cast<opentracing::Tracer>(tracer));
}
} // namespace vc
