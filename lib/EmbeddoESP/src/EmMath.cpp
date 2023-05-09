/**
 * @file EmMath.cpp
 * @author Julio César Caldeira <julio@embeddo.io>
 * @brief Implementação de funções e acessórios matemáticos
 * @version 0.1
 * @date 2021-12-02
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <Arduino.h>
#include <EmMath.hpp>

using namespace Embeddo;

double Vec::operator[](Axis axis) const {
  switch (axis) {
    case AXIS_X:
      return this->x;
    case AXIS_Y:
      return this->y;
    case AXIS_Z:
      return this->z;
    case AXIS_N:
      return this->mag();
    default:
      return this->_dummy;
  }
}

double &Vec::operator[](Axis axis) {
  switch (axis) {
    case AXIS_X:
      return this->x;
    case AXIS_Y:
      return this->y;
    case AXIS_Z:
      return this->z;
    case AXIS_N:
      this->m_mag = this->mag();
      return this->m_mag;
    default:
      return this->_dummy;
  }
}

Vec Vec::operator-() const {
  Vec v = *this;

  for (int i = AXIS_X; i <= AXIS_Z; i++)
    v[(Axis) i] = -v[(Axis) i];

  return v;
}

Vec Vec::operator+(const Vec &val) const {
  Vec v = *this;
  v += val;
  return v;
}
void Vec::operator*=(const double &val) {
  this->x *= val;
  this->y *= val;
  this->z *= val;
}

void Vec::operator+=(const double &val) {
  this->x += val;
  this->y += val;
  this->z += val;
}

void Vec::operator+=(const Vec &val) {
  this->x += val.x;
  this->y += val.y;
  this->z += val.z;
}

Vec Vec::operator*(const double &val) {
  Vec v = *this;

  v *= val;

  return v;
}

double Vec::mag() const {
  return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}