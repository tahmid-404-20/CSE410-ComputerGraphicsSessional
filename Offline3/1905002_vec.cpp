#include <cmath>
#include <ostream>
#include <vector>
// #include <bits/stdc++.h>
#pragma once
class Vec {
public:
  double x, y, z;

  Vec() {
    x = 0;
    y = 0;
    z = 0;
  }

  Vec(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  // overloaded operators
  Vec operator+(Vec v) {
    Vec result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;

    return result;
  }

  Vec operator-(Vec v) {
    Vec result;
    result.x = x - v.x;
    result.y = y - v.y;
    result.z = z - v.z;

    return result;
  }

  Vec operator*(double scalar) {
    Vec result;
    result.x = x * scalar;
    result.y = y * scalar;
    result.z = z * scalar;

    return result;
  }

  Vec operator*(Vec v) {
    Vec result;
    result.x = x * v.x;
    result.y = y * v.y;
    result.z = z * v.z;

    return result;
  }

  Vec operator/(double scalar) {
    Vec result;
    result.x = x / scalar;
    result.y = y / scalar;
    result.z = z / scalar;

    return result;
  }

  // overload unary -
  Vec operator-() {
    Vec result;
    result.x = -x;
    result.y = -y;
    result.z = -z;

    return result;
  }

  // methods
  double getMagnitude() { return sqrt(x * x + y * y + z * z); }

  Vec getCrossProduct(Vec v) {
    Vec result;
    result.x = y * v.z - z * v.y;
    result.y = z * v.x - x * v.z;
    result.z = x * v.y - y * v.x;

    return result;
  }

  double getDotProduct(Vec v) { return x * v.x + y * v.y + z * v.z; }

  Vec getNormalizedResult() {
    Vec result;
    double magnitude = sqrt(x * x + y * y + z * z);

    result.x = x / magnitude;
    result.y = y / magnitude;
    result.z = z / magnitude;

    return result;
  }

  Vec rotateAroundAxis(Vec axis, double theta) {
    axis = axis.getNormalizedResult();

    Vec x = *this;
    Vec v1 = x * cos(theta);                         // xcos(theta)
    Vec v2 = (axis.getCrossProduct(x) * sin(theta)); // (a x x)sin(theta)
    Vec v3 = axis * ((axis.getDotProduct(x)) *
                     (1 - cos(theta))); // a(a . x)(1 - cos(theta))

    return v1 + v2 + v3;
  }

  double getAngleBetweenVector(Vec v) {
    Vec a = *this;
    Vec b = v;

    double dot_product = a.getDotProduct(b);
    double magnitude_a = a.getMagnitude();
    double magnitude_b = b.getMagnitude();

    double res = dot_product / (magnitude_a * magnitude_b);

    if (res > 1) {
      res = 1;
    } else if (res < -1) {
      res = -1;
    }

    return acos(dot_product / (magnitude_a * magnitude_b));
  }

  // overload << operator
  friend std::ostream &operator<<(std::ostream &os, const Vec &v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
  }
};

// 3X3 double matrix
class Matrix {
public:
  double matrix[3][3];

  Matrix() {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        matrix[i][j] = 0;
      }
    }
  }

  Matrix(std::vector<std::vector<double>> matrix) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        this->matrix[i][j] = matrix[i][j];
      }
    }
  }

  // methods
  double determinant() {
    return matrix[0][0] *
               (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] *
               (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] *
               (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
  }
};
