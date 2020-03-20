#pragma once
#include <vector>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <jaegertracing/Tracer.h>

#include <pl/annotations.hpp>
#include <pl/noncopyable.hpp>

#include "actor_id.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include "vector_timestamp.hpp"

namespace vc {
/**
 * Type for the timestamp server.
 */
class server : public QObject {
  Q_OBJECT

public:
  PL_NONCOPYABLE(server);

  /**
   * Creates a server object.
   * @param aid The unique actor_id to use.
   * @param l The logger to write to.
   * @param parent The QObject parent to use.
   */
  server(actor_id aid, logger& l, QObject* parent = PL_NO_PARENT);

  /**
   * Shuts down the TCP server if it was started.
   */
  ~server() override;

  /**
   * Listens for incoming connections.
   * @return true on success; otherwise false.
   */
  [[nodiscard]] bool listen();

private:
  /**
   * Sets up QObject connections.
   */
  void setup_connections();

  /**
   * Callback to handle incoming connections.
   */
  void on_new_connection();

  /**
   * Callback to handle incoming data on a client socket.
   */
  void on_client_ready_read();

  /**
   * Reads a packet from a TCP socket connected to a client.
   * @param socket The socket to read from.
   * @param parent_span The parent tracing span.
   * @return The packet received on success; otherwise error.
   */
  static tl::expected<packet, error>
  read_client_request(QTcpSocket* socket, const opentracing::Span& parent_span);

  /**
   * Handles an incoming request from a client.
   * @param socket The socket connected to the sending client.
   * @param parent_span The parent tracing span.
   */
  void handle_client_request(QTcpSocket* socket,
                             const opentracing::Span& parent_span);

  actor_id aid_;
  logger& logger_;
  bool is_listening_;
  QTcpServer tcp_server_;
  std::vector<QTcpSocket*> clients_;
  vector_timestamp vstamp_;
};
} // namespace vc
