#include <cmath>
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

  Vec operator/(double scalar) {
    Vec result;
    result.x = x / scalar;
    result.y = y / scalar;
    result.z = z / scalar;

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
};


// int main() {
//   Vec lookAt(0, 0, 0);
//   Vec up(0, 1, 0);
//   Vec eye(0, 0, 5);

//   double angle_in_radians = 30 * M_PI / 180;

//   Vec look = (lookAt - eye).getNormalizedResult();
//   Vec right = (look.getCrossProduct(up)).getNormalizedResult();

//   Vec new_look = look.rotateAroundAxis(right, angle_in_radians);
//   Vec new_up = up.rotateAroundAxis(right, angle_in_radians);

//   printf("new_look: %lf %lf %lf\n", new_look.x, new_look.y, new_look.z);
//   printf("new_up: %lf %lf %lf\n", new_up.x, new_up.y, new_up.z);
// }