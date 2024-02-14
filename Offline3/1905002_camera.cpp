// #include "bits/stdc++.h"
#include "1905002_vec.cpp"

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

  // a constructor to set the camera position, look position and up vector
  Camera(double ex, double ey, double ez, double lx, double ly, double lz,
         double ux, double uy, double uz) {
    this->ex = ex;
    this->ey = ey;
    this->ez = ez;
    this->lx = lx;
    this->ly = ly;
    this->lz = lz;
    this->ux = ux;
    this->uy = uy;
    this->uz = uz;

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
      moveLookPositionAlongVector(lookUnit, distance);
    } else {
      moveCameraPositionAlongVector(lookUnit, -distance);
      moveLookPositionAlongVector(lookUnit, -distance);
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
    look = look * (-1); // from lookAt to eye
    Vec up = getUpUnitVector();

    Vec look_xy = Vec(look.x, look.y, 0);
    Vec rotation_axis = Vec(look_xy.y, -look_xy.x, 0);

    // printf("rotation_axis mag %lf: ", rotation_axis.getMagnitude());

    if (rotation_axis.getMagnitude() < 0.000001) {
      if (upDir) {
        ez += distance;
      } else {
        ez -= distance;
      }

      return;
    }

    Vec new_look;
    if (upDir) {
      new_look = Vec(ex - lx, ey - ly, ez - lz + distance);
      ez += distance;
    } else {
      new_look = Vec(ex - lx, ey - ly, ez - lz - distance);
      ez -= distance;
    }
    double angle = new_look.getAngleBetweenVector(look);

    if (upDir) {
      up = up.rotateAroundAxis(rotation_axis, angle);
    } else {
      up = up.rotateAroundAxis(rotation_axis, -angle);
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