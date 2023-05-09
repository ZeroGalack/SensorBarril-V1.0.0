/**
 * @file EmWifi.cpp
 * @brief Implements Wifi related structures and procedures
 * @author Julio César Caldeira <julio@embeddo.io>,
           Maria Beatriz Florencio <beatriz@embeddo.io>
 */

#include <ArduinoOTA.h>
#include <WiFi.h>

#include <EmID.hpp>
#include <EmWifi.hpp>
#include <Embeddo.hpp>

using namespace Embeddo;

WifiClass Embeddo::Wifi("", "");

#define FTP_OK                    \
  if (!this->ftp.isConnected()) { \
    this->ftp.CloseConnection();  \
    return false;                 \
  }

#define AFTP_OK                    \
  if (!this->aftp.isConnected()) { \
    this->aftp.CloseConnection();  \
    return false;                  \
  }

WifiClass::WifiClass(String ssid, String psk) : ssid(ssid), psk(psk) {
}

void WifiClass::connect() {
  if (this->apMode)
    this->connectAP();
  else
    this->connectSTA();

  for (const auto &fun : this->onConnected)
    fun();
}

void WifiClass::connectSTA() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    trace("Conectando no WiFi %s", this->ssid.c_str());

    WiFi.begin(this->ssid.c_str(), this->psk.c_str());

    for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++)
      vTaskDelay(100);
  }

  trace("WiFi Conectado!");

  setupOTA();
}

void WifiClass::connectAP() {
  WiFi.mode(WIFI_AP_STA);

  trace("Iniciando AP: %s (%s)", this->ssid.c_str(), this->psk.c_str());

  WiFi.softAP(this->ssid.c_str(), this->psk.c_str());
  trace("IP local: %s", WiFi.softAPIP().toString().c_str());

  vTaskDelay(500);
  this->setupOTA();
}

void WifiClass::setupOTA() {
  ArduinoOTA.setHostname(EM_OTA_HOSTNAME);
  ArduinoOTA.setPassword(EM_OTA_PW);

  /* Adicionar callbacks para o OTA */
  ArduinoOTA
      .onStart([=]() {
        String type;

        switch (ArduinoOTA.getCommand()) {
          case U_FLASH:
            type = "sketch";
            break;
          default:
            type = "filesystem";
        }

        this->ota_started = true;
        trace("OTA: iniciado, tipo: %s", type.c_str());
      })
      .onEnd([=]() {
        this->ota_started = false;
        Serial.println("");
        trace("OTA: terminado");
      })
      .onProgress([](uint16_t prog, uint16_t total) {
        _trace("OTA: progresso: %u%%\r", (prog / (total / 100)));
      })
      .onError([=](ota_error_t err) {
        trace_e("OTA: erro (%u): ", err);

        switch (err) {
          case OTA_AUTH_ERROR:
            trace("Erro de autenticação");
            break;
          case OTA_BEGIN_ERROR:
            trace("Falha ao iniciar OTA");
            break;
          case OTA_CONNECT_ERROR:
            trace("Erro de conexão");
            break;
          case OTA_RECEIVE_ERROR:
            trace("Erro ao receber novo programa");
            break;
          case OTA_END_ERROR:
            trace("Erro ao finalizar o OTA");
            break;
          default:
            trace("Erro desconhecido");
        }

        this->ota_started = false;
      });

  ArduinoOTA.setTimeout(60000);
  ArduinoOTA.begin();

  trace("OTA iniciado");
}

void WifiClass::handleOTA() {
  if (!this->isConnected())
    return;

  for (int i = 0; i < 5; i++)
    ArduinoOTA.handle();

  while (this->ota_started)
    ArduinoOTA.handle();
}

bool WifiClass::isConnected() {
  if (this->apMode && this->isRunning())
    return true;

  return WiFi.isConnected();
}

void WifiClass::service() {
  if (!this->ssid) {
    trace("Especifique um SSID válido");
    return;
  }

  for (;;) {
    ArduinoOTA.end();
    this->connect();

    for (;;) {
      if (!this->isConnected()) {
        for (const auto &fun : this->onDisconnected)
          fun();
        break;
      }

      this->handleOTA();
      vTaskDelay(200);
    }
  }
}

void WifiClass::stop() {
  if (this->alwaysOn)
    return;

  WiFi.disconnect();
  ArduinoOTA.end();

  for (const auto &fun : this->onDisconnected)
    fun();

  Service::stop();
}