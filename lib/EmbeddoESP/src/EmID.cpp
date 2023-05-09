/**
 * @file EmID.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
           Maria Beatriz Florencio <beatriz@embeddo.io>
 * @brief Implementation of ID related routines
 * @date 2021-10-04
 *
 * @copyright Copyright (c) 2021 Embeddo
 *
 */

#include <HTTPClient.h>
#include <SPIFFS.h>

#include <EmID.hpp>
#include <Embeddo.hpp>
#include <WiFi.h>

using namespace Embeddo;

static String m_id = "";

String ID::getID() {
  if (m_id != "")
    return m_id;

  if (!SPIFFS.begin(true))
    return "fatal";

  File file = SPIFFS.open("/ID.txt", FILE_READ);

  if (!file)
    return "";

  m_id = file.readString();

  file.close();

  return m_id;
}

String ID::downloadID() {
  if (!WiFi.isConnected())
    return "";

  HTTPClient client;
  String     url = "http://" + String(EM_HOST) +
               ":" + EM_PORT + "/registrar?chipId=" + String(ID::chipID());

  trace("HTTP request: %s", url.c_str());
  client.begin(url);

  int responseCode = client.GET();

  if (responseCode <= 0)
    return "";

  String response = client.getString();

  client.end();

  return response;
}

bool ID::saveID(String id) {
  if (!SPIFFS.begin(true))
    return false;

  File file = SPIFFS.open("/ID.txt", FILE_WRITE);

  if (!file)
    return false;

  if (!file.print(id))
    return false;

  file.close();
  m_id = id;

  return true;
}

uint32_t ID::chipID() {
  uint32_t chipId = 0;

  for (int i = 0; i < 17; i = i + 8)
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;

  return chipId;
}