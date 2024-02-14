#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include "1905002_vec.cpp"
#include <vector>

// contains color, from 0 to 1
class Color {
public:
  double r, g, b;

  Color() {
    r = 0;
    g = 0;
    b = 0;
  }

  Color(double r, double g, double b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

class PointLight {
public:
  Vec position;
  Color color;

  PointLight() {
    position = Vec(0, 0, 0);
    color = Color(1, 1, 1);
  }

  PointLight(Vec position, Color color) {
    this->position = position;
    this->color = color;
  }

  void draw() {
    glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    glColor3f(color.r, color.g, color.b);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
  }
};

class SpotLight {
public:
  PointLight pointLight;
  Vec direction;
  double cutOffAngle;

  SpotLight() {
    pointLight = PointLight();
    direction = Vec(0, 0, 0);
    cutOffAngle = 0;
  }

  SpotLight(Vec position, Color color, Vec direction, double cutOffAngle) {
    pointLight = PointLight(position, color);
    this->direction = direction;
    this->cutOffAngle = cutOffAngle;
  }
};

class Object {
public:
  Vec reference_point;
  double height, width, length;
  Color color;
  double coEfficients[4]; // ambient,diffuse,reflection coefficients
  int shine;              // exponent term of specular component

  Object() {
    reference_point = Vec(0, 0, 0);
    height = 0;
    width = 0;
    length = 0;
    color = Color(0, 0, 0);
    coEfficients[0] = 0;
    coEfficients[1] = 0;
    coEfficients[2] = 0;
    coEfficients[3] = 0;
    shine = 0;
  }

  virtual void draw() = 0;
  void setColor(double r, double g, double b) { color = Color(r, g, b); }
  void setShine(int shine) { this->shine = shine; }
  void setCoEfficients(double a, double d, double s, double r) {
    coEfficients[0] = a;
    coEfficients[1] = d;
    coEfficients[2] = s;
    coEfficients[3] = r;
  }
};

extern std::vector<Object *> objects;
extern std::vector<PointLight> pointLights;
extern std::vector<SpotLight> spotLights;

class Sphere : public Object {
  int sectorsCount, stacksCount;

public:
  Sphere(Vec center, double radius) {
    this->reference_point = center;
    this->height = radius;
    this->width = radius;
    this->length = radius;
    this->sectorsCount = 50;
    this->stacksCount = 100;
  }

  void draw() {
    // openGL code to draw sphere
    glPushMatrix();
    glTranslated(reference_point.x, reference_point.y, reference_point.z);
    glColor3f(color.r, color.g, color.b);
    glutSolidSphere(height, sectorsCount, stacksCount);
    glPopMatrix();
  }
};

class Floor : public Object {
  int tesselation;

public:
  Floor(double floorWidth, double tileWidth) {
    this->reference_point = Vec(-floorWidth / 2, -floorWidth / 2, 0);
    this->height = floorWidth;
    this->width = floorWidth;
    this->length = 0;
    this->tesselation = floorWidth / tileWidth;
  }

  void draw() {
    // openGL code to draw floor
    glPushMatrix();
    glTranslated(reference_point.x, reference_point.y, reference_point.z);
    for (int i = 0; i < tesselation; i++) {
      for (int j = 0; j < tesselation; j++) {
        if ((i + j) % 2 == 0) {
          glColor3f(1, 1, 1);
        } else {
          glColor3f(0, 0, 0);
        }
        glBegin(GL_QUADS);
        glVertex3f(i, j, 0);
        glVertex3f(i + 1, j, 0);
        glVertex3f(i + 1, j + 1, 0);
        glVertex3f(i, j + 1, 0);
        glEnd();
      }
    }
    glPopMatrix();
  }
};

class Triangle : public Object {
  Vec a, b, c;

public:
  Triangle(Vec a, Vec b, Vec c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  void draw() {
    // openGL code to draw triangle
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glEnd();
    glPopMatrix();
  }
};
