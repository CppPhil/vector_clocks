#pragma once
#include <QObject>
#include <QTcpSocket>

#include <pl/annotations.hpp>
#include <pl/noncopyable.hpp>

#include "actor_id.hpp"
#include "logger.hpp"
#include "vector_timestamp.hpp"

namespace vc {
class client : public QObject {
  Q_OBJECT

public:
  PL_NONCOPYABLE(client);

  client(actor_id aid, logger& l, QObject* parent = PL_NO_PARENT);

  virtual ~client();

  void connect();

private:
  void request_time_from_server();

  void on_ready_read();

  actor_id aid_;
  logger& logger_;
  bool is_connected_;
  QTcpSocket socket_;
  vector_timestamp vstamp_;
};
} // namespace vc
