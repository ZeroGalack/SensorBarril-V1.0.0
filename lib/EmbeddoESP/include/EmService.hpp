/**
 * @file EmService.hpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Estruturas relacionadas a criação de serviços
 * @version 0.1
 * @date 2021-11-19
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

namespace Embeddo {
typedef std::function<void(void)> ServiceCallback; ///< Tipo de callback

/**
 * @brief Define um serviço do ESP
 *
 */
class Service {
public:
  Service();

  /**
   * @brief Para o serviço antes de destruir o objeto
   *
   */
  virtual ~Service();

  /**
   * @brief Inicia o serviço como uma task freeRTOS
   *
   */
  virtual void start();

  /**
   * @brief Finaliza a execução do serviço
   *
   */
  virtual void stop();

  /**
   * @brief Função para inicialização interna do serviço. Será rodada somente
     uma vez
   *
   */
  virtual void begin() {
  }

  /**
   * @brief Retorna se o serviço está rodando ou não
   *
   * @return true
   * @return false
   */
  bool isRunning();

  /**
   * @brief Suspende todos os serviços criados por essa classe
   *
   */
  static void suspendAll();

  /**
   * @brief Resume todos os serviços criados por essa classe
   *
   */
  static void resumeAll();

  String   name       = "";                   ///< Nome do serviço
  uint32_t stackDepth = 2048;                 ///< Tamanho da stack
  uint8_t  priority   = tskIDLE_PRIORITY + 1; ///< Prioridade do serviço
  uint8_t  coreId     = 1;                    ///< Em qual núcleo rodar

  ServiceCallback onStart = nullptr; ///< Callback para inicialização de serviço
  ServiceCallback onStop = nullptr; ///< Callback para finalização de serviço

protected:
  /**
   * @brief Serve como trampolim do código C para C++
   *
   * @param _this Ponteiro para instância dessa classe
   */
  void static m_init(void *_this);

  /**
   * @brief Código do serviço
   *
   */
  virtual void service() = 0;

  bool              ranBegin     = false; ///< Se já rodou begin()
  TaskHandle_t      m_taskHandle = nullptr;
  SemaphoreHandle_t canStopSemaphore =
      nullptr; ///< Indica se serviço pode ser parado ou não

private:
  static std::vector<TaskHandle_t> m_services;
};
} // namespace Embeddo
