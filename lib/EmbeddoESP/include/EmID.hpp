/**
 * @file EmID.hpp
 * @author Julio César Caldeira <julio@embeddo.io>
           Maria Beatriz Florencio <beatriz@embeddo.io>
 * @brief ID related declarations
 * @date 2021-10-04
 *
 * @copyright Copyright (c) 2021 Embeddo
 *
 */

#pragma once

#include <Arduino.h>

namespace Embeddo {
namespace ID {
  String   getID();
  String   downloadID();
  uint32_t chipID();
  bool     saveID(String id);
}
} // namespace Embeddo