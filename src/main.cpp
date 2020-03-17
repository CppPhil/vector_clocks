#include <climits>

#include "main_window.hpp"

#include <QApplication>

/**
 * Entry point of the application.
 * @param argc Argument count.
 * @param argv Command line arguments.
 * @return 0 on success; otherwise a non-zero error code.
 */
int main(int argc, char* argv[]) {
  static_assert(CHAR_BIT == 8,
                "A byte doesn't contain 8 bits on this platform.");

  QApplication application(argc, argv);
  vc::main_window main_window;

  main_window.show();

  return QApplication::exec();
}
