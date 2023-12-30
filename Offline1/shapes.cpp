#include "co-ordinate.cpp"
#include <bits/stdc++.h>
#include <cmath>
#include <vector>

#define ANGLE_AT_CENTER 70.5287794 * M_PI / 180.0

// for task3, use this class to keep the computed values for efficiency
class Cylinder {
public:
  int sectors;
  double radius, height;
  std::vector<Point2D> points;

  void computePoints() {
    double angle_step = ANGLE_AT_CENTER / sectors;
    double angle = -ANGLE_AT_CENTER / 2.0;

    for (int i = 0; i <= sectors; i++) {
      double x1 = radius * cos(angle);
      double y1 = radius * sin(angle);

      points.push_back(Point2D(x1, y1));

      angle += angle_step;
    }
  }

  Cylinder(double radius, double height, int sectors) {
    this->radius = radius;
    this->height = height;
    this->sectors = sectors;
    computePoints();
  }

  void updateRadiusAndHeight(double radius, double height) {
    this->radius = radius;
    this->height = height;
    points.clear();

    if (this->radius < 0) {
      this->radius = 0;
    }
    computePoints();
  }
};

class Sphere {
private:
  void compute_circular_unit_vector();
  void compute_points_on_updated_radius();
  std::vector<std::vector<Point3D>>
      circular_unit_vectors; // only updated at constructor

  void compute_sphere_points();

public:
  int sectors, stacks;
  double radius;

  std::vector<std::vector<Point3D>>
      points; // every time radius is update, this is updated

  Sphere(double radius, int sectors, int stacks) {
    this->radius = radius;
    this->sectors = sectors;
    this->stacks = stacks;

    if (this->radius < 0.0) {
      this->radius = 0.0;
    }

    compute_circular_unit_vector();
    compute_points_on_updated_radius();
  }

  void update_radius(double radius) {
    this->radius = radius;
    points.clear();

    if (this->radius < 0) {
      this->radius = 0;
    }
    compute_points_on_updated_radius();
  }
};

void Sphere::compute_circular_unit_vector() {
  // from -1 to 1, divide into sectors, same for stacks
  double sector_step = 2.0 / sectors;
  double stack_step = 2.0 / stacks;
  for (int i = 0; i <= this->stacks; i++) {
    circular_unit_vectors.push_back(std::vector<Point3D>());

    double y = -1.0 + i * stack_step;
    for (int j = 0; j <= this->sectors; j++) {
      double x = -1.0 + j * sector_step;
      double z = 1.0;

      circular_unit_vectors[i].push_back(Point3D(x, y, z).normalize_point());
    }
  }
}

void Sphere::compute_points_on_updated_radius() {
  points.clear();
  for (int i = 0; i < this->circular_unit_vectors.size(); i++) {
    points.push_back(std::vector<Point3D>());
    for (int j = 0; j < this->circular_unit_vectors[i].size(); j++) {
      points[i].push_back(circular_unit_vectors[i][j] * this->radius);
    }
  }
}