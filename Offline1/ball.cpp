#include "co-ordinate.cpp"
#include "vec.cpp"
#include <vector>

#define BALL_ROTATION_ANGLE 0.05

enum LastMove { FORWARD, BACKWARD };

class Sector {
public:
  Point3D p1, p2, p3, p4;

  Sector(Point3D p1, Point3D p2, Point3D p3, Point3D p4) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->p4 = p4;
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

  // points of the ball, its a vector of vectors of Point3D
  std::vector<std::vector<Sector>> points;
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
    generatePointsBall2();
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
    generatePointsBall2();
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

  void rotateBall3(double angle) {
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

  void rotateBall2(double angle) {
    Vec rotation_axis = Vec(-sin(move_angle_x), cos(move_angle_x), 0);

    for (int i = 0; i < points.size(); i++) {
      for (int j = 0; j < points[i].size(); j++) {
        points[i][j].p1.assignVector(
            points[i][j].p1.getVector().rotateAroundAxis(rotation_axis, angle));
        points[i][j].p2.assignVector(
            points[i][j].p2.getVector().rotateAroundAxis(rotation_axis, angle));
        points[i][j].p3.assignVector(
            points[i][j].p3.getVector().rotateAroundAxis(rotation_axis, angle));
        points[i][j].p4.assignVector(
            points[i][j].p4.getVector().rotateAroundAxis(rotation_axis, angle));
      }
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

    // computeAngles();

    double angle = speed / radius / 2.0; // avg before and after collision

    double temp_move_angle_x = this->move_angle_x;
    this->move_angle_x = this->prev_move_angle_x;

    // rotate at the direction before collision
    if (last_move == FORWARD) {
      rotateBall3(angle);
    } else {
      rotateBall3(-angle);
    }

    // computeAngles();

    this->move_angle_x = temp_move_angle_x;
    if (last_move == FORWARD) {
      rotateBall3(angle);
    } else {
      rotateBall3(-angle);
    }

    // computeAngles();
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

    rotateBall3(angle);
    computeAngles();
  }
};

void Ball ::generatePointsBall() {
  for (int stackStep = 0; stackStep < stack_count; stackStep++) {
    double phi1 = 90.0 - (180.0 * stackStep) / stack_count;
    double phi2 = 90.0 - (180.0 * (stackStep + 1)) / stack_count;

    points.push_back(std::vector<Sector>());

    for (int sectorStep = 0; sectorStep < sector_count; sectorStep++) {
      double theta1 = (360.0 * sectorStep) / sector_count;
      double theta2 = (360.0 * (sectorStep + 1)) / sector_count;

      // anticlockWise, p1,p2,p3,p4

      // ph1,theta1
      Point3D p1 = getCartesianCoordinates3D(radius, phi1, theta1);

      // phi2, theta1
      Point3D p2 = getCartesianCoordinates3D(radius, phi2, theta1);

      // phi2, theta2
      Point3D p3 = getCartesianCoordinates3D(radius, phi2, theta2);

      // phi1, theta2
      Point3D p4 = getCartesianCoordinates3D(radius, phi1, theta2);

      points[stackStep].push_back(Sector(p1, p2, p3, p4));
    }
  }
}


void Ball::generatePointsBall2() {
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