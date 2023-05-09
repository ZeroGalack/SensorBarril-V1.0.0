/**
 * @file EmHttpsUpdate.hpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Utilitários para atualização remota
 * @version 0.1
 * @date 2022-05-15
 *
 * Copyright (c) 2022 Embeddo
 *
 * Por enquanto usamos um servidor estático, com binário de firmware público. O
 * método de atualização atual é simplesmente um GET no arquivo de firmware.
 *
 * TODO:
 * - Definir tipo de request (POST vs GET)
 * - Definir parâmetros de request (incluir ID do chip?)
 *
 */

#pragma once

#include <EmService.hpp>

namespace Embeddo {
namespace HttpsUpdate {
/**
 * @brief Checa se existe atualização disponível
 *
 */
void checkUpdate();

/**
 * @brief Inicia atualização
 *
 */
void doUpdate();

/**
 * Callbacks
 *
 * Callbacks definidas para sucesso e erro. Os parâmetros passados são a
 * descrição da operação e flag de reinicialização necessária
 *
 */

extern std::function<void(String, bool)> onError;   ///< Callback para erro
extern std::function<void(String, bool)> onSuccess; ///< Callback para sucesso
extern String host; ///< Host para atualização remota
extern String path; ///< Caminho do arquivo (experimental)
} // namespace HttpsUpdate
} // namespace Embeddo