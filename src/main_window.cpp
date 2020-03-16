#include <cassert>
#include <cstdio>

#include "client.hpp"
#include "main_window.hpp"
#include "server.hpp"

namespace vc {
main_window::main_window(QWidget* parent)
  : QMainWindow(parent), ui_(), ofs_("logfile.log"), logger_(ofs_) {
  assert(ofs_.operator bool() && "Couldn't open log file!");

  setWindowTitle("Vector clocks");

  connect(ui_.pushButton, &QAbstractButton::clicked, this,
          &main_window::on_button_click);
}

void main_window::on_button_click() {
  auto* serv = new server(actor_id{1}, logger_, this);
  if (!serv->listen()) {
    fprintf(stderr, "Server failed to listen.\n");
    return;
  }

  auto* cl = new client(actor_id{2}, logger_, this);
  cl->connect();
}
} // namespace vc
