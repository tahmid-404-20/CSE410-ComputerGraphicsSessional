#include "co-ordinate.cpp"
#include "vec.cpp"
#include <vector>

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

  int collision_count;

  std::vector<std::vector<PointPair>> point_pair_array;

  void generatePointsBall();
  void generatePointsBall2();

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

    collision_count = 0;

    generatePointsBall();
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

    this->collision_count = 0;

    generatePointsBall();
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

    for (int i = 0; i < point_pair_array.size(); i++) {
      for (int j = 0; j < point_pair_array[i].size(); j++) {
        point_pair_array[i][j].p1.assignVector(
            point_pair_array[i][j].p1.getVector().rotateAroundAxis(
                rotation_axis, angle));
        point_pair_array[i][j].p2.assignVector(
            point_pair_array[i][j].p2.getVector().rotateAroundAxis(
                rotation_axis, angle));
      }
    }
  }

  void fixPointingVector() {
    pointing_vector = prev_pointing_vector;

    double angle = speed / radius / 2.0; // avg before and after collision

    double temp_move_angle_x = this->move_angle_x;
    this->move_angle_x = this->prev_move_angle_x;

    // rotate at the direction before collision
    if (last_move == FORWARD) {
      rotateBall(angle);
    } else {
      rotateBall(-angle);
    }

    this->move_angle_x = temp_move_angle_x;
    if (last_move == FORWARD) {
      rotateBall(angle);
    } else {
      rotateBall(-angle);
    }
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
  }
};

void Ball::generatePointsBall() {
  for (int stackStep = 0; stackStep < stack_count; stackStep++) {
    double phi1 = 90.0 - (180.0 * stackStep) / stack_count;
    double phi2 = 90.0 - (180.0 * (stackStep + 1)) / stack_count;

    point_pair_array.push_back(std::vector<PointPair>());

    for (int sectorStep = 0; sectorStep < sector_count; sectorStep++) {
      double theta = (360.0 * sectorStep) / sector_count;

      // anticlockWise, p1,p2,p3,p4
      Point3D p1 = getCartesianCoordinates3D(radius, phi1, theta);
      Point3D p2 = getCartesianCoordinates3D(radius, phi2, theta);

      point_pair_array[stackStep].push_back(PointPair(p1, p2));
    }
  }
}