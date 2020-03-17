#pragma once
#include <QObject>
#include <QTcpSocket>

#include <pl/annotations.hpp>
#include <pl/noncopyable.hpp>

#include "actor_id.hpp"
#include "logger.hpp"
#include "vector_timestamp.hpp"

namespace vc {
/**
 * Client type.
 *
 * Repeatedly requests a time stamp from the server.
 */
class client : public QObject {
  Q_OBJECT

public:
  PL_NONCOPYABLE(client);

  /**
   * Creates a client object.
   * @param aid The unique actor_id to use.
   * @param l The logger to write to.
   * @param parent The QObject parent.
   */
  client(actor_id aid, logger& l, QObject* parent = PL_NO_PARENT);

  /**
   * Disconnects the client from the server if it has been connected.
   */
  virtual ~client() override;

  /**
   * Connects the client to the server.
   */
  void connect();

private:
  /**
   * Requests time stamp from the server.
   */
  void request_time_from_server();

  /**
   * Handles response from the server.
   */
  void on_ready_read();

  actor_id aid_;
  logger& logger_;
  bool is_connected_;
  QTcpSocket socket_;
  vector_timestamp vstamp_;
};
} // namespace vc
