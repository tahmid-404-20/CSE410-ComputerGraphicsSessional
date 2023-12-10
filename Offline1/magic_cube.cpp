#include "camera.cpp"
#include "co-ordinate.cpp"
#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define MAX_LENGTH_TRIANGLE 1
#define CHANGE_AMOUNT 0.1
#define OBJECT_ROTATION_ANGLE 0.1

double step = 0.5;
double rotation_angle = 0.05;

Camera camera(2, 2, 2, 0, 0, 0, 0, 0, 1);

double triangle_length = 1;
double current_object_rotation_angle = 0;

void init() {
  printf("Do your initialization here\n");
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f); // Set background color to black and opaque

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 500);
}

void idle() {
  glutPostRedisplay(); // Post a re-paint request to activate display(),
                       // variable set koire then call koro display
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

  case 'a':
    printf("a pressed\n");
    { current_object_rotation_angle -= OBJECT_ROTATION_ANGLE; }
    break;
  case 'd':
    printf("d pressed\n");
    { current_object_rotation_angle += OBJECT_ROTATION_ANGLE; }
    break;

  case ',':
    printf(", pressed\n");
    {
      if (triangle_length >= CHANGE_AMOUNT) {
        triangle_length -= CHANGE_AMOUNT;
      }
    }
    break;
  case '.':
    printf(". pressed\n");
    {
      if (triangle_length <= MAX_LENGTH_TRIANGLE - CHANGE_AMOUNT) {
        triangle_length += CHANGE_AMOUNT;
      }
    }
    break;
  default:
    printf("We don't know what you pressedBRO\n");
    break;
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

void drawTriangle(double a) {
  glBegin(GL_TRIANGLES);
  {
    glVertex3f(a, 0, 0);
    glVertex3f(0, a, 0);
    glVertex3f(0, 0, a);
  }
  glEnd();
}

// a should be less than MAX_LENGTH_TRIANGLE
void drawOctahedron(double a) {

  if (a > MAX_LENGTH_TRIANGLE) {
    printf("a should be less than MAX_LENGTH_TRIANGLE\n");
    return;
  }

  double shift = (MAX_LENGTH_TRIANGLE - a) / 3.0;

  bool isCyan = false;
  for (int i = 0; i < 8; i++) {
    glPushMatrix();
    {

      isCyan = !isCyan;

      if (isCyan) {
        glColor3f(0.0f, 1.0f, 1.0f);
      } else { // set color to magenta
        glColor3f(1.0f, 0.0f, 1.0f);
      }
      if (i >= 4) {
        glRotatef(180, 0, 1, 0);
      }
      glRotatef(i * 90, 0, 0, 1);
      glTranslated(shift, shift, shift);
      glScaled(a, a, a);
      drawTriangle(1);
    }
    glPopMatrix();
  }
}

void display() {

  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera.ex, camera.ey, camera.ez, camera.lx, camera.ly, camera.lz,
            camera.ux, camera.uy, camera.uz);

  glColor3f(0.0f, 0.0f, 1.0f); // Green
  drawAxes();

  // set color to cyan
  glColor3f(0.0f, 1.0f, 1.0f);
  glPushMatrix();
  glRotatef(current_object_rotation_angle * 180 / M_PI, 0, 0, 1);
  drawOctahedron(triangle_length);
  glPopMatrix();

  glutSwapBuffers();
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

  // glutTimerFunc(1, checkCollision, 10);
  glutIdleFunc(idle);
  init();

  glutMainLoop();
  return 0;
}