#include "vec.cpp"

#define BALL_ROTATION_ANGLE 0.05

class Ball {
public:
    double x_coord, y_coord;
    double radius;

    double speed;
    double move_angle_x;  // angle with x-axis in xy plane

    double ball_rotation_angle;
    double orientation_angle_x, orientation_angle_y;

    int sector_count;
    int stack_count;

    Vec rotation_axis;

    Ball() {
        x_coord = 0;
        y_coord = 0;
        radius = 1;
        speed = 0.5;
        move_angle_x = 0;
        ball_rotation_angle = 0.05;
        orientation_angle_x = 0;
        orientation_angle_y = 0;

        sector_count = 8;
        stack_count = 20;
    }

    // constructor to set co-ordinates, direction and speed
    Ball(double x, double y, double radius, double speed, double move_angle_x, double ball_rotation_angle = BALL_ROTATION_ANGLE) {
        this->x_coord = x;
        this->y_coord = y;
        this->radius = radius;
        this->speed = speed;
        this->move_angle_x = move_angle_x;
        this->ball_rotation_angle = ball_rotation_angle;
        this->orientation_angle_x = 0;
        this->orientation_angle_y = 0;

        this->sector_count = 8;
        this->stack_count = 20;
    }

    void changeMoveDirectionAngle(bool clockwise = true) {
        if (clockwise) {
            move_angle_x -= ball_rotation_angle;
        } else {
            move_angle_x += ball_rotation_angle;
        }
    }

    void moveForwardOrBackward(bool forward = true) {
        double d_x, d_y;
        if (forward) {
            d_x = speed * cos(move_angle_x);
            d_y = speed * sin(move_angle_x);
        } else {
            d_x = -speed * cos(move_angle_x);
            d_y = -speed * sin(move_angle_x);
        }

        x_coord += d_x;
        y_coord += d_y;

        orientation_angle_x += d_x / radius;
        orientation_angle_y -= d_y / radius;
    }
};
