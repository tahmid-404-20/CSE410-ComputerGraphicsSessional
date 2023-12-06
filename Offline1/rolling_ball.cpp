#include "ball.cpp"
#include "camera.cpp"
#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define CHECKERBOARD_SIZE 80
#define CLEARANCE 0.1

#define PI 3.1415926535897

using namespace std;

int animate = 1;
double step = 0.5;
double rotation_angle = 0.05;

int x_coord, y_coord;
bool rotateZ = false;

// a openGL integer
GLint counter = 0;
Camera camera;
Ball ball;

class Point3D {
public:
  double x, y, z;
  Point3D() {
    x = 0;
    y = 0;
    z = 0;
  }
  Point3D(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
};

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
    { ball.moveForwardOrBackward(true); }
    break;
  case 'k': // move backward
    printf("k pressed\n");
    { ball.moveForwardOrBackward(false); }
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

Point3D getCartesianCoordinates(double r, double phi, double theta) {
  phi = phi * M_PI / 180;
  theta = theta * M_PI / 180;
  return Point3D(r * cos(phi) * cos(theta), r * cos(phi) * sin(theta),
                 r * sin(phi));
}

bool show = false;

void drawSphere(double r, int sectorCount, int stackCount) {
  // both ints are even
  bool alterColor = false;

  bool isRed = false;
  for (int stackStep = 0; stackStep < stackCount; stackStep++) {
    double phi1 = 90.0 - (180.0 * stackStep) / stackCount;
    double phi2 = 90.0 - (180.0 * (stackStep + 1)) / stackCount;

    if(show) printf("phi1: %lf, phi2: %lf\n", phi1, phi2);

    if(stackStep * 2 >= stackCount) {
      isRed = true;
    }

    for (int sectorStep = 0; sectorStep < sectorCount; sectorStep++) {
      double theta1 = (360.0 * sectorStep) / sectorCount;
      double theta2 = (360.0 * (sectorStep + 1)) / sectorCount;

      if (show) printf("theta1: %lf, theta2: %lf\n", theta1, theta2);

      isRed = !isRed;      

      if(isRed) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
      } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
      }

      // anticlockWise, p1,p2,p3,p4

      // ph1,theta1
      Point3D p1 = getCartesianCoordinates(r, phi1, theta1);

      // phi2, theta1
      Point3D p2 = getCartesianCoordinates(r, phi2, theta1);

      // phi2, theta2
      Point3D p3 = getCartesianCoordinates(r, phi2, theta2);

      // phi1, theta2
      Point3D p4 = getCartesianCoordinates(r, phi1, theta2);

      if(show) {
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
      if(show) printf("\n");
    }
  }
  show = false;
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

void drawTriangle(double a) {
  glBegin(GL_TRIANGLES);
  {
    glVertex3f(0, -a, 0);
    glVertex3f(0, a, 0);
    glVertex3f(2 * a, 0, 0);
  }
  glEnd();
}

void drawRectangle(double width, double height) {

  double a = width / 2.0;
  double b = height;
  glBegin(GL_QUADS);
  {
    glVertex3f(0, -a, 0);
    glVertex3f(0, a, 0);
    glVertex3f(b, a, 0);
    glVertex3f(b, -a, 0);
  }
  glEnd();
}

void drawArrow(double a) {
  double line_height = 0.8 * a;
  double line_width = 0.1 * a;

  double triangle_width = 0.2 * a;

  glPushMatrix();
  drawRectangle(line_width, line_height);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(line_height, 0, 0);
  drawTriangle(triangle_width);
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

  // glBegin(GL_LINES);{
  //     glColor3f(1.0f, 1.0f, 1.0f); // Green
  //     glVertex2f(-1.0f, 0.0f);
  //     glVertex2f(1.0f, 0.0f);

  //     glVertex2f(0.0f, -1.0f);
  //     glVertex2f(0.0f, 1.0f);

  // }glEnd();

  glColor3f(1.0f, 0.0f, 0.0f); // Green
  drawAxes();

  drawCheckers(1);

  double angle_in_degree = ball.move_angle_x * 180 / PI;

  // direction_arrow
  glColor3f(0.0f, 1.0f, 0.0f); // Green
  glPushMatrix();
  glTranslatef(ball.x_coord, ball.y_coord, CLEARANCE);
  glRotatef(angle_in_degree, 0, 0, 1);
  drawArrow(3);
  glPopMatrix();

  // ball
  glColor3f(1.0f, 0.0f, 0.0f); // Red
  glPushMatrix();
  glTranslatef(ball.x_coord, ball.y_coord, CLEARANCE + ball.radius);
  glRotatef(ball.orientation_angle_y * 180 / PI, 1, 0, 0);
  glRotatef(ball.orientation_angle_x * 180 / PI, 0, 1, 0);
  // glTranslatef(0, 0, -ball.radius);
  // drawCube(ball.radius);
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
  glutIdleFunc(idle);
  init();

  glutMainLoop();
  return 0;
}