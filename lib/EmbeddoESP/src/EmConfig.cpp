/**
 * @file EmSD.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief
 * @version 0.1
 * @date 2022-03-11
 *
 * @copyright Copyright (c) 2022 Embeddo
 *
 */

#include <EmConfig.hpp>

using namespace Embeddo;

ConfigProvider::ConfigProvider(SDFS &sd, String path) : path(path), m_sd(sd) {
}

bool ConfigProvider::load() {
  trace("Lendo arquivo %s", this->path.c_str());
  assert(!this->path.isEmpty());

  File file = SD.open(this->path, FILE_READ);

  if (!file) {
    trace_e("Falha ao abrir arquivo %s", this->path.c_str());
    return false;
  }

  DeserializationError err = deserializeJson(this->doc, file.readString());

  file.close();

  if (err != DeserializationError::Ok) {
    trace_e("Arquivo de configuração inválido");
    return false;
  }

  return true;
}

bool ConfigProvider::save() {
  trace("Salvando arquivo %s", this->path.c_str());
  assert(!this->path.isEmpty());

  File file = SD.open(this->path + ".tmp", FILE_WRITE);

  if (!file) {
    trace_e("Falha ao abrir arquivo %s", this->path.c_str());
    return false;
  }

  String jsonStr;
  serializeJson(this->doc, jsonStr);

  file.write((uint8_t *) jsonStr.c_str(), jsonStr.length());
  file.close();

  if (SD.exists(this->path) && !SD.remove(this->path)) {
    trace_e("Falha ao escrever arquivo de configuração: falha ao remover "
            "arquivo antigo: %s",
            this->path.c_str());
    return false;
  }

  if (!SD.rename(this->path + ".tmp", this->path)) {
    trace_e("Falha ao escrever arquivo de configuração: falha ao renomear: %s",
            this->path.c_str());
    return false;
  };

  return true;
}