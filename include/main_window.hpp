#pragma once
#include "ui_main_window.h"
#include <QPixmap> // QPixmap
#include <memory>  // std::unique_ptr

namespace vc {
class main_window : public QMainWindow {
  Q_OBJECT

public:
  explicit main_window(QWidget* parent = nullptr);

  Ui::MainWindow m_ui;
};
} // namespace vc
