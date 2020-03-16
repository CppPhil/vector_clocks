#pragma once
#include <vector>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <pl/annotations.hpp>
#include <pl/noncopyable.hpp>

#include "actor_id.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include "vector_timestamp.hpp"

namespace vc {
class server : public QObject {
  Q_OBJECT

public:
  PL_NONCOPYABLE(server);

  server(actor_id aid, logger& l, QObject* parent = PL_NO_PARENT);

  virtual ~server();

  [[nodiscard]] bool listen();

private:
  void setup_connections();

  void on_new_connection();

  void on_client_ready_read();

  tl::expected<packet, error> read_client_request(QTcpSocket* socket);

  void handle_client_request(QTcpSocket* socket);

  actor_id aid_;
  logger& logger_;
  bool is_listening_;
  QTcpServer tcp_server_;
  std::vector<QTcpSocket*> clients_;
  vector_timestamp vstamp_;
};
} // namespace vc
