#include "EmComm.hpp"
#include <utility>

using namespace Embeddo::Network;

bool Link::send(Packet &p) {
  return txQueue.push(p);
}

bool Link::recv(Packet &p) {
  return rxQueue.pop(p);
}

PollStatus Link::poll() {
  bool canSend = !txQueue.isFull();
  bool canRecv = !rxQueue.isEmpty();
  bool canTransmit = !txQueue.isEmpty();
  bool canFeed = !rxQueue.isFull();

  return (PollStatus) { canSend, canRecv, canTransmit, canFeed };
}

bool Link::feed(Packet &p) {
  return rxQueue.push(p);
}

bool Link::transmit(Packet &p) {
  return txQueue.pop(p);
}

Comm::Comm(DeviceId id, handle_packet_t handlePacketFn, void *handleCtx) : id(id), handlePacketFn(handlePacketFn), handleCtx(handleCtx) {}

void Comm::addLink(Link *link) {
  links.push_back(link);
}

void Comm::hintRoute(DeviceId id, Link *through, uint8_t ttl) {
  auto routeIter = routes.find(id);
  Route newRoute = { through, ttl };
  if (routeIter == routes.end()) {
    routes[id] = newRoute;
  } else if (routeIter->second.ttl <= ttl) {
    routeIter->second = newRoute;
  }
}

bool Comm::step() {
  bool any = false;
  for (const auto& linkp : links) {
    Packet p;
    if (linkp->recv(p)) {
      hintRoute(p.source, linkp, p.ttl);
      if (p.destination == id) {
        if (handlePacketFn)
          handlePacketFn(handleCtx, this, p);
        any = true;
      } else {
        p.ttl--;
        any = any || send(p);
      }
    }
  }
  return any;
}

bool Comm::send(Packet &p) {
  if (p.ttl == 0) return false;

  for (const auto& route : routes) {
    if (route.first == p.destination) {
      return route.second.through->send(p);
    }
  }

  return false;
}
