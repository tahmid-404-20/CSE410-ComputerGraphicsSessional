#pragma once
#include "vec.cpp"
#include <cmath>

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

  Vec getVector() { return Vec(this->x, this->y, this->z); }

  void assignVector(Vec v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
  }

  Point3D normalize_point() {
    double magnitude = sqrt(x * x + y * y + z * z);
    return Point3D(x / magnitude, y / magnitude, z / magnitude);
  }

  Point3D operator*(double scalar) {
    return Point3D(this->x * scalar, this->y * scalar, this->z * scalar);
  }
};

class Point2D {
public:
  double x, y;
  Point2D() {
    x = 0;
    y = 0;
  }
  Point2D(double x, double y) {
    this->x = x;
    this->y = y;
  }
};

class PointPair2D {
public:
  Point2D p1, p2;

  PointPair2D(Point2D p1, Point2D p2) {
    this->p1 = p1;
    this->p2 = p2;
  }
};

class PointPair {
public:
  Point3D p1, p2;

  PointPair(Point3D p1, Point3D p2) {
    this->p1 = p1;
    this->p2 = p2;
  }
};

Point3D getCartesianCoordinates3D(double r, double phi, double theta) {
  phi = phi * M_PI / 180;
  theta = theta * M_PI / 180;
  return Point3D(r * cos(phi) * cos(theta), r * cos(phi) * sin(theta),
                 r * sin(phi));
}
