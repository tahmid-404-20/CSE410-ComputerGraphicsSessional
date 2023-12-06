#include "vec.cpp"

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

  void moveForwardOrBackward(double distance, bool forward = true) {
    Vec lookUnit = getLookUnitVector();

    if (forward) {
      moveCameraPositionAlongVector(lookUnit, distance);
    } else {
      moveCameraPositionAlongVector(lookUnit, -distance);
    }
  }

  void moveLeftOrRight(double distance, bool left = true) {
    Vec right = getRightUnitVector();

    if (left) {
      moveCameraPositionAlongVector(right, -distance);
      moveLookPositionAlongVector(right, -distance);
    } else {
      moveCameraPositionAlongVector(right, distance);
      moveLookPositionAlongVector(right, distance);
    }
  }

  void moveUpOrDown(double distance, bool upDir = true) {
    Vec up = getUpUnitVector();

    if (upDir) {
      moveCameraPositionAlongVector(up, distance);
      moveLookPositionAlongVector(up, distance);
    } else {
      moveCameraPositionAlongVector(up, -distance);
      moveLookPositionAlongVector(up, -distance);
    }
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

  void moveUpOrDownWithoutChangingReferencePoint(double distance,
                                                 bool upDir = true) {
    Vec look = getLookVector();
    look = look * (-1);
    Vec up = getUpUnitVector();
    Vec right = look.getCrossProduct(up);  // the axis of rotation

    Vec xy = Vec(ex, ey, 0);
    Vec prev = look;

    double prev_angle = acos(prev.getDotProduct(xy) / (prev.getMagnitude() * xy.getMagnitude()));
    
    if(upDir) {
      ez += distance;
    } else {
      ez -= distance;
    }

    Vec new_look = getLookVector() * (-1);
    double new_angle = acos(new_look.getDotProduct(xy) / (new_look.getMagnitude() * xy.getMagnitude()));

    double angle = new_angle - prev_angle;

    if (upDir) {
      up = up.rotateAroundAxis(right, angle);
    } else {
      up = up.rotateAroundAxis(right, -angle);
    }

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