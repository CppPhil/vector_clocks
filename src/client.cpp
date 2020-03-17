#include <cstdint>
#include <cstdio>

#include <QHostAddress>
#include <QTimer>

#include "client.hpp"
#include "ntoh.hpp"
#include "packet.hpp"
#include "server_port.hpp"

namespace vc {
client::client(actor_id aid, logger& l, QObject* parent)
  : QObject(parent),
    aid_(aid),
    logger_(l),
    is_connected_(false),
    socket_(),
    vstamp_(aid_) {
}

client::~client() {
  if (is_connected_)
    socket_.disconnectFromHost();
}

void client::connect() {
  socket_.connectToHost(QHostAddress("127.0.0.1"), server_port);
  is_connected_ = true;

  QObject::connect(&socket_, &QIODevice::readyRead, this,
                   &client::on_ready_read);

  // Request a new time stamp from the server every second.
  auto* timer = new QTimer(this);
  QObject::connect(timer, &QTimer::timeout, this,
                   &client::request_time_from_server);
  constexpr auto timer_timeout_ms = 1000;
  timer->start(timer_timeout_ms);
}

void client::request_time_from_server() {
  // Tick own vstamp for send event.
  if (!vstamp_.tick(aid_).has_value()) {
    fprintf(stderr, "Client couldn't tick its vector timestamp!\n");
    return;
  }

  const auto vstamp_binary = vstamp_.serialize_to_binary();
  constexpr char payload[] = "GIEVTIMEPLX";

  const packet pkt(vstamp_binary.data(), vstamp_binary.size(), payload,
                   sizeof(payload));

  VC_LOG_INFO(logger_, vstamp_, aid_, "SEND Client sent \"{}\" to server.",
              payload);

  const auto pkt_bin = pkt.serialize_to_binary();

  if (socket_.write(reinterpret_cast<const char*>(pkt_bin.data()),
                    pkt_bin.size())
      == -1) {
    fprintf(stderr, "Client couldn't send packet!\n");
    return;
  }
}

void client::on_ready_read() {
  auto* the_sender = sender();

  if (the_sender == nullptr)
    return;

  auto* sock = qobject_cast<QTcpSocket*>(the_sender);

  if (sock == nullptr)
    return;

  uint64_t vstamp_len, payload_len;
  if (sock->read(reinterpret_cast<char*>(&vstamp_len), sizeof(vstamp_len))
      == -1) {
    fprintf(stderr, "Client couldn't read vstamp_len!\n");
    return;
  }
  vstamp_len = ntoh(vstamp_len);

  std::vector<char> vstamp_buf(vstamp_len);
  if (sock->read(vstamp_buf.data(), vstamp_len) == -1) {
    fprintf(stderr, "Client couldn't read vstamp_buf!\n");
    return;
  }

  if (sock->read(reinterpret_cast<char*>(&payload_len), sizeof(payload_len))
      == -1) {
    fprintf(stderr, "Client couldn't read payload_len!\n");
    return;
  }
  payload_len = ntoh(payload_len);

  std::vector<char> payload_buf(payload_len);
  if (sock->read(payload_buf.data(), payload_len) == -1) {
    fprintf(stderr, "Client couldn't read payload_buf!\n");
    return;
  }

  const packet rcvd_pkt(vstamp_buf.data(), vstamp_len, payload_buf.data(),
                        payload_len);

  const auto exp_their_vc = vector_timestamp::deserialize_from_binary(
    rcvd_pkt.vstamp_buffer().data(), rcvd_pkt.vstamp_buffer().size());

  if (!exp_their_vc.has_value()) {
    fprintf(stderr, "Client failed to deserialize incoming vector clock!\n");
    return;
  }

  const auto their_vc = *exp_their_vc;

  const std::string buf(rcvd_pkt.payload_buffer().begin(),
                        rcvd_pkt.payload_buffer().end());

  // Tick own clock for receive event.
  if (!vstamp_.tick(aid_)) {
    fprintf(stderr, "Client failed to tick own vector clock!\n");
    return;
  }

  // Merge incoming vector clock into own vector clock.
  vstamp_.merge(their_vc);

  VC_LOG_INFO(logger_, vstamp_, aid_,
              "RECV Client received time from server: \"{}\".", buf.data());
}
} // namespace vc
