#include <cmath>
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

  Vec operator+=(Vec v) {
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
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

class Camera {
private:
public:
  double ex, ey, ez;
  double lx, ly, lz;
  double ux, uy, uz;

private:
  Vec getLookVector() {
    Vec look;
    look.x = lx - ex;
    look.y = ly - ey;
    look.z = lz - ez;

    return look;
  }

public:
  Camera() {
    ex = 20;
    ey = 20;
    ez = 20;
    lx = 0;
    ly = 0;
    lz = 0;
    ux = -1.0 / 2.0;
    uy = -1.0 / 2.0;
    uz = 1;

    setUpVector(Vec(ux, uy, uz).getNormalizedResult());
  }

  //  setters
  void setUpVector(Vec up) {
    ux = up.x;
    uy = up.y;
    uz = up.z;
  }
  Vec getLookUnitVector() {
    Vec look;
    look.x = lx - ex;
    look.y = ly - ey;
    look.z = lz - ez;

    return look.getNormalizedResult();
  }

  Vec getRightUnitVector() {
    Vec look = getLookUnitVector();
    Vec up(ux, uy, uz);

    Vec right = look.getCrossProduct(up);

    return right.getNormalizedResult();
  }

  Vec getUpUnitVector() {
    Vec up = Vec(ux, uy, uz);

    return up.getNormalizedResult();
  }

  void moveCameraPositionAlongVector(Vec look, double distance) {
    ex += look.x * distance;
    ey += look.y * distance;
    ez += look.z * distance;
  }

  void moveLookPositionAlongVector(Vec look, double distance) {
    lx += look.x * distance;
    ly += look.y * distance;
    lz += look.z * distance;
  }

  void lookLeftOrRight(double angle, bool left = true) {
    Vec look = getLookVector();
    Vec up = Vec(ux, uy, uz);

    if (left) {
      look = look.rotateAroundAxis(up, angle);
    } else {
      look = look.rotateAroundAxis(up, -angle);
    }

    lx = ex + look.x;
    ly = ey + look.y;
    lz = ez + look.z;
  }

  void lookUpOrDown(double angle, bool upDir = true) {
    Vec look = getLookVector();
    Vec up = Vec(ux, uy, uz);
    Vec right = look.getCrossProduct(up);

    if (upDir) {
      look = look.rotateAroundAxis(right, angle);
      up = up.rotateAroundAxis(right, angle);
    } else {
      look = look.rotateAroundAxis(right, -angle);
      up = up.rotateAroundAxis(right, -angle);
    }

    lx = ex + look.x;
    ly = ey + look.y;
    lz = ez + look.z;

    setUpVector(up);
  }

  void tiltClockwiseOrAntiClockwise(double angle, bool clockwise = true) {
    Vec look = getLookVector();
    Vec up = Vec(ux, uy, uz);

    if (clockwise) {
      up = up.rotateAroundAxis(look, angle);
    } else {
      up = up.rotateAroundAxis(look, -angle);
    }

    setUpVector(up);
  }
};