#include "1905002_camera.cpp"
#include "1905002_classes.cpp"
#include <bits/stdc++.h>
#include <unordered_map>
#include <vector>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#define CHECKERBOARD_SIZE 80

double rotation_angle = 0.025;
double step = 0.5;

double nearPlane = 1, farPlane = 1000, fovY = 60, aspectRatio = 1;

// in pixels
int imageWidth = 2000;
int imageHeight = 2000;

int recursionLevel = 3;

Camera camera;

void init() {
  printf("Do your initialization here\n");
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f); // Set background color to black and opaque

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
}

void capture() {
  // initialize bitmap image array
  std::vector<std::vector<Color>> image;

  for (int i = 0; i < imageHeight; i++) {
    std::vector<Color> row;
    for (int j = 0; j < imageWidth; j++) {
      row.push_back(Color(0, 0, 0));
    }
    image.push_back(row);
  }

  // in actual world space
  double windowHeight = 2 * nearPlane * tan(fovY / 2 * (M_PI / 180));
  double windowWidth = windowHeight * aspectRatio;
  Vec l = camera.getLookUnitVector();
  Vec r = camera.getRightUnitVector();
  Vec u = camera.getUUnitVector();

  Vec eye = Vec(camera.ex, camera.ey, camera.ez);

  Vec topLeft = eye + l * nearPlane - r * (windowWidth / 2) + u * (windowHeight / 2);

  double du = windowWidth / imageWidth;
  double dv = windowHeight / imageHeight;

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {

      Vec pixel = topLeft + r * (j + 1.0/2.0) * du - u * ( i + 1.0/2.0 )* dv;
      Vec direction = (pixel - eye).getNormalizedResult();

      // std::cout << "Pixel : " << pixel << " ---> Direction : " << direction << std::endl;

      // starting from nearplane pixel
      Ray ray(pixel, direction);

      Color color;

      double tMin = 1e26;
      int nearestObjectIndex = -1;

      for (int k = 0; k < objects.size(); k++) {
        Color dummyColor;
        double t = objects[k]->intersect(&ray, &dummyColor, 1);
        if (t > 0 && t < tMin) {
          tMin = t;
          nearestObjectIndex = k;
        }
      }
      
      if(nearestObjectIndex != -1) {
        tMin = objects[nearestObjectIndex]->intersect(&ray, &color, 1);
      }
      image[i][j] = color;
    }
  }

  // save the image
  std::ofstream out("output.bmp", std::ios::binary);

  out << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      Color color = image[i][j];
      out << (unsigned char)(std::min(1.0, color.r) * 255) << (unsigned char)(std::min(1.0, color.g) * 255) << (unsigned char)(std::min(1.0, color.b) * 255);
    }
  }

  out.close();

  std::cout << "Image saved" << std::endl;



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
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, true); }
    break;

  case '6': // tilt clockwise
    printf("6 pressed\n");
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, false); }
    break;

  case '0':
    printf("0 pressed\n");
    capture();
    break;

  case 'w':
    printf("w pressed\n");
    camera.moveUpOrDownWithoutChangingReferencePoint(step * 2, true);
    break;

  case 's':
    printf("s pressed\n");
    camera.moveUpOrDownWithoutChangingReferencePoint(step * 2, false);
    break;

  default:
    printf("We don't know what you pressed!!!\n");
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

std::vector<Object *> objects;
std::vector<PointLight> pointLights;
std::vector<SpotLight> spotLights;

void clearObjects() {
  for (auto object : objects) {
    delete object;
  }
  objects.clear();
}

void testSetObjects() {
  Sphere *sphere = new Sphere(Vec(0, 0, 10), 10);
  sphere->setColor(0, 0, 1);
  sphere->setCoEfficients(0.2, 0.6, 0.3, 0.4);
  sphere->setShine(6);
  objects.push_back(sphere);

  sphere = new Sphere(Vec(-20, 10, 10), 10);
  sphere->setColor(0, 1, 0);
  sphere->setCoEfficients(0.2, 0.6, 0.6, 0.5);
  sphere->setShine(6);
  objects.push_back(sphere);

  sphere = new Sphere(Vec(20, -10, 10), 8);
  sphere->setColor(1, 1, 0);
  sphere->setCoEfficients(0.2, 0.6, 0.6, 0.5);
  sphere->setShine(6);
  objects.push_back(sphere);

  sphere = new Sphere(Vec(10, -20, 30), 8);
  sphere->setColor(0, 1, 1);
  sphere->setCoEfficients(0.2, 0.6, 0.6, 0.5);
  sphere->setShine(6);
  objects.push_back(sphere);


  Floor *floor = new Floor(1000, 20);
  floor->setCoEfficients(0.4, 0.4, 0.3, 0.1);
  floor->setShine(1);
  objects.push_back(floor);

  PointLight pointLight(Vec(0, 0, 60), Color(1, 0, 0));
  pointLights.push_back(pointLight);

  PointLight pointLight2(Vec(-30, -40, 20), Color(0.5, 1, 0.5));
  pointLights.push_back(pointLight2);

  // (207,89,6)
  SpotLight spotLight(Vec(20, 20, 20), Color(207.0/255.0, 89/255.0, 6/255.0), Vec(-1, -1, -3), 45);
  spotLights.push_back(spotLight);


}

void displayObjects() {
  for (auto object : objects) {
    object->draw();
  }

  for (auto pointLight : pointLights) {
    pointLight.draw();
  }

  for (auto spotLight : spotLights) {
    spotLight.draw();
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

  displayObjects();

  glutSwapBuffers();
}

void idle() {
  glutPostRedisplay(); // Post a re-paint request to activate display(),
                       // variable set koire then call koro display
}

void Timer(int value) {
  // printf("We are in Timer function. couter : %d\n", ++counter);
  glutPostRedisplay();
  glutTimerFunc(10, Timer, 0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(600, 600); // Set the window's initial width & height
  glutInitWindowPosition(750,
                         250); // Position the window's initial top-left corner
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Rolling Ball - 1905002");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboardListener);
  glutSpecialFunc(keyboardSpecialListener);

  glutIdleFunc(idle);
  init();

  testSetObjects();

  glutMainLoop();

  clearObjects();
  return 0;
}