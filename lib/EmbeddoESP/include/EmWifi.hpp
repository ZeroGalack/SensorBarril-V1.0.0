/**
 * @file EmWifi.hpp
 * @brief Wifi related structs declarations
 * @author Julio César Caldeira <julio@embeddo.io>,
           Maria Beatriz Florencio <beatriz@embeddo.io>
 */

#pragma once

#include <Arduino.h>
#include <EmService.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Embeddo {
typedef std::function<void(void)> connectCallback;

class WifiClass : public Service {
public:
  explicit WifiClass(String ssid, String psk);

  /**
   * @brief List reachable access points in Serial. Might be useful in future
   *
   */
  void listAccessPoints();

  /**
   * @brief Get connection status
   *
   * @return true
   * @return false
   */
  bool isConnected();

  void stop() override;

  String ssid;
  String psk;

  bool apMode   = false; ///< Modo de funcionamento, true = ap
  bool alwaysOn = true;  ///< Flag de execução eterna

  std::vector<connectCallback> onConnected;    ///< Callback de conexão
  std::vector<connectCallback> onDisconnected; ///< Callback de desconexão

private:
  /**
   * @brief Serviço do WiFi
   * Reconecta automaticamente em perda de sinal e ouve eventos do OTA
   *
   */
  void service() override;

  /**
   * @brief Tries to connect (blocking) to AP
   *
   */
  void connect();

  /**
   * @brief Sets up Arduino OTA
   *
   */
  void setupOTA();

  /**
   * @brief Handle OTA events
   *
   */
  void handleOTA();

  /**
   * @brief Conecta num AP existente
   *
   */
  void connectSTA();

  /**
   * @brief Cria um AP
   *
   */
  void connectAP();

  bool ota_started = false;
};

extern WifiClass Wifi;
} // namespace Embeddo