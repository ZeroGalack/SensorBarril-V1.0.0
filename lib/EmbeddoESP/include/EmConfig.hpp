/**
 * @file EmSD.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Singleton para controlar cartão SD
 * @version 0.1
 * @date 2022-03-11
 *
 * @copyright Copyright (c) 2022 Embeddo
 *
 */

#pragma once

#include <ArduinoJson.h>
#include <Embeddo.hpp>
#include <SD.h>

namespace Embeddo {
class ConfigProvider {
public:
  explicit ConfigProvider(SDFS &sd = SD, String path = "");

  /**
   * @brief Carrega a configuração do armazenamento na memória
   *
   * @return true
   * @return false
   */
  virtual bool load();

  /**
   * @brief Salva a configuração da memória no armazenamento
   *
   * @return true
   * @return false
   */
  virtual bool save();

  String              path = ""; ///< Caminho para o arquivo
  DynamicJsonDocument doc =
      DynamicJsonDocument(2048); ///< Objeto dinâmico de configuração

protected:
  SDFS &m_sd;
};
} // namespace Embeddo