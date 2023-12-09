#include "vec.cpp"
#include <bits/stdc++.h>

#define BALL_ROTATION_ANGLE 0.05

enum LastMove { FORWARD, BACKWARD };

class Ball {
private:
  Vec prev_pointing_vector;
  double prev_move_angle_x;

public:
  double x_coord, y_coord;
  double radius;

  double speed;
  double move_angle_x; // angle with x-axis in xy plane

  double ball_rotation_angle;

  double rotation_angle_x, rotation_angle_y;
  double rotation_angle_z;

  LastMove last_move;

  int sector_count;
  int stack_count;

  Vec rotation_axis;

  Vec pointing_vector;

  int collision_count;

  Ball() {
    x_coord = 0;
    y_coord = 0;
    radius = 1;
    speed = 0.3;
    move_angle_x = M_PI / 4;
    ball_rotation_angle = 0.05;

    rotation_angle_x = 0;
    rotation_angle_y = 0;
    rotation_angle_z = 0;

    sector_count = 8;
    stack_count = 20;

    pointing_vector = Vec(0, 0, 1);

    last_move = FORWARD;

    collision_count = 0;
  }

  // constructor to set co-ordinates, direction and speed
  Ball(double x, double y, double radius, double speed, double move_angle_x,
       double ball_rotation_angle = BALL_ROTATION_ANGLE) {
    this->x_coord = x;
    this->y_coord = y;
    this->radius = radius;
    this->speed = speed;
    this->move_angle_x = move_angle_x;
    this->ball_rotation_angle = ball_rotation_angle;

    this->rotation_angle_x = 0;
    this->rotation_angle_y = 0;
    this->rotation_angle_z = 0;

    this->sector_count = 8;
    this->stack_count = 20;

    this->pointing_vector = Vec(0, 0, 1);
    this->last_move = FORWARD;

    this->collision_count = 0;
  }

  void computeAngles() {
    double a, b, c;
    a = pointing_vector.x;
    b = pointing_vector.y;
    c = pointing_vector.z;

    double lamda = sqrt(b * b + c * c);

    // rotate around y-axis first!!!!!
    rotation_angle_x = -atan2(b, c);    // rotate-clockwise around x-axis
    rotation_angle_y = atan2(a, lamda); // rotate-anticlockwise around y-axis
  }

  void changeMoveDirectionAngle(bool clockwise = true) {
    if (clockwise) {
      move_angle_x -= ball_rotation_angle;
    } else {
      move_angle_x += ball_rotation_angle;
    }
  }

  double changeInRotationAngleZ(Vec direction_vector, Vec pointing_vector,
                                double angle) {
    double change_in_z = 0.0;

    Vec pointing_vector_xy = pointing_vector.getProjectionXYPlane();

    if (pointing_vector_xy.getMagnitude() == 0.0) {
      printf("I am zero\n");
      return 0;
    }

    Vec rotation_axis = Vec(-direction_vector.y, direction_vector.x, 0);

    double theta1 = pointing_vector_xy.angleBetweenVectors(direction_vector);
    double theta2 = pointing_vector_xy.angleBetweenVectors(rotation_axis);

    // printf("theta1: %lf\n", theta1 * 180 / M_PI);
    // printf("theta2: %lf\n", theta2 * 180 / M_PI);

    double ninety = M_PI / 2;

    if (theta2 == ninety) {
      printf("Theta2 is zero\n");
      return 0;
    }

    // double angle_in_degree =angle * 180 / M_PI;
    change_in_z = -angle * sin(theta1) * pointing_vector_xy.getMagnitude();
    printf("change in z %lf\n", change_in_z);
    if (theta2 > ninety) { // anticlockwise assuming forward, if backward, angle
                           // is negative

    } else { // clockwise assuming forward
      change_in_z = -change_in_z;
    }

    return change_in_z;
  }

  void rotateBall(double angle) {
    // z needs to be computed differently, during rotation
    double change_in_z =
        changeInRotationAngleZ(Vec(cos(move_angle_x), sin(move_angle_x), 0),
                               pointing_vector, angle) /
        2.0;
    // printf("change in z rotateBall %lf\n", change_in_z);
    rotation_angle_z += change_in_z;

    Vec rotation_axis = Vec(-sin(move_angle_x), cos(move_angle_x), 0);
    prev_pointing_vector = pointing_vector;
    pointing_vector = pointing_vector.rotateAroundAxis(rotation_axis, angle)
                          .getNormalizedResult();

    change_in_z =
        changeInRotationAngleZ(Vec(cos(move_angle_x), sin(move_angle_x), 0),
                               pointing_vector, angle / 2.0);
    // printf("change in z rotateBall %lf\n", change_in_z);
    rotation_angle_z += change_in_z;

    // printf("rotation_angle_z: %lf\n", rotation_angle_z);
    prev_move_angle_x = move_angle_x;
  }

  void fixPointingVector() {
    pointing_vector = prev_pointing_vector;

    computeAngles();

    double angle = speed / radius / 2.0; // avg before and after collision

    double temp_move_angle_x = this->move_angle_x;
    this->move_angle_x = this->prev_move_angle_x;

    // rotate at the direction before collision
    if (last_move == FORWARD) {
      rotateBall(angle);
    } else {
      rotateBall(-angle);
    }

    computeAngles();

    this->move_angle_x = temp_move_angle_x;
    if (last_move == FORWARD) {
      rotateBall(angle);
    } else {
      rotateBall(-angle);
    }

    computeAngles();
  }

  void moveForwardOrBackward(bool forward = true) {
    Vec move_direction = Vec(cos(move_angle_x), sin(move_angle_x), 0);

    double d_x, d_y;
    double angle = speed / radius;
    if (forward) {
      d_x = speed * cos(move_angle_x);
      d_y = speed * sin(move_angle_x);
      last_move = FORWARD;
    } else {
      d_x = -speed * cos(move_angle_x);
      d_y = -speed * sin(move_angle_x);
      angle = -angle;
      last_move = BACKWARD;
    }

    x_coord += d_x;
    y_coord += d_y;

    rotateBall(angle);
    computeAngles();
  }
};
