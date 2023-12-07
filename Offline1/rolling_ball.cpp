#include "ball.cpp"
#include "camera.cpp"
#include "co-ordinate.cpp"
#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define CHECKERBOARD_SIZE 80

#define BOUNDARY_RADIUS 12
#define BOUNDARY_DIMENSION 4
#define BOUNDARY_HEIGHT 2

#define CLEARANCE 0.1

#define SIMULATION_CALL_INTERVAL 50

#define DT 10

#define PI 3.1415926535897

using namespace std;

int animate = 1;
double step = 0.5;
double rotation_angle = 0.05;

int x_coord, y_coord;
bool rotateZ = false;

bool simulationMode = false;

// a openGL integer
GLint counter = 0;
Camera camera;
Ball ball;

void init() {
  printf("Do your initialization here\n");
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f); // Set background color to black and opaque

  counter = 0;
  x_coord = 0;
  y_coord = 0;
  rotateZ = false;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 500);
}

void checkCollisionManual(Ball *ball) {
  double x = ball->x_coord;
  double y = ball->y_coord;
  double r = ball->radius;

  double dx = ball->speed * cos(ball->move_angle_x);
  double dy = ball->speed * sin(ball->move_angle_x);

  bool x_change = false;
  bool y_change = false;

  if (x + r > BOUNDARY_RADIUS ||
      x - r < -BOUNDARY_RADIUS) { // collision with plane orthogonal to x-axis

    ball->move_angle_x = M_PI - ball->move_angle_x;
    x_change = true;
    printf("x_change\n");
  }

  if (y + r > BOUNDARY_RADIUS || y - r < -BOUNDARY_RADIUS) {
    ball->move_angle_x = -ball->move_angle_x;
    y_change = true;
    printf("y_change\n");
  }

  double B;
  if (x_change) {
    double prev_x = x - dx;
    if (x + r > BOUNDARY_RADIUS) {
      B = BOUNDARY_RADIUS - r;
    } else {
      B = -BOUNDARY_RADIUS + r;
    }
    ball->x_coord = B - (x - B);
  }

  if (y_change) {
    if (y + r > BOUNDARY_RADIUS) {
      B = BOUNDARY_RADIUS - r;
    } else {
      B = -BOUNDARY_RADIUS + r;
    }
    ball->y_coord = B - (y - B);
  }

  if (x_change || y_change) {
    ball->fixPointingVector();
  }
}

void keyboardSpecialListener(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    printf("Up arrow pressed\n");
    camera.moveForwardOrBackward(step, true);
    break;

  case GLUT_KEY_DOWN:
    printf("Down arrow pressed\n");
    camera.moveForwardOrBackward(step, false);
    break;

  case GLUT_KEY_RIGHT:
    printf("Right arrow pressed\n");
    { camera.moveLeftOrRight(step, false); }
    break;

  case GLUT_KEY_LEFT:
    printf("Left arrow pressed\n");
    { camera.moveLeftOrRight(step, true); }
    break;

  case GLUT_KEY_PAGE_UP:
    printf("Page up pressed\n");
    { camera.moveUpOrDown(step, true); }
    break;

  case GLUT_KEY_PAGE_DOWN:
    printf("Page down pressed\n");
    { camera.moveUpOrDown(step, false); }
    break;

  default:
    printf("We don't know what you pressedBRO\n");
    break;
  }
}

void keyboardListener(unsigned char key, int x, int y) {
  switch (key) {
  case '1': // look left
    printf("1 pressed\n");
    { camera.lookLeftOrRight(rotation_angle, true); }
    break;

  case '2': // look right
    printf("2 pressed\n");
    { camera.lookLeftOrRight(rotation_angle, false); }
    break;

  case '3': // look up
    printf("3 pressed\n");
    { camera.lookUpOrDown(rotation_angle, true); }
    break;

  case '4': // look down
    printf("4 pressed\n");
    { camera.lookUpOrDown(rotation_angle, false); }
    break;

  case '5': // tilt anti-clockwise
    printf("5 pressed\n");
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, false); }
    break;

  case '6': // tilt clockwise
    printf("6 pressed\n");
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, true); }
    break;

    // case 'w':
    //   printf("w pressed\n");
    //   { camera.moveUpOrDownWithoutChangingReferencePoint(step*2, true); }
    //   break;

    // case 's':
    //   printf("w pressed\n");
    //   { camera.moveUpOrDownWithoutChangingReferencePoint(step*2, false); }
    //   break;

  case 'j': // rotate counter-clockwise
    printf("j pressed\n");
    { ball.changeMoveDirectionAngle(false); }
    break;
  case 'l': // rotate clockwise
    printf("l pressed\n");
    { ball.changeMoveDirectionAngle(true); }
    break;

  case 'i': // move forward
    printf("i pressed\n");
    {
      if (!simulationMode) {
        ball.moveForwardOrBackward(true);
        checkCollisionManual(&ball);
      }
    }
    break;
  case 'k': // move backward
    printf("k pressed\n");
    {
      if (!simulationMode) {
        ball.moveForwardOrBackward(false);
        checkCollisionManual(&ball);
      }
    }
    break;

  case ' ':
    printf("space pressed\n");
    { simulationMode = !simulationMode; }
    break;

  default:
    printf("We don't know what you pressed\n");
    break;
  }
}

void drawCheckerBox(double a, int color = 0) {
  // color = 0 -> black, color = 1 -> white
  glBegin(GL_QUADS);
  {
    if (color == 0) {
      glColor3f(0.0f, 0.0f, 0.0f); // Black
    } else {
      glColor3f(1.0f, 1.0f, 1.0f); // White
    }
    glVertex3f(0, 0, 0);
    glVertex3f(0, a, 0);
    glVertex3f(a, a, 0);
    glVertex3f(a, 0, 0);
  }
  glEnd();
}

void drawCheckers(double a) {
  for (int i = -CHECKERBOARD_SIZE; i < CHECKERBOARD_SIZE; i++) {
    for (int j = -CHECKERBOARD_SIZE; j < CHECKERBOARD_SIZE; j++) {
      glPushMatrix();
      glTranslatef(i * a, j * a, 0);
      drawCheckerBox(a, (i + j) % 2);
      glPopMatrix();
    }
  }
}

bool show = false;

void drawSphere(double r, int sectorCount, int stackCount) {
  // both ints are even
  bool alterColor = false;

  bool isRed = false;
  for (int stackStep = 0; stackStep < stackCount; stackStep++) {
    double phi1 = 90.0 - (180.0 * stackStep) / stackCount;
    double phi2 = 90.0 - (180.0 * (stackStep + 1)) / stackCount;

    if (show)
      printf("phi1: %lf, phi2: %lf\n", phi1, phi2);

    if (stackStep * 2 >= stackCount) {
      isRed = true;
    }

    for (int sectorStep = 0; sectorStep < sectorCount; sectorStep++) {
      double theta1 = (360.0 * sectorStep) / sectorCount;
      double theta2 = (360.0 * (sectorStep + 1)) / sectorCount;

      if (show)
        printf("theta1: %lf, theta2: %lf\n", theta1, theta2);

      isRed = !isRed;

      if (isRed) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
      } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
      }

      // anticlockWise, p1,p2,p3,p4

      // ph1,theta1
      Point3D p1 = getCartesianCoordinates3D(r, phi1, theta1);

      // phi2, theta1
      Point3D p2 = getCartesianCoordinates3D(r, phi2, theta1);

      // phi2, theta2
      Point3D p3 = getCartesianCoordinates3D(r, phi2, theta2);

      // phi1, theta2
      Point3D p4 = getCartesianCoordinates3D(r, phi1, theta2);

      if (show) {
        printf("p1: %lf %lf %lf\n", p1.x, p1.y, p1.z);
        printf("p2: %lf %lf %lf\n", p2.x, p2.y, p2.z);
        printf("p3: %lf %lf %lf\n", p3.x, p3.y, p3.z);
        printf("p4: %lf %lf %lf\n", p4.x, p4.y, p4.z);
      }

      // first and last stack contains only 1 triangle
      if (stackStep != (stackCount - 1)) {
        glBegin(GL_TRIANGLES);
        {
          glVertex3f(p1.x, p1.y, p1.z);
          glVertex3f(p2.x, p2.y, p2.z);
          glVertex3f(p3.x, p3.y, p3.z);
        }
        glEnd();
      }

      if (stackStep != 0) {
        glBegin(GL_TRIANGLES);
        {
          glVertex3f(p1.x, p1.y, p1.z);
          glVertex3f(p3.x, p3.y, p3.z);
          glVertex3f(p4.x, p4.y, p4.z);
        }
        glEnd();
      }
      if (show)
        printf("\n");
    }
  }
  show = false;
}

void drawCone(double r, double h, int sectorCount) {
  for (int sectorStep = 0; sectorStep < sectorCount; sectorStep++) {
    double theta1 = (360.0 * sectorStep) / sectorCount;
    double theta2 = (360.0 * (sectorStep + 1)) / sectorCount;

    // anticlockWise, p1,p2

    // theta1
    Point3D p1 = getCartesianCoordinates3D(r, 0, theta1);

    // theta2
    Point3D p2 = getCartesianCoordinates3D(r, 0, theta2);

    // top
    Point3D p3 = Point3D(0, 0, h);

    glBegin(GL_TRIANGLES);
    {
      glVertex3f(p1.x, p1.y, p1.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glVertex3f(p3.x, p3.y, p3.z);
    }
    glEnd();

    // bottom
    glBegin(GL_TRIANGLES);
    {
      glVertex3f(p1.x, p1.y, 0);
      glVertex3f(p2.x, p2.y, 0);
      glVertex3f(p3.x, p3.y, 0);
    }
    glEnd();
  }
}

// fapa cylinder, vorat korte gele 2ta circle lagbe
void drawCylinder(double r, double h, int sectorCount) {
  for (int sectorStep = 0; sectorStep < sectorCount; sectorStep++) {
    double theta1 = (360.0 * sectorStep) / sectorCount;
    double theta2 = (360.0 * (sectorStep + 1)) / sectorCount;

    // anticlockWise, p1,p2,p3,p4

    // theta1
    Point3D p1 = getCartesianCoordinates3D(r, 0, theta1);

    // theta2
    Point3D p2 = getCartesianCoordinates3D(r, 0, theta2);

    glBegin(GL_QUADS);
    {
      glVertex3f(p1.x, p1.y, 0);
      glVertex3f(p2.x, p2.y, 0);
      glVertex3f(p2.x, p2.y, h);
      glVertex3f(p1.x, p1.y, h);
    }
    glEnd();
  }
}

void drawAxes() {
  glBegin(GL_LINES);
  {
    glVertex3f(-100, 0, 0);
    glVertex3f(100, 0, 0);

    glVertex3f(0, -100, 0);
    glVertex3f(0, 100, 0);

    glVertex3f(0, 0, -100);
    glVertex3f(0, 0, 100);
  }
  glEnd();
}

void drawRectangle(double width, double height) {
  double a = width / 2;
  double b = height;
  glBegin(GL_QUADS);
  {
    glVertex3f(0, -a, 0);
    glVertex3f(b, -a, 0);
    glVertex3f(b, a, 0);
    glVertex3f(0, a, 0);
  }
  glEnd();
}

void drawBoundaryWallOfRadius(double width, double height, double a) {
  glPushMatrix();
  glTranslatef(a, 0, 0);
  glRotatef(-90, 0, 1, 0);
  drawRectangle(width, height);
  glPopMatrix();
}

bool ok = true;

void drawNGonBoundary(int n, double radius, double wallHeight) {
  double angle = 360.0 / n;

  double theta = angle / 2.0;
  double width = 2 * radius * tan(theta * PI / 180.0);

  if (ok) {
    printf("angle: %lf, theta: %lf, width: %lf radius: %lf tan: %lf\n", angle,
           theta, width, radius, tan(theta * PI / 360.0));
    ok = false;
  }

  for (int i = 0; i < n; i++) {
    glPushMatrix();
    glRotatef(i * angle, 0, 0, 1);
    drawBoundaryWallOfRadius(width, wallHeight, radius);
    glPopMatrix();
  }
}

void drawSquare(double a) {
  glBegin(GL_QUADS);
  {
    glVertex3f(a, a, 0);
    glVertex3f(a, -a, 0);
    glVertex3f(-a, -a, 0);
    glVertex3f(-a, a, 0);
  }
  glEnd();
}

void drawCube(double a) {
  glBegin(GL_QUADS);
  {

    drawSquare(a);

    glPushMatrix();
    glTranslatef(-a, 0, a);
    glRotatef(-90, 0, 1, 0);
    drawSquare(a);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glPushMatrix();
    glTranslatef(0, -a, a);
    glRotatef(-90, 1, 0, 0);
    drawSquare(a);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glPushMatrix();
    glTranslatef(0, 0, 2 * a);
    drawSquare(a);
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, -a, a);
    drawSquare(a);
    glPopMatrix();

    glColor3f(1.0f, 0.0f, 1.0f); // Magenta
    glPushMatrix();
    glTranslatef(a, 0, a);
    glRotatef(-90, 0, 1, 0);
    drawSquare(a);
    glPopMatrix();
  }
  glEnd();
}

void drawArrow(double a) {
  double line_height = 0.8 * a;
  double line_width = 0.025 * a;

  double triangle_width = 0.1 * a;

  glPushMatrix();
  drawCylinder(line_width, line_height, 16);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, line_height);
  drawCone(triangle_width, 2 * triangle_width, 16);
  glPopMatrix();
}

void display() {
  // printf("Display function called for %d times\n", counter);

  // glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set background color to black and
  // opaque
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera.ex, camera.ey, camera.ez, camera.lx, camera.ly, camera.lz,
            camera.ux, camera.uy, camera.uz);

  glColor3f(1.0f, 0.0f, 0.0f); // Green
  drawAxes();

  drawCheckers(4);

  // set color red
  glColor3f(1.0f, 0.0f, 0.0f); // Red
  drawNGonBoundary(BOUNDARY_DIMENSION, BOUNDARY_RADIUS, BOUNDARY_HEIGHT);

  // up_arrow on ball, cyan color
  glColor3f(0.0f, 1.0f, 1.0f); // Cyan
  glPushMatrix();
  glTranslatef(ball.x_coord, ball.y_coord, ball.radius + CLEARANCE);
  drawArrow(3);
  glPopMatrix();

  // direction_arrow
  glColor3f(0.0f, 0.0f, 1.0f); // Blue
  glPushMatrix();
  glTranslatef(ball.x_coord, ball.y_coord, ball.radius + CLEARANCE);
  glRotatef(ball.move_angle_x * 180 / PI, 0, 0, 1);
  glRotatef(90, 0, 1, 0);
  drawArrow(3);
  glPopMatrix();

  // ball
  glColor3f(1.0f, 0.0f, 0.0f); // Red
  glPushMatrix();
  glTranslatef(ball.x_coord, ball.y_coord, CLEARANCE + ball.radius);
  glRotatef(ball.rotation_angle_x * 180 / PI, 1, 0, 0);
  glRotatef(ball.rotation_angle_y * 180 / PI, 0, 1, 0);
  drawSphere(ball.radius, ball.sector_count, ball.stack_count);
  glPopMatrix();

  glutSwapBuffers();
}

void idle() {
  if (animate) {
    // counter++;
  }
  // counter++;
  glutPostRedisplay(); // Post a re-paint request to activate display(),
                       // variable set koire then call koro display
}

void ballMovement(int value) {
  if (simulationMode) {
    ball.moveForwardOrBackward(true);
    checkCollisionManual(&ball);
  }

  glutPostRedisplay();
  glutTimerFunc(SIMULATION_CALL_INTERVAL, ballMovement, SIMULATION_CALL_INTERVAL);
}

// void checkCollision(int value) {

//   // we are in manual control
//   if (!simulationMode) {
//     glutTimerFunc(10, checkCollision, 10);
//     return;
//   } else {
//     checkCollisionManual(&ball);

//     glutPostRedisplay();
//     glutTimerFunc(10, checkCollision, 10);
//   }
// }

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(600, 600); // Set the window's initial width & height
  glutInitWindowPosition(750,
                         250); // Position the window's initial top-left corner
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Test");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboardListener);
  glutSpecialFunc(keyboardSpecialListener);

  glutTimerFunc(1, ballMovement, 200);
  // glutTimerFunc(1, checkCollision, 10);
  glutIdleFunc(idle);
  init();

  glutMainLoop();
  return 0;
}