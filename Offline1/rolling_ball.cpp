#include "ball.cpp"
#include "camera.cpp"
#include "co-ordinate.cpp"
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

#define BOUNDARY_RADIUS 12
#define BOUNDARY_DIMENSION 4
#define BOUNDARY_HEIGHT 2

#define CLEARANCE 0.1

#define SIMULATION_CALL_INTERVAL 50

#define DT 10

#define PI 3.1415926535897

using namespace std;

enum Wall {
  LEFT,
  RIGHT,
  TOP,
  BOTTOM
}; // think x axis as horizontal
   // and y axis as vertical

double step = 0.5;
double rotation_angle = 0.05;

int x_coord, y_coord;
bool rotateZ = false;

bool simulationMode = false;
int collisionTracksCurrentSimulation;
double simulationTime;

void ballMoveMentEventDriven(int hashIndex);
void checkCollisionManual(Ball *ball);
void predictNextCollision(Ball &ball);

Camera camera;
Ball ball;

class Event {
public:
  double time;
  int collisionCount;
  int id;

  Event() {
    this->time = 0;
    this->collisionCount = 0;
    this->id = 0;
  }

  Event(double time, int collisionCount, int id) {
    this->time = time;
    this->collisionCount = collisionCount;
    this->id = id;
  }

  bool operator<(const Event &other) const { return time > other.time; }
};

priority_queue<Event> eventQueue;
unordered_map<int, Event> collisionMap;

double predictCollisionTime(Ball &ball, Wall wall) {
  double x = ball.x_coord;
  double y = ball.y_coord;
  double r = ball.radius;

  // the ball moves ball.speed pixels in each SIMULATION_CALL_INTERVAL, so we
  // need to calculate the time to reach the wall
  double dx = ball.speed * cos(ball.move_angle_x);
  double dy = ball.speed * sin(ball.move_angle_x);

  double t = 0;

  if ((wall == LEFT || wall == RIGHT) && dx == 0) {
    return -1;
  } else if ((wall == TOP || wall == BOTTOM) && dy == 0) {
    return -1;
  }

  switch (wall) {
  case LEFT:
    t = (BOUNDARY_RADIUS - r - x) / dx;
    break;
  case RIGHT:
    t = (-BOUNDARY_RADIUS + r - x) / dx;
    break;
  case TOP:
    t = (BOUNDARY_RADIUS - r - y) / dy;
    break;
  case BOTTOM:
    t = (-BOUNDARY_RADIUS + r - y) / dy;
    break;
  }

  t *= SIMULATION_CALL_INTERVAL;

  return t;
}

void initializeSimulationMode() {
  ball.collision_count = 0;
  collisionTracksCurrentSimulation = 0;
  simulationTime = 0.0;

  while (!eventQueue.empty()) {
    eventQueue.pop();
  }

  collisionMap.clear();

  Wall wall;
  for (int i = 0; i < BOUNDARY_DIMENSION; i++) {
    wall = (Wall)i;
    double t = predictCollisionTime(ball, wall);
    if (t - 0.0 >= 0.00000001) {
      Event event(t, ball.collision_count, collisionTracksCurrentSimulation++);
      eventQueue.push(event);
    }
  }

  // pop the first event
  Event event = eventQueue.top();
  eventQueue.pop();

  collisionMap[event.id] = event;
  glutTimerFunc(event.time, ballMoveMentEventDriven, event.id);
}

void ballMoveMentEventDriven(int hashIndex) {
  if (!simulationMode) {
    collisionMap.erase(hashIndex);
    return;
  }

  Event event = collisionMap[hashIndex];

  if (event.collisionCount != ball.collision_count) {
    printf("Event is not valid anymore in ballMovementEventDriven\n");
    collisionMap.erase(hashIndex);
    return;
  }

  collisionMap.erase(hashIndex);
  ball.moveForwardOrBackward(true);
  // ball.collision_count++;
  checkCollisionManual(&ball);
  predictNextCollision(ball);
}

void init() {
  printf("Do your initialization here\n");
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f); // Set background color to black and opaque

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

  if (simulationMode) {
    if (x_change || y_change) {
      ball->collision_count++;
    }

    predictNextCollision(*ball);
  }
}

void predictNextCollision(Ball &ball) {
  Wall wall;
  for (int i = 0; i < BOUNDARY_DIMENSION; i++) {
    wall = (Wall)i;
    double t = predictCollisionTime(ball, wall);
    if (t - 0.0 >= 0.00000001) {
      Event event(t, ball.collision_count, collisionTracksCurrentSimulation++);
      eventQueue.push(event);
    }
  }

  while (true) {
    Event event = eventQueue.top();
    eventQueue.pop();

    if (event.collisionCount != ball.collision_count) {
      printf("Event is not valid anymore in Predict NextCollision\n");
      collisionMap.erase(event.id);
      continue;
    }

    collisionMap[event.id] = event;
    glutTimerFunc(event.time, ballMoveMentEventDriven, event.id);
    break;
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
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, true); }
    break;

  case '6': // tilt clockwise
    printf("6 pressed\n");
    { camera.tiltClockwiseOrAntiClockwise(rotation_angle, false); }
    break;

  case 'j': // rotate counter-clockwise
    printf("j pressed\n");
    {
      ball.changeMoveDirectionAngle(false);
      if (simulationMode) {
        predictNextCollision(ball);
      }
    }
    break;
  case 'l': // rotate clockwise
    printf("l pressed\n");
    {
      ball.changeMoveDirectionAngle(true);
      if (simulationMode) {
        predictNextCollision(ball);
      }
    }
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

  case 'w':
    printf("w pressed\n");
    camera.moveUpOrDownWithoutChangingReferencePoint(step * 2, true);
    break;

  case 's':
    printf("s pressed\n");
    camera.moveUpOrDownWithoutChangingReferencePoint(step * 2, false);
    break;

  case ' ':
    printf("space pressed\n");
    {
      simulationMode = !simulationMode;
      if (simulationMode) {
        initializeSimulationMode();
      }
    }
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

void drawSphere(Ball &ball) {
  vector<vector<PointPair>> points = ball.point_pair_array;

  int stackCount = ball.stack_count;
  int sectorCount = ball.sector_count;

  bool alterColor = false;

  bool isRed = false;
  for (int stackStep = 0; stackStep < stackCount; stackStep++) {
    if (stackStep * 2 >= stackCount) {
      isRed = true;
    }

    for (int sectorStep = 0; sectorStep < sectorCount; sectorStep++) {
      isRed = !isRed;

      if (isRed) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
      } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
      }

      // anticlockWise, p1,p2,p3,p4
      PointPair pp1 = points[stackStep][sectorStep];
      PointPair pp2 = points[stackStep][(sectorStep + 1) % sectorCount];

      Point3D p1 = pp1.p1;
      Point3D p2 = pp1.p2;
      Point3D p3 = pp2.p2;
      Point3D p4 = pp2.p1;

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
    }
  }
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

void drawNGonBoundary(int n, double radius, double wallHeight) {
  double angle = 360.0 / n;

  double theta = angle / 2.0;
  double width = 2 * radius * tan(theta * PI / 180.0);

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

  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera.ex, camera.ey, camera.ez, camera.lx, camera.ly, camera.lz,
            camera.ux, camera.uy, camera.uz);

  // glColor3f(1.0f, 0.0f, 0.0f); // Green
  // drawAxes();

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
  drawSphere(ball);
  glPopMatrix();

  glutSwapBuffers();
}

void idle() {
  glutPostRedisplay(); // Post a re-paint request to activate display(),
                       // variable set koire then call koro display
}

void ballMovement(int value) {
  if (simulationMode) {
    ball.moveForwardOrBackward(true);
  }

  glutPostRedisplay();
  glutTimerFunc(SIMULATION_CALL_INTERVAL, ballMovement,
                SIMULATION_CALL_INTERVAL);
}

void Timer(int value){
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

  glutTimerFunc(1, ballMovement, 200);
  glutIdleFunc(idle);
  init();

  glutMainLoop();
  return 0;
}