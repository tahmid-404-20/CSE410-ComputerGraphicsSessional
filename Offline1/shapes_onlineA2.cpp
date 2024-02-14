#include "camera.cpp"
#include "co-ordinate.cpp"
#include "shapes.cpp"
#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define MAX_LENGTH_TRIANGLE 1
#define CHANGE_AMOUNT 0.05
#define CHANGE_AMOUNT_RADIUS                                                   \
  CHANGE_AMOUNT / sqrt(3) // the inscribed sphere has radius a/sqrt(3) [triangle
                          // is  (a,0,0), (0,a,0), (0,0,a)]
#define OBJECT_ROTATION_ANGLE 0.1

#define OBJECT_SECTORS 200

double step = 0.5;
double rotation_angle = 0.05;

Camera camera(2, 2, 2, 0, 0, 0, 0, 0, 1);

double triangle_length = 1;
double sphere_radius = 0;

double current_object_rotation_angle = 0;

Cylinder cylinder(0.0, MAX_LENGTH_TRIANGLE *sqrt(2), OBJECT_SECTORS);
HalfCylinder halfCylinder(0.0, MAX_LENGTH_TRIANGLE *sqrt(2), OBJECT_SECTORS);
Sphere sphere(0.0, OBJECT_SECTORS, OBJECT_SECTORS);
HalfSphere halfSphere(0.0, OBJECT_SECTORS, OBJECT_SECTORS);

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

bool show = false;

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
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, true); }
    break;

  case '6': // tilt clockwise
    printf("6 pressed\n");
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, false); }
    break;

  case 'w':
    printf("w pressed\n");
    { camera.moveUpOrDownWithoutChangingReferencePoint(step / 2.0, true); }
    break;

  case 's':
    printf("s pressed\n");
    { camera.moveUpOrDownWithoutChangingReferencePoint(step / 2.0, false); }
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
    printf("Triangle length: %lf CHANGE_AMOUNT %lf\n", triangle_length,
           CHANGE_AMOUNT);
    {
      if (triangle_length > 0) {
        triangle_length -= CHANGE_AMOUNT;
        sphere_radius += CHANGE_AMOUNT_RADIUS;

        if (sphere_radius > MAX_LENGTH_TRIANGLE / sqrt(3)) {
          sphere_radius = MAX_LENGTH_TRIANGLE / sqrt(3);
        }

        if (triangle_length < 0) {
          triangle_length = 0;
        }

        cylinder.updateRadiusAndHeight(sphere_radius,
                                       triangle_length * sqrt(2));

        halfCylinder.updateRadiusAndHeight(sphere_radius,
                                           triangle_length * sqrt(2));
        sphere.update_radius(sphere_radius);
        halfSphere.update_radius(sphere_radius);
        //  print the values
        printf("triangle_length: %lf\n", triangle_length);
        printf("sphere_radius: %lf\n", sphere_radius);
        printf("cylinder.height: %lf\n", cylinder.height);

        if (!show) {
          printf("cylinder.points.size(): %d\n", cylinder.points.size());
          show = true;
        }
      }
    }
    break;
  case '.':
    printf(". pressed\n");
    {
      if (triangle_length <= MAX_LENGTH_TRIANGLE) {
        triangle_length += CHANGE_AMOUNT;
        sphere_radius -= CHANGE_AMOUNT_RADIUS;

        if (sphere_radius < 0) {
          sphere_radius = 0.0;
        }

        if (triangle_length > MAX_LENGTH_TRIANGLE) {
          triangle_length = MAX_LENGTH_TRIANGLE;
        }

        cylinder.updateRadiusAndHeight(sphere_radius,
                                       triangle_length * sqrt(2));
        halfCylinder.updateRadiusAndHeight(sphere_radius,
                                           triangle_length * sqrt(2));

        sphere.update_radius(sphere_radius);
        halfSphere.update_radius(sphere_radius);

        //  print the values
        printf("triangle_length: %lf\n", triangle_length);
        printf("sphere_radius: %lf\n", sphere_radius);
        printf("cylinder.height: %lf\n", cylinder.height);
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

void drawSphereSegment(Sphere &sphere) {
  // std::vector<std::vector<Point3D>> points = sphere.points;
  std::vector<std::vector<Point3D>> points = sphere.points;

  for (int i = 0; i < points.size() - 1; i++) { // stacks
    std::vector<Point3D> points_on_sector = points[i];
    std::vector<Point3D> points_on_next_sector = points[i + 1];

    for (int j = 0; j < points_on_sector.size() - 1; j++) {
      Point3D p1 = points_on_sector[j];
      Point3D p2 = points_on_sector[j + 1];
      Point3D p3 = points_on_next_sector[j];
      Point3D p4 = points_on_next_sector[j + 1];

      // anti-clockwise p1, p3, p4, p2
      glBegin(GL_QUADS);
      {
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p2.x, p2.y, p2.z);
      }
      glEnd();
    }
  }
}

void drawHalfSphereSegment(HalfSphere &sphere) {
  // std::vector<std::vector<Point3D>> points = sphere.points;
  std::vector<std::vector<Point3D>> points = sphere.points;

  for (int i = 0; i < points.size() - 1; i++) { // stacks
    std::vector<Point3D> points_on_sector = points[i];
    std::vector<Point3D> points_on_next_sector = points[i + 1];

    for (int j = 0; j < points_on_sector.size() - 1; j++) {
      Point3D p1 = points_on_sector[j];
      Point3D p2 = points_on_sector[j + 1];
      Point3D p3 = points_on_next_sector[j];
      Point3D p4 = points_on_next_sector[j + 1];

      // anti-clockwise p1, p3, p4, p2
      glBegin(GL_QUADS);
      {
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p2.x, p2.y, p2.z);
      }
      glEnd();
    }
  }
}

void drawCylinderSegment(Cylinder &cylinder) {
  std::vector<Point2D> points = cylinder.points;
  double height = cylinder.height;

  for (int i = 0; i < points.size()-1; i++) {
    Point2D p1 = points[i];
    Point2D p2 = points[i+1];

    glBegin(GL_QUADS);
    {
      glVertex3f(p1.x, p1.y, height / 2.0);
      glVertex3f(p1.x, p1.y, -height / 2.0);
      glVertex3f(p2.x, p2.y, -height / 2.0);
      glVertex3f(p2.x, p2.y, height / 2.0);
    }
    glEnd();
  }
}

void drawHalfCylinderSegment(HalfCylinder &cylinder) {
  std::vector<Point2D> points = cylinder.points;
  double height = cylinder.height;

  for (int i = 0; i < points.size()-1; i++) {
    Point2D p1 = points[i];
    Point2D p2 = points[i+1];

    glBegin(GL_QUADS);
    {
      glVertex3f(p1.x, p1.y, height / 2.0);
      glVertex3f(p1.x, p1.y, -height / 2.0);
      glVertex3f(p2.x, p2.y, -height / 2.0);
      glVertex3f(p2.x, p2.y, height / 2.0);
    }
    glEnd();
  }
}

void drawASphere(Sphere &sphere) {
  glPushMatrix();
  glTranslatef(0, 0, triangle_length);
  drawSphereSegment(sphere);
  glPopMatrix();
}

void drawAHalfSphere(HalfSphere &sphere) {
  glPushMatrix();
  glTranslatef(0, 0, triangle_length);
  drawHalfSphereSegment(sphere);
  glPopMatrix();
}

void drawACylinder(Cylinder &cylinder) {
  glPushMatrix();
  glRotatef(-45, 0, 1, 0);
  glTranslatef(triangle_length / sqrt(2), 0, 0);
  drawCylinderSegment(cylinder);
  glPopMatrix();
}

void drawAllSpheres(Sphere &sphere) {
  // top and bottom are red colors
  glColor3f(1.0f, 0.0f, 0.0f);
  glPushMatrix();
  drawASphere(sphere);
  glPopMatrix();

  glPushMatrix();
  glRotatef(180, 1, 0, 0);
  drawASphere(sphere);
  glPopMatrix();

  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0) {
      // blue
      glColor3f(0.0f, 0.0f, 1.0f);
    } else {
      // green
      glColor3f(0.0f, 1.0f, 0.0f);
    }
    glPushMatrix();
    glRotatef(i * 90, 0, 0, 1);
    glRotatef(90, 1, 0, 0);
    // drawAHalfSphere(halfSphere);
    drawASphere(sphere);
    glPopMatrix();
  }
}

void drawAllCylinders(Cylinder &cylinder) {
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    {
      glRotatef(i * 90, 0, 0, 1);
      drawACylinder(cylinder);
    }
    glPopMatrix();

    for (int i = 0; i < 4; i++) {
      glPushMatrix();
      {
        glRotatef(45 + i * 90, 0, 0, 1);
        glRotatef(90, 1, 0, 0);
        glTranslatef(triangle_length / sqrt(2), 0, 0);
        drawCylinderSegment(cylinder);
      }
      glPopMatrix();
    }

    // for lower half cut
    // for (int i = 0; i < 4; i++) {
    //   glPushMatrix();
    //   {
    //     glRotatef(45 + i * 90, 0, 0, 1);
    //     glRotatef(90, 1, 0, 0);
    //     glTranslatef(triangle_length / sqrt(2), 0, 0);
    //     drawHalfCylinderSegment(halfCylinder);
    //   }
    //   glPopMatrix();
    // }

    glPushMatrix();
    {
      glRotatef(i * 90, 0, 0, 1);
      glRotatef(180, 1, 0, 0);
      drawACylinder(cylinder);
    }
    glPopMatrix();
  }
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
    // if( i >= 4) {
    //   break;
    // }
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

  // glColor3f(0.0f, 0.0f, 1.0f); // Green
  // drawAxes();

  // set color to cyan
  glColor3f(0.0f, 1.0f, 1.0f);
  glPushMatrix();
  glRotatef(current_object_rotation_angle * 180 / M_PI, 0, 0, 1);
  drawOctahedron(triangle_length);
  glPopMatrix();

  // // set color to yellow
  glColor3f(1.0f, 1.0f, 0.0f);
  glPushMatrix();
  glRotatef(current_object_rotation_angle * 180 / M_PI, 0, 0, 1);
  drawAllCylinders(cylinder);
  glPopMatrix();

  glPushMatrix();
  glRotatef(current_object_rotation_angle * 180 / M_PI, 0, 0, 1);
  drawAllSpheres(sphere);
  glPopMatrix();

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(600, 600); // Set the window's initial width & height
  glutInitWindowPosition(750,
                         250); // Position the window's initial top-left corner
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Magic Cube - 1905002");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboardListener);
  glutSpecialFunc(keyboardSpecialListener);

  // glutTimerFunc(1, checkCollision, 10);
  glutIdleFunc(idle);
  init();

  glutMainLoop();
  return 0;
}