#include "vec.cpp"

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

  LastMove last_move;

  int sector_count;
  int stack_count;

  Vec rotation_axis;

  Vec pointing_vector;

  Ball() {
    x_coord = 0;
    y_coord = 0;
    radius = 1;
    speed = 0.5;
    move_angle_x = M_PI / 4;
    ball_rotation_angle = 0.05;

    rotation_angle_x = 0;
    rotation_angle_y = 0;

    sector_count = 8;
    stack_count = 20;

    pointing_vector = Vec(0, 0, 1);

    last_move = FORWARD;
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

    this->sector_count = 8;
    this->stack_count = 20;

    this->pointing_vector = Vec(0, 0, 1);
    this->last_move = FORWARD;
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

  void rotateBall(double angle) {
    Vec rotation_axis = Vec(-sin(move_angle_x), cos(move_angle_x), 0);
    prev_pointing_vector = pointing_vector;
    pointing_vector = pointing_vector.rotateAroundAxis(rotation_axis, angle)
                          .getNormalizedResult();

    prev_move_angle_x = move_angle_x;
  }

  void fixPointingVector() {
    pointing_vector = prev_pointing_vector;
    computeAngles();

    // speed = speed / 2;
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

    // if (forward) {
    //   angle = -angle;
    // }

    rotateBall(angle);  
    computeAngles();
  }
};
