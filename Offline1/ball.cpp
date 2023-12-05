#include "ball_config.cpp"
#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define CHECKERBOARD_SIZE 80

using namespace std;

int animate = 1;
double step = 0.5;
double rotation_angle = 0.05;

int x_coord, y_coord;
bool rotateZ = false;

// a openGL integer
GLint counter = 0;
Camera camera;

void init() {
  printf("Do your initialization here\n");
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f); // Set background color to black and opaque

  counter = 45;
  x_coord = 2;
  y_coord = 2;
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
    {
      camera.moveLeftOrRight(step, false);
    }
    break;

  case GLUT_KEY_LEFT:
    printf("Left arrow pressed\n");
    {
      camera.moveLeftOrRight(step, true);
    }
    break;

  case GLUT_KEY_PAGE_UP:
    printf("Page up pressed\n");
    {
      camera.moveUpOrDown(step, true);
    }
    break;

  case GLUT_KEY_PAGE_DOWN:
    printf("Page down pressed\n");
    {
      camera.moveUpOrDown(step, false);
    }
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
  }
  glEnd();
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

  // drawSquare(0.25);
  glColor3f(1.0f, 0.0f, 0.0f); // Red

  // // translation
  // glPushMatrix();
  // glTranslatef(x_coord, y_coord, 0);
  // drawCube(0.5);
  // glPopMatrix();

  // // rotation on the point (x_coord, y_coord)
  // glPushMatrix();
  // // glLoadIdentity();
  // // glTranslatef(x_coord, y_coord, 0);
  // // // if(rotateZ) {
  // //     glRotatef(counter % 360, 0, 0, 1);
  // //     rotateZ = false;
  // // // }
  // glTranslatef(-x_coord, -y_coord, 0);
  // drawCube(0.5);
  // glPopMatrix();

  glPushMatrix();
  // glLoadIdentity();
  glTranslatef(x_coord, y_coord, 0);
  // // if(rotateZ) {
  glRotatef(counter % 360, 0, 0, 1);
  rotateZ = false;
  // // }
  // glTranslatef(-x_coord, -y_coord, 0);
  drawCube(1);
  glPopMatrix();
  // glBegin(GL_QUADS); {
  //     glColor3f(1.0f, 0.0f, 0.0f); // Red
  //     glVertex2f(0.0f, 0.0f);
  //     glVertex2f(0.5f, 0.0f);
  //     glVertex2f(0.5f, 0.5f);
  //     glVertex2f(0.0f, 0.5f);

  // }glEnd();
  // glColor3f(0.0f, 1.0f, 0.0f); // Green
  // for(int i=0;i < 4; i++) {
  //     glPushMatrix();
  //     glRotatef(coun, 0, 0, 1);
  //     glTranslatef(0.5, 0.5, 0);
  //     drawSquare(0.25);
  //     glPopMatrix();
  // }

  // for(int i = 0; i < 8; i++){
  // glPushMatrix();
  // glRotatef((counter)%360 , 0, 0, 1);
  // glTranslatef(0.5, 0.5, 0);
  // glColor3f(1.0f, 0.0f, 0.0f); // Red
  // drawSquare(0.1);
  // glPopMatrix();
  // }

  // glFlush();
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