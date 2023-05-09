/**
 * @file Socket.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Implementação do Socket
 * @version 0.1
 * @date 2022-02-23
 *
 * @copyright Copyright (c) 2022 Embeddo
 *
 */

#include <EmConfig.hpp>
#include <EmMath.hpp>
#include <EmSocket.hpp>
#include <Embeddo.hpp>

void SocketServer::start() {
  assert(Embeddo::Wifi.isConnected());

  this->m_server.stop();
  this->m_server = WiFiServer(this->port);
  this->m_server.begin();
  trace("Servidor TCP inicializado");

  Embeddo::Service::start();
}

uint16_t SocketServer::clientCount() const {
  return this->clients.size();
}

void SocketServer::service() {
  for (;;) {
    vTaskDelay(250);

    for (auto it = this->clients.cbegin(); it != this->clients.cend();) {
      if (!it->second->isRunning()) {
        delete it->second;
        this->clients.erase(it++);
      } else
        ++it;
    }

    /* Para de aceitar clientes */
    if (this->clientCount() > this->maxClients)
      continue;

    WiFiClient client = this->m_server.available();

    if (client) {
      SocketClient *sockClient                    = new SocketClient(client);
      this->clients[(uint32_t) client.remoteIP()] = sockClient;
      sockClient->start();

      for (const auto &fun : this->newClientCallback)
        fun(sockClient);
    }
  }
}

SocketClient::SocketClient() : Embeddo::Service() {
  this->stackDepth = 5120;
}

SocketClient::SocketClient(WiFiClient client) : SocketClient() {
  this->m_client = client;
  trace("Novo cliente: %s", this->m_client.remoteIP().toString().c_str());
  this->autoconnect = false;
}

SocketClient::SocketClient(IPAddress ip, uint16_t port) : SocketClient() {
  this->ip   = ip;
  this->port = port;
  this->host = this->ip.toString();
}

bool SocketClient::connect() {
  if (this->m_client.connected())
    return true;

  if (!WiFi.isConnected())
    return false;

  trace("Conectando a %s:%u...", this->host.c_str(), this->port);

  if (this->m_client.connect(this->host.c_str(), this->port)) {
    for (const auto &fun : this->onConnectedCbs)
      fun();

    this->ip = this->m_client.localIP();

    return true;
  }

  trace("Falha ao conectar");

  return false;
}

bool SocketClient::isConnected() {
  return WiFi.isConnected() && this->m_client.connected();
}

void SocketClient::service() {
  for (;;) {
    if (!this->isConnected()) {
      trace("Desconectado");

      for (const auto &fun : this->onDisconnectedCbs)
        fun();

      if (!this->autoconnect)
        this->stop();

      while (!this->connect())
        vTaskDelay(250);
    }

    uint32_t available = this->m_client.available();

    if (available)
      for (const auto &fun : this->availableCbs)
        fun(this, available);

    vTaskDelay(250);
  }
}

uint32_t SocketClient::available() {
  return this->m_client.available();
}

void SocketClient::stop() {
  trace("Cliente %s desconectou", this->m_client.remoteIP().toString().c_str());
  this->m_client.stop();
  Embeddo::Service::stop();
}

uint32_t SocketClient::send(const uint8_t *data, uint32_t len) {
  return this->m_client.write(data, len);
}

uint32_t SocketClient::send(const uint8_t data) {
  return this->send(&data, 1);
}

uint32_t SocketClient::send(const String &data) {
  return this->send((uint8_t *) data.c_str(), data.length() + 1);
}

uint32_t SocketClient::recv(uint8_t *data, uint32_t len) {
  return this->m_client.read(data, len);
}
