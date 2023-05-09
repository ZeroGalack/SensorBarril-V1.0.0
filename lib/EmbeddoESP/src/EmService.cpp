/**
 * @file EmService.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Implementação da estrutura de serviço
 * @version 0.1
 * @date 2021-11-19
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <Arduino.h>

#include "EmService.hpp"
#include "Embeddo.hpp"

using namespace Embeddo;

std::vector<TaskHandle_t> Service::m_services;

Service::Service() {
  this->canStopSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(this->canStopSemaphore);
}

Service::~Service() {
  this->stop();
}

void Service::start() {
  if (this->m_taskHandle)
    return; // Não inicia o serviço se já estiver rodando

  if (!this->ranBegin) {
    this->begin();
    this->ranBegin = true;
  }

  xSemaphoreGive(this->canStopSemaphore);

  if (this->onStart)
    this->onStart();

  xTaskCreatePinnedToCore(this->m_init,
                          this->name.c_str(),
                          this->stackDepth,
                          this,
                          this->priority,
                          &this->m_taskHandle,
                          this->coreId);

  Service::m_services.push_back(this->m_taskHandle);
}

void Service::stop() {
  if (!this->m_taskHandle)
    return; // Não tenta parar um serviço que não esteja rodando

  static bool here = false;

  if (here)
    return;

  here = true;

  uint16_t priority = uxTaskPriorityGet(NULL);
  vTaskPrioritySet(NULL, this->priority + 1);
  xSemaphoreTake(this->canStopSemaphore, portMAX_DELAY);
  vTaskPrioritySet(NULL, priority);
  
  TaskHandle_t handle = this->m_taskHandle;
  this->m_taskHandle  = nullptr;

  if (this->onStop)
    this->onStop();

  removeVector(Service::m_services, handle);
  trace("Task finalizada: %p", handle);
  here = false;
  vTaskDelete(handle);
}

bool Service::isRunning() {
  return this->m_taskHandle;
}

void Service::m_init(void *_this) {
  Service *s = (Service *) _this;
  trace("Task iniciada: %p (core %u)", s->m_taskHandle, s->coreId);
  s->service();
  s->stop();
}

void Service::suspendAll() {
  TaskHandle_t curTask = xTaskGetCurrentTaskHandle();

  for (TaskHandle_t handle : Service::m_services)
    if (handle != curTask)
      vTaskSuspend(handle);
}

void Service::resumeAll() {
  for (TaskHandle_t handle : Service::m_services)
    vTaskResume(handle);
}
