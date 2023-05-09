/**
 * @file EmHttpsUpdate.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief
 * @version 0.1
 * @date 2022-05-14
 *
 * Copyright (c) 2022 Embeddo
 *
 * TODO: Implementar checkTask
 *
 */

#include <EmCert.h>
#include <EmHttpsUpdate.hpp>
#include <Embeddo.hpp>
#include <WiFiClientSecure.h>

using namespace Embeddo::HttpsUpdate;

String                            Embeddo::HttpsUpdate::host      = "iam.slg.br";
String                            Embeddo::HttpsUpdate::path      = "/firmware.bin";
std::function<void(String, bool)> Embeddo::HttpsUpdate::onError   = nullptr;
std::function<void(String, bool)> Embeddo::HttpsUpdate::onSuccess = nullptr;

static WiFiClientSecure client;

void checkTask(void *);
void updateTask(void *);

void Embeddo::HttpsUpdate::checkUpdate() {
  xTaskCreatePinnedToCore(checkTask, "CheckUpdate", 10240, NULL, 1, NULL, 1);
}

void Embeddo::HttpsUpdate::doUpdate() {
  xTaskCreatePinnedToCore(updateTask, "DoUpdate", 10240, NULL, 1, NULL, 1);
}

void checkTask(void *) {
  trace("Não implementado");
  vTaskDelete(NULL);
}

void updateTask(void *) {
  client.setCACert(EM_CERT);
  trace("Tentando conectar em %s", host.c_str());

  uint32_t timeout = 5000; ///< Timeout da response
  uint32_t millisStart;    ///< Controle de timeout
  uint32_t contentLen = 0; ///< Content-Length header
  bool     canBegin;       ///< Flag para iniciar atualização
  uint32_t written;        ///< Número de bytes escritos no flash
  String   desc;           ///< Descrição da operação, usada nas callbacks
  bool     reboot = false; ///< Flag de reinicialização necessária

  if (!client.connect(host.c_str(), 443)) {
    desc = "falha ao conectar ao servidor";
    goto err;
  }

  trace("Iniciando GET");

  client.printf("GET https://%s/%s HTTP/1.0\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n\r\n",
                host.c_str(),
                path.c_str(),
                host.c_str());

  // Timeout checker
  millisStart = millis();
  while (!client.available()) {
    if (millis() - millisStart > timeout) {
      desc = "timeout atingido";
      goto err;
    }

    vTaskDelay(100);
  }

  // Iterador de header, esse loop acaba assim que os headers acabam
  while (client.available()) {
    String line = client.readStringUntil('\n');
    line.trim();

    // Se não tiver nenhum byte, chegamos ao fim dos headers
    if (!line.length())
      break;

    // Checa se a request foi bem sucedida
    if (line.startsWith("HTTP/1.1"))
      if (line.indexOf("200") < 0) {
        desc = "Código HTTP != 200; header: " + String(line.c_str());
        goto err;
      }

    // Pega tamanho do binário
    if (line.startsWith("Content-Length: ")) {
      contentLen = atol(line.substring(strlen("Content-Length: ")).c_str());
      trace("Tamanho do firmware: %u", contentLen);
    }
  }

  // Se não existe tamanho ou não foi encontrado...
  if (!contentLen)
    goto end;

  canBegin = Update.begin(contentLen);

  if (!canBegin) {
    desc = "não há espaço suficiente";
    goto err;
  }

  trace("Iniciando atualização de firmware...");

  written = Update.writeStream(client);

  if (written != contentLen) {
    desc = "escreveu " + String(written) + "/" + contentLen + " bytes";
    goto err;
  }

  if (Update.end()) {
    trace("Atualização completa!");

    if (Update.isFinished()) {
      desc = "Atualização de firmware realizada com sucesso!";
    } else {
      desc = "Alguma coisa deu errado";
      goto err;
    }
  } else {
    desc = "código: " + String(Update.getError());
    goto err;
  }

end:
  client.flush();
  client.stop();

  trace("%s", desc.c_str());

  if (onSuccess)
    onSuccess(desc, true);

  vTaskDelete(NULL);

err:
  client.flush();
  client.stop();

  desc = "Erro ao atualizar firmware: " + desc;
  trace("%s", desc.c_str());

  if (onError)
    onError(desc, reboot);

  vTaskDelete(NULL);
}