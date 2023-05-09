/**
 * @file EmMath.hpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Declarações de métodos e acessórios matemáticos
 * @version 0.1
 * @date 2021-12-02
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <Arduino.h>

namespace Embeddo {
/**
 * @brief Enumeração para eixos dimensionais
 *
 */
enum Axis {
  AXIS_N, ///< Norma/intensidade
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
  AXIS_W, ///< Se necessário multiplicações com matrizes 4x4
};

struct vec3;

/**
 * @brief Representa um vetor de três dimensões
 *
 */
struct Vec {
  double x; ///< Componente X
  double y; ///< Componente Y
  double z; ///< Componente Z

  explicit Vec() : x(0), y(0), z(0) {
  }
  Vec(double x, double y, double z) : x(x), y(y), z(z) {
  }

  /**
   * @brief Retorna módulo (intensidade, norma) do vetor
   *
   * @return double
   */
  virtual double mag() const;

  /**********************************************
   * OPERADORES                                 *
   **********************************************/

  double  operator[](Axis axis) const;
  double &operator[](Axis axis);

  /* Operadores unários */
  Vec operator-() const;

  /* Operadores de aritmética */
  Vec operator*(const double &val);
  Vec operator/(const double &val) {
    return this->operator*(1 / val);
  }

  Vec operator+(const Vec &val) const;

  /* Operadores de atribuição */
  void operator*=(const double &val);
  void operator/=(const double &val) {
    this->operator*=(1 / val);
  }
  void operator+=(const double &val);
  void operator-=(const double &val) {
    this->operator+=(-val);
  }
  void operator+=(const Vec &val);
  void operator-=(const Vec &val) {
    this->operator+=(-val);
  }

private:
  double _dummy;
  double m_mag;
};

struct vec3 {
  float x; ///< Componente X
  float y; ///< Componente Y
  float z; ///< Componente Z

  explicit vec3() : x(0), y(0), z(0) {
  }

  vec3(double x, double y, double z) : x(x), y(y), z(z) {
  }

  vec3(const Vec &vec) : x(vec.x), y(vec.y), z(vec.z) {
  }

  Vec toVec() {
    Vec vec;
    vec.x = this->x;
    vec.y = this->y;
    vec.z = this->z;
    return vec;
  }
} __attribute__((packed));

inline uint8_t bcd2uint(uint8_t bcd) {
  return (bcd & 0b00001111) + ((bcd & 0b11110000) >> 4) * 10;
}

inline uint8_t uint2bcd(uint8_t u) {
  return ((u / 10) << 4) + (u % 10);
}
} // namespace Embeddo
