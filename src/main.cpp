#include <climits>

#include <QApplication>

#include <jaegertracing/Tracer.h>

#include "main_window.hpp"
#include "setup_tracer.hpp"

/**
 * Entry point of the application.
 * @param argc Argument count.
 * @param argv Command line arguments.
 * @return 0 on success; otherwise a non-zero error code.
 */
int main(int argc, char* argv[]) {
  static_assert(CHAR_BIT == 8,
                "A byte doesn't contain 8 bits on this platform.");

  constexpr auto expected_argc = 2;
  constexpr auto config_file_path_index = 1;

  if (argc != expected_argc)
    return -1;

  vc::setup_tracer(argv[config_file_path_index]);

  QApplication application(argc, argv);
  vc::main_window main_window;

  main_window.show();

  opentracing::Tracer::Global()->Close();
  return QApplication::exec();
}
