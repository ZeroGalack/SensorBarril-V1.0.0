#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Embeddo {
namespace Network {

const size_t PACKET_QUEUE_SIZE = 16;

// Não usar enum por não ter um tamanho comum definido entre várias plataformas
typedef uint8_t PacketType;

const PacketType PT_INDUCTION_ACTIVITY = 0x01;
const PacketType PT_KEEPALIVE = 0x53;
const PacketType PT_ECHO = 0xEC;
const PacketType PT_ECHO_RESPONSE = 0x65;


typedef uint16_t DeviceId;

#pragma pack(push, 1)
struct Packet {
  PacketType type;
  uint8_t ttl;
  DeviceId source;
  DeviceId destination;
  uint8_t data[10];
};

struct PollStatus {
  uint8_t canSend : 1;
  uint8_t canRecv : 1;
  uint8_t canTransmit : 1;
  uint8_t canFeed : 1;
};
#pragma pack(pop)


template <typename T, size_t N>
class SimpleCircularQueue {
  public:
    bool push(T &p) {
      if (isFull())
        return false;

      q[end] = p;
      incIndex(end);

      if (start == end) full = true;

      return true;
    }

    bool isFull() {
      return start == end && full;
    }

    bool isEmpty() {
      return start == end && !full;
    }

    bool pop(T &p) {
      if (isEmpty())
        return false;

      p = q[start];
      incIndex(start);

      full = false;

      return true;
    }

    SimpleCircularQueue() : start(0), end(0), full(false) {}

  private:
    T q[N];
    int start;
    int end;
    volatile bool full;

    void incIndex(int &i) {
      i = (i + 1) % N;
    }
};

typedef SimpleCircularQueue<Packet, PACKET_QUEUE_SIZE> PacketQueue;

class Link {
  private:
    PacketQueue rxQueue;
    PacketQueue txQueue;

  public:
    // Envia um pacote por este link (coloca na txQueue)
    bool send(Packet &p);
    // Recebe um pacote por este link (retira da rxQueue)
    bool recv(Packet &p);

    // Verifica o estado das filas deste link
    PollStatus poll();

    // Coloca pacote recebido na rxQueue
    bool feed(Packet &p);
    // Retira pacote da txQueue para transmitir
    bool transmit(Packet &p);
};

typedef std::vector<Link *> LinkList;

struct Route {
  Link *through;
  uint8_t ttl;
};

typedef void (*handle_packet_t)(void *ctx, void *comm, Packet &p);

class Comm {
  private:
    LinkList links;
    std::unordered_map<DeviceId, Route> routes;
    DeviceId id;

    handle_packet_t handlePacketFn;
    void *handleCtx;

  public:
    Comm(DeviceId id, handle_packet_t handlePacketFn, void *handleCtx);

    // Adiciona um link ao gerenciador
    void addLink(Link *link);

    // Dá hint ao gerenciador sobre uma rota por um link
    void hintRoute(DeviceId id, Link *through, uint8_t ttl);

    // Processa um pacote de cada rxQueue de cada link
    bool step();

    // Envia um pacote
    bool send(Packet &p);
};

} // namespace Network
} // namespace Embeddo
