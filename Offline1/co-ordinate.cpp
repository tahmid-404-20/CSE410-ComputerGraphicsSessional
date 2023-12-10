#pragma once
#include <cmath>
#include "vec.cpp"

class Point3D {
public:
  double x, y, z;
  Point3D() {
    x = 0;
    y = 0;
    z = 0;
  }
  Point3D(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  Vec getVector() {
  return Vec(this->x,this->y,this->z);
}

void assignVector(Vec v) {
  this->x = v.x;
  this->y = v.y;
  this->z = v.z;
}
};

Point3D getCartesianCoordinates3D(double r, double phi, double theta) {
  phi = phi * M_PI / 180;
  theta = theta * M_PI / 180;
  return Point3D(r * cos(phi) * cos(theta), r * cos(phi) * sin(theta),
                 r * sin(phi));
}
