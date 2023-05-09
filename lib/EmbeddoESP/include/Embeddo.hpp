/**
 * @file Embeddo.hpp
 * @author Julio César Caldeira <julio@embeddo.io>,
           Maria Beatriz Florencio <beatriz@embeddo.io>
 * @brief Definições e macros utilitários
 * @date 2021-11-16
 *
 * @copyright Copyright (c) 2021 Embeddo
 *
 */

#pragma once

#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <deque>

#include <EmConfig.hpp>
#include <EmHttpsUpdate.hpp>
#include <EmID.hpp>
#include <EmMath.hpp>
#include <EmWifi.hpp>

/*************************************************
 * Configurações genéricas                       *
 *************************************************/

/* WiFi para configuração do ID */
#define EM_WIFI_SSID "EMBEDDO_ESP"
#define EM_WIFI_PSK  "31415926"

/* Configuração OTA de primeiro início */
#define EM_OTA_HOSTNAME "EMBEDDO_ESP"
#define EM_OTA_PW       "31415926"

#define EM_HOST "iam.slg.br"
#define EM_PORT 1935

#define _trace(fmt, ...)  Serial.printf("### %s " fmt, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#define trace(fmt, ...)   _trace(fmt "\r\n", ##__VA_ARGS__)
#define trace_e(fmt, ...) trace("%s:%d " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define PACKED __attribute__((__packed__))

typedef std::function<void(void)> AsyncFun;

namespace Embeddo {
/**
 * @brief Função de setup. Checa ID, inicia WiFi, etc
 *
 */
void setup(bool dev = false, bool checkReset = true);

/**
 * @brief Remove item do vetor
 *
 * @tparam T
 * @param v
 * @param item
 */
template <typename T> inline void removeVector(std::vector<T> &v, const T &item) {
  v.erase(std::remove(v.begin(), v.end(), item), v.end());
}

/**
 * @brief Retorna média aritmética dos valores no vetor
 *
 * @tparam T
 * @param v
 * @return double
 */
template <typename T, template <typename...> class C> inline T meanContainer(const C<T> &v) {
  T mean;

  for (const T &t : v)
    mean += t;

  return mean / v.size();
}
} // namespace Embeddo