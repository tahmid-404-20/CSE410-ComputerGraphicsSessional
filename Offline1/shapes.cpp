#include "co-ordinate.cpp"
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

    for (int i = 0; i < sectors; i++) {
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