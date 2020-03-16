#pragma once
#include <fstream>

#include "logger.hpp"
#include "ui_main_window.h"

namespace vc {
class main_window : public QMainWindow {
  Q_OBJECT

public:
  explicit main_window(QWidget* parent = nullptr);

private:
  void on_button_click();

  Ui::MainWindow ui_;
  std::ofstream ofs_;
  logger logger_;
};
} // namespace vc
