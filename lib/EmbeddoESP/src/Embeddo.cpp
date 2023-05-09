/**
 * @file Embeddo.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Implementação de funções essenciais
 * @date 2021-11-17
 *
 * @copyright Copyright (c) 2021 Embeddo
 *
 */

#include <EmID.hpp>
#include <Embeddo.hpp>

#include <rom/rtc.h>

using namespace Embeddo;

static bool checkID(bool dev);
static bool checkReset(bool chkReset);

void Embeddo::setup(bool dev, bool chkReset) {
  Wifi.ssid = EM_WIFI_SSID;
  Wifi.psk  = EM_WIFI_PSK;

  if (!checkReset(chkReset) || !checkID(dev)) {
    Wifi.apMode = true;
    Wifi.start();

    while (1)
      vTaskDelay(5000);
  }

  Wifi.stop();
}

bool checkID(bool dev) {
  trace("Checando ID...");

  int    i;
  String id = "";

  for (i = 0; i < 3; i++) {
    id = ID::getID();

    if (id != "fatal")
      break;

    trace("Erro SPIFFS (%d/3)", i + 1);

    vTaskDelay(1000);
  }

  if (i == 3) {
    trace_e("Erro grave SPIFFS, abortando...");
    return false;
  }

  if (id != "" && id != "dev") {
    trace("ID: %s", id.c_str());
    return true;
  }

  if (dev) {
    ID::saveID("dev");
    return true;
  }

  trace("Sem ID, fazendo download...");

  Wifi.start();

  while (!Wifi.isConnected())
    vTaskDelay(1000);

  uint32_t tries = 0;

  while (id == "") {
    if (Wifi.isConnected()) {
      if (tries > 1)
        vTaskDelay(500);

      trace("Tentando download (%d)", ++tries);
      id = ID::downloadID();
    }
  }

  trace("Novo ID: %s", id.c_str());

  for (i = 0; i < 3 && !ID::saveID(id); i++) {
    trace("Salvando ID (%d)", i + 2);
    vTaskDelay(500);
  }

  if (i == 3) {
    trace_e("Erro grave SPIFFS, abortando...");
    return false;
  }

  return true;
}

void printResetReason(int reason) {
  switch (reason) {
    case 1:
      trace("(%d) Vbat power on reset", reason);
      break;
    case 3:
      trace("(%d) Software reset digital core", reason);
      break;
    case 4:
      trace("(%d) Legacy watch dog reset digital core", reason);
      break;
    case 5:
      trace("(%d) Deep Sleep reset digital core", reason);
      break;
    case 6:
      trace("(%d) Reset by SLC module, reset digital core", reason);
      break;
    case 7:
      trace("(%d) Timer Group0 Watch dog reset digital core", reason);
      break;
    case 8:
      trace("(%d) Timer Group1 Watch dog reset digital core", reason);
      break;
    case 9:
      trace("(%d) RTC Watch dog Reset digital core", reason);
      break;
    case 10:
      trace("(%d) Instrusion tested to reset CPU", reason);
      break;
    case 11:
      trace("(%d) Time Group reset CPU", reason);
      break;
    case 12:
      trace("(%d) Software reset CPU", reason);
      break;
    case 13:
      trace("(%d) RTC Watch dog Reset CPU", reason);
      break;
    case 14:
      trace("(%d) for APP CPU, reseted by PRO CPU", reason);
      break;
    case 15:
      trace("(%d) Reset when the vdd voltage is not stable", reason);
      break;
    case 16:
      trace("(%d) RTC Watch dog reset digital core and rtc module", reason);
      break;
    default:
      trace("(%d) NO_MEAN", reason);
  }
}

bool checkReset(bool chkReset) {
  int reason = rtc_get_reset_reason(0);

  switch (reason) {
    case 1:  ///< POWERON_RESET
    case 16: ///< RTCWDT_RTC_RESET (reset pelo botão)
      return true;

    default:
      trace("Erro no programa principal, entrando em modo de recuperação");
      printResetReason(reason);
      return !chkReset;
  }
}