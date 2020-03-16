#pragma once
#include <fstream>

#include <QMainWindow>

#include "logger.hpp"

namespace vc {
class main_window : public QMainWindow {
  Q_OBJECT

public:
  explicit main_window(QWidget* parent = nullptr);

private:
  void on_button_click();

  std::ofstream ofs_;
  logger logger_;
};
} // namespace vc
