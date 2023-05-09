/**
 * @file Socket.hpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Declara estrutura Socket
 * @version 0.1
 * @date 2022-02-23
 *
 * @copyright Copyright (c) 2022 Embeddo
 *
 */

#pragma once

#include <EmService.hpp>
#include <WiFi.h>
#include <deque>
#include <functional>
#include <map>

class SocketServer;
class SocketClient;

typedef std::function<void(SocketClient *)> NewClientCallback;

class SocketServer : public Embeddo::Service {
public:
  using Embeddo::Service::Service;

  void start() override;

  /**
   * @brief Quantidade de clientes conectados
   *
   * @return uint16_t
   */
  uint16_t clientCount() const;

  String   host;
  uint16_t port;
  uint16_t maxClients = 8;

  std::map<uint32_t, SocketClient *> clients;
  std::deque<NewClientCallback>      newClientCallback;

protected:
  void service() override;

private:
  WiFiServer m_server;
};

typedef std::function<void(SocketClient *, uint32_t)> AvailableCallback;
typedef std::function<void(void)>                     ConnectionCallback;

class SocketClient : public Embeddo::Service {
public:
  explicit SocketClient(WiFiClient client);
  SocketClient(IPAddress ip, uint16_t port);
  SocketClient();

  std::deque<AvailableCallback>  availableCbs;
  std::deque<ConnectionCallback> onConnectedCbs, onDisconnectedCbs;
  IPAddress                      ip;
  String                         host;
  uint16_t                       port;
  bool                           autoconnect = true;

  void stop() override;

  uint32_t send(const uint8_t *data, uint32_t len);
  uint32_t send(const uint8_t data);
  uint32_t send(const String &data);

  uint32_t recv(uint8_t *data, uint32_t len);

  uint32_t available();

  bool connect();
  bool isConnected();

protected:
  void service() override;

  WiFiClient m_client;
};
