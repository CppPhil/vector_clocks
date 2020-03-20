#include <cstdint>
#include <cstdio>

#include <QTime>

#include <pl/algo/ranged_algorithms.hpp>

#include "ntoh.hpp"
#include "server.hpp"
#include "server_port.hpp"

namespace vc {
server::server(actor_id aid, logger& l, QObject* parent)
  : QObject(parent),
    aid_(aid),
    logger_(l),
    is_listening_(false),
    tcp_server_(PL_NO_PARENT),
    clients_(),
    vstamp_(aid_) {
  setup_connections();
}

server::~server() {
  if (is_listening_)
    tcp_server_.close();

  // Free the memory occupied by the TCP sockets for the clients.
  for (auto* client : clients_)
    delete client;
}

[[nodiscard]] bool server::listen() {
  const auto ret_val = tcp_server_.listen(QHostAddress("127.0.0.1"),
                                          server_port);

  if (ret_val)
    is_listening_ = true;

  return ret_val;
}

void server::setup_connections() {
  connect(&tcp_server_, &QTcpServer::newConnection, this,
          &server::on_new_connection);
}

void server::on_new_connection() {
  for (QTcpSocket* current_client = nullptr;
       (current_client = tcp_server_.nextPendingConnection()) != nullptr;) {
    clients_.push_back(current_client);
    connect(current_client, &QIODevice::readyRead, this,
            &server::on_client_ready_read);
  }
}

void server::on_client_ready_read() {
  auto span = opentracing::Tracer::Global()->StartSpan("server: on_ready_read");

  auto* the_sender = sender();

  if (the_sender == nullptr)
    return;

  auto* client = qobject_cast<QTcpSocket*>(the_sender);

  if (client == nullptr)
    return;

  handle_client_request(client, *span);
}

tl::expected<packet, error>
server::read_client_request(QTcpSocket* socket,
                            const opentracing::Span& parent_span) {
  auto span = opentracing::Tracer::Global()->StartSpan(
    "server: read_client_request",
    {opentracing::ChildOf(&parent_span.context())});

  uint64_t vstamp_size;
  if (socket->read(reinterpret_cast<char*>(&vstamp_size), sizeof(vstamp_size))
      == -1)
    return VC_UNEXPECTED("Couldn't read vstamp_size from TCP socket.");
  vstamp_size = ntoh(vstamp_size);

  std::vector<char> vstamp_buf(vstamp_size);
  if (socket->read(vstamp_buf.data(), vstamp_size) == -1)
    return VC_UNEXPECTED("Couldn't read vstamp_buf from TCP socket.");

  uint64_t payload_size;
  if (socket->read(reinterpret_cast<char*>(&payload_size), sizeof(payload_size))
      == -1)
    return VC_UNEXPECTED("Couldn't read payload_size from TCP socket.");
  payload_size = ntoh(payload_size);

  std::vector<char> payload_buf(payload_size);
  if (socket->read(payload_buf.data(), payload_size) == -1)
    return VC_UNEXPECTED("Couldn't read payload_buf from TCP socket.");

  return packet(vstamp_buf.data(), vstamp_size, payload_buf.data(),
                payload_size);
}

void server::handle_client_request(QTcpSocket* socket,
                                   const opentracing::Span& parent_span) {
  auto span = opentracing::Tracer::Global()->StartSpan(
    "server: handle_client_request",
    {opentracing::ChildOf(&parent_span.context())});

  const auto exp_pkt = read_client_request(socket, *span);

  if (exp_pkt.has_value()) {
    const auto pkt = *exp_pkt;

    // The payload that the client is expected to send.
    constexpr char give_time_msg[] = "GIEVTIMEPLX";

    if (pl::algo::equal(pkt.payload_buffer(), give_time_msg)) {
      const auto exp_their_vc = vector_timestamp::deserialize_from_binary(
        pkt.vstamp_buffer().data(), pkt.vstamp_buffer().size());

      if (!exp_their_vc.has_value()) {
        fprintf(stderr, "Server didn't receive proper vector_timestamp!\n");
        return;
      }

      // Get the sending client's vector clock.
      const auto their_vc = *exp_their_vc;

      // Tick own clock (receive event)
      if (!vstamp_.tick(aid_).has_value()) {
        fprintf(stderr, "Server couldn't tick own clock for receive event!\n");
        return;
      }

      // Merge it
      vstamp_.merge(their_vc);

      VC_LOG_INFO(logger_, vstamp_, aid_, "RECV Server received \"{}\".",
                  give_time_msg);

      // Tick own clock (send event)
      if (!vstamp_.tick(aid_).has_value()) {
        fprintf(stderr, "Server couldn't tick own clock for send event!\n");
        return;
      }

      const auto own_vstamp_binary = vstamp_.serialize_to_binary();
      const auto response_payload
        = QTime::currentTime().toString(Qt::DateFormat::RFC2822Date).toUtf8();

      const packet response_packet(own_vstamp_binary.data(),
                                   own_vstamp_binary.size(),
                                   response_payload.data(),
                                   response_payload.size());

      const auto response_packet_binary = response_packet.serialize_to_binary();

      VC_LOG_INFO(logger_, vstamp_, aid_, "SENT Server sent \"{}\".",
                  response_payload.toStdString());

      // Send the response to the client.
      if (socket->write(
            reinterpret_cast<const char*>(response_packet_binary.data()),
            response_packet_binary.size())
          == -1) {
        fprintf(stderr, "Server couldn't write response to client!\n");
        return;
      }

      span->SetTag("Response", response_payload.toStdString());

    } else {
      fprintf(stderr, "Server received unexpected payload from client!\n");
      return;
    }
  } else {
    fprintf(stderr, "Server couldn't read packet from client!\n");
    return;
  }
}
} // namespace vc
