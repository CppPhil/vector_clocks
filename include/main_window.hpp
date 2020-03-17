#pragma once
#include <fstream>

#include <QMainWindow>

#include <pl/annotations.hpp>

#include "logger.hpp"

namespace vc {
/**
 * Type for the window widget in the GUI.
 */
class main_window : public QMainWindow {
  Q_OBJECT

public:
  /**
   * Creates the main_window.
   * @param parent The QWidget parent.
   *
   * Opens the log file.
   * Initializes the logger.
   * Creates the "Run"-button.
   */
  explicit main_window(QWidget* parent = PL_NO_PARENT);

private:
  /**
   * Callback routine for the button's OnClick event.
   */
  void on_button_click();

  std::ofstream ofs_; /**< The output filestream for the logger */
  logger logger_;
};
} // namespace vc
